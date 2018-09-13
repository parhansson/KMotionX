// GCodeInterpreter.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "rs274ngc_return.h"
#include "rs274ngc.h"
#include "GCodeInterpreter.h"
#include "HiResTimer.h"

#ifdef _KMOTIONX
void * DoInvokeShell(void * lpdwParam);
#else
DWORD DoInvokeShell(LPDWORD lpdwParam);
#endif

CGCodeInterpreter::CGCodeInterpreter(CCoordMotion *CoordM)
{ 
	CM=CoordMotion=CoordM;   // set global variable for the low level Interpreter
	GC=this;

	ToolFile[0] = 0;
	SetupFile[0] = 0;
	VarsFile[0] = 0;

	m_CurrentLine=0;

	p_setup = &_setup;
	m_Resume=false;
	CoordMotion->m_realtime_Sequence_number_valid=ExecutionInProgress=false;

	for (int i=0; i<MAX_MCODE_ACTIONS; i++)
	{
		McodeActions[i].Action=0;
		
		for (int k=0; k<MAX_MCODE_DOUBLE_PARAMS; k++)
			McodeActions[i].dParams[k]=0;
		
		McodeActions[i].String[0]=0;
	}

	p_setup->length_units=CANON_UNITS_INCHES;
	p_setup->origin_index=1;
	p_setup->tool_table_index=1;

	m_InitializeOnExecute = true; // make sure we initialize at least once
	m_ReadToolFile = true; // make sure we read the tool file at least once

	m_UserFn = NULL;
	m_UserFnMCode = NULL;
	m_ScreenScriptCallback = NULL;

	m_InvokeThreadID = m_InterpretThreadID = -1;
}

CGCodeInterpreter::~CGCodeInterpreter()
{
}


int CGCodeInterpreter::Interpret(
			  int board_type,
		      const char *fname,
			  int start, int end,
			  int restart,
              G_STATUS_CALLBACK *StatusFn,
			  G_COMPLETE_CALLBACK *CompleteFn)
{
	CoordMotion->m_board_type=board_type;
	CoordMotion->ClearAbort();
	CoordMotion->m_AxisDisabled=false;
	CoordMotion->RapidParamsDirty=true;

	m_CurrentLine=m_start=start;
	m_end=end;
	m_restart=restart;
	m_CompleteFn=CompleteFn;
	m_StatusFn=StatusFn;
	strcpy(m_InFile,fname);
	m_Halt=m_HaltNextLine=false;
	CoordMotion->ClearHalt();

	LaunchExecution();	
	
	return 0;
}

// returns a pointer to the GCode Parameters

MOTION_PARAMS *CGCodeInterpreter::GetMotionParams()
{
	return &CoordMotion->Kinematics->m_MotionParams;
}

#ifdef _KMOTIONX
void * DoExecuteShell(void *lpdwParam)
{
	CGCodeInterpreter *p=(CGCodeInterpreter*)lpdwParam;

	p->m_InterpretThreadID = kmx::getThreadId("GCodeInterpreter::DoExecuteShell");

	p->m_exitcode = p->DoExecute();

	p->DoExecuteComplete();

	p->m_InterpretThreadID = -1;

	pthread_exit(0);
	return 0;
}
#else
DWORD DoExecuteShell(LPDWORD lpdwParam)
{
	CGCodeInterpreter *p=(CGCodeInterpreter*)lpdwParam;

	p->m_InterpretThreadID = GetCurrentThreadId();

	p->m_exitcode = p->DoExecute();

	p->DoExecuteComplete();

	p->m_InterpretThreadID = -1;

	return 0;
}
#endif


void CGCodeInterpreter::Halt()
{
	m_Halt=true;
	CoordMotion->SetHalt();
}

bool CGCodeInterpreter::GetHalt()
{
	return m_Halt;
}

void CGCodeInterpreter::HaltNextLine()
{
	m_HaltNextLine=true;
}

bool CGCodeInterpreter::GetHaltNextLine()
{
	return m_HaltNextLine;
}

void CGCodeInterpreter::Abort()
{
	CoordMotion->SetAbort();
}

bool CGCodeInterpreter::GetAbort()
{
	return CoordMotion->GetAbort();
}

void CGCodeInterpreter::SetToolFile(char *f)
{
	strncpy(ToolFile,f,MAX_PATH);
}

void CGCodeInterpreter::SetSetupFile(char *f)
{
	strncpy(SetupFile,f,MAX_PATH);
}

void CGCodeInterpreter::SetVarsFile(char *f)
{
	strncpy(VarsFile,f,MAX_PATH);
}

int CGCodeInterpreter::rs274ErrorExit(int status)
{
	char ErrDescr[200];
	ErrDescr[0]='\0';
	rs274ngc_close();

	if (CoordMotion->GetAbort())
	{
		if (CoordMotion->m_AxisDisabled)
		{
		  strcpy(ErrDescr,"Axis Disabled - GCode Aborted");
			status=1000;
		}
		else
		{
			strcpy(ErrDescr,"GCode Aborted");
			status=1001;
		}
	}
	else
	{
		rs274ngc_error_text(status,ErrDescr,200);
	}

	strcat(ErrorOutput,ErrDescr);

	return status;
}


int CGCodeInterpreter::ReadSetupFile()
{
	int status = RS274NGC_OK;
	
	if (SetupFile[0]!=0)
	{
		status = read_setup_file(SetupFile, &_setup);
	}
	
	return status;
}


int CGCodeInterpreter::InitializeInterp(void)
{
	int status;

	Output[0]='\0';
	line_number=0;

	// initialize the trajectory planner
	tp_init();

	CoordMotion->SetTPParams();

	CoordMotion->DownloadInit();  // intialize download/look ahead variables  
	
	status = rs274ngc_init();
	if (status != RS274NGC_OK)	return rs274ErrorExit(status);

	
	if (ToolFile[0]!=0)
	{
		status = read_tool_file(ToolFile, &_setup);
		if (status != RS274NGC_OK)	return rs274ErrorExit(status);
	}

	if (SetupFile[0]!=0)
	{
		status = read_setup_file(SetupFile, &_setup);
		if (status != RS274NGC_OK)	return rs274ErrorExit(status);
	}
	CoordMotion->m_PreviouslyStopped = STOPPED_NONE;
	m_InitializeOnExecute = false;
	return 0;
}




