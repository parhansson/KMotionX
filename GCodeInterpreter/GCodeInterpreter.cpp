// GCodeInterpreter.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
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
	m_InFile=fname;
	m_Halt=false;
	CoordMotion->ClearHalt();

	LaunchExecution();	
	
	return 0;
}

// returns a pointer to the GCode Parameters

MOTION_PARAMS *CGCodeInterpreter::GetMotionParams()
{
	return &CoordMotion->Kinematics->m_MotionParams;
}



DWORD DoExecuteShell(LPDWORD lpdwParam)
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
	CString ErrDescr;

	rs274ngc_close();

	if (CoordMotion->GetAbort())
	{
		if (CoordMotion->m_AxisDisabled)
		{
			ErrDescr="Axis Disabled - GCode Aborted";
			status=1000;
		}
		else
		{
			ErrDescr="GCode Aborted";
			status=1001;
		}
	}
	else
	{
		rs274ngc_error_text(status,ErrDescr.GetBuffer(200),200);
		ErrDescr.ReleaseBuffer();
	}

	ErrorOutput+=ErrDescr;

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

	Output="";
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
	CString OutputCRLF;

	ErrorOutput="";

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
				ErrorOutput="Unable to read defined coordinate system axis positions - Axis Disabled ";
			else
				ErrorOutput="Unable to read defined coordinate system axis positions ";
		}

		if (result != 0) return 1005;
	}

	status = rs274ngc_open(m_InFile.GetBuffer(0)); 
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
			ErrorOutput="Error while reading GCode file ";
			return NCE_A_FILE_IS_ALREADY_OPEN;
		}
	}


	SetupTracker.ClearHistory();
	
	for(  ; ; m_GCodeReads++)
	{
		// record what line we are doing in the GCode
		_setup.current_line = m_CurrentLine = m_GCodeReads;

		SetupTracker.InsertState(&_setup);  // save the interpreter state
		
		// give output to caller			
		Output.Replace("\n","\r\n");

		m_StatusFn(m_CurrentLine,Output);
		
		Output="";  // clear it


		if (((m_end!=-1)&&(m_CurrentLine>m_end)) || (CoordMotion->m_Simulate && m_Halt) || CoordMotion->GetAbort())
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


int CGCodeInterpreter::DoExecuteComplete()
{
	CoordMotion->ExecutionStop();

	if (CoordMotion->m_Stopping != STOPPED_NONE)
	{
		// if we were previously stopped then restore the state
		if (CoordMotion->m_PreviouslyStopped)
		{
			SetupTracker.RestoreState(_setup.sequence_number-1,&_setup);
			SET_FEED_RATE(_setup.feed_rate);
			m_StoppedInterpState = _setup;

			// convert stopped absolute coordinates to GCode interpreter coordinates

			ConvertAbsoluteToInterpreterCoord(CoordMotion->m_StoppedMachinex, CoordMotion->m_StoppedMachiney, CoordMotion->m_StoppedMachinez, CoordMotion->m_StoppedMachinea, CoordMotion->m_StoppedMachineb, CoordMotion->m_StoppedMachinec,
												 &CoordMotion->m_Stoppedx,       &CoordMotion->m_Stoppedy, &CoordMotion->m_Stoppedz, &CoordMotion->m_Stoppeda, &CoordMotion->m_Stoppedb, &CoordMotion->m_Stoppedc);

			_setup.file_pointer=NULL;
		}

		m_exitcode=1005;
	}
	
	m_CompleteFn(m_exitcode,_setup.current_line,_setup.sequence_number+1,ErrorOutput);

	return 0;
}

int CGCodeInterpreter::LaunchExecution()
{
	DWORD ID;
	

	// Now start a worker thread that performs the G Code
	
	HANDLE Thread=CreateThread( 
		NULL,                        /* no security attributes        */ 
		0,                           /* use default stack size        */ 
		(LPTHREAD_START_ROUTINE) ::DoExecuteShell, /* thread function       */ 
		this,							 /* argument to thread function   */ 
		0,                           /* use default creation flags    */ 
		&ID);   

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
	CString s;
	double value;
	int ivalue,ipersist;

	if (FlushBeforeUnbufferedOperation && CoordMotion->m_Simulate) return 0;

	// If we were called from a button and we had been previously aborted then clear the Abort
	if (!FlushBeforeUnbufferedOperation && CoordMotion->GetAbort())
		CoordMotion->ClearAbort();
	
	if (i>=MAX_MCODE_ACTIONS_M1+MAX_MCODE_ACTIONS_BUTTONS)
		p=&McodeActions[i-100+MAX_MCODE_ACTIONS_M1+MAX_MCODE_ACTIONS_BUTTONS];
	else
		p=&McodeActions[i];

	switch (p->Action)
	{
	case M_Action_None:
		break;

	case M_Action_Setbit:
		if (FlushBeforeUnbufferedOperation)  // false for User button
		{
			s.Format("SetStateBitBuf%d=%d",(int)p->dParams[0],(int)p->dParams[1]);
			if (CoordMotion->DoKMotionBufCmd(s)) return 1;
		}
		else
		{
			s.Format("SetStateBit%d=%d",(int)p->dParams[0],(int)p->dParams[1]);
			if (CoordMotion->DoKMotionCmd(s,FlushBeforeUnbufferedOperation)) return 1;
		}
		break;

	case M_Action_Waitbit:
		if (FlushBeforeUnbufferedOperation)  // false for User button (doesn't make sense for button)
		{
			if (p->dParams[1]==0)
				s.Format("WaitNotBitBuf%d",(int)p->dParams[0]);
			else
				s.Format("WaitBitBuf%d",(int)p->dParams[0]);

			if (CoordMotion->DoKMotionBufCmd(s)) return 1;
		}
		break;

	case M_Action_SetTwoBits:
		if (FlushBeforeUnbufferedOperation)  // false for User button
		{
			s.Format("SetStateBitBuf%d=%d",(int)p->dParams[0],(int)p->dParams[1]);
			if (CoordMotion->DoKMotionBufCmd(s)) return 1;
			s.Format("SetStateBitBuf%d=%d",(int)p->dParams[2],(int)p->dParams[3]);
			if (CoordMotion->DoKMotionBufCmd(s)) return 1;
		}
		else
		{
			s.Format("SetStateBit%d=%d",(int)p->dParams[0],(int)p->dParams[1]);
			if (CoordMotion->DoKMotionCmd(s,FlushBeforeUnbufferedOperation)) return 1;
			s.Format("SetStateBit%d=%d",(int)p->dParams[2],(int)p->dParams[3]);
			if (CoordMotion->DoKMotionCmd(s,FlushBeforeUnbufferedOperation)) return 1;
		}
		break;

	case M_Action_DAC:
		value = p_setup->speed * CoordMotion->GetSpindleRateOverride() * p->dParams[1] + p->dParams[2];   // scale and offset
		ivalue = (int)floor(value+0.5); 
		if (ivalue < (int)p->dParams[3]) ivalue = (int)p->dParams[3];
		if (ivalue > (int)p->dParams[4]) ivalue = (int)p->dParams[4];
		s.Format("DAC%d=%d",(int)p->dParams[0],ivalue);
		if (CoordMotion->DoKMotionCmd(s,FlushBeforeUnbufferedOperation)) return 1;
		break;

	case M_Action_Program:
	case M_Action_Program_wait:
	case M_Action_Program_wait_sync:
		if (FlushBeforeUnbufferedOperation)
		{
			if (CoordMotion->FlushSegments()) {CoordMotion->SetAbort(); return 1;}  
			if (CoordMotion->WaitForSegmentsFinished(TRUE)) {CoordMotion->SetAbort(); return 1;}
		}


		s = p->String;
		s = s.Right(4);
		s.MakeLower();
		if (s.Right(4) == ".ngc")
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
					s.Format("SetPersistHex %d %x",ipersist, p_setup->selected_tool_slot);
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
					s.Format("SetPersistHex %d %x",ipersist, *(int *)&fspeed);
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
						s.Format("SetPersistHex %d %x",ipersist, *(int *)&p);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
						ipersist++;
						count++;
					}
	
					if (p_setup->block1.q_flag)
					{
						float q = (float)p_setup->block1.q_number;
						s.Format("SetPersistHex %d %x",ipersist, *(int *)&q);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
						ipersist++;
						count++;
					}
	
					if (p_setup->block1.r_flag)
					{
						float r = (float)p_setup->block1.r_number;
						s.Format("SetPersistHex %d %x",ipersist, *(int *)&r);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
						ipersist++;
						count++;
					}
	
					if (count==0)  // if no parameters just set the MCode number
					{
						s.Format("SetPersistHex %d %x",ipersist,i);
						if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
					}
				}
			}
	
			// If a C File is specified then Compile and load it
	
			if (p->String[0])
			{
				CString Err;
	
				if (CoordMotion->KMotionDLL->CompileAndLoadCoff(p->String, (int)p->dParams[0], Err.GetBuffer(500), 499))
				{
					Err.ReleaseBuffer();
					CoordMotion->KMotionDLL->DoErrMsg("Error Compiling and Loading KMotion Program\r\r" + ((CString)p->String) + "\r\r" + Err);
					return 1;
				}
			}
	
			// Now execute the thread!
	
			s.Format("Execute%d",(int)p->dParams[0]);
			if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
	
			if (p->Action == M_Action_Program_wait || p->Action == M_Action_Program_wait_sync)
			{
				CString response;
	
				int count=0;
	
				s.Format("CheckThread%d",(int)p->dParams[0]);
				do
				{
					if (count++) Sleep(10);
					
					if (CoordMotion->KMotionDLL->WriteLineReadLine(s,response.GetBufferSetLength(MAX_LINE))) 
					{
						CoordMotion->SetAbort(); 
						return 1;
					}
					response.ReleaseBuffer();
	
					if (CoordMotion->GetAbort()) return 1;
				}
				while (response!="0");
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
						ErrorOutput="Unable to read defined coordinate system axis positions - Axis Disabled ";
					else
						ErrorOutput="Unable to read defined coordinate system axis positions ";
	
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
		s="";
		CString s0;
		if (i==6)  // tool change
		{
			s.Format(" %d",p_setup->selected_tool_slot);
		}
		else if (i==10)  // set speed
		{
			float fspeed = (float)(p_setup->speed * CoordMotion->GetSpindleRateOverride());
			s.Format(" %f",fspeed);
		}
		else
		{
			int count=0;
			// for other M Codes check if P Q or R words are present on the same line
			// if so, download them into successive persist vars as floats

			if (p_setup->block1.p_flag)
			{
				s0.Format(" %f", p_setup->block1.p_number);
				s=s+s0;
			}

			if (p_setup->block1.q_flag)
			{
				s0.Format(" %f", p_setup->block1.q_number);
				s=s+s0;
			}

			if (p_setup->block1.r_flag)
			{
				s0.Format(" %f", p_setup->block1.r_number);
				s=s+s0;
			}
		}

		int result = ExecutePC(p->String+s);  // call the executable with parameters
		if (result)
		{
			CString Err;

			Err.Format("Error Executing PC Program:\r\r%s\r\r"
				"Return code = %d\r\rAbort?",p->String,result);

			if (AfxMessageBox(Err,MB_YESNO)==IDYES) Abort();
		};

		break;
	}
	return 0;
}


