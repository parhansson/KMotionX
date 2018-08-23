// KMotionDLL.cpp : Defines the entry point for the DLL application.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "StdAfx.h"
#include <vector>

#define KMotionBd 1

#include "PARAMS.h"
#include "VERSION.h"
#include "COFF.h"
#include "CLOAD.h"


#ifndef _KMOTIONX
extern CString MainPathDLL;
extern CString MainPath;
extern CString MainPathRoot;
#endif

void CKMotionDLL::_init(int boardid)
{
	BoardID = boardid;
	PipeOpen=false;
	ServerMessDisplayed=false;
	ErrMessageDisplayed=false;

	PipeMutex = new CMutex(FALSE,"KMotionPipe",0);
	ConsoleHandler=NULL;
	ErrMsgHandler=NULL;
  use_tcp = false;
  remote_tcp = false;

}

CKMotionDLL::CKMotionDLL(int boardid)
{ 
	_init(boardid);
}


CKMotionDLL::CKMotionDLL(int boardid, unsigned int dfltport, const char * url)
{
	_init(boardid);
	use_tcp = true;
	tcp_port = dfltport;
	strncpy(hostname, url, sizeof(hostname)-1);
	hostname[sizeof(hostname)-1] = 0;
}


CKMotionDLL::~CKMotionDLL()
{
  if (PipeOpen)
	{
		PipeOpen=false;
#ifdef _KMOTIONX
		if((*KMotionLocal.sharePtr) == 2)
#else
		if(share == 2)
#endif
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
	
	result = WaitToken("WriteLineReadLine");

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

int CKMotionDLL::KMotionLock(const char *CallerID)
{
	return PipeCmdStr(ENUM_KMotionLock, CallerID);
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


int CKMotionDLL::WaitToken(bool display_msg, int TimeOut_ms, const char *CallerID)
{
	CHiResTimer Timer;
	int result;

	if (ErrMessageDisplayed) return KMOTION_NOT_CONNECTED;

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

		result = KMotionLock(CallerID);

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

void CKMotionDLL::Console(const char *buf)
{
    if (ConsoleHandler)
        ConsoleHandler(buf);
}

// Note: ALL User Thread Numbers start with 1

int CKMotionDLL::LoadCoff(int Thread, const char *Name, int PackToFlash)
{
	char s[50];
	unsigned int EntryPoint;

	if (Thread==0) return 1;
#ifndef SIMULATE_LOADCOFF
	if (PackToFlash==0 && CheckKMotionVersion()) return 1; 

	if (PackToFlash==0)
	{
		sprintf(s,"Kill %d", Thread);  // make sure the Thread isn't running
		if (WriteLine(s)) return 1;
	}
#endif
	
	int result =  ::LoadCoff(this, Name, &EntryPoint, PackToFlash);

	if (result) return result;
#ifndef SIMULATE_LOADCOFF
	if (Thread >= 0 && PackToFlash==0)
	{
		// Set the entry point for the thread
		sprintf(s,"EntryPoint%d %X",Thread,EntryPoint);
		result = WriteLine(s);
		if (result) return result;
	}
#endif
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

	if (s == NULL)
	{
		d[8] = 0;
		Pipe(d, 1 + 8, r, &m);
	}
	else
	{
		strcpy(d + 8, s);
		Pipe(d, strlen(s) + 1 + 8, r, &m);
	}

	memcpy(&result,r+1,4);
	
	return result;
}




int CKMotionDLL::OpenPipe()
{
#ifdef _KMOTIONX
	if (use_tcp) {
        return PipeFile.Open(tcp_port, hostname);
	}
	else
        return PipeFile.Open(SOCK_PATH, CFile::modeReadWrite);
#else
    // As yet, no TCP on Windows
	LPTSTR lpszPipename = "\\\\.\\pipe\\kmotionpipe"; 
    return PipeFile.Open(lpszPipename, CFile::modeReadWrite);
#endif
}



int CKMotionDLL::Pipe(const char *s, int n, char *r, int *m)
{
	unsigned char Reply = 0xAA;
	char ErrorMsg[MAX_LINE];
	bool ReceivedErrMsg=false;
	bool CFExcept=false;
	std::vector<char *> cons;
	unsigned short msglen, len;
	const char * serr_msg;


	static int EntryCount=0;

	if (ServerMessDisplayed) 
		return 1;


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
			if (!OpenPipe())
			{
			    #define OPEN_ATTEMPTS 100
			    if (!remote_tcp) {
				    // pipe won't open try to launch server
				    LaunchServer();
				
				    for (i=0; i<OPEN_ATTEMPTS; i++) // try for a few secs
				    {
					    if (OpenPipe())
						    break;
					
					    Sleep(100);
				    }
			    }
			    else
			        i = OPEN_ATTEMPTS;

				if (i==OPEN_ATTEMPTS)
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

        msglen = n;
		PipeFile.Write(&msglen, sizeof(msglen));   // Send the request length prefix
		PipeFile.Write(s, n);           // Send the request
		
		for (;;)
		{
		    len = 0;
		    while (len < sizeof(msglen))
		        len += PipeFile.Read((char *)(&msglen)+len, sizeof(msglen)-len);
		    if (msglen > MAX_LINE+1)
		        throw std::system_error(E2BIG, std::system_category(), "Read");
		    len = 0;
		    while (len < msglen)
			    len += PipeFile.Read(r+len, msglen-len);     // Get the response
            *m = len;
			// the first byte of the response is the destination
			// currently DEST_NORMAL, DEST_CONSOLE
			
			if (*r == DEST_CONSOLE)
			{
				PipeFile.Write(&Reply, 1);     // Send an ACK back to server
		
				// send it to the console if someone registered a callback
			    // Rather than doing the callback here, just queue up the messages so they can be
			    // handled after the lock is released.  Helps prevent server hang if callback crashes.
		        int len = strlen(r+1)+1;
		        char * m = (char *)malloc(len);
		        if (m) 
		        {
		            memcpy(m, r+1, len);
		            cons.push_back(m);
		        }
			}
			else if (*r == DEST_ERRMSG)
			{
				PipeFile.Write(&Reply, 1);     // Send an ACK back to server
				
				// because callback might throw an exception, delay doing the User Callback
				// until everything is received back from the Server and we clean up
				strcpy(ErrorMsg,r+1);
				//ErrorMsg=r+1;
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
	catch (std::system_error & serr)
	{
		EntryCount--;
		CFExcept = true;
		PipeMutex->Unlock();
		serr_msg = serr.what();
	}

	for (unsigned i = 0; i < cons.size(); ++i) 
	{
	    try
	    {
		    Console(cons[i]);
		}
		catch (...)
		{
		    // Ignore errors in callback
		}
		free(cons[i]);
	}
	
	if (CFExcept)
	{
	    // Exception caught above
		if (ServerMessDisplayed) return 1;
		ServerMessDisplayed=TRUE;

		//DoErrMsg("Unable to Connect to KMotion Server");
		DoErrMsg(serr_msg);
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

#ifdef _KMOTIONX
	return kmx::LaunchServer();
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
#endif

	return 0;
}

int CKMotionDLL::CompileAndLoadCoff(const char *Name, int Thread)
{
	return CompileAndLoadCoff(Name, Thread, NULL, 0);
}

int CKMotionDLL::CompileAndLoadCoff(const char *Name, int Thread, char *Err, int MaxErrLen)
{
	int result,BoardType;
	char OutFile[MAX_PATH];

	if (Thread<=0 || Thread>7) 
	{
		if(Err) snprintf(Err, MaxErrLen, "Invalid Thread Number %d Valid Range (1-7)",Thread);
		return 1;
	}
	
	ConvertToOut(Thread,Name,OutFile,MAX_PATH);

	if (CheckKMotionVersion(&BoardType)) 
	{
		if(Err) snprintf(Err, MaxErrLen, "Board type mismatch");
	    return 1;
	}

	// Compile the C File

	result = Compile(Name,OutFile,BoardType,Thread,Err,MaxErrLen);
	if (Err && MaxErrLen) Err[MaxErrLen-1]='\0'; 
	if (result) return result;

	// Download the .out File

	result = LoadCoff(Thread, OutFile);
	if (result) return result;


	return 0;
}

int CKMotionDLL::Compile(const char *Name, const char *OutFile, const int BoardType, int Thread, char *Err, int MaxErrLen)
{
#ifndef _KMOTIONX
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

	CString path;
	ExtractPath(Name,path.GetBufferSetLength(MAX_PATH));
	path.ReleaseBuffer();
	IncSrcPath2="-I \"" + path + "\"";

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
#else

	if (Err) *Err = 0;
        
	//normalize_slashes()

	if (Thread==0) return 1;

	char command[MAX_LINE +1];
	if(kmx::getCompileCommand(Name, OutFile, GetLoadAddress(Thread,BoardType),BoardType != BOARD_TYPE_KMOTION, command, sizeof(command))){
		DoErrMsg(command);
		return 1;
	}

	debug("%s\n",command);
	int exitCode = 0;

	FILE * out = popen(command, "r");
	if (!out) {
	    exitCode = errno;
	    debug("popen failed: %s\n",strerror(exitCode));
	    if (Err)
	      strncpy(Err,strerror(exitCode),MaxErrLen);
	    return exitCode;
	}
	if (Err)
	{
	  size_t len = fread(Err, 1, MaxErrLen-1, out);
	  Err[len] = 0;
	  if(len > 0){
			DoErrMsg(Err);
			DoErrMsg(command); //Show command that failed. Lazy me, should concat messages
	    debug("fread: %s\n",Err);
	  }
	}
	exitCode = pclose(out);
	if (exitCode < 0) {
	    exitCode = errno;
	    debug("pclose failed: %s\n",strerror(exitCode));
      if (Err)
        strncpy(Err,strerror(exitCode),MaxErrLen);
  }

	return exitCode;
#endif
}


void CKMotionDLL::ConvertToOut(int thread, const char *InFile, char *OutFile, int MaxLength)
{
	int OFileMaxLength;
	char *OFile;
	char ThreadString[10];
	char suffix[5];
	const char *psuf;

	OFileMaxLength = strlen(InFile)+10;
	OFile = new char[OFileMaxLength];

	snprintf(ThreadString, 10, "(%d).out",thread);

	memset (OFile,'\0',OFileMaxLength); //ensure empty string as well as null terminated when using strncpy

	if (thread>0) {
		psuf = strrchr(InFile,'.');
		if(psuf != NULL){
			strcpy(suffix, psuf);
			_strlwr(suffix);

		} else {
			suffix[0] = '\0';
		}
		if(strcmp(suffix,".c") == 0
			|| strcmp(suffix,".txt") == 0
			|| strcmp(suffix,".cpp") == 0){

			strncpy(OFile,InFile,psuf-InFile);//remove suffix
		} else {
			strcpy(OFile,InFile); //use original name
		}
		strcat(OFile, ThreadString);
	}

	strncpy(OutFile,OFile,MaxLength);
	if (MaxLength)
	    OutFile[MaxLength-1] = 0;
	delete [] OFile;
}

void CKMotionDLL::ExtractPath(const char *InFile, char *path){
	const char *pch;
	pch=strrchr(InFile,PATH_SEPARATOR);
	strcpy(path,InFile);
	//null terminate string at last slash position, unless no path,
	// in which case make it a ".".
	if (!pch)
	    strcpy(path, ".");
	else
	    path[pch-InFile] ='\0';
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
	char BoardVersion[MAX_LINE];
	char CoffVersion[81];
	char OutFile[MAX_PATH];
	char ms[MAX_LINE];

	if (type) *type = BOARD_TYPE_UNKNOWN;

	if (KMotionLock("CheckKMotionVersion") == KMOTION_LOCKED)  // see if we can get access
	{
		// Get the firmware date from the KMotion Card which
		// will be in PT (Pacific Time)
		ReleaseToken();
		result = WriteLineReadLine("Version",BoardVersion);

		if (result) return result;

		// now get date stamp from firmware .out file
		//TODO check for first 4 charachters not just anywhere in string
		//This should solve this. If resultpointer is the same as input the string starts with KFLOP
		//if(strstr(BoardVersion,"KFLOP") == BoardVersion)
		if(strstr(BoardVersion,"KFLOP") != NULL)
		//if (BoardVersion.Find("KFLOP")==0)
		{
			kmx::getDspFile(OutFile, true);
			if (type) *type = BOARD_TYPE_KFLOP;
		}
		else
		{
			kmx::getDspFile(OutFile, false);
			if (type) *type = BOARD_TYPE_KMOTION;
		}

		if (GetBoardTypeOnly) return 0;

		int result = ExtractCoffVersionString(OutFile,CoffVersion);

		if (result)
		{
			sprintf(ms,"Error Extracting Version Information from file" LINE_BREAK LINE_BREAK " %s",OutFile);
			DoErrMsg(ms);
			return 1;
		}

		CoffVersion[strlen(CoffVersion)-1] = '\0'; //remove trailing \n
		

		// check if they match exactly
		if (strcmp(CoffVersion,BoardVersion) == 0) return 0;
		//TODO check for first 4 charachters not just anywhere in string
		if(strstr(BoardVersion,"KFLOP") != NULL)
			sprintf(ms,"DSP_KFLOP.out Date Stamp Doesn't match KFLOP Firmware" LINE_BREAK LINE_BREAK
				" Before compiling programs please use Flash/Config Screen and select:" LINE_BREAK
				" Download New Version.  This will install compatible Firmware with" LINE_BREAK
				" this version of software" LINE_BREAK LINE_BREAK
				" %s  KFLOP Firmware" LINE_BREAK
				" %s  DSP_KFLOP.out file",
				BoardVersion,
				CoffVersion);
		else
			sprintf(ms,"DSP_KMotion.out Date Stamp Doesn't match KMotion Firmware" LINE_BREAK LINE_BREAK
				" Before compiling programs please use Flash/Config Screen and select:" LINE_BREAK
				" Download New Version.  This will install compatible Firmware with" LINE_BREAK
				" this version of software" LINE_BREAK LINE_BREAK
				" %s  KMotion Firmware" LINE_BREAK
				" %s  DSP_KMotion.out file",
				BoardVersion,
				CoffVersion);


		
		DoErrMsg(ms);

		return 1;
	}

	//Should this return 1
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
	FILHDR  file_hdr;
	AOUTHDR o_filehdr;
	int have_aout = 0;
	char name2[9];
	unsigned int start_text=0, end_text=0, start_bss=0, end_bss=0, start_data=0, end_data=0; 
	unsigned int min,max;

	*size_text = *size_bss = *size_data = *size_total = 0;
	
	f = fopen(InFile,"rb");

	if (!f) return 1;

	if (fread(&file_hdr, FILHSZ, 1, f) != 1) return 1;
	
	#if 0
	printf("CheckCoffSize:\n"
	        " f_nscns=%u\n"
	        " f_nsyms=%d\n"
	        " f_opthdr=%d\n"
	        " f_flags=0x%04X\n"
	        , file_hdr.f_nscns, file_hdr.f_nsyms, file_hdr.f_opthdr, file_hdr.f_flags);
	#endif

    if (file_hdr.f_opthdr == AOUTSZ) {
	    if (fread(&o_filehdr, AOUTSZ, 1, f) != 1) return 1;
	    have_aout = 1;
	}

	// first read the string table

	if (fseek(f,file_hdr.f_symptr + file_hdr.f_nsyms * SYMESZ,SEEK_SET)) return 1;
	if (fread(&str_size, sizeof(str_size), 1, f) != 1) {
	    // No strings.  Since don't support -g compiler option on Linux, a lot of
	    // things can be missing.  We won't find the start/stop symbols either.
	    Coff_str_table = NULL;
	}
    else {
	    Coff_str_table = (char *)malloc(str_size);

	    if (fread(Coff_str_table, str_size-4, 1, f) != 1) {free(Coff_str_table); return 1;};
	}

	// read/process all the symbols

	// seek back to symbols

	if (file_hdr.f_nsyms) {

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
	}

	fclose(f);
	free(Coff_str_table); 

	if (file_hdr.f_nsyms) {
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
	}
	else if (have_aout) {
	    *size_text = o_filehdr.tsize;
	    *size_bss = o_filehdr.bsize;
	    *size_data = o_filehdr.dsize;
	    // Not strictly correct...
	    *size_total = *size_text + *size_bss + *size_data;
	}
	
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

void CKMotionDLL::ErrMsg(const char *buf)
{
	if (!ErrMessageDisplayed)
	{
		ErrMessageDisplayed=true;
		if (ErrMsgHandler)
		{
			try
			{
				ErrMsgHandler(buf);
			}
			catch (...)
			{
				ErrMessageDisplayed=false;
			}
		}
		else
		{
			AfxMessageBox(buf,MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		}
		ErrMessageDisplayed=false;
	}
}

void CKMotionDLL::DoErrMsg(const char *s)
{
	try
	{
	    ErrMsg(s);
	}
	catch (...) {}
}

int CKMotionDLL::GetStatus(MAIN_STATUS& status, bool lock)
{ 
    int i,result,n,token;
	char s[257];
	char *sp = s;
	int *p=(int *)&status;

	if(lock)
	{
		token = WaitToken( false, 100, "GetStatus");
		if (token != KMOTION_LOCKED) return 1;
	}

	// KMotion is available read the status
	sprintf(s,"GetStatus");
	if (WriteLine(s))
	{
		if (lock) ReleaseToken();
		return 1;
	}

	n=sizeof(status)/sizeof(int);
	memset(s,'\0',257);

	for (i=0; i<n; i++)
	{
		if (*sp == '\0') //string is empty time to read next line
		{
			if (ReadLineTimeOut(s,5000))
			{
				if (lock) ReleaseToken();
				return 1;
			}
			sp = s; //Reset pointer
			// change the CRLF at the to a space and null terminate
			s[strlen(s)-2]=' ';
			s[strlen(s)-1]='\0';

		}

		// get a hex 32 bit int which may really be anything
		result = sscanf(sp,"%8x",p++);

		if (result!=1)
		{
			if (lock) ReleaseToken();
			return 1;
		}

		if (strlen(sp) >=9)
		{
			sp=sp+9;
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
