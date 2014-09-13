// KMotionDLL.cpp : Defines the entry point for the DLL application.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#include "stdafx.h"

#define KMotionBd 1

#include "PARAMS.h"
#include "VERSION.h"
#include "COFF.h"
#include "CLOAD.h"


extern CString MainPathDLL;
extern CString MainPath;
extern CString MainPathRoot;


CKMotionDLL::CKMotionDLL(int boardid)
{ 
	BoardID = boardid;
	PipeOpen=false;
	ServerMessDisplayed=false;

	PipeMutex = new CMutex(FALSE,"KMotionPipe",NULL);

	ConsoleHandler=NULL;
	ErrMsgHandler=NULL;
}

CKMotionDLL::~CKMotionDLL()
{
	LPTSTR lpszPipename = "\\\\.\\pipe\\kmotionpipe"; 
	if (PipeOpen)
	{
		PipeOpen=false;
		if(share==2)
		{
			PipeFile.Close();
			Sleep(100);  // give some time for Server to close
		}
	}
	delete PipeMutex;
}


int CKMotionDLL::WriteLineReadLine(const char *s, char *response)
{

	// Send Code, board, string -- Get Dest (byte), Result (int), and string
	char d[MAX_LINE+1];
	char r[MAX_LINE+1];
	int result, m, code = ENUM_WriteLineReadLine;
	
	result = WaitToken();

	if (result) return result;

	memcpy(d,&code,4); 
	memcpy(d+4,&BoardID,4); 
	strcpy(d+8,s); 

	Pipe(d, strlen(s)+1+8 ,r, &m);

	memcpy(&result,r+1,4);
	strcpy(response,r+1+4); 
	
	ReleaseToken();
	return result;
}

int CKMotionDLL::WriteLine(const char *s)
{
	return PipeCmdStr(ENUM_WriteLine, s);
}

int CKMotionDLL::WriteLineWithEcho(const char *s)
{
	return PipeCmdStr(ENUM_WriteLineWithEcho, s);
}

int CKMotionDLL::ReadLineTimeOut(char *response, int TimeOutms)
{
	// Send Code, BoardID, timeout -- Get Dest, Result (int), and string
	char d[MAX_LINE+1];
	char r[MAX_LINE+1];
	int result, m, code = ENUM_ReadLineTimeOut;

	memcpy(d,&code,4); 
	memcpy(d+4,&BoardID,4); 
	memcpy(d+8,&TimeOutms,4); 

	Pipe(d, 12 ,r, &m);

	memcpy(&result,r+1,4);
	strcpy(response,r+1+4); 
	
	return result;
}

int CKMotionDLL::ListLocations(int *nlocations, int *list)
{
	// Send Code -- Get Dest, Result (int), nlocations (int), List (ints)
	char d[MAX_LINE+1];
	char r[MAX_LINE+1];
	int result, m, code = ENUM_ListLocations;

	memcpy(d,&code,4); 

	Pipe(d, 4 ,r, &m);

	memcpy(&result,r+1,4);
	if (result==0) 
	{
		memcpy(nlocations,r+1+4,4); 
		memcpy(list,r+1+8, *nlocations*sizeof(int)); 
	}
	else
	{
		*nlocations=0; 
	}
	return result;
}

int CKMotionDLL::Failed()
{
	return PipeCmd(ENUM_Failed);
}

int CKMotionDLL::Disconnect()
{
	return PipeCmd(ENUM_Disconnect);
}

int CKMotionDLL::FirmwareVersion()
{
	return PipeCmd(ENUM_FirmwareVersion);
}

int CKMotionDLL::CheckForReady()
{
	return PipeCmd(ENUM_CheckForReady);
}

int CKMotionDLL::KMotionLock()
{
	return PipeCmd(ENUM_KMotionLock);
}

int CKMotionDLL::USBLocation()
{
	return PipeCmd(ENUM_USBLocation);
}

int CKMotionDLL::KMotionLockRecovery()
{
	return PipeCmd(ENUM_KMotionLockRecovery);
}

