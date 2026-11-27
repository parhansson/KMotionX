//
//  main.cpp
//  GCodeTest
//
//  Created by Par Hansson on 2012-04-20.
//  Copyright (c) 2012 P.Hansson. All rights reserved.
//

// #define BOOL int
// #define FALSE 0
#include "GCodeInterpreterX.h"
#include <limits.h>
#include <stdlib.h>

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
int ErrorLineNo, CurrentLineNo;
wchar_t ErrorMsg[512];
int exitcode;
bool Finished;
void CompleteCallback(int status, int line_no, int sequence_number, const wchar_t *err);
void StatusCallback(int line_no, const wchar_t *msg);
int UserCallback(const wchar_t *msg);
int UserMCodeCallback(int mcode);

void CompleteCallback(int status, int line_no, int sequence_number, const wchar_t *err)
{
	printf("Complete -- status: %d line: %d  error: %s\n", status, line_no, kmx::wstrtostr(err).c_str());
	ErrorLineNo = line_no;
	wcscpy_s(ErrorMsg, 512, err);
	exitcode = status;
	Finished = true;
}

void StatusCallback(int line_no, const wchar_t *msg)
{
	printf("Status -- line: %d message:\n%s\n", line_no, kmx::wstrtostr(msg).c_str());
	CurrentLineNo = line_no;
}

int UserCallback(const wchar_t *msg)
{
	MessageBoxW(0, msg, L"User callback", MB_OK);
	return 0;
}

int UserMCodeCallback(int mcode)
{
	char msg[100];
	double Var1000 = Interpreter->p_setup->parameters[1000];
	snprintf(msg, 100, "M%d Trigger GCode Var 1000 = %f\n", mcode, Var1000);
	MessageBox(0, msg, "M code callback", MB_OK);

	return 0;
}

int main(int argc, char *argv[])
{
	int DisplayedLineNo, BoardType;
	BoardType = BOARD_TYPE_KFLOP;
	char InFile[MAX_PATH];
	char m4_cfile[MAX_PATH];
	char setup_cfile[MAX_PATH];
	char tool_file[MAX_PATH];
	int setup_thread = 1;
	int m4_thread = 3;
	char command[MAX_LINE];

	char *rootDir;
	rootDir = getenv("PWD");

	char tmp_path[MAX_PATH];

	snprintf(tmp_path, MAX_PATH, "%s/../KMotionX/examples/ExecuteGCode/Stepper3Axis.c", rootDir);
	realpath(tmp_path, setup_cfile);

	snprintf(m4_cfile, MAX_PATH, "%s/c-programs/BlinkKFLOP.c", kmx::getMachineDataPath());

	snprintf(tool_file, MAX_PATH, "%s/data/Default.tbl", kmx::getMachineDataPath());

	// strcpy(setup_cfile,m4_cfile);
	snprintf(tmp_path, MAX_PATH, "%s/../GCode Programs/SimpleCircle.ngc", rootDir);
	realpath(tmp_path, InFile);

	CKMotionDLL *KM = new CKMotionDLL(0);

	bool simulate = false;

	if (!simulate)
	{

		wchar_t errMsg[MAX_LINE];
		errMsg[0] = 0;

		printf("Loading file %s to thread %d\n", setup_cfile, setup_thread);
		if (KM->CompileAndLoadCoff(kmx::strtowstr(setup_cfile).c_str(), setup_thread, errMsg, MAX_LINE))
		{
			printf("Loading file %s to thread %d failed.\n%s\nExiting.\n", setup_cfile, setup_thread, kmx::wstrtostr(errMsg).c_str());
			exit(1);
		}
		else
		{
			snprintf(command, MAX_LINE, "Execute%d", setup_thread);
			KM->WriteLine(command);
		}
	}
	// exit(0);

	CCoordMotion *CM = new CCoordMotion(KM);

	CM->m_Simulate = simulate;

	Interpreter = new CGCodeInterpreter(CM);

	MOTION_PARAMS *p = Interpreter->GetMotionParams();

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
	wchar_t wtool_file[MAX_PATH];
	wcscpy_s(wtool_file, MAX_PATH, kmx::strtowstr(tool_file));
	Interpreter->SetToolFile(wtool_file);
	Interpreter->SetupFile[0] = 0;

	Finished = false;

	Interpreter->SetUserCallback(UserCallback); // Set this to handle (USR,Message) Callbacks

	Interpreter->SetUserMCodeCallback(UserMCodeCallback); // Set this to handle MCode Callbacks

	// configure the Action for MCode 3 to do a Callback
	Interpreter->McodeActions[3].Action = M_Action_Callback;
	// configure the Action for MCode 105 to do a Callback
	Interpreter->McodeActions[MCODE_ACTIONS_M100_OFFSET + 5].Action = M_Action_Callback;

	// configure the Action for MCode 4to execute a program in thread 3
	Interpreter->McodeActions[4].Action = M_Action_Program_wait_sync;
	Interpreter->McodeActions[4].dParams[0] = m4_thread;
	wcscpy_s(Interpreter->McodeActions[4].String, 256, kmx::strtowstr(m4_cfile));

	// CM->m_Simulate = true;
	//  Execute the GCode!
	printf("Interpreting file %s\n", InFile);
	Interpreter->Interpret(BoardType, kmx::strtowstr(InFile).c_str(), 0, -1, true, StatusCallback, CompleteCallback);

	// Display Current Line Number while executing

	DisplayedLineNo = 0;
	while (!Finished)
	{
		if (CurrentLineNo > DisplayedLineNo)
		{
			DisplayedLineNo = CurrentLineNo;
			// printf("Current Line = %d\n",CurrentLineNo);
		}
		sleep(1);
	}

	// Check Interpreter's exit code

	if (exitcode)
	{
		printf("Error in line %d\n", ErrorLineNo);
		printf("%s\n", kmx::wstrtostr(ErrorMsg).c_str());
	}
	else
	{
		// printf("%s\n",GCodeOutput.c_str());
	}

	// getchar();
	return 0;
}