int CGCodeInterpreter::DoExecute()
{
	int status;
	char trash[INTERP_TEXT_SIZE];
	char * read_ok;
	int program_status;
	//CString OutputCRLF;

	ErrorOutput[0]='\0';

	program_status = RS274NGC_OK;

//  This doesn't work yet and results in a memory leak
//  so leave this static for now
//	int * device = new int; 
//	*device=(int)this;  
//  SET_CANON_DEVICE(device);

	if (DoResumeSafe()) return 1;  // check for re-position motions

	// initialize everything if we are starting at the beginning

	if (m_restart || m_InitializeOnExecute)
	{
		status = InitializeInterp();
		if (status) return status;
	}

	CoordMotion->m_Stopping = STOPPED_NONE;

// always read the tool file now (some users set it externally)
//	if (m_restart || m_ReadToolFile)
	{
		if (ToolFile[0]!=0)
		{
			status = read_tool_file(ToolFile, &_setup);
			if (status != RS274NGC_OK)	return rs274ErrorExit(status);
		}
		m_ReadToolFile = false;
	}
	

	if (!CoordMotion->m_Simulate)
	{
		int result;

		// find out which axis is which
		// force refresh and save results

		CoordMotion->m_DefineCS_valid = false;

		if  (CoordMotion->m_PreviouslyStopped)
			result = CoordMotion->ReadCurAbsPosition(&CoordMotion->current_x,&CoordMotion->current_y,&CoordMotion->current_z,
													&CoordMotion->current_a,&CoordMotion->current_b,&CoordMotion->current_c,&CoordMotion->current_u,&CoordMotion->current_v,true);
		else
			result = ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,&_setup.AA_current,&_setup.BB_current,&_setup.CC_current,&_setup.UU_current,&_setup.VV_current);

		if (result == 1)
		{
			if (CoordMotion->m_AxisDisabled)
			  strcpy(ErrorOutput,"Unable to read defined coordinate system axis positions - Axis Disabled ");
			else
			  strcpy(ErrorOutput,"Unable to read defined coordinate system axis positions ");
		}

		if (result != 0) return 1005;
	}

	status = rs274ngc_open(m_InFile);
	if (status != RS274NGC_OK)	return rs274ErrorExit(status);

	if (_setup.percent_flag == ON)
		m_GCodeReads = 1;
	else
		m_GCodeReads = 0;
			
	for( ; m_GCodeReads<m_CurrentLine ; m_GCodeReads++)
	{
		read_ok = fgets(trash, INTERP_TEXT_SIZE,_setup.file_pointer);
		if (!read_ok) 
		{
		  strcpy(ErrorOutput,"Error while reading GCode file ");
			return NCE_A_FILE_IS_ALREADY_OPEN;
		}
	}

	CoordMotion->m_realtime_Sequence_number_valid=false;
	ExecutionInProgress=true;

	SetupTracker.ClearHistory();
	
	_setup.current_line=m_GCodeReads;

	if (m_end == -1) m_end = -99;  // -1 is valid previous line when looping back to line 0

	for(  ; ; m_GCodeReads++)
	{
		// record what line we are doing in the GCode

		_setup.current_line = m_CurrentLine = m_GCodeReads;

		StateSaved=false;  // remember we should save the state at some point
		
		// give output to caller
#ifndef _KMOTIONX
		//not needed on linux
		CString tmpStr = Output;
		tmpStr.Replace("\n","\r\n");
		strcpy(Output, tmpStr);
#endif
		m_StatusFn(m_CurrentLine,Output);
		
		Output[0]='\0';  // clear it


		if (((m_end!=-99)&&(m_CurrentLine>m_end)) || (CoordMotion->m_Simulate && m_Halt) || CoordMotion->GetAbort() || m_HaltNextLine)
		{
			rs274ngc_close();
			return 0;
		}

		status = rs274ngc_read();
		if (status == RS274NGC_ENDFILE)
		{
			rs274ngc_close();
			return RS274NGC_ENDFILE;
		}

		if (status != RS274NGC_OK)	return rs274ErrorExit(status);

		status = rs274ngc_execute(NULL);

		SaveStateOnceOnly();  // if it wasn't saved before first motion then save it now

		if (m_CurrentLine != _setup.current_line)
		{
			// line number changed

			if (m_end!=-99)
				m_end += _setup.current_line - m_CurrentLine;
		
			m_GCodeReads = m_CurrentLine = _setup.current_line;  // get back line no (in case we called a subroutine)
		}

		if (CoordMotion->GetAbort()) return rs274ErrorExit(status);
		
		if (status == RS274NGC_EXIT)
		{
			rs274ngc_close();
			_setup.current_line = m_CurrentLine = 0;  // reset back to the beginning
			return program_status;
		}
	
		if (status != RS274NGC_OK)	return rs274ErrorExit(status);

		// successfully executed one line clear stopped status
		if (!CoordMotion->GetAbort())
			CoordMotion->m_PreviouslyStopped = STOPPED_NONE;

	}

	return 0;
}

// Save the Interpreter State once per line just before first motion is created
void CGCodeInterpreter::SaveStateOnceOnly()
{
	if (!StateSaved)
	{
		StateSaved=true;
		SetupTracker.InsertState(&_setup);  // save the interpreter state
	}
}

int CGCodeInterpreter::DoExecuteComplete()
{
	if (!CoordMotion->GetAbort() && !CoordMotion->GetHalt() && CoordMotion->ExecutionStop())
	{
		// check again now that motion stopped if no Abort or Halt must have been an error
		if (!CoordMotion->GetAbort() && !CoordMotion->GetHalt())
		{
			if (CoordMotion->m_AxisDisabled)
				CoordMotion->KMotionDLL->DoErrMsg("Error Executing GCode\r\rAxis Disabled");
			else
				CoordMotion->KMotionDLL->DoErrMsg("Error Executing GCode");

			m_exitcode = 1005;
		}
	}

	if (CoordMotion->m_Stopping != STOPPED_NONE)
	{
		// if we were previously stopped then restore the state
		if (CoordMotion->m_PreviouslyStopped)
		{
			SetupTracker.RestoreState(CoordMotion->m_PreviouslyStoppedSeqNo,&_setup);
			SET_FEED_RATE(_setup.feed_rate);
			m_StoppedInterpState = _setup;

			// convert stopped absolute coordinates to GCode interpreter coordinates

			ConvertAbsoluteToInterpreterCoord(CoordMotion->m_StoppedMachinex, CoordMotion->m_StoppedMachiney, CoordMotion->m_StoppedMachinez, 
				CoordMotion->m_StoppedMachinea, CoordMotion->m_StoppedMachineb, CoordMotion->m_StoppedMachinec, CoordMotion->m_StoppedMachineu, CoordMotion->m_StoppedMachinev,
				&CoordMotion->m_Stoppedx, &CoordMotion->m_Stoppedy, &CoordMotion->m_Stoppedz, 
				&CoordMotion->m_Stoppeda, &CoordMotion->m_Stoppedb, &CoordMotion->m_Stoppedc, &CoordMotion->m_Stoppedu, &CoordMotion->m_Stoppedv);

			_setup.file_pointer=NULL;
		}

		m_exitcode=1005;
	}
	
	m_CompleteFn(m_exitcode,_setup.current_line,_setup.sequence_number,ErrorOutput);

	ExecutionInProgress=false;
	return 0;
}

int CGCodeInterpreter::LaunchExecution()
{
#ifdef _KMOTIONX
	//printf("%s:%d LaunchExecution\n",__FILE__,__LINE__);
    pthread_t thr;
    
    if(pthread_create(&thr, NULL, &::DoExecuteShell, this))
    {
        printf("Could not create thread\n");
        return -1;
    }
    
    // We don't use pthread_join(), so allow system to clean up resources.
    pthread_detach(thr);
#else
	DWORD ID;
	

	// Now start a worker thread that performs the G Code
	
	HANDLE Thread=CreateThread( 
		NULL,                        /* no security attributes        */ 
		0,                           /* use default stack size        */ 
		(LPTHREAD_START_ROUTINE) ::DoExecuteShell, /* thread function       */ 
		this,							 /* argument to thread function   */ 
		0,                           /* use default creation flags    */ 
		&ID);   
#endif

	return 0;
}

	

volatile bool InvokeInterpComplete;

void InvokeCompleteCallback(int status, int lineno, int sequence_number, const char *err)
{
	if (status)
	{
		CM->KMotionDLL->DoErrMsg(err);
	}
	InvokeInterpComplete=true;
}

void InvokeStatusCallback(int line_no, const char *msg)
{
}


	

// invoke an MCode action to Set A Bit, Dac, execute a program, etc...

int CGCodeInterpreter::InvokeAction(int i, BOOL FlushBeforeUnbufferedOperation)
{
	MCODE_ACTION *p;

	if (i<MAX_MCODE_ACTIONS_M1 + MAX_MCODE_ACTIONS_BUTTONS)
		p = &McodeActions[i];
	else if (i >= 100)
		p = &McodeActions[i - 100 + MCODE_ACTIONS_M100_OFFSET];
	else
		p = &McodeActions[i - 24 + MCODE_ACTIONS_SPECIAL_OFFSET];

	return InvokeAction(i, FlushBeforeUnbufferedOperation, p);
}


typedef struct
{
	int i;
	BOOL FlushBeforeUnbufferedOperation;
	MCODE_ACTION p;
	CGCodeInterpreter *GC;
} INVOKE_PARAMS;


int CGCodeInterpreter::InvokeAction(int i, BOOL FlushBeforeUnbufferedOperation, MCODE_ACTION *p)
{
	// check if action was from a GUI Button (no Flush required)
	// if so create a Worker Thread to perform the Action

	if (FlushBeforeUnbufferedOperation)
	{
		//no just call it directly
		return InvokeActionDirect(i, FlushBeforeUnbufferedOperation, p);
	}
	else
	{
		// create a Worker Thread to do it

		// wait if a previous action is still in progress
		CHiResTimer Timer;
		Timer.Start();
		while (m_InvokeThreadID != -1)
		{
			Sleep(10);

			if (CoordMotion->GetAbort()) return 1;

			if (Timer.Elapsed_Seconds() > 10.0)
			{
				CoordMotion->KMotionDLL->DoErrMsg("Timeout waiting on Action to complete");
				return 1;
			}
		}

		// Now start a worker thread that performs the Action


		INVOKE_PARAMS *params = new INVOKE_PARAMS;

		params->i = i;
		params->FlushBeforeUnbufferedOperation = FlushBeforeUnbufferedOperation;
		params->p = *p;
		params->GC = this;
		
#ifdef _KMOTIONX
    pthread_t thr;
    params->GC->m_InvokeThreadID++;
    if(pthread_create(&thr, NULL, &::DoInvokeShell, params))
    {
				params->GC->m_InvokeThreadID--;
				delete params;
        printf("Could not create thread\n");
        return -1;
    }
    
    // We don't use pthread_join(), so allow system to clean up resources.
    pthread_detach(thr);
#else

		HANDLE Thread = CreateThread(
			NULL,                        /* no security attributes        */
			0,                           /* use default stack size        */
			(LPTHREAD_START_ROUTINE) ::DoInvokeShell, /* thread function       */
			params,						 /* argument to thread function   */
			0,                           /* use default creation flags    */
			&params->GC->m_InvokeThreadID);
#endif
		return 0;
	}
}

#ifdef _KMOTIONX
void * DoInvokeShell(void * lpdwParam)
{
	INVOKE_PARAMS *params = (INVOKE_PARAMS*)lpdwParam;
	params->GC->m_InvokeExitcode = params->GC->InvokeActionDirect(params->i, params->FlushBeforeUnbufferedOperation, &params->p);
	//TODO mutex lock before changing m_InvokeThreadID
	params->GC->m_InvokeThreadID--;
	delete params;
	pthread_exit(0);
	return 0;
}
#else
DWORD DoInvokeShell(LPDWORD lpdwParam)
{
	INVOKE_PARAMS *params = (INVOKE_PARAMS*)lpdwParam;

	params->GC->m_InvokeExitcode = params->GC->InvokeActionDirect(params->i, params->FlushBeforeUnbufferedOperation, &params->p);

	params->GC->m_InvokeThreadID = -1;

	delete params;

	return 0;
}
#endif



int CGCodeInterpreter::InvokeActionDirect(int i, BOOL FlushBeforeUnbufferedOperation, MCODE_ACTION *p)
{
	char s[MAX_LINE]; char s0[32]; char e[MAX_PATH];
	double value;
	int ivalue,ipersist,result;
	bool MCode;

	if (i == -1)  // determine if MCode in GCode or from a button
		MCode = false;
	else if (i < MAX_MCODE_ACTIONS_M1)
		MCode = true;
	else if (i<MAX_MCODE_ACTIONS_M1 + MAX_MCODE_ACTIONS_BUTTONS)
		MCode = false;  // User Button
	else if (i >= 100)
		MCode = true;
	else
		MCode = false;  // Specials



	if (FlushBeforeUnbufferedOperation && CoordMotion->m_Simulate) return 0;

	// If we were called from a button and we had been previously aborted then clear the Abort and any Halt
	if (!FlushBeforeUnbufferedOperation && CoordMotion->GetAbort())
	{
		CoordMotion->ClearAbort();
		CoordMotion->ClearHalt();
	}
	

	switch (p->Action)
	{
	case M_Action_None:
		break;

	case M_Action_Setbit:
		if (FlushBeforeUnbufferedOperation)  // false for User button
		{
			sprintf(s, "SetStateBitBuf%d=%d",(int)p->dParams[0],(int)p->dParams[1]);
			if (CoordMotion->DoKMotionBufCmd(s,p_setup->sequence_number)) return 1;
		}
		else
		{
			sprintf(s, "SetStateBit%d=%d",(int)p->dParams[0],(int)p->dParams[1]);
			if (CoordMotion->DoKMotionCmd(s,FlushBeforeUnbufferedOperation)) return 1;
		}
		break;

	case M_Action_Waitbit:
		if (FlushBeforeUnbufferedOperation)  // false for User button (doesn't make sense for button)
		{
			if (p->dParams[1]==0)
				sprintf(s, "WaitNotBitBuf%d",(int)p->dParams[0]);
			else
				sprintf(s, "WaitBitBuf%d",(int)p->dParams[0]);

			if (CoordMotion->DoKMotionBufCmd(s)) return 1;
		}
		break;

	case M_Action_SetTwoBits:
		if (FlushBeforeUnbufferedOperation)  // false for User button
		{
			sprintf(s, "SetStateBitBuf%d=%d",(int)p->dParams[0],(int)p->dParams[1]);
			if (CoordMotion->DoKMotionBufCmd(s,p_setup->sequence_number)) return 1;
			sprintf(s, "SetStateBitBuf%d=%d",(int)p->dParams[2],(int)p->dParams[3]);
			if (CoordMotion->DoKMotionBufCmd(s,p_setup->sequence_number)) return 1;
		}
		else
		{
			sprintf(s, "SetStateBit%d=%d",(int)p->dParams[0],(int)p->dParams[1]);
			if (CoordMotion->DoKMotionCmd(s,FlushBeforeUnbufferedOperation)) return 1;
			sprintf(s, "SetStateBit%d=%d",(int)p->dParams[2],(int)p->dParams[3]);
			if (CoordMotion->DoKMotionCmd(s,FlushBeforeUnbufferedOperation)) return 1;
		}
		break;

	case M_Action_DAC:
		value = p_setup->speed * CoordMotion->GetSpindleRateOverride() * p->dParams[1] + p->dParams[2];   // scale and offset
		ivalue = (int)floor(value+0.5); 
		if (ivalue < (int)p->dParams[3]) ivalue = (int)p->dParams[3];
		if (ivalue > (int)p->dParams[4]) ivalue = (int)p->dParams[4];
		sprintf(s, "DAC%d=%d",(int)p->dParams[0],ivalue);
		if (CoordMotion->DoKMotionCmd(s,FlushBeforeUnbufferedOperation)) return 1;
		break;

	case M_Action_Program:
	case M_Action_Program_wait:
	case M_Action_Program_wait_sync:
		if (FlushBeforeUnbufferedOperation)
		{
			if (CoordMotion->FlushSegments()) {CoordMotion->SetAbort(); return 1;}  
			if (CoordMotion->WaitForSegmentsFinished()) {CoordMotion->SetAbort(); return 1;}
		}

		e[0] = '\0';
		if(strlen(p->String) >= 4){
			strcpy(e,p->String+ strlen(p->String) -4);
			_strlwr(e);
		}
		if(e[0] != '\0' && strcmp(e,".ngc")==0)
		{
			if (_setup.file_pointer!= NULL)
			{
				// A GCode file is currently running
				// don't allow the Interpreter to re-ener
				CoordMotion->KMotionDLL->DoErrMsg("Running a GCode file from a GCode file is not allowed.  Check if an MCode is assigned to a Gcode File and is also called from a GCode File");
				CoordMotion->SetAbort(); 
				return 1;
			}

			int BoardType=BOARD_TYPE_UNKNOWN;
			if (CoordMotion->m_Simulate==0)
				if (CoordMotion->KMotionDLL->CheckKMotionVersion(&BoardType)) return 1;

			InvokeInterpComplete=false;

			Interpret(BoardType,p->String,0,-1,false,InvokeStatusCallback,InvokeCompleteCallback);

			do
			{
				Sleep(10);
				if (CoordMotion->GetAbort()) return 1;
			}
			while (!InvokeInterpComplete);
		}
		else
		{
			ipersist = (int)p->dParams[1];
	
			// set a persist variable to either the Mcode value
			// or the related variable (normally speed)
			// if variable is -1 then don't set anything
	
			if (p->dParams[1] >= 0)
			{
				if (i==6)  // tool change
				{
					sprintf(s, "SetPersistHex %d %x",ipersist, p_setup->tool_table[p_setup->selected_tool_slot].slot);
					if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
					sprintf(s, "SetPersistHex %d %x",ipersist+1, p_setup->tool_table[p_setup->selected_tool_slot].id);
					if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
				}
				else if (i==10)  // set speed
				{
					float fspeed = (float)(p_setup->speed * CoordMotion->GetSpindleRateOverride());

					// if in CSS mode then S is not in RPM but rather in feet/min or meters/min
					// convert to standard units of inches/sec and download to KFLOP  
					if (p_setup->spindle_mode==CANON_SPINDLE_CSS)
					{
						if (p_setup->length_units == CANON_UNITS_MM)
							fspeed /= 60.0f * 0.0254f;
						else
							fspeed *= 12.0f/60.0f;
					}
					sprintf(s, "SetPersistHex %d %x",ipersist, *(int *)&fspeed);
					if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
				}
				else
				{
					int count=0;
					// for other M Codes check if P Q or R words are present on the same line
					// if so, download them into successive persist vars as floats
	
					if (MCode && p_setup->block1.p_flag)
					{
						float p = (float)p_setup->block1.p_number;
						sprintf(s, "SetPersistHex %d %x",ipersist, *(int *)&p);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
						ipersist++;
						count++;
					}
	
					if (MCode && p_setup->block1.q_flag)
					{
						float q = (float)p_setup->block1.q_number;
						sprintf(s, "SetPersistHex %d %x",ipersist, *(int *)&q);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
						ipersist++;
						count++;
					}
	
					if (MCode && p_setup->block1.r_flag)
					{
						float r = (float)p_setup->block1.r_number;
						sprintf(s, "SetPersistHex %d %x",ipersist, *(int *)&r);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
						ipersist++;
						count++;
					}
	
					if (count==0)  // if no parameters just set the MCode number
					{
						sprintf(s, "SetPersistHex %d %x",ipersist,i);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
					}
				}
			}
	
			// If a C File is specified then Compile and load it
	
			if(e[0] != '\0' && strcmp(e,".out")==0)
			{
#ifdef _KMOTIONX
				char FileName[MAX_PATH];
				strcpy(FileName, p->String);
				if(strchr(FileName,PATH_SEPARATOR) - FileName > -1){
					sprintf(FileName,"%s%s%s", CoordMotion->MainPathRoot, C_PROGRAMS_DIR,  p->String);
				}
#else
				CString FileName = p->String; 
				if (FileName.Find("\\\\") == -1 && FileName.Find(':') == -1)  // if no Path in File name add in the default path
				{
					FileName = CoordMotion->MainPathRoot + ((CString)C_PROGRAMS_DIR) + FileName;
				}
#endif

				if (CoordMotion->KMotionDLL->LoadCoff((int)p->dParams[0], FileName))
				{
					char message[1024];
					sprintf(message,"Error Loading KMotion Coff Program\r\r%s\r\r", FileName);
					CoordMotion->KMotionDLL->DoErrMsg(message);
					return 1;
				}
			}
			else if (p->String[0])
			{
				char Err[500];

#ifdef _KMOTIONX
				char FileName[MAX_PATH];
				strcpy(FileName, p->String);
				if(strchr(FileName,PATH_SEPARATOR) == NULL){
					sprintf(FileName,"%s%s%s", CoordMotion->MainPathRoot, C_PROGRAMS_DIR,  p->String);
				}
#else
				CString FileName = p->String; 
				if (FileName.Find("\\\\") == -1 && FileName.Find(':') == -1)  // if no Path in File name add in the default path
				{
					FileName = CoordMotion->MainPathRoot + ((CString)C_PROGRAMS_DIR) + FileName;
				}
#endif
				if (CoordMotion->KMotionDLL->CompileAndLoadCoff(FileName, (int)p->dParams[0], Err, 499))
				{
					char message[1024];
					sprintf(message,"Error Compiling and Loading KMotion Program\r\r%s\r\r%s", FileName, Err );
					CoordMotion->KMotionDLL->DoErrMsg(message);
					return 1;
				}
			}
	
			// Now execute the thread!
	
			sprintf(s, "Execute%d",(int)p->dParams[0]);
			if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
	
			if (p->Action == M_Action_Program_wait || p->Action == M_Action_Program_wait_sync)
			{
				char response[MAX_LINE];
	
				int count=0;
	
				sprintf(s, "CheckThread%d",(int)p->dParams[0]);
				do
				{
					if (count++) Sleep(10);
					
					if (CoordMotion->KMotionDLL->WriteLineReadLine(s,response))
					{
						CoordMotion->SetAbort(); 
						return 1;
					}
	
					if (CoordMotion->GetAbort()) return 1;

					// if not called from a button check for Halt
					if (FlushBeforeUnbufferedOperation && CoordMotion->CheckMotionHalt(true)) return 2;
				}
				while (strcmp(response,"0")!=0);
			}
	
			if (p->Action == M_Action_Program_wait_sync)
			{
				// don't sample positions until everything is stopped 
				if (CoordMotion->WaitForSegmentsFinished()) return 1;
				if (CoordMotion->WaitForMoveXYZABCFinished()) return 1;
	
				if (!CoordMotion->m_Simulate && ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,
					      &_setup.AA_current, &_setup.BB_current, &_setup.CC_current, &_setup.UU_current, &_setup.VV_current))
				{
					if (CoordMotion->m_AxisDisabled)
					  strcpy(ErrorOutput,"Unable to read defined coordinate system axis positions - Axis Disabled ");
					else
					  strcpy(ErrorOutput,"Unable to read defined coordinate system axis positions ");
	
					return 1;
				}
			}
		}
		CoordMotion->RapidParamsDirty=true;
		break;

	case M_Action_Callback:
		if (GC->m_UserFnMCode)
		{
			if (CoordMotion->FlushSegments()) {CoordMotion->SetAbort(); return 1;}  
			if (CoordMotion->WaitForSegmentsFinished(TRUE)) {CoordMotion->SetAbort(); return 1;}
			
			// call back to the User Application and pass the MCode that caused the callback
			if (GC->m_UserFnMCode(i)){CoordMotion->SetAbort(); return 1;}

			// don't sample positions until everything is stopped 
			if (CoordMotion->WaitForSegmentsFinished()) return 1;
			if (CoordMotion->WaitForMoveXYZABCFinished()) return 1;

			if (!CoordMotion->m_Simulate && ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,
							&_setup.AA_current,&_setup.BB_current,&_setup.CC_current,&_setup.UU_current,&_setup.VV_current))
				return 1;
		}
		break;

	case M_Action_Program_PC:
		// If running from GCode finish any motion first
		if (FlushBeforeUnbufferedOperation)
		{
			if (CoordMotion->FlushSegments()) {CoordMotion->SetAbort(); return 1;}  
			if (CoordMotion->WaitForSegmentsFinished(TRUE)) {CoordMotion->SetAbort(); return 1;}
		}

		// we will be executing a PC Program pass any related parameters
		s[0]='\0'; //s="";
		if (i==6)  // tool change
		{
			sprintf(s, " %d",p_setup->selected_tool_slot);
		}
		else if (i==10)  // set speed
		{
			float fspeed = (float)(p_setup->speed * CoordMotion->GetSpindleRateOverride());
			sprintf(s, " %f",fspeed);
		}
		else
		{
			// for other M Codes check if P Q or R words are present on the same line
			// if so, download them into successive persist vars as floats

			if (p_setup->block1.p_flag)
			{
				sprintf(s0, " %f", p_setup->block1.p_number);
				strcat(s,s0);
			}

			if (p_setup->block1.q_flag)
			{
				sprintf(s0, " %f", p_setup->block1.q_number);
				strcat(s,s0);
			}

			if (p_setup->block1.r_flag)
			{
				sprintf(s0, " %f", p_setup->block1.r_number);
				strcat(s,s0);
			}
		}
		char pcCmd[MAX_LINE];
		sprintf(pcCmd,"%s%s",p->String,s);
		result = ExecutePC(pcCmd);  // call the executable with parameters
		if (result)
		{
			char Err[350];

			sprintf(Err,"Error Executing PC Program:\r\r%s\r\r"
				"Return code = %d\r\rAbort?",p->String,result);

			if (AfxMessageBox(Err,MB_YESNO)==IDYES) Abort();
		};
		break;

		case M_Action_ScreenScript:
		if (m_ScreenScriptCallback)
			if (m_ScreenScriptCallback(p->String)) return 1;
		break;
	}

	// if not called from a button check for Halt
	if (FlushBeforeUnbufferedOperation && CoordMotion->CheckMotionHalt(true)) return 2;

	return 0;
}


int CGCodeInterpreter::ExecutePC(const char *Name)
{
#ifdef _KMOTIONX
	int exitcode;
	//TODO implement timeout
	exitcode = system(Name);
#else
	SECURITY_ATTRIBUTES sa          = {0};
	STARTUPINFO         si          = {0};
	PROCESS_INFORMATION pi          = {0};
	HANDLE              hPipeOutputRead  = NULL;
	HANDLE              hPipeOutputWrite = NULL;
	HANDLE              hPipeInputRead   = NULL;
	HANDLE              hPipeInputWrite  = NULL;
	BOOL                bTest = 0;
	DWORD               dwNumberOfBytesRead = 0;

	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	
	// Create pipe for standard output redirection.
	CreatePipe(&hPipeOutputRead,  // read handle
		&hPipeOutputWrite, // write handle
		&sa,      // security attributes
		0      // number of bytes reserved for pipe - 0 default
		);
	
	// Create pipe for standard input redirection.
	CreatePipe(&hPipeInputRead,  // read handle
		&hPipeInputWrite, // write handle
		&sa,      // security attributes
		0      // number of bytes reserved for pipe - 0 default
		);
	
	// Make child process use hPipeOutputWrite as standard out,
	// and make sure it does not show on screen.
	si.cb = sizeof(si);
	si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput   = hPipeInputRead;
	si.hStdOutput  = hPipeOutputWrite;
	si.hStdError   = hPipeOutputWrite;

	CString cmd=Name;  // build command line
	
	CreateProcess (
		NULL,
		cmd.GetBuffer(0), 
		NULL, NULL,
		TRUE, 0,
		NULL, NULL,
		&si, &pi);

	
	// Now that handles have been inherited, close it to be safe.
	// You don't want to read or write to them accidentally.
	CloseHandle(hPipeOutputWrite);
	CloseHandle(hPipeInputRead);
	
	
	// Wait for CONSPAWN to finish.
	int result;
	do
	{
		if (CoordMotion->GetAbort()) return 1;
		result = WaitForSingleObject(pi.hProcess, 1000);
	}
	while (result == WAIT_TIMEOUT);

	DWORD exitcode;
	result = GetExitCodeProcess(pi.hProcess,&exitcode);
	
	// Now test to capture DOS application output by reading
	// hPipeOutputRead.  Could also write to DOS application
	// standard input by writing to hPipeInputWrite.

	
	// Close all remaining handles
	CloseHandle (pi.hProcess);
    CloseHandle (pi.hThread);
	CloseHandle (hPipeOutputRead);
	CloseHandle (hPipeInputWrite);
#endif
	return exitcode;
}



double CGCodeInterpreter::ConvertAbsToUserUnitsX(double x)
{
	return InchesToUserUnitsX(x - p_setup->axis_offset_x  - p_setup->origin_offset_x - p_setup->tool_xoffset);
}

double CGCodeInterpreter::ConvertAbsToUserUnitsY(double y)
{
	return InchesToUserUnits(y - p_setup->axis_offset_y  - p_setup->origin_offset_y - p_setup->tool_yoffset);
}

double CGCodeInterpreter::ConvertAbsToUserUnitsZ(double z)
{
	return InchesToUserUnits(z - p_setup->axis_offset_z  - p_setup->origin_offset_z - p_setup->tool_length_offset);
}

double CGCodeInterpreter::ConvertAbsToUserUnitsA(double a)
{
	return InchesOrDegToUserUnitsA(a - p_setup->AA_axis_offset  - p_setup->AA_origin_offset);
}

double CGCodeInterpreter::ConvertAbsToUserUnitsB(double b)
{
	return InchesOrDegToUserUnitsB(b - p_setup->BB_axis_offset  - p_setup->BB_origin_offset);
}

double CGCodeInterpreter::ConvertAbsToUserUnitsC(double c)
{
	return InchesOrDegToUserUnitsC(c - p_setup->CC_axis_offset  - p_setup->CC_origin_offset);
}

double CGCodeInterpreter::ConvertAbsToUserUnitsU(double u)
{
	return InchesToUserUnits(u - p_setup->UU_axis_offset  - p_setup->UU_origin_offset);
}

double CGCodeInterpreter::ConvertAbsToUserUnitsV(double v)
{
	return InchesToUserUnits(v - p_setup->VV_axis_offset  - p_setup->VV_origin_offset);
}

double CGCodeInterpreter::InchesToUserUnits(double inches)
{
	return p_setup->length_units==CANON_UNITS_INCHES ? inches : inches * 25.4;
}

double CGCodeInterpreter::InchesToUserUnitsX(double inches)
{
	if (p_setup->DiameterMode)
		return p_setup->length_units==CANON_UNITS_INCHES ? inches * 2.0 : inches * 25.4 * 2.0;
	else
		return p_setup->length_units==CANON_UNITS_INCHES ? inches : inches * 25.4;
}


double CGCodeInterpreter::InchesOrDegToUserUnitsA(double input)
{
	if (CoordMotion->Kinematics->m_MotionParams.DegreesA)
		return input;
	else
		return p_setup->length_units==CANON_UNITS_INCHES ? input : input * 25.4;
}

double CGCodeInterpreter::InchesOrDegToUserUnitsB(double input)
{
	if (CoordMotion->Kinematics->m_MotionParams.DegreesB)
		return input;
	else
		return p_setup->length_units==CANON_UNITS_INCHES ? input : input * 25.4;
}

double CGCodeInterpreter::InchesOrDegToUserUnitsC(double input)
{
	if (CoordMotion->Kinematics->m_MotionParams.DegreesC)
		return input;
	else
		return p_setup->length_units==CANON_UNITS_INCHES ? input : input * 25.4;
}


double CGCodeInterpreter::UserUnitsToInches(double inches)
{
	return p_setup->length_units==CANON_UNITS_INCHES ? inches : inches / 25.4;
}

double CGCodeInterpreter::UserUnitsToInchesX(double inches)
{
	if (p_setup->DiameterMode)
		return p_setup->length_units==CANON_UNITS_INCHES ? inches * 0.5 : inches / 25.4 * 0.5;
	else
		return p_setup->length_units==CANON_UNITS_INCHES ? inches : inches / 25.4;
}

double CGCodeInterpreter::UserUnitsToInchesOrDegA(double input)
{
	if (CoordMotion->Kinematics->m_MotionParams.DegreesA)
		return input;
	else
		return p_setup->length_units==CANON_UNITS_INCHES ? input : input / 25.4;
}

double CGCodeInterpreter::UserUnitsToInchesOrDegB(double input)
{
	if (CoordMotion->Kinematics->m_MotionParams.DegreesB)
		return input;
	else
		return p_setup->length_units==CANON_UNITS_INCHES ? input : input / 25.4;
}

double CGCodeInterpreter::UserUnitsToInchesOrDegC(double input)
{
	if (CoordMotion->Kinematics->m_MotionParams.DegreesC)
		return input;
	else
		return p_setup->length_units==CANON_UNITS_INCHES ? input : input / 25.4;
}


void CGCodeInterpreter::ConvertAbsoluteToInterpreterCoord(double x, double y, double z, double a, double b, double c,
	double *gx, double *gy, double *gz, double *ga, double *gb, double *gc, setup_pointer psetup)
{
	double dummyu, dummyv;
	ConvertAbsoluteToInterpreterCoord(x, y, z, a, b, c, 0.0, 0.0, gx, gy, gz, ga, gb, gc, &dummyu, &dummyv, psetup);
}

void CGCodeInterpreter::ConvertAbsoluteToInterpreterCoord(double x,double y,double z,double a,double b,double c, double u, double v,
						double *gx,double *gy,double *gz,double *ga,double *gb,double *gc, double *gu, double *gv, setup_pointer psetup)
{
	if (!psetup) psetup = p_setup;

	*gx  = InchesToUserUnitsX(x) - psetup->axis_offset_x  - psetup->origin_offset_x - psetup->tool_xoffset;
	*gy  = InchesToUserUnits(y) - psetup->axis_offset_y  - psetup->origin_offset_y - psetup->tool_yoffset;
	*gz  = InchesToUserUnits(z) - psetup->axis_offset_z  - psetup->origin_offset_z - psetup->tool_length_offset;
	*ga  = InchesOrDegToUserUnitsA(a) - psetup->AA_axis_offset - psetup->AA_origin_offset;
	*gb  = InchesOrDegToUserUnitsB(b) - psetup->BB_axis_offset - psetup->BB_origin_offset;
	*gc  = InchesOrDegToUserUnitsC(c) - psetup->CC_axis_offset - psetup->CC_origin_offset;
	*gu  = InchesToUserUnits(u) - psetup->UU_axis_offset - psetup->UU_origin_offset;
	*gv  = InchesToUserUnits(v) - psetup->VV_axis_offset - psetup->VV_origin_offset;
}

void CGCodeInterpreter::ConvertAbsoluteToMachine(double x, double y, double z, double a, double b, double c,
	double *gx, double *gy, double *gz, double *ga, double *gb, double *gc)
{
	double dummyu, dummyv;
	ConvertAbsoluteToMachine(x, y, z, a, b, c, 0.0, 0.0, gx, gy, gz, ga, gb, gc, &dummyu, &dummyv);
}

void CGCodeInterpreter::ConvertAbsoluteToMachine(double x, double y, double z, double a, double b, double c, double u, double v,
	double *gx, double *gy, double *gz, double *ga, double *gb, double *gc, double *gu, double *gv)
{
	*gx  = InchesToUserUnitsX(x);
	*gy  = InchesToUserUnits(y);
	*gz  = InchesToUserUnits(z);
	*ga  = InchesOrDegToUserUnitsA(a);
	*gb = InchesOrDegToUserUnitsB(b);
	*gc = InchesOrDegToUserUnitsC(c);
	*gu = InchesToUserUnits(u);
	*gv = InchesToUserUnits(v);
}
int CGCodeInterpreter::ReadAndSyncCurPositions(double *x,double *y,double *z,double *a,double *b,double *c,double *u,double *v)
{
	double dummyu, dummyv;

	if (u == NULL) u = &dummyu;
	if (v == NULL) v = &dummyv;

	// find out which axis is which
	// force refresh and save results
	
    CoordMotion->m_DefineCS_valid = false;

	if (CoordMotion->ReadCurAbsPosition(&CoordMotion->current_x,&CoordMotion->current_y,&CoordMotion->current_z,
		&CoordMotion->current_a,&CoordMotion->current_b,&CoordMotion->current_c,&CoordMotion->current_u,&CoordMotion->current_v,true)) return 1;

	// convert absolute coordinates to GCode interpreter coordinates

	ConvertAbsoluteToInterpreterCoord(CoordMotion->current_x,CoordMotion->current_y,CoordMotion->current_z,
		CoordMotion->current_a,CoordMotion->current_b,CoordMotion->current_c,CoordMotion->current_u,CoordMotion->current_v, x, y, z, a, b, c, u, v);
	
	return 0;    
}

// Call with fixture number 1 through 9
//
// 1 = G54 = default 
// 2 = G55 
// 3 = G56 
// 4 = G57 
// 5 = G58 
// 6 = G59 
// 7 = G59.1 
// 8 = G59.2 
// 9 = G59.3 

int CGCodeInterpreter::ChangeFixtureNumber(int fixture)
{
	switch (fixture)
	{
	case 1:	return rs274ngc_execute("G54");
	case 2:	return rs274ngc_execute("G55");
	case 3:	return rs274ngc_execute("G56");
	case 4:	return rs274ngc_execute("G57");
	case 5:	return rs274ngc_execute("G58");
	case 6:	return rs274ngc_execute("G59");
	case 7:	return rs274ngc_execute("G59.1");
	case 8:	return rs274ngc_execute("G59.2");
	case 9:	return rs274ngc_execute("G59.3");
	}
	return 0;
}



int CGCodeInterpreter::SetOrigin(int index, double x, double y, double z, double a, double b, double c)
{
	int k;

	if (index == 0) // axis offset??
		k = 5210;  // yes
	else
		k = (5200 + (index * 20));  // no, origin (fixture) offsets

	p_setup->parameters[k + 1] = x;
	p_setup->parameters[k + 2] = y;
	p_setup->parameters[k + 3] = z;
	p_setup->parameters[k + 4] = a;
	p_setup->parameters[k + 5] = b;
	p_setup->parameters[k + 6] = c;
	
	return 0;
}

int CGCodeInterpreter::SetOrigin(int index, double x, double y, double z, double a, double b, double c, double u, double v)
{
	int k;

	if (index == 0) // axis offset??
		k = 5210;  // yes
	else
		k = (5200 + (index * 20));  // no, origin (fixture) offsets

	p_setup->parameters[k + 1] = x;
	p_setup->parameters[k + 2] = y;
	p_setup->parameters[k + 3] = z;
	p_setup->parameters[k + 4] = a;
	p_setup->parameters[k + 5] = b;
	p_setup->parameters[k + 6] = c;
	p_setup->parameters[k + 7] = u;
	p_setup->parameters[k + 8] = v;

	return 0;
}

int CGCodeInterpreter::GetOrigin(int index, double *x, double *y, double *z, double *a, double *b, double *c, double *u, double *v)
{
	int k;
	
	if (index == 0) // axis offset??
		k = 5210;  // yes
	else
		k = (5200 + (index * 20));  // no, origin (fixture) offsets

	*x = p_setup->parameters[k + 1];
	*y = p_setup->parameters[k + 2];
	*z = p_setup->parameters[k + 3];
	*a = p_setup->parameters[k + 4];
	*b = p_setup->parameters[k + 5];
	*c = p_setup->parameters[k + 6];
	*u = p_setup->parameters[k + 7];
	*v = p_setup->parameters[k + 8];

	return 0;
}

int CGCodeInterpreter::GetOrigin(int index, double *x, double *y, double *z, double *a, double *b, double *c)
{
	int k;

	if (index == 0) // axis offset??
		k = 5210;  // yes
	else
		k = (5200 + (index * 20));  // no, origin (fixture) offsets

	*x = p_setup->parameters[k + 1];
	*y = p_setup->parameters[k + 2];
	*z = p_setup->parameters[k + 3];
	*a = p_setup->parameters[k + 4];
	*b = p_setup->parameters[k + 5];
	*c = p_setup->parameters[k + 6];

	return 0;
}

int CGCodeInterpreter::DoResumeSafe()
{
	double SafeZ,SafeZMachine,Machinex,Machiney,Machinez,Machinea, Machineb, Machinec, Machineu, Machinev;

	if (!m_Resume) return 0;

	m_Resume=false;

	// read the current positions

	if (CoordMotion->ReadCurAbsPosition(&CoordMotion->current_x,&CoordMotion->current_y,&CoordMotion->current_z,
 		&CoordMotion->current_a, &CoordMotion->current_b, &CoordMotion->current_c, &CoordMotion->current_u, &CoordMotion->current_v,true)) return 1;


	if (CoordMotion->ReadCurAbsPosition(&Machinex, &Machiney, &Machinez, &Machinea, &Machineb, &Machinec, &Machineu, &Machinev, true)) return 1;

	SafeZ = UserUnitsToInches(m_ResumeSafeZ);

	// compute the safe z height as an absolute position
	// in user units
	if (m_ResumeSafeRelAbs)
	{
		// absolute height, convert value to Inches
		SafeZMachine = UserUnitsToInches(m_ResumeSafeZ+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset);
	}
	else
	{
		// relative move, convert value to inches
		SafeZMachine = Machinez + UserUnitsToInches(m_ResumeSafeZ);
	}

	SetupTracker.ClearHistory();
	SetupTracker.InsertState(&_setup);  // save the interpreter state (including current sequence number)

	if (m_ResumeMoveToSafeZ)
	{
		// keep everything where it is, except move z
		Machinez = SafeZMachine;
		if (CoordMotion->StraightTraverse(Machinex,Machiney,Machinez,Machinea, Machineb, Machinec, Machineu, Machinev, false,_setup.sequence_number+1)) return 2;
	}

	if (m_ResumeTraverseXY)
	{
		// keep everything where it is, except move xy (and abc)
		Machinex = UserUnitsToInchesX(m_ResumeTraverseSafeX+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset);
		Machiney = UserUnitsToInches(m_ResumeTraverseSafeY+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset);
		Machinea = CoordMotion->m_StoppedMachinea;
		Machineb = CoordMotion->m_StoppedMachineb;
		Machinec = CoordMotion->m_StoppedMachinec;
		if (CoordMotion->StraightTraverse(Machinex, Machiney, Machinez, Machinea, Machineb, Machinec, Machineu, Machinev, false, _setup.sequence_number + 1)) return 2;
	}

	if (m_ResumeSafeStartSpindle)
	{
		if (m_ResumeSafeSpindleCWCCW==0)
		{
			InvokeAction(3);  // do the defined action for M Code
		}
		else
		{
			InvokeAction(4);  // do the defined action for M Code
		}
	}

	if (m_ResumeDoSafeFeedZ)
	{
		Machinez = GC->UserUnitsToInches(m_ResumeFeedSafeZ+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset);

		if (CoordMotion->StraightFeed(UserUnitsToInches(m_ResumeZFeedRate) / 60.0, Machinex, Machiney, Machinez, Machinea, Machineb, Machinec,
			Machineu, Machinev, _setup.sequence_number + 1, 0)) return 2;
	}

	if (m_ResumeRestoreFeedRate)
	{
		p_setup->feed_rate = m_ResumeResumeFeedRate;
		SetFeedRate(m_ResumeResumeFeedRate);
	}

	if (CoordMotion->FlushSegments()) {CoordMotion->SetAbort(); return 1;}  
	if (CoordMotion->WaitForSegmentsFinished(TRUE)) {CoordMotion->SetAbort(); return 1;}

	return 0;    
}

void CGCodeInterpreter::SetFeedRate(double rate)
{
	SET_FEED_RATE(rate);
}


// Set GCode User function call back that is called from GCode as (USR,message)
void CGCodeInterpreter::SetUserCallback(G_USER_CALLBACK *UserFn)
{
	m_UserFn = UserFn;
}

// Set GCode User function call back that is called from GCode as Mxxx
void CGCodeInterpreter::SetUserMCodeCallback(G_M_USER_CALLBACK *UserFn)
{
	m_UserFnMCode = UserFn;
}

// Set Invoke Action Callback to call KMotionCNC to executa Screen Script File
void CGCodeInterpreter::SetScreenScriptCallback(G_SCREENSCRIPT_CALLBACK *UserFn)
{
	m_ScreenScriptCallback = UserFn;
}

int CGCodeInterpreter::rs274ngc_save_parameters()
{
	char file[MAX_PATH+1];
	GET_EXTERNAL_PARAMETER_FILE_NAME(file, MAX_PATH);
	return ::rs274ngc_save_parameters(file,p_setup->parameters);
}

bool CGCodeInterpreter::rs274ngc_save_parameters_changed(void)
{
	return ::rs274ngc_save_parameters_changed();
}



int CGCodeInterpreter::DoReverseSearch(const char * InFile, int CurrentLine)
{
	int GCodeReads,status;
	char trash[INTERP_TEXT_SIZE];
	char * read_ok;
	char s[MAX_LINE];

	if (CurrentLine==0) return 0;  // should always be ok to set the first line

	CoordMotion->m_Stopping = STOPPED_NONE;

// always read the tool file now (some users set it externally)
//	if (m_restart || m_ReadToolFile)
	{
		if (ToolFile[0]!=0)
		{
			status = read_tool_file(ToolFile, &_setup);
			if (status != RS274NGC_OK)	return rs274ErrorExit(status);
		}
		m_ReadToolFile = false;
	}
	

	if (!CoordMotion->m_Simulate)
	{
		int result;

		// find out which axis is which
		// force refresh and save results

		CoordMotion->m_DefineCS_valid = false;

		if  (CoordMotion->m_PreviouslyStopped)
			result = CoordMotion->ReadCurAbsPosition(&CoordMotion->current_x,&CoordMotion->current_y,&CoordMotion->current_z,
					&CoordMotion->current_a,&CoordMotion->current_b,&CoordMotion->current_c,&CoordMotion->current_u,&CoordMotion->current_v,true);
		else
			result = ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,
				&_setup.AA_current,&_setup.BB_current,&_setup.CC_current,&_setup.UU_current,&_setup.VV_current);

		if (result == 1)
		{
			if (CoordMotion->m_AxisDisabled)
				strcpy(ErrorOutput,"Unable to read defined coordinate system axis positions - Axis Disabled ");
			else
				strcpy(ErrorOutput,"Unable to read defined coordinate system axis positions ");
		}

		if (result != 0) return 1005;
	}

	status = rs274ngc_open(InFile); 
	if (status != RS274NGC_OK)	return rs274ErrorExit(status);

	if (_setup.percent_flag == ON)
		GCodeReads = 1;
	else
		GCodeReads = 0;


	// read the entire file into this array of lines
	// up until the line we wish to start
#ifdef _KMOTIONX
	//char *LineArray = new char[CurrentLine+1][INTERP_TEXT_SIZE+1];
	//this works on linux due to a C++ extension
	char LineArray[CurrentLine+1][INTERP_TEXT_SIZE+1];
#define CLEAN_ARRAY do{ } while( false )
#else
	CString *LineArray = new CString[CurrentLine+1];
	if (!LineArray) return 1;