// Try and get the token for the Board
//
//    return with the token (return value = KMOTION_LOCKED)
// OR 
//    if there is a problem with the board 
//    display a message (return value = KMOTION_NOT_CONNECTED)


int CKMotionDLL::WaitToken(bool display_msg, int TimeOut_ms)
{
	CHiResTimer Timer;
	int result;

	Timer.Start();

	int count=0;
	do
	{
		// this Mutex helps maintain a waiting list
		// so everybody gets a chance at the token
		// rather than leaving it random.  Also make
		// sure we have everything before we proceed
		// so we don't get stuck somewhere (deadlocked)
		
		if (!PipeMutex->Lock(TimeOut_ms))
		{
			return KMOTION_IN_USE;
		}

		if (Timer.Elapsed_Seconds() > 2.0 * TimeOut_ms * 0.001)
		{
			PipeMutex->Unlock();
			return KMOTION_IN_USE;
		}

		if (count++)
		{
			Sleep(10);
		}

		result = KMotionLock();

		if (result == KMOTION_IN_USE)
			PipeMutex->Unlock();
	}
	while (result == KMOTION_IN_USE);

	if (result == KMOTION_NOT_CONNECTED && display_msg)
	{
		char s[256];

		if (BoardID>0)
			sprintf(s,"Can't Connect to KMotion Board 0x%X", BoardID);
		else
			sprintf(s,"Can't Connect to KMotion Board #%d", BoardID);

		DoErrMsg(s);

	}

	if (result != KMOTION_LOCKED) 
		PipeMutex->Unlock();      // keep the pipe if we have the token
	
	return result;
}

void CKMotionDLL::ReleaseToken()
{
	PipeCmd(ENUM_ReleaseToken);
	PipeMutex->Unlock();      // also release the pipe
}

int  CKMotionDLL::ServiceConsole()
{
	return PipeCmd(ENUM_ServiceConsole);
}

int CKMotionDLL::SetConsoleCallback(CONSOLE_HANDLER *ch)
{
	ConsoleHandler = ch;

	// tell the server who is the server for 
	// the console 
	return PipeCmd(ENUM_SetConsole);
}

int CKMotionDLL::SetErrMsgCallback(ERRMSG_HANDLER *ch)
{
	ErrMsgHandler = ch;
	return 0;
}


// Note: ALL User Thread Numbers start with 1

int CKMotionDLL::LoadCoff(int Thread, const char *Name, int PackToFlash)
{
	CString s;
	unsigned int EntryPoint;

	if (Thread==0) return 1;

	if (PackToFlash==0 && CheckKMotionVersion()) return 1; 

	if (PackToFlash==0)
	{
		s.Format("Kill %d", Thread);  // make sure the Thread isn't running
		if (WriteLine(s)) return 1;
	}
	
	int result =  ::LoadCoff(this, Name, &EntryPoint, PackToFlash);

	if (result) return result;

	if (Thread >= 0 && PackToFlash==0)
	{
		// Set the entry point for the thread
		
		s.Format("EntryPoint%d %X",Thread,EntryPoint);
		result = WriteLine(s);
		if (result) return result;
	}

	return 0;
}


// send code, board

int CKMotionDLL::PipeCmd(int code)
{
	char d[MAX_LINE+1];
	char r[MAX_LINE+1];
	int result, m;

	memcpy(d,&code,4); 
	memcpy(d+4,&BoardID,4); 

	Pipe(d, 8 ,r, &m);

	memcpy(&result,r+1,4);
	
	return result;
}

// send code, board, string

int CKMotionDLL::PipeCmdStr(int code, const char *s)
{
	char d[MAX_LINE+1];
	char r[MAX_LINE+1];
	int result, m;

	memcpy(d,&code,4); 
	memcpy(d+4,&BoardID,4); 
	strcpy(d+8,s); 

	Pipe(d, strlen(s)+1+8 ,r, &m);

	memcpy(&result,r+1,4);
	
	return result;
}