int CGCodeInterpreter::ExecutePC(const char *Name)
{
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
	
	return exitcode;
}



double CGCodeInterpreter::ConvertAbsToUserUnitsX(double x)
{
	return InchesToUserUnits(x - p_setup->axis_offset_x  - p_setup->origin_offset_x);
}

double CGCodeInterpreter::ConvertAbsToUserUnitsY(double y)
{
	return InchesToUserUnits(y - p_setup->axis_offset_y  - p_setup->origin_offset_y);
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
													double *gx,double *gy,double *gz,double *ga,double *gb,double *gc)
{
	*gx  = InchesToUserUnits(x) - _setup.axis_offset_x  - _setup.origin_offset_x;
	*gy  = InchesToUserUnits(y) - _setup.axis_offset_y  - _setup.origin_offset_y;
	*gz  = InchesToUserUnits(z) - _setup.axis_offset_z  - _setup.origin_offset_z - _setup.tool_length_offset;
	*ga  = InchesOrDegToUserUnitsA(a) - _setup.AA_axis_offset - _setup.AA_origin_offset;
	*gb  = InchesOrDegToUserUnitsB(b) - _setup.BB_axis_offset - _setup.BB_origin_offset;
	*gc  = InchesOrDegToUserUnitsC(c) - _setup.CC_axis_offset - _setup.CC_origin_offset;
}

