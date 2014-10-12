//
//  main.cpp
//  GCodeTest
//
//  Created by Par Hansson on 2012-04-20.
//  Copyright (c) 2012 P.Hansson. All rights reserved.
//

//#define BOOL int
//#define FALSE 0
#include "../../GCodeInterpreter/StdAfx.h"



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
	printf("CALLBACK Complete -- status: %d line: %d  error: %s\n", status,line_no,err);
	ErrorLineNo=line_no;
	strcpy(ErrorMsg,err);
	exitcode=status;
	Finished=true;
}

void StatusCallback(int line_no, const char *msg)
{
	printf("CALLBACK Status -- line: %d message:\n%s\n", line_no,msg);
	CurrentLineNo=line_no;
}

int UserCallback(const char *msg)
{
//	MessageBox(NULL,msg,"SimpleGCode",MB_OK);
    printf("CALLBACK User -- message: \n%s\n",msg);
	return 0;
}

int UserMCodeCallback(int mcode)
{
	double Var1000 = Interpreter->p_setup->parameters[1000];
	printf("CALLBACK User MCode -- %d Trigger GCode Var 1000 = %f\n",mcode,Var1000);
    //MessageBox(NULL,s,"SimpleGCode",MB_OK);

	return 0;
}


int main(int argc, char* argv[])
{
	printf("Main\n");
	int DisplayedLineNo,BoardType;
	BoardType = BOARD_TYPE_KFLOP;
    //std::string InFile="/Users/parhansson/git/kflop/KMotionX/GCode Programs/SimpleCircle.ngc";

    char* rootDir;
	rootDir = getenv("PWD");
	char InFile[256];
	//printf("%s:%d Environment KMOTION_ROOT not set, falling back to %s\n",__FILE__,__LINE__,rootDir);
	sprintf(InFile,"%s/../GCode Programs/SimpleCircle.ngc",rootDir);
	printf("Interpreting file %s\n", InFile);
	CKMotionDLL *KM = new CKMotionDLL(0);

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

	//if (Interpreter.CoordMotion.KMotionDLL.CheckKMotionVersion(board, &BoardType)) return 1;


	Interpreter->SetUserCallback(UserCallback);  // Set this to handle (USR,Message) Callbacks
	// configure the Action for MCode 3 to do a Callback
	Interpreter->McodeActions[3].Action = M_Action_Callback;
	// configure the Action for MCode 105 to do a Callback
	Interpreter->McodeActions[MCODE_ACTIONS_M100_OFFSET+5].Action = M_Action_Callback;

	// configure the Action for MCode 4to execute a program in thread 3
	Interpreter->McodeActions[4].Action = M_Action_Program_wait_sync;
	Interpreter->McodeActions[4].dParams[0] = 3;
	char cfile[256];
	sprintf(cfile,"%s/../C Programs/BlinkKFLOP.c",rootDir);
	strcpy(Interpreter->McodeActions[4].String,cfile);

	//CM->m_Simulate = true;
	Interpreter->SetUserMCodeCallback(UserMCodeCallback);  // Set this to handle MCode Callbacks
	// Execute the GCode!

	printf("Interpret gcode file.\n");
	Interpreter->Interpret(BoardType,InFile,0,-1,true,StatusCallback,CompleteCallback);

	// Display Current Line Number while executing

	DisplayedLineNo=0;
	while (!Finished)
	{
		if (CurrentLineNo>DisplayedLineNo)
		{
			DisplayedLineNo=CurrentLineNo;
			printf("Current Line = %d\n",CurrentLineNo);
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