#define CLEAN_ARRAY do{ delete [] LineArray; } while( false )
#endif

	for( ; GCodeReads<=CurrentLine ; GCodeReads++)
	{
		read_ok = fgets(trash, INTERP_TEXT_SIZE,_setup.file_pointer);
		if (!read_ok) 
		{
			rs274ngc_close();
			CLEAN_ARRAY;
			AfxMessageBox("Error while reading GCode file ");
			return 1;
		}
#ifdef _KMOTIONX
		strcpy(LineArray[GCodeReads],trash);
#else
		LineArray[GCodeReads]=trash;
#endif
	}

	rs274ngc_close();

	bool Done=false;

	bool FoundG=false;
	bool FoundF=false;
	bool FoundUnits=false;
	bool FoundX=CoordMotion->x_axis < 0;
	bool FoundY=CoordMotion->y_axis < 0;
	bool FoundZ=CoordMotion->z_axis < 0;
	bool FoundA=CoordMotion->a_axis < 0;
	bool FoundB=CoordMotion->b_axis < 0;
	bool FoundC=CoordMotion->c_axis < 0;
	bool FoundU=CoordMotion->u_axis < 0;
	bool FoundV=CoordMotion->v_axis < 0;

	bool FoundX0,FoundY0,FoundZ0,FoundA0,FoundB0,FoundC0,FoundU0,FoundV0,FoundG0,FoundF0,FoundUnits0;

	p_setup->motion_mode=-1;  //set motion mode as undefined

	int G=-1;
	int Units=-1;
	double xprep = 0, yprep = 0, zprep = 0, aprep = 0, bprep = 0, cprep = 0, uprep = 0, vprep = 0;
	double x0 = 0, y0 = 0, z0 = 0, a0 = 0, b0 = 0, c0 = 0, u0 = 0, v0 = 0, f0 = 0;
	double x = 0, y = 0, z = 0, a = 0, b = 0, c = 0, u = 0, v = 0, f = 0;
	block *BL=&p_setup->block1;
	block block0;
	bool FirstLoop=true;


	// we need to determine what everthing would be 
	// after the previous line was executed

	while (!FoundG || (!FoundF && (G==-1 || G==G_1 || G==G_2 || G==G_3)) 
		    || !FoundX || !FoundY || !FoundZ || !FoundA || !FoundB || !FoundC|| !FoundU || !FoundV
			|| !FoundUnits)
	{
		if (--GCodeReads<0) break;

		status = rs274ngc_read(LineArray[GCodeReads]);
		if (status == RS274NGC_ENDFILE)
		{
			rs274ngc_close();
			CLEAN_ARRAY;
			return RS274NGC_ENDFILE;
		}

		// save what is on the line the User wants to switch to
		if (FirstLoop)
			block0 = p_setup->block1;


		if (!FoundG)
		{
			if (BL->motion_to_be == G_0 ||
				BL->motion_to_be == G_1 ||
				BL->motion_to_be == G_2 ||
				BL->motion_to_be == G_3 ||
				BL->motion_to_be == G_4) FoundG=true;

			if (FoundG) G=BL->motion_to_be;
		}

		if (!FoundUnits)
		{
			if (BL->g_modes[6] == G_20 ||
				BL->g_modes[6] == G_21) FoundUnits=true;

			if (FoundUnits) Units=BL->g_modes[6];
		}

		if (!FoundF && BL->f_number >= 0.0) {FoundF=true; f=BL->f_number;}
		if (!FoundX && BL->x_flag) {FoundX=true; x=BL->x_number;}
		if (!FoundY && BL->y_flag) {FoundY=true; y=BL->y_number;}
		if (!FoundZ && BL->z_flag) {FoundZ=true; z=BL->z_number;}
		if (!FoundA && BL->a_flag) {FoundA=true; a=BL->a_number;}
		if (!FoundB && BL->b_flag) { FoundB = true; b = BL->b_number; }
		if (!FoundC && BL->c_flag) { FoundC = true; c = BL->c_number; }
		if (!FoundU && BL->u_flag) { FoundU = true; u = BL->u_number; }
		if (!FoundV && BL->v_flag) { FoundV = true; v = BL->v_number; }

		if (FirstLoop)
		{
			FirstLoop = false;
			FoundX0 = FoundX; x0 = x;
			FoundY0 = FoundY; y0 = y;
			FoundZ0 = FoundZ; z0 = z;
			FoundA0 = FoundA; a0 = a;
			FoundB0 = FoundB; b0 = b;
			FoundC0 = FoundC; c0 = c;
			FoundU0 = FoundU; u0 = u;
			FoundV0 = FoundV; v0 = v;
			FoundG0 = FoundG;
			FoundF0 = FoundF;
			FoundUnits0 = FoundUnits;

			// reset to not found as we need to know both where
			// the current line will go and where we were prior
			FoundX = CoordMotion->x_axis < 0;
			FoundY = CoordMotion->y_axis < 0;
			FoundZ = CoordMotion->z_axis < 0;
			FoundA = CoordMotion->a_axis < 0;
			FoundB = CoordMotion->b_axis < 0;
			FoundC = CoordMotion->c_axis < 0;
			FoundU = CoordMotion->u_axis < 0;
			FoundV = CoordMotion->v_axis < 0;
		}
	}

	CLEAN_ARRAY;

	if ((block0.g_modes[6]!=G_20 || block0.g_modes[6]!=G_21) && FoundUnits)  // if units not specified on current line and different 
	{
		if (Units==G_20 && p_setup->length_units!=CANON_UNITS_INCHES)
		{
			if (AfxMessageBox("Backward scan found G20 Inches Mode.  Switch to Inches?" ,MB_YESNO | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL)==IDYES)
			{
				p_setup->length_units=CANON_UNITS_INCHES;
			}
		}
		if (Units==G_21 && p_setup->length_units!=CANON_UNITS_MM)
		{
			if (AfxMessageBox("Backward scan found G21 MM Mode.  Switch to MM?" ,MB_YESNO | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL)==IDYES)
			{
				p_setup->length_units=CANON_UNITS_MM;
			}
		}
	}

	if (block0.motion_to_be == -1)
	{
		sprintf(s, "New Line does not contain a G mode.  Backward scan found:\r\rG%d\r\rUse this mode?", G / 10);

		if (AfxMessageBox(s, MB_YESNO | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL) == IDNO)
		{
			return 1;
		}
	}

	// if jumping to a G0 assume User knows what is intended and it is ok to rapid to that location
	// Also make sure all valid axes are specified in the line.
	if (G == G_0)
	{
		if ((CoordMotion->x_axis < 0 || FoundX0) &&
			(CoordMotion->y_axis < 0 || FoundY0) &&
			(CoordMotion->z_axis < 0 || FoundZ0) &&
			(CoordMotion->a_axis < 0 || FoundA0) &&
			(CoordMotion->b_axis < 0 || FoundB0) &&
			(CoordMotion->c_axis < 0 || FoundC0) &&
			(CoordMotion->u_axis < 0 || FoundU0) &&
			(CoordMotion->v_axis < 0 || FoundV0))
			return 1;
	}

	if (GCodeReads<0) // scanned all the way to the beginning?
	{
		strcpy(s,"Error unable to determine starting conditions for this line.\r\r");
		if (!FoundX) strcat(s,"X? ");
		if (!FoundY) strcat(s,"Y? ");
		if (!FoundZ) strcat(s,"Z? ");
		if (!FoundA) strcat(s,"A? ");
		if (!FoundB) strcat(s,"B? ");
		if (!FoundC) strcat(s,"C? ");
		if (!FoundU) strcat(s,"U? ");
		if (!FoundV) strcat(s,"V? ");
		if (!FoundF && (G==-1 || G==G_1 || G==G_2 || G==G_3))strcat(s,"F? ");
		if (!FoundUnits) strcat(s,"G20/21? ");
		
		AfxMessageBox(s,MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
		return 1;
	}


	if (block0.f_number==-1)
	{
		if (FoundF)
		{
			sprintf(s, "New Line does not contain a Feedrate F command.  Backward scan found:\r\rF%g\r\rUse this feedrate?",f);
			// Ask if not already set to that value
			if (p_setup->feed_rate == f || AfxMessageBox(s,MB_YESNO | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL)==IDYES)
			{
				p_setup->feed_rate=m_StoppedInterpState.feed_rate=f;
			}
			else
			{
				m_StoppedInterpState.feed_rate=p_setup->feed_rate;
			}
		}
		else
		{
			AfxMessageBox("New Line does not contain a Feedrate F command.  Unable to determine previous feedrate", MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
		}
	}
	else
	{
		p_setup->feed_rate=m_StoppedInterpState.feed_rate=block0.f_number;
	}

	// intially setup for preparation move to move to prior position
	if (FoundX) xprep = x;
	if (FoundY) yprep = y;
	if (FoundZ) zprep = z;
	if (FoundA) aprep = a;
	if (FoundB) bprep = b;
	if (FoundC) cprep = c;
	if (FoundU) uprep = u;
	if (FoundV) vprep = v;


	// for Rapids move directly to where the new Line will go
	// otherwise move to where we would be previously
	if (G == G_0)
	{
		if (FoundX0) xprep = x0;
		if (FoundY0) yprep = y0;
		if (FoundZ0) zprep = z0;
		if (FoundA0) aprep = a0;
		if (FoundB0) bprep = b0;
		if (FoundC0) cprep = c0;
		if (FoundU0) uprep = u0;
		if (FoundV0) vprep = v0;
	}



	if (G != G_0 || !FoundX0 || !FoundY0 ||!FoundZ0 ||!FoundA0 ||!FoundB0 ||!FoundC0 ||!FoundU0 ||!FoundV0)
	{
		char v[128];

		if (G == G_0)
			strcpy(s,"Backward scan found target position as:\r\r");
		else
			strcpy(s,"Backward scan found prior position as:\r\r");

		if (CoordMotion->x_axis >= 0) { sprintf(v," X%g", xprep); strcat(s, v); }
		if (CoordMotion->y_axis >= 0) { sprintf(v," Y%g", yprep); strcat(s, v); }
		if (CoordMotion->z_axis >= 0) { sprintf(v," Z%g", zprep); strcat(s, v); }
		if (CoordMotion->a_axis >= 0) { sprintf(v," A%g", aprep); strcat(s, v); }
		if (CoordMotion->b_axis >= 0) { sprintf(v," B%g", bprep); strcat(s, v); }
		if (CoordMotion->c_axis >= 0) { sprintf(v," C%g", cprep); strcat(s, v); }
		if (CoordMotion->u_axis >= 0) { sprintf(v," U%g", uprep); strcat(s, v); }
		if (CoordMotion->v_axis >= 0) { sprintf(v," V%g", vprep); strcat(s, v); }
		strcat(s,"\r\rShould a Safe Z move be made to these coordinates?");
		if (AfxMessageBox(s,MB_YESNO | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL)==IDNO)
		{
			return 1;
		}
	}

	int group = _gees[G*10];

	if (group==0)  // G4 = group0
		p_setup->active_g_codes[1]=-1;
	else           // G0,G1,G2,G3
		p_setup->active_g_codes[0]=-1;

	p_setup->motion_mode=G;

	// intially setup for preparation move to move to prior position
	CoordMotion->m_Stoppedx = p_setup->current_x = xprep;
	CoordMotion->m_Stoppedy = p_setup->current_y = yprep;
	CoordMotion->m_Stoppedz = p_setup->current_z = zprep;
	CoordMotion->m_Stoppeda = p_setup->AA_current = aprep;
	CoordMotion->m_Stoppedb = p_setup->BB_current = bprep;
	CoordMotion->m_Stoppedc = p_setup->CC_current = cprep;
	CoordMotion->m_Stoppedu = p_setup->UU_current = uprep;
	CoordMotion->m_Stoppedv = p_setup->VV_current = vprep;

	CoordMotion->m_StoppedMachinex = UserUnitsToInchesX(CoordMotion->m_Stoppedx + _setup.axis_offset_x + _setup.origin_offset_x + _setup.tool_xoffset);
	CoordMotion->m_StoppedMachiney = UserUnitsToInches(CoordMotion->m_Stoppedy + _setup.axis_offset_y + _setup.origin_offset_y + _setup.tool_yoffset);
	CoordMotion->m_StoppedMachinez = UserUnitsToInches(CoordMotion->m_Stoppedz + _setup.axis_offset_z + _setup.origin_offset_z + _setup.tool_length_offset);
	CoordMotion->m_StoppedMachinea = InchesOrDegToUserUnitsA(CoordMotion->m_Stoppeda) - _setup.AA_axis_offset - _setup.AA_origin_offset;
	CoordMotion->m_StoppedMachineb = InchesOrDegToUserUnitsB(CoordMotion->m_Stoppedb) - _setup.BB_axis_offset - _setup.BB_origin_offset;
	CoordMotion->m_StoppedMachinec = InchesOrDegToUserUnitsC(CoordMotion->m_Stoppedc) - _setup.CC_axis_offset - _setup.CC_origin_offset;
	CoordMotion->m_StoppedMachineu = UserUnitsToInches(CoordMotion->m_Stoppedu) - _setup.UU_axis_offset - _setup.UU_origin_offset;
	CoordMotion->m_StoppedMachinev = UserUnitsToInches(CoordMotion->m_Stoppedv) - _setup.VV_axis_offset - _setup.VV_origin_offset;

	CoordMotion->m_PreviouslyStopped = CoordMotion->m_Stopping = STOPPED_INDEP;

	return 0;
}

int CGCodeInterpreter::SetCSS(int mode)  // set CSS mode
{
	char s[64];

	if (CoordMotion->m_Simulate) return 0;


	if (mode==CANON_SPINDLE_CSS)
	{
		double x_res=CoordMotion->Kinematics->m_MotionParams.CountsPerInchX;
		float x_factor=0.0;
		if (x_res != 0.0)
			x_factor = (float)(1.0 / x_res);

		float xoffset = (float)(UserUnitsToInchesX(_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset)*x_res);

		float fspeed = (float)(p_setup->speed * CoordMotion->GetSpindleRateOverride());

		// convert feet per minute or meters per minute -> inches/sec

		if (p_setup->length_units == CANON_UNITS_MM)
			fspeed /= 60.0f * 0.0254f;
		else
			fspeed *= 12.0f/60.0f;

		float max_rpm=1e9;
		if (p_setup->block1.d_number!=-1) max_rpm = (float)p_setup->block1.d_number;

		sprintf(s, "SetPersistHex %d %x",PC_COMM_CSS_X_OFFSET, *(int *)&xoffset);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}

		sprintf(s, "SetPersistHex %d %x",PC_COMM_CSS_X_FACTOR, *(int *)&x_factor);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}

		sprintf(s, "SetPersistHex %d %x",PC_COMM_CSS_S, *(int *)&fspeed);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}

		sprintf(s, "SetPersistHex %d %x",PC_COMM_CSS_MAX_RPM, *(int *)&max_rpm);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
	}

	sprintf(s, "SetPersistHex %d %x",PC_COMM_CSS_MODE, mode);
	if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}

	return 0;
}
