// GCodeInterpreter.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "rs274ngc_return.h"
#include "rs274ngc.h"
#include "GCodeInterpreter.h"


CGCodeInterpreter::CGCodeInterpreter(CCoordMotion *CoordM)
{ 
	CoordMotion=CoordM;   // set global variable for the low level Interpreter
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
#else
DWORD DoExecuteShell(LPDWORD lpdwParam)
#endif
{
	CGCodeInterpreter *p=(CGCodeInterpreter*)lpdwParam;

	p->m_exitcode = p->DoExecute();

	p->DoExecuteComplete();

	return 0;
}


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
	strncpy(f,ToolFile,MAX_PATH);
}

void CGCodeInterpreter::SetSetupFile(char *f)
{
	strncpy(f,SetupFile,MAX_PATH);
}

void CGCodeInterpreter::SetVarsFile(char *f)
{
	strncpy(f,VarsFile,MAX_PATH);
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
													&CoordMotion->current_a,&CoordMotion->current_b,&CoordMotion->current_c,true);
		else
			result = ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,&_setup.AA_current,&_setup.BB_current,&_setup.CC_current);

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

	for(  ; ; m_GCodeReads++)
	{
		// record what line we are doing in the GCode

		_setup.current_line = m_CurrentLine = m_GCodeReads;

		SetupTracker.InsertState(&_setup);  // save the interpreter state
		
		// give output to caller
#ifndef _KMOTIONX
		//not needed on linux
		CString tmpStr = Output;
		tmpStr.Replace("\n","\r\n");
		strcpy(Output, tmpStr);
#endif
		m_StatusFn(m_CurrentLine,Output);
		
		Output[0]='\0';  // clear it


		if (((m_end!=-1)&&(m_CurrentLine>m_end)) || (CoordMotion->m_Simulate && m_Halt) || CoordMotion->GetAbort() || m_HaltNextLine)
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

			if (m_end!=-1)
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
	CoordMotion->ExecutionStop();

	if (CoordMotion->m_Stopping != STOPPED_NONE)
	{
		// if we were previously stopped then restore the state
		if (CoordMotion->m_PreviouslyStopped)
		{
			SetupTracker.RestoreState(CoordMotion->m_PreviouslyStoppedSeqNo,&_setup);
			SET_FEED_RATE(_setup.feed_rate);
			m_StoppedInterpState = _setup;

			// convert stopped absolute coordinates to GCode interpreter coordinates

			ConvertAbsoluteToInterpreterCoord(CoordMotion->m_StoppedMachinex, CoordMotion->m_StoppedMachiney, CoordMotion->m_StoppedMachinez, CoordMotion->m_StoppedMachinea, CoordMotion->m_StoppedMachineb, CoordMotion->m_StoppedMachinec,
												 &CoordMotion->m_Stoppedx, &CoordMotion->m_Stoppedy, &CoordMotion->m_Stoppedz, &CoordMotion->m_Stoppeda, &CoordMotion->m_Stoppedb, &CoordMotion->m_Stoppedc);

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
		GC->CoordMotion->KMotionDLL->DoErrMsg(err);
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
	char s[MAX_LINE];
	double value;
	int ivalue,ipersist;

	if (FlushBeforeUnbufferedOperation && CoordMotion->m_Simulate) return 0;

	// If we were called from a button and we had been previously aborted then clear the Abort and any Halt
	if (!FlushBeforeUnbufferedOperation && CoordMotion->GetAbort())
	{
		CoordMotion->ClearAbort();
		CoordMotion->ClearHalt();
	}
	
	if (i<MAX_MCODE_ACTIONS_M1+MAX_MCODE_ACTIONS_BUTTONS)
		p=&McodeActions[i];
	else if (i>=100)
		p=&McodeActions[i-100+MCODE_ACTIONS_M100_OFFSET];
	else
		p=&McodeActions[i-24+MCODE_ACTIONS_SPECIAL_OFFSET];

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

		s[0] = '\0';
		if(strlen(p->String) >= 4){
			strcpy(s,p->String+ strlen(p->String) -4);
			_strlwr(s);
		}
		if(s[0] != '\0' && strcmp(s,".ngc")==0)
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
	
					if (p_setup->block1.p_flag)
					{
						float p = (float)p_setup->block1.p_number;
						sprintf(s, "SetPersistHex %d %x",ipersist, *(int *)&p);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
						ipersist++;
						count++;
					}
	
					if (p_setup->block1.q_flag)
					{
						float q = (float)p_setup->block1.q_number;
						sprintf(s, "SetPersistHex %d %x",ipersist, *(int *)&q);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
						ipersist++;
						count++;
					}
	
					if (p_setup->block1.r_flag)
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
	
			if (p->String[0])
			{
				char Err[500];
	
				if (CoordMotion->KMotionDLL->CompileAndLoadCoff(p->String, (int)p->dParams[0], Err, 499))
				{
					char message[1024];
					sprintf(message,"Error Compiling and Loading KMotion Program\r\r%s\r\r%s", p->String, Err );
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
				}
				while (strcmp(response,"0")!=0);
			}
	
			if (p->Action == M_Action_Program_wait_sync)
			{
				// don't sample positions until everything is stopped 
				if (CoordMotion->WaitForSegmentsFinished()) return 1;
				if (CoordMotion->WaitForMoveXYZABCFinished()) return 1;
	
				if (!CoordMotion->m_Simulate && ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,
					                                                               &_setup.AA_current,&_setup.BB_current,&_setup.CC_current))
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
															   &_setup.AA_current,&_setup.BB_current,&_setup.CC_current))
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
		//s="";
		s[0]='\0';
		char s0[32];
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
		int result = ExecutePC(pcCmd);  // call the executable with parameters
		if (result)
		{
			char Err[350];

			sprintf(Err,"Error Executing PC Program:\r\r%s\r\r"
				"Return code = %d\r\rAbort?",p->String,result);

			if (AfxMessageBox(Err,MB_YESNO)==IDYES) Abort();
		};

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



	
void CGCodeInterpreter::ConvertAbsoluteToInterpreterCoord(double x,double y,double z,double a,double b,double c, 
													double *gx,double *gy,double *gz,double *ga,double *gb,double *gc,setup_pointer psetup)
{
	if (!psetup) psetup = p_setup;

	*gx  = InchesToUserUnitsX(x) - psetup->axis_offset_x  - psetup->origin_offset_x - psetup->tool_xoffset;
	*gy  = InchesToUserUnits(y) - psetup->axis_offset_y  - psetup->origin_offset_y - psetup->tool_yoffset;
	*gz  = InchesToUserUnits(z) - psetup->axis_offset_z  - psetup->origin_offset_z - psetup->tool_length_offset;
	*ga  = InchesOrDegToUserUnitsA(a) - psetup->AA_axis_offset - psetup->AA_origin_offset;
	*gb  = InchesOrDegToUserUnitsB(b) - psetup->BB_axis_offset - psetup->BB_origin_offset;
	*gc  = InchesOrDegToUserUnitsC(c) - psetup->CC_axis_offset - psetup->CC_origin_offset;
}

void CGCodeInterpreter::ConvertAbsoluteToMachine(double x,double y,double z,double a,double b,double c, 
											double *gx,double *gy,double *gz,double *ga,double *gb,double *gc)
{
	*gx  = InchesToUserUnitsX(x);
	*gy  = InchesToUserUnits(y);
	*gz  = InchesToUserUnits(z);
	*ga  = InchesOrDegToUserUnitsA(a);
	*gb  = InchesOrDegToUserUnitsB(b);
	*gc  = InchesOrDegToUserUnitsC(c);
}	
int CGCodeInterpreter::ReadAndSyncCurPositions(double *x,double *y,double *z,double *a,double *b,double *c)
{
	// find out which axis is which
	// force refresh and save results

    CoordMotion->m_DefineCS_valid = false;

	if (CoordMotion->ReadCurAbsPosition(&CoordMotion->current_x,&CoordMotion->current_y,&CoordMotion->current_z,
		&CoordMotion->current_a,&CoordMotion->current_b,&CoordMotion->current_c,true)) return 1;

	// convert absolute coordinates to GCode interpreter coordinates

	ConvertAbsoluteToInterpreterCoord(CoordMotion->current_x,CoordMotion->current_y,CoordMotion->current_z,
		CoordMotion->current_a,CoordMotion->current_b,CoordMotion->current_c, x, y, z, a, b, c);
	
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
 
	int k = (5200 + (index * 20));

	p_setup->parameters[k + 1] = x;
	p_setup->parameters[k + 2] = y;
	p_setup->parameters[k + 3] = z; 
	p_setup->parameters[k + 4] = a;
	p_setup->parameters[k + 5] = b;
	p_setup->parameters[k + 6] = c; 


	return 0;
}

int CGCodeInterpreter::GetOrigin(int index, double *x, double *y, double *z, double *a, double *b, double *c)
{
 
	int k = (5200 + (index * 20));

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
	double SafeZ,SafeZMachine,Machinex,Machiney,Machinez,Machinea,Machineb,Machinec;

	if (!m_Resume) return 0;

	m_Resume=false;

	// read the current positions

	if (CoordMotion->ReadCurAbsPosition(&CoordMotion->current_x,&CoordMotion->current_y,&CoordMotion->current_z,
 									    &CoordMotion->current_a,&CoordMotion->current_b,&CoordMotion->current_c,true)) return 1;


	if (CoordMotion->ReadCurAbsPosition(&Machinex,&Machiney,&Machinez,&Machinea,&Machineb,&Machinec,true)) return 1;

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
		if (CoordMotion->StraightTraverse(Machinex,Machiney,Machinez,Machinea,Machineb,Machinec,false,_setup.sequence_number+1)) return 2;
	}

	if (m_ResumeTraverseXY)
	{
		// keep everything where it is, except move xy (and abc)
		Machinex = UserUnitsToInchesX(m_ResumeTraverseSafeX+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset);
		Machiney = UserUnitsToInches(m_ResumeTraverseSafeY+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset);
		Machinea = CoordMotion->m_StoppedMachinea;
		Machineb = CoordMotion->m_StoppedMachineb;
		Machinec = CoordMotion->m_StoppedMachinec;
		if (CoordMotion->StraightTraverse(Machinex,Machiney,Machinez,Machinea,Machineb,Machinec,false,_setup.sequence_number+1)) return 2;
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

		if (CoordMotion->StraightFeed(UserUnitsToInches(m_ResumeZFeedRate)/60.0,Machinex,Machiney,Machinez,Machinea,Machineb,Machinec,_setup.sequence_number+1,0)) return 2;
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

int CGCodeInterpreter::rs274ngc_save_parameters()
{
	char file[MAX_PATH+1];
	GET_EXTERNAL_PARAMETER_FILE_NAME(file, MAX_PATH);
	return ::rs274ngc_save_parameters(file,p_setup->parameters);
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
													&CoordMotion->current_a,&CoordMotion->current_b,&CoordMotion->current_c,true);
		else
			result = ReadAndSyncCurPositions(&_setup.current_x,&_setup.current_y,&_setup.current_z,&_setup.AA_current,&_setup.BB_current,&_setup.CC_current);

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
		LineArray[GCodeReads] = trash;
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

	bool FoundX0,FoundY0,FoundZ0,FoundA0,FoundB0,FoundC0,FoundG0,FoundF0,FoundUnits0;


	int G=-1;
	int Units=-1;
	double x=0,y=0,z=0,a=0,b=0,c=0,f=0;
	block *BL=&p_setup->block1;
	block block0;
	bool FirstLoop=true;

	if (--GCodeReads<0) {CLEAN_ARRAY; return 1;}

	status = rs274ngc_read(LineArray[GCodeReads]);
	if (status == RS274NGC_ENDFILE)
	{
		rs274ngc_close();
		CLEAN_ARRAY;
		return RS274NGC_ENDFILE;
	}

	// save what is on the line the User wants to switch to

	block0=p_setup->block1;

	// we need to determine what everthing would be 
	// after the previous line was executed

	while (!FoundG || (!FoundF && (G==-1 || G==G_1 || G==G_2 || G==G_3)) 
		    || !FoundX || !FoundY || !FoundZ || !FoundA || !FoundB || !FoundC
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
		if (!FoundB && BL->b_flag) {FoundB=true; b=BL->b_number;}
		if (!FoundC && BL->c_flag) {FoundC=true; c=BL->c_number;}

		if (FirstLoop)
		{
			FirstLoop=false;
			FoundX0=FoundX;
			FoundY0=FoundY;
			FoundZ0=FoundZ;
			FoundA0=FoundA;
			FoundB0=FoundB;
			FoundC0=FoundC;
			FoundG0=FoundG;
			FoundF0=FoundF;
			FoundUnits0=FoundUnits;
		}
	}

	CLEAN_ARRAY;

	if ((block0.g_modes[6]!=G_20 || block0.g_modes[6]!=G_21) && FoundUnits)  // if units not specified on current line and different 
	{
		if (Units==G_20 && p_setup->length_units!=CANON_UNITS_INCHES)
		{
			if (AfxMessageBox("Backward scan found G20 Inches Mode.  Switch to Inches?" ,MB_YESNO)==IDYES)
			{
				p_setup->length_units=CANON_UNITS_INCHES;
			}
		}
		if (Units==G_21 && p_setup->length_units!=CANON_UNITS_MM)
		{
			if (AfxMessageBox("Backward scan found G21 MM Mode.  Switch to MM?" ,MB_YESNO)==IDYES)
			{
				p_setup->length_units=CANON_UNITS_MM;
			}
		}
	}

	// if jumping to a G0 assume User knows what is intended and it is ok to rapid to that location 
	if (block0.motion_to_be==G_0) return 1;


	if (GCodeReads<0)
	{
		strcpy(s,"Unable to determine starting conditions for this line.\r\r");
		if (!FoundX) strcat(s,"X? ");
		if (!FoundY) strcat(s,"Y? ");
		if (!FoundZ) strcat(s,"Z? ");
		if (!FoundA) strcat(s,"A? ");
		if (!FoundB) strcat(s,"B? ");
		if (!FoundC) strcat(s,"C? ");
		if (!FoundF && (G==-1 || G==G_1 || G==G_2 || G==G_3))strcat(s,"F? ");
		if (!FoundUnits) strcat(s,"G20/21? ");
		
		AfxMessageBox(s);
		return 1;
	}

	if (block0.motion_to_be==-1)
	{
		sprintf(s, "New Line does not contain a G mode.  Backward scan found:\r\rG%d\r\rUse this mode?",G/10);

		if (AfxMessageBox(s,MB_YESNO)==IDNO)
		{
			return 1;
		}
	}

	if (block0.f_number==-1)
	{
		if (FoundF)
		{
			sprintf(s, "New Line does not contain a Feedrate F command.  Backward scan found:\r\rF%g\r\rUse this feedrate?",f);
			if (AfxMessageBox(s,MB_YESNO)==IDYES)
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
			AfxMessageBox("New Line does not contain a Feedrate F command.  Unable to determine previous feedrate");
		}
	}
	else
	{
		p_setup->feed_rate=m_StoppedInterpState.feed_rate=block0.f_number;
	}


	if (!FoundX0 || !FoundY0 ||!FoundZ0 ||!FoundA0 ||!FoundB0 ||!FoundC0)
	{
		char v[32];
		strcpy(s,"Backward scan found previous position as:\r\r");
		if (CoordMotion->x_axis >= 0){ sprintf(v," X%g",x); strcat(s,v);}
		if (CoordMotion->y_axis >= 0){ sprintf(v," Y%g",y); strcat(s,v);}
		if (CoordMotion->z_axis >= 0){ sprintf(v," Z%g",z); strcat(s,v);}
		if (CoordMotion->a_axis >= 0){ sprintf(v," A%g",a); strcat(s,v);}
		if (CoordMotion->b_axis >= 0){ sprintf(v," B%g",b); strcat(s,v);}
		if (CoordMotion->c_axis >= 0){ sprintf(v," C%g",c); strcat(s,v);}
		strcat(s,"\rShould a Safe Z move be made using these coordinates?");
		if (AfxMessageBox(s,MB_YESNO)==IDNO)
		{
			return 1;
		}
	}

	if (!FoundG0)
	{
		int group = _gees[G*10];

		if (group==0)  // G4 = group0
			p_setup->active_g_codes[1]=-1;
		else           // G0,G1,G2,G3
			p_setup->active_g_codes[0]=-1;

		p_setup->motion_mode=G;
	}

	if (FoundX) {CoordMotion->m_Stoppedx=x; p_setup->current_x=x;}
	if (FoundY) {CoordMotion->m_Stoppedy=y; p_setup->current_y=y;}
	if (FoundZ) {CoordMotion->m_Stoppedz=z; p_setup->current_z=z;}
	if (FoundA) {CoordMotion->m_Stoppeda=a; p_setup->AA_current=a;}
	if (FoundB) {CoordMotion->m_Stoppedb=b; p_setup->BB_current=b;}
	if (FoundC) {CoordMotion->m_Stoppedc=c; p_setup->CC_current=c;}

	CoordMotion->m_StoppedMachinex = UserUnitsToInchesX(CoordMotion->m_Stoppedx+_setup.axis_offset_x+_setup.origin_offset_x+_setup.tool_xoffset);
	CoordMotion->m_StoppedMachiney = UserUnitsToInches(CoordMotion->m_Stoppedy+_setup.axis_offset_y+_setup.origin_offset_y+_setup.tool_yoffset);
	CoordMotion->m_StoppedMachinez = UserUnitsToInches(CoordMotion->m_Stoppedz+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset);
	CoordMotion->m_StoppedMachinea  = InchesOrDegToUserUnitsA(CoordMotion->m_Stoppeda) - _setup.AA_axis_offset - _setup.AA_origin_offset;
	CoordMotion->m_StoppedMachineb  = InchesOrDegToUserUnitsB(CoordMotion->m_Stoppedb) - _setup.BB_axis_offset - _setup.BB_origin_offset;
	CoordMotion->m_StoppedMachinec  = InchesOrDegToUserUnitsC(CoordMotion->m_Stoppedc) - _setup.CC_axis_offset - _setup.CC_origin_offset;



	CoordMotion->m_PreviouslyStopped = CoordMotion->m_Stopping = STOPPED_INDEP;

	return 0;
}

int CGCodeInterpreter::SetCSS(int mode)  // set CSS mode
{
	char s[64];


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