int CKMotionDLL::Pipe(const char *s, int n, char *r, int *m)
{
	unsigned char Reply = 0xAA;
	CString ErrorMsg;
	bool ReceivedErrMsg=false;


	static int EntryCount=0;

	if (ServerMessDisplayed) 
		return 1;

	LPTSTR lpszPipename = "\\\\.\\pipe\\kmotionpipe"; 

	try
	{
		PipeMutex->Lock();

		if (EntryCount>0)
		{
			int Result=KMOTION_IN_USE;
			memcpy(r+1,&Result,sizeof(Result));
			PipeMutex->Unlock();
			return 1;
		}
	
		EntryCount++;

		if (!PipeOpen)
		{
			int i;
			
			PipeOpen=true;  // only try once
			if (!PipeFile.Open(lpszPipename, CFile::modeReadWrite))
			{
				// pipe won't open try to launch server
				LaunchServer();
				
				for (i=0; i<100; i++) // try for a few secs
				{
					if (PipeFile.Open(lpszPipename, CFile::modeReadWrite))
						break;
					
					Sleep(100);
				}

				if (i==100)
				{
					EntryCount--;
					if (ServerMessDisplayed) return 1;
					ServerMessDisplayed=TRUE;
					DoErrMsg("Unable to Connect to KMotion Server");
					PipeMutex->Unlock();
					exit(1);
				}
			}
		}

		PipeFile.Write(s, n);           // Send the request
		
		for (;;)
		{
			*m = PipeFile.Read(r, MAX_LINE+1);     // Get the response

			// the first byte of the response is the destination
			// currently DEST_NORMAL, DEST_CONSOLE
			
			if (*r == DEST_CONSOLE)
			{
				PipeFile.Write(&Reply, 1);     // Send an ACK back to server
		
				// send it to the console if someone registered a callback

				if (ConsoleHandler)
					ConsoleHandler(r+1);
			}
			else if (*r == DEST_ERRMSG)
			{
				PipeFile.Write(&Reply, 1);     // Send an ACK back to server
				
				// because callback might throw an exception, delay doing the User Callback
				// until everything is received back from the Server and we clean up
				
				ErrorMsg=r+1;
				ReceivedErrMsg=true;
			}
			else
			{
				break;
			}
		}

		EntryCount--;

		PipeMutex->Unlock();
	}
	catch (CFileException)
	{
		EntryCount--;
		if (ServerMessDisplayed) return 1;
		ServerMessDisplayed=TRUE;
		DoErrMsg("Unable to Connect to KMotion Server");
		PipeMutex->Unlock();
		exit(1);
	}

	if (ReceivedErrMsg)
	{
		DoErrMsg(ErrorMsg);
	}
	
	return 0;
 }



int CKMotionDLL::LaunchServer()
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

	CString cmd;  // build command line

	cmd = "KMotionServer.exe";
	
	if (!CreateProcess (
		NULL,
		cmd.GetBuffer(0), 
		NULL, NULL,
		TRUE, 0,
		NULL, NULL,
		&si, &pi))
	{
		ServerMessDisplayed = true;
		DoErrMsg("Unable to execute:\r\rKMotionServer.exe\r\r"
			"Try re-installing software or copy this file to the same location as KMotion.exe");
		exit(1);
	}

	
	// Now that handles have been inherited, close it to be safe.
	// You don't want to read or write to them accidentally.
	CloseHandle(hPipeOutputWrite);
	CloseHandle(hPipeInputRead);

	return 0;
}

int CKMotionDLL::CompileAndLoadCoff(const char *Name, int Thread)
{
	return CompileAndLoadCoff(Name, Thread, NULL, 0);
}

int CKMotionDLL::CompileAndLoadCoff(const char *Name, int Thread, char *Err, int MaxErrLen)
{
	int result,BoardType;
	CString OutFile;
	CString BindTo;


	if (Thread<=0 || Thread>7) 
	{
		CString s;
		s.Format("Invalid Thread Number %d Valid Range (1-7)",Thread);
		strcpy(Err,s);
		return 1;
	}
	
	ConvertToOut(Thread,Name,OutFile.GetBuffer(MAX_PATH),MAX_PATH);
	OutFile.ReleaseBuffer();

	if (CheckKMotionVersion(&BoardType)) return 1;

	// Compile the C File

	result = Compile(Name,OutFile,BoardType,Thread,Err,MaxErrLen);
	if (result) return result;

	// Download the .out File

	result = LoadCoff(Thread, OutFile);
	if (result) return result;


	return 0;
}

int CKMotionDLL::Compile(const char *Name, const char *OutFile, const int BoardType, int Thread, char *Err, int MaxErrLen)
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
	CHAR                szMsg[100];


	if (Thread==0) return 1;

	// Try and locate the Compiler

	CString Errors;
	CString Compiler = MainPathDLL + COMPILER;
	CString OFile=OutFile;

	FILE *f=fopen(Compiler,"r");  // try where the KMotionDLL was irst

	if (f==NULL)
	{
		Compiler = MainPath + "\\Release" + COMPILER;
	
		f=fopen(Compiler,"r");  // try in the released directory next
		if (f==NULL)
		{
			Compiler = MainPath + "\\Debug" + COMPILER;
			f=fopen(Compiler,"r");  // try in the debug directory next
			if (f==NULL)
			{
				DoErrMsg("Error Locating TCC67.exe Compiler");
				return 1;
			}
		}
	}
	fclose(f);

	
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

	CString cmd;  // build command line
	CString BindTo,IncSrcPath1,IncSrcPath2;

	if (BoardType == BOARD_TYPE_KMOTION)
		IncSrcPath1="-I \"" + MainPathRoot + "\\DSP_KMotion\" ";
	else
		IncSrcPath1="-I \"" + MainPathRoot + "\\DSP_KFLOP\" ";

	IncSrcPath2="-I \"" + ExtractPath(Name) + "\"";

	if (BoardType == BOARD_TYPE_KMOTION)
		BindTo = MainPathRoot + "\\DSP_KMotion\\DSPKMotion.out";
	else 
		BindTo = MainPathRoot + "\\DSP_KFLOP\\DSPKFLOP.out";


	cmd.Format(" -text %08X -g -nostdinc " + IncSrcPath1 + IncSrcPath2 + " -o ",GetLoadAddress(Thread,BoardType));
	cmd = Compiler + cmd;
	cmd += "\"" + OFile + "\" \"" + Name + "\" \"" + BindTo +"\"";
	
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
	WaitForSingleObject (pi.hProcess, INFINITE);

	DWORD exitcode;
	int result = GetExitCodeProcess(pi.hProcess,&exitcode);
	
	// Now test to capture DOS application output by reading
	// hPipeOutputRead.  Could also write to DOS application
	// standard input by writing to hPipeInputWrite.

	if (exitcode==0)
	{
		Errors="";
	}
	else
	{
		char *s = Errors.GetBuffer(10001);
		
		bTest=ReadFile(
			hPipeOutputRead,      // handle of the read end of our pipe
			s,					  // address of buffer that receives data
			10000,                // number of bytes to read
			&dwNumberOfBytesRead, // address of number of bytes read
			NULL                  // non-overlapped.
			);

		
		if (!bTest)
		{
			wsprintf(szMsg, "Error #%d reading compiler output.",GetLastError());
			DoErrMsg(szMsg);
			return 1;
		}

		// do something with data.
		s[dwNumberOfBytesRead] = 0;  // null terminate
		Errors.ReleaseBuffer();
	}


	
	// Close all remaining handles
	CloseHandle (pi.hProcess);
    CloseHandle (pi.hThread);
	CloseHandle (hPipeOutputRead);
	CloseHandle (hPipeInputWrite);
	
	/*----------Console application (CONSPAWN.EXE) code------*/

	if (Err)
		strncpy(Err,Errors,MaxErrLen);

	return exitcode;
}



void CKMotionDLL::ConvertToOut(int thread, const char *InFile, char *OutFile, int MaxLength)
{
	CString OFile;
	CString IFile=InFile;

	CString InFileWithCase=InFile;
	CString ThreadString;

	if (thread>0)
	{

		ThreadString.Format("(%d).out",thread);

		IFile.MakeLower();

		if (IFile.Right(2)= ".c")
		{
			OFile = InFileWithCase.Left(InFileWithCase.GetLength()-2);
		}
		else if (IFile.Find(".txt")!=-1)
		{
			OFile = InFileWithCase.Left(InFileWithCase.GetLength()-4);
		}
		else if (IFile.Find(".cpp")!=-1)
		{
			OFile = InFileWithCase.Left(InFileWithCase.GetLength()-4);
		}
		else
		{
			OFile = InFileWithCase;
		}

		OFile += ThreadString;
	}

	strncpy(OutFile,OFile,MaxLength);
}


CString CKMotionDLL::ExtractPath(CString InFile)
{
	int next_pos=0,pos;

	CString OutFile;

	do
	{
		pos=next_pos;
		next_pos = InFile.Find('\\',pos+1);
	}
	while (next_pos!=-1);

	if (pos>0)
		OutFile = InFile.Left(pos);
	else
		OutFile = "";

	return OutFile;
}

unsigned int CKMotionDLL::GetLoadAddress(int thread, int BoardType) 
{
	if (BoardType == BOARD_TYPE_KFLOP)
		return USER_PROG_ADDRESS_KFLOP + (thread-1) * MAX_USER_PROG_SIZE_KFLOP;
	else
		return USER_PROG_ADDRESS_KMOTION + (thread-1) * MAX_USER_PROG_SIZE_KMOTION;

}


int CKMotionDLL::CheckKMotionVersion(int *type, bool GetBoardTypeOnly) 
{
	int result;
	CString BoardVersion;
	CString CoffVersion;
	CString OutFile;
	CString ms;

	if (type) *type = BOARD_TYPE_UNKNOWN;

	if (KMotionLock() == KMOTION_LOCKED)  // see if we can get access
	{
		// Get the firmware date from the KMotion Card which
		// will be in PT (Pacific Time)
		ReleaseToken();
		result = WriteLineReadLine("Version",BoardVersion.GetBufferSetLength(MAX_LINE));
		BoardVersion.ReleaseBuffer();
		if (result) return result;

		// now get date stamp from firmware .out file

		if (BoardVersion.Find("KFLOP")==0)
		{
			OutFile = MainPathRoot +"\\DSP_KFLOP\\DSPKFLOP.out";
			if (type) *type = BOARD_TYPE_KFLOP;
		}
		else
		{
			OutFile = MainPathRoot +"\\DSP_KMotion\\DSPKMotion.out";
			if (type) *type = BOARD_TYPE_KMOTION;
		}
		if (GetBoardTypeOnly) return 0;

		int result = ExtractCoffVersionString(OutFile,CoffVersion.GetBuffer(81));	
		CoffVersion.ReleaseBuffer();

		if (result)
		{
			ms.Format("Error Extracting Version Information from file\r\r"
				" %s",
				OutFile.GetBuffer(0));
			
			DoErrMsg(ms);
			return 1;
		}

		CoffVersion.Remove('\n');
		

		// check if they match exactly

		if (CoffVersion == BoardVersion) return 0;

		if (BoardVersion.Find("KFLOP")==0)
			ms.Format("DSP_KFLOP.out Date Stamp Doesn't match KFLOP Firmware\r\r"
				" Before compiling programs please use Flash/Config Screen and select:\r"
				" Download New Version.  This will install compatible Firmware with\r"
				" this version of software\r\r"
				" %s  KFLOP Firmware\r"
				" %s  DSP_KFLOP.out file",
				BoardVersion.GetBuffer(0),
				CoffVersion.GetBuffer(0));
		else
			ms.Format("DSP_KMotion.out Date Stamp Doesn't match KMotion Firmware\r\r"
				" Before compiling programs please use Flash/Config Screen and select:\r"
				" Download New Version.  This will install compatible Firmware with\r"
				" this version of software\r\r"
				" %s  KMotion Firmware\r"
				" %s  DSP_KMotion.out file",
				BoardVersion.GetBuffer(0),
				CoffVersion.GetBuffer(0));


		
		DoErrMsg(ms);

		return 1;
	}

	return 0;
}



// return size of coff file sections

int CKMotionDLL::CheckCoffSize(const char *InFile, int *size_text, int *size_bss, int *size_data, int *size_total)
{
	FILE *f;
	unsigned int str_size;
	char *Coff_str_table, *name;
	int i,k;
	syment csym;
	char name2[9];
	unsigned int start_text=0, end_text=0, start_bss=0, end_bss=0, start_data=0, end_data=0; 
	unsigned int min,max;

	
	f = fopen(InFile,"rb");

	if (!f) return 1;

	if (fread(&file_hdr, FILHSZ, 1, f) != 1) return 1;

	if (fread(&o_filehdr, sizeof(o_filehdr), 1, f) != 1) return 1;

	// first read the string table

	if (fseek(f,file_hdr.f_symptr + file_hdr.f_nsyms * SYMESZ,SEEK_SET)) return 1;
	if (fread(&str_size, sizeof(int), 1, f) != 1) return 1;

	Coff_str_table = (char *)malloc(str_size);

	if (fread(Coff_str_table, str_size-4, 1, f) != 1) {free(Coff_str_table); return 1;};

	// read/process all the symbols

	// seek back to symbols

	if (fseek(f,file_hdr.f_symptr,SEEK_SET)) {free(Coff_str_table); return 1;};

	for (i=0; i< file_hdr.f_nsyms; i++)
	{
		if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); return 1;};

		if (csym._n._n_n._n_zeroes == 0)
		{
			name = Coff_str_table + csym._n._n_n._n_offset - 4 ;
		}
		else
		{
			name = csym._n._n_name;

			if (name[7] != 0)
			{
				for (k=0; k<8; k++)
					name2[k] = name[k];

				name2[8]=0;

				name = name2;
			}
		}

		// check for the names we are looking for

		if (strcmp("__start_.text",name)==0)  start_text = csym.n_value;
		if (strcmp("__stop_.text" ,name)==0)  end_text   = csym.n_value;
		if (strcmp("__start_.bss" ,name)==0)  start_bss  = csym.n_value;
		if (strcmp("__stop_.bss"  ,name)==0)  end_bss    = csym.n_value;
		if (strcmp("__start_.data",name)==0)  start_data = csym.n_value;
		if (strcmp("__stop_.data" ,name)==0)  end_data   = csym.n_value;

		// skip any aux records

		if (csym.n_numaux == 1)
		{
			if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); return 1;};
			i++;
		}
	}

	fclose(f);
	free(Coff_str_table); 

	*size_text = end_text-start_text;
	*size_bss  = end_bss -start_bss;
	*size_data = end_data-start_data;
	
	
	min = 0xffffffff;
	if (start_text != 0 && min > start_text) min = start_text;
	if (start_bss  != 0 && min > start_bss ) min = start_bss;
	if (start_data != 0 && min > start_data) min = start_data;
	
	max = 0x0;
	if (end_text != 0 && max < end_text) max = end_text;
	if (end_bss  != 0 && max < end_bss ) max = end_bss;
	if (end_data != 0 && max < end_data) max = end_data;

	*size_total = max-min;
	
	return 0;
}



// Find special version string that is pointed to by symbol
//
// VersionAndBuildTime
//
// within the  .const section of the COFF file
//