void CGCodeInterpreter::ConvertAbsoluteToMachine(double x,double y,double z,double a,double b,double c, 
											double *gx,double *gy,double *gz,double *ga,double *gb,double *gc)
{
	*gx  = InchesToUserUnits(x);
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
		Machinex = UserUnitsToInches(m_ResumeTraverseSafeX+_setup.axis_offset_x+_setup.origin_offset_x);
		Machiney = UserUnitsToInches(m_ResumeTraverseSafeY+_setup.axis_offset_y+_setup.origin_offset_y);
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
	CString s;

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
				ErrorOutput="Unable to read defined coordinate system axis positions - Axis Disabled ";
			else
				ErrorOutput="Unable to read defined coordinate system axis positions ";
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
	CString *LineArray = new CString[CurrentLine+1];
	
	if (!LineArray) return 1;


	for( ; GCodeReads<=CurrentLine ; GCodeReads++)
	{
		read_ok = fgets(trash, INTERP_TEXT_SIZE,_setup.file_pointer);
		if (!read_ok) 
		{
			rs274ngc_close();
			delete [] LineArray;
			AfxMessageBox("Error while reading GCode file ");
			return 1;
		}
		LineArray[GCodeReads]=trash;
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

	if (--GCodeReads<0) return 1;

	status = rs274ngc_read(LineArray[GCodeReads]);
	if (status == RS274NGC_ENDFILE)
	{
		rs274ngc_close();
		delete [] LineArray;
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

	delete [] LineArray;

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
		s="Unable to determine starting conditions for this line.\r\r";
		if (!FoundX) s+="X? ";
		if (!FoundY) s+="Y? ";
		if (!FoundZ) s+="Z? ";
		if (!FoundA) s+="A? ";
		if (!FoundB) s+="B? ";
		if (!FoundC) s+="C? ";
		if (!FoundF && (G==-1 || G==G_1 || G==G_2 || G==G_3))s+="F? ";
		if (!FoundUnits) s+="G20/21? ";
		
		AfxMessageBox(s);
		return 1;
	}

	if (block0.motion_to_be==-1)
	{
		s.Format("New Line does not contain a G mode.  Backward scan found:\r\rG%d\r\rUse this mode?",G/10);

		if (AfxMessageBox(s,MB_YESNO)==IDNO)
		{
			return 1;
		}
	}

	if (block0.f_number==-1)
	{
		if (FoundF)
		{
			s.Format("New Line does not contain a Feedrate F command.  Backward scan found:\r\rF%g\r\rUse this feedrate?",f);
			if (AfxMessageBox(s,MB_YESNO)==IDYES)
			{
				if (FoundF)
				{
					p_setup->feed_rate=m_StoppedInterpState.feed_rate=f;
				}
				else if (block0.f_number!=-1)
				{
					p_setup->feed_rate=m_StoppedInterpState.feed_rate=block0.f_number;
				}
			}
		}
		else
		{
			AfxMessageBox("New Line does not contain a Feedrate F command.  Unable to determine previous feedrate");
		}
	}

	if (!FoundX0 || !FoundY0 ||!FoundZ0 ||!FoundA0 ||!FoundB0 ||!FoundC0)
	{
		CString v;
		s = "Backward scan found previous position as:\r\r";
		if (CoordMotion->x_axis >= 0){ v.Format(" X%g",x); s = s+v;}
		if (CoordMotion->y_axis >= 0){ v.Format(" Y%g",y); s = s+v;}
		if (CoordMotion->z_axis >= 0){ v.Format(" Z%g",z); s = s+v;}
		if (CoordMotion->a_axis >= 0){ v.Format(" A%g",a); s = s+v;}
		if (CoordMotion->b_axis >= 0){ v.Format(" B%g",b); s = s+v;}
		if (CoordMotion->c_axis >= 0){ v.Format(" C%g",c); s = s+v;}
		s=s+"\rShould a Safe Z move be made using these coordinates?";
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

	CoordMotion->m_StoppedMachinex = UserUnitsToInches(CoordMotion->m_Stoppedx+_setup.axis_offset_x+_setup.origin_offset_x);
	CoordMotion->m_StoppedMachiney = UserUnitsToInches(CoordMotion->m_Stoppedy+_setup.axis_offset_y+_setup.origin_offset_y);
	CoordMotion->m_StoppedMachinez = UserUnitsToInches(CoordMotion->m_Stoppedz+_setup.axis_offset_z+_setup.origin_offset_z+_setup.tool_length_offset);
	CoordMotion->m_StoppedMachinea  = InchesOrDegToUserUnitsA(CoordMotion->m_Stoppeda) - _setup.AA_axis_offset - _setup.AA_origin_offset;
	CoordMotion->m_StoppedMachineb  = InchesOrDegToUserUnitsB(CoordMotion->m_Stoppedb) - _setup.BB_axis_offset - _setup.BB_origin_offset;
	CoordMotion->m_StoppedMachinec  = InchesOrDegToUserUnitsC(CoordMotion->m_Stoppedc) - _setup.CC_axis_offset - _setup.CC_origin_offset;



	CoordMotion->m_PreviouslyStopped = CoordMotion->m_Stopping = STOPPED_INDEP;

	return 0;
}

int CGCodeInterpreter::SetCSS(int mode)  // set CSS mode
{
	CString s;


	if (mode==CANON_SPINDLE_CSS)
	{
		double x_res=CoordMotion->Kinematics->m_MotionParams.CountsPerInchX;
		float x_factor=0.0;
		if (x_res != 0.0)
			x_factor = (float)(1.0 / x_res);

		float xoffset = (float)(UserUnitsToInches(_setup.axis_offset_x+_setup.origin_offset_x)*x_res);

		float fspeed = (float)(p_setup->speed * CoordMotion->GetSpindleRateOverride());

		// convert feet per minute or meters per minute -> inches/sec

		if (p_setup->length_units == CANON_UNITS_MM)
			fspeed /= 60.0f * 0.0254f;
		else
			fspeed *= 12.0f/60.0f;

		float max_rpm=1e9;
		if (p_setup->block1.d_number!=-1) max_rpm = (float)p_setup->block1.d_number;

		s.Format("SetPersistHex %d %x",PC_COMM_CSS_X_OFFSET, *(int *)&xoffset);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}

		s.Format("SetPersistHex %d %x",PC_COMM_CSS_X_FACTOR, *(int *)&x_factor);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}

		s.Format("SetPersistHex %d %x",PC_COMM_CSS_S, *(int *)&fspeed);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}

		s.Format("SetPersistHex %d %x",PC_COMM_CSS_MAX_RPM, *(int *)&max_rpm);
		if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}
	}

	s.Format("SetPersistHex %d %x",PC_COMM_CSS_MODE, mode);
	if (CoordMotion->KMotionDLL->WriteLine(s)) {CoordMotion->SetAbort(); return 1;}

	return 0;
}