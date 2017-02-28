//
//  main.cpp
//  GCodeTest
//
//  Created by Par Hansson on 2012-04-20.
//  Copyright (c) 2012 P.Hansson. All rights reserved.
//

//#define BOOL int
//#define FALSE 0
#include "GCodeInterpreterX.h"



/*
#pragma pack(1)
typedef struct
{
    int id;
    char[50] text;
} MESSAGE;

// Send a message
MESSAGE msg;
msg.id = 1;
strcpy(msg.text, "This is a test");
send(socket, (char*)&msg);

// Receive a message
char buffer[100];
recv(socket, buffer, 100);
MESSAGE* msg = (MESSAGE*)buffer;
printf("id=%d\n", msg->id);
printf("text=%s\n", msg->text);

*/
// Global Variables

CGCodeInterpreter *Interpreter;
int	ErrorLineNo, CurrentLineNo;
char ErrorMsg[512];
int	exitcode;
bool Finished;
void CompleteCallback(int status, int line_no, int sequence_number, const char *err);
void StatusCallback(int line_no, const char *msg);
int UserCallback(const char *msg);
int UserMCodeCallback(int mcode);

void CompleteCallback(int status, int line_no, int sequence_number, const char *err)
{
	printf("Complete -- status: %d line: %d  error: %s\n", status,line_no,err);
	ErrorLineNo=line_no;
	strcpy(ErrorMsg,err);
	exitcode=status;
	Finished=true;
}

void StatusCallback(int line_no, const char *msg)
{
	printf("Status -- line: %d message:\n%s\n", line_no,msg);
	CurrentLineNo=line_no;
}

int UserCallback(const char *msg)
{
	MessageBox(0,msg,"User callback",MB_OK);
	return 0;
}

int UserMCodeCallback(int mcode)
{
	char msg[100];
	double Var1000 = Interpreter->p_setup->parameters[1000];
	sprintf(msg, "M%d Trigger GCode Var 1000 = %f\n",mcode,Var1000);
	MessageBox(0,msg,"M code callback",MB_OK);

	return 0;
}


int main(int argc, char* argv[])
{
	int DisplayedLineNo,BoardType;
	BoardType = BOARD_TYPE_KFLOP;
	char InFile[256];
	char m4_cfile[256];
	char setup_cfile[256];
	int setup_thread = 1;
	int m4_thread = 3;
	char command[MAX_LINE];

    char* rootDir;
	rootDir = getenv("PWD");

	sprintf(setup_cfile,"%s/../KMotionX/examples/ExecuteGCode/Stepper3Axis.c",rootDir);
	sprintf(m4_cfile,"%s/../C Programs/BlinkKFLOP.c",rootDir);
	//strcpy(setup_cfile,m4_cfile);
	sprintf(InFile,"%s/../GCode Programs/SimpleCircle.ngc",rootDir);


	CKMotionDLL *KM = new CKMotionDLL(0);


	printf("Loading file %s to thread %d\n", setup_cfile, setup_thread);
	if(KM->CompileAndLoadCoff(setup_cfile,setup_thread)){
		printf("Loading file %s to thread %d failed. Exiting.\n", setup_cfile, setup_thread);
		exit(1);
	} else {
		sprintf(command,"Execute%d",setup_thread);
		KM->WriteLine(command);
	}
	//exit(0);

	CCoordMotion *CM = new CCoordMotion(KM);

	//CM->m_Simulate =true;


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
	strcpy(Interpreter->ToolFile,"");
	strcpy(Interpreter->SetupFile,"");

	Finished=false;

	Interpreter->SetUserCallback(UserCallback);  // Set this to handle (USR,Message) Callbacks

	Interpreter->SetUserMCodeCallback(UserMCodeCallback);  // Set this to handle MCode Callbacks

	// configure the Action for MCode 3 to do a Callback
	Interpreter->McodeActions[3].Action = M_Action_Callback;
	// configure the Action for MCode 105 to do a Callback
	Interpreter->McodeActions[MCODE_ACTIONS_M100_OFFSET+5].Action = M_Action_Callback;

	// configure the Action for MCode 4to execute a program in thread 3
	Interpreter->McodeActions[4].Action = M_Action_Program_wait_sync;
	Interpreter->McodeActions[4].dParams[0] = m4_thread;
	strcpy(Interpreter->McodeActions[4].String,m4_cfile);

	//CM->m_Simulate = true;
	// Execute the GCode!
	printf("Interpreting file %s\n", InFile);
	Interpreter->Interpret(BoardType,InFile,0,-1,true,StatusCallback,CompleteCallback);

	// Display Current Line Number while executing

	DisplayedLineNo=0;
	while (!Finished)
	{
		if (CurrentLineNo>DisplayedLineNo)
		{
			DisplayedLineNo=CurrentLineNo;
			//printf("Current Line = %d\n",CurrentLineNo);
		}
		sleep(1);
	}

	// Check Interpreter's exit code

	if (exitcode)
	{
		printf("Error in line %d\n",ErrorLineNo);
		printf("%s\n",ErrorMsg);
	}
	else
	{
		//printf("%s\n",GCodeOutput.c_str());
	}

	//getchar();
	return 0;
}