int CKMotionDLL::ExtractCoffVersionString(const char *InFile, char *Version)
{
	FILE *f;
	unsigned int str_size;
	char *Coff_str_table, *name;
	int i,k;
	syment csym;
	char name2[9];
	unsigned int VersionAddress=0; 
	SCNHDR sect_hdr;
	FILHDR  file_hdr;                       /* FILE HEADER STRUCTURE              */
	AOUTHDR o_filehdr;                      /* OPTIONAL (A.OUT) FILE HEADER       */

	
	f = fopen(InFile,"rb");

	if (!f) return 1;

	if (fread(&file_hdr, FILHSZ, 1, f) != 1) return 1;

	if (fread(&o_filehdr, sizeof(o_filehdr), 1, f) != 1) return 1;

	// search for the .const section header

	for (i=0; i<file_hdr.f_nscns; i++)
	{
		if (fread(&sect_hdr, SCNHSZ, 1, f) != 1) return 1;

		if (strcmp(".const",sect_hdr.s_name)==0) break;  // found it?
	}

	if (i==file_hdr.f_nscns) return 1;


	// now read the string table

	if (fseek(f,file_hdr.f_symptr + file_hdr.f_nsyms * SYMESZ,SEEK_SET)) return 1;
	if (fread(&str_size, sizeof(int), 1, f) != 1) return 1;

	Coff_str_table = (char *)malloc(str_size);

	if (fread(Coff_str_table, str_size-4, 1, f) != 1) {free(Coff_str_table); return 1;};

	// read/process all the symbols

	// seek back to symbols

	if (fseek(f,file_hdr.f_symptr,SEEK_SET)) {free(Coff_str_table); return 1;};

	for (i=0; i< file_hdr.f_nsyms; i++)
	{
		if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); return 1;};

		if (csym._n._n_n._n_zeroes == 0)
		{
			name = Coff_str_table + csym._n._n_n._n_offset - 4 ;
		}
		else
		{
			name = csym._n._n_name;

			if (name[7] != 0)
			{
				for (k=0; k<8; k++)
					name2[k] = name[k];

				name2[8]=0;

				name = name2;
			}
		}

		// check for the names we are looking for

		if (strcmp("_VersionAndBuildTime",name)==0)
		{
			VersionAddress = csym.n_value;
			break;
		}

		// skip any aux records

		if (csym.n_numaux == 1)
		{
			if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); return 1;};
			i++;
		}
	}


	free(Coff_str_table); 

	if (VersionAddress==0) return 1;

	// compute file offset for the string

	int offset = sect_hdr.s_scnptr +         // file pointer to data
		(VersionAddress - sect_hdr.s_paddr); // plus offset - physical address


	if (fseek(f,offset,SEEK_SET)) return 1;
	
	for (i=0; i<80; i++)
	{
		if (fread(Version+i, sizeof(char), 1, f) != 1) return 1;
		if (Version[i]==0) break;
	}

	fclose(f);

	if (i==80) return 1;
	
	return 0;
}


void CKMotionDLL::DoErrMsg(const char *s)
{
	static int MessageDisplayed=false;

	if (!MessageDisplayed)
	{
		MessageDisplayed=true;
		if (ErrMsgHandler)
		{
			__try
			{
				ErrMsgHandler(s);
			}
			__finally
			{
				MessageDisplayed=false;
			}
		}
		else
		{
			AfxMessageBox(s,MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		}
		MessageDisplayed=false;
	}
}

int CKMotionDLL::GetStatus(MAIN_STATUS& status, bool lock)
{ 
    int i,result,n,token;
	CString s; 
	int *p=(int *)&status;
	if(lock)
	{
		token = WaitToken( false, 100);
		if (token != KMOTION_LOCKED) return 1;
	}

	// KMotion is available read the status
	s.Format("GetStatus");  
	if (WriteLine(s))
	{
		if (lock) ReleaseToken();
		return 1;
	}

	n=sizeof(status)/sizeof(int);

	s.Empty();

	for (i=0; i<n; i++)
	{
		if (s.IsEmpty())
		{
			if (ReadLineTimeOut(s.GetBuffer(257),5000))
			{
				if (lock) ReleaseToken();
				return 1;
			}

			s.ReleaseBuffer();

			// change the CRLF at the to a space

			s.Delete(s.GetLength()-2,2);

			s += ' ';
		}

		// get a hex 32 bit int which may really be anything
		
		result = sscanf(s.GetBuffer(0),"%8X",p++);

		if (result!=1)
		{
			if (lock) ReleaseToken();
			return 1;
		}

		if (s.GetLength() >=9)
		{
			s.Delete(0,9);
		}
		else
		{
			if (lock) ReleaseToken();
			return 1;
		}

		// check if first word contains version

		if (i==0)
		{
			if ((p[-1]>>16)==306)  // check for previous version
			{
			}

			if ((p[-1]>>16)==0)
			{
				// probably old version of DSP Code

				// set version to something

				status.VersionAndSize=0;
				status.ThreadActive=0;

				p[0] = p[-1];
				 p++;
				n=n-2;
			}
		}
	} 
	if(lock)ReleaseToken(); 
	return 0;
}
