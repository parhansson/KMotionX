// SimpleGCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GCodeInterpreter.h"

// Global Variables

CGCodeInterpreter *Interpreter;
CString	GCodeOutput;
int	ErrorLineNo, CurrentLineNo;
CString	ErrorMsg;
int	exitcode;
bool Finished;

void CompleteCallback(int status, int line_no, int sequence_number, const char *err)
{
	ErrorLineNo=line_no;
	ErrorMsg=err;
	exitcode=status;
	Finished=true;
}

void StatusCallback(int line_no, const char *msg)
{
	CurrentLineNo=line_no;
	GCodeOutput+=msg;
}

int UserCallback(const char *msg)
{
	MessageBox(NULL,msg,"SimpleGCode",MB_OK);
	return 0;
}

int UserMCodeCallback(int mcode)
{
	double Var1000 = Interpreter->p_setup->parameters[1000];
	CString s;
	s.Format("MCode %d Trigger GCode Var 1000 = %f",mcode,Var1000); 
	MessageBox(NULL,s,"SimpleGCode",MB_OK);
	return 0;
}


int main(int argc, char* argv[])
{
	int DisplayedLineNo,BoardType,board=0;
	CString InFile="C:\\KMotionSrc\\GCode Programs\\SimpleCircle.ngc";

	
	CKMotionDLL *KM = new CKMotionDLL(0);
	CCoordMotion *CM = new CCoordMotion(KM);
    Interpreter = new CGCodeInterpreter(CM);

	MOTION_PARAMS *p=Interpreter->GetMotionParams();

	p->BreakAngle = 30;
	p->MaxAccelX = 1;
	p->MaxAccelY = 1;
	p->MaxAccelZ = 1;
	p->MaxAccelA = 1;
	p->MaxAccelB = 1;
	p->MaxAccelC = 1;
	p->MaxVelX = 1;
	p->MaxVelY = 1;
	p->MaxVelZ = 1;
	p->MaxVelA = 1;
	p->MaxVelB = 1;
	p->MaxVelC = 1;
	p->CountsPerInchX = 100;
	p->CountsPerInchY = 100;
	p->CountsPerInchZ = 100;
	p->CountsPerInchA = 100;
	p->CountsPerInchB = 100;
	p->CountsPerInchC = 100;
	p->DegreesA = p->DegreesB = p->DegreesC = FALSE; 
	p->ArcsToSegs = true;

	strcpy(Interpreter->ToolFile,"");
	strcpy(Interpreter->SetupFile,"");

	Finished=false;

	if (Interpreter->CoordMotion->KMotionDLL->CheckKMotionVersion( &BoardType)) return 1;

	Interpreter->SetUserCallback(UserCallback);  // Set this to handle (USR,Message) Callbacks

	// configure the Action for MCode 3 to do a Callback
	Interpreter->McodeActions[3].Action = M_Action_Callback;
	// configure the Action for MCode 105 to do a Callback
	Interpreter->McodeActions[MCODE_ACTIONS_M100_OFFSET+5].Action = M_Action_Callback;
	
	Interpreter->SetUserMCodeCallback(UserMCodeCallback);  // Set this to handle MCode Callbacks

	// Execute the GCode!

	Interpreter->Interpret(BoardType,InFile,0,-1,true,StatusCallback,CompleteCallback);

	// Display Current Line Number while executing

	DisplayedLineNo=0;
	while (!Finished)
	{
		if (CurrentLineNo>DisplayedLineNo)
		{
			DisplayedLineNo=CurrentLineNo;
			printf("Current Line = %d\r",CurrentLineNo);
		}
		Sleep(10);
	}

	// Check Interpreter's exit code

	if (exitcode)
	{
		printf("Error in line %d\n",ErrorLineNo);
		printf("%s\n",ErrorMsg.GetBuffer(0));
	}
	else
	{
		printf("%s\n",GCodeOutput.GetBuffer(0));
	}

	getchar();
	return 0;
}

