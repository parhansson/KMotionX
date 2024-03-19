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
#include "Elf.h"


#ifndef _KMOTIONX
extern CString MainPathDLL;
extern CString MainPathDLL64;
extern CString MainPath;
extern CString MainPathRoot;
#endif


CKMotionDLL::CKMotionDLL(int boardid)
{ 
	BoardID = boardid;
	PipeOpen = false;
	ServerMessDisplayed = false;
	ErrMessageDisplayed = false;
	ReadStatus = true;
	PipeMutex = new CMutex(FALSE,"KMotionPipe",0);

	ConsoleHandler = NULL;
	ErrMsgHandlerW = NULL;
	ErrMsgHandler = NULL;
	use_tcp = false;
	remote_tcp = false;
}


CKMotionDLL::CKMotionDLL(int boardid, unsigned int dfltport, const char * url) : CKMotionDLL(boardid)
{
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
	
	result = WaitToken("WriteLineReadLine");

	if (result) return result;

	memcpy(d,&code,4); 
	memcpy(d+4,&BoardID,4); 
	strcpy(d+8,s); 

	Pipe(d, (int)strlen(s)+1+8 ,r, &m);

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


int CKMotionDLL::WaitToken(char* CallerID)
{
	return WaitToken(false, 100000, CallerID);
}

int CKMotionDLL::WaitToken(bool display_msg, int TimeOut_ms, const char *CallerID)
{
	CHiResTimer Timer;
	int result;

//tktk
// 	if (ErrMessageDisplayed) 
//		return KMOTION_NOT_CONNECTED;

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

		if ((unsigned int)BoardID > MAX_USB_ID)
			snprintf(s, 256, " %u.%u.%u.%u", (BoardID>>24)&0xff, (BoardID >> 16) & 0xff, (BoardID >> 8) & 0xff, BoardID & 0xff);
		else if (BoardID>0)
			snprintf(s, 256, " 0x%X", BoardID);
		else
			snprintf(s, 256, " #%d", BoardID);

		DoErrMsg(Translate("Can't Connect to KMotion Board") + kmx::strtowstr(s));

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

int CKMotionDLL::SetErrMsgCallbackW(ERRMSG_HANDLER_WIDE* ch)
{
	ErrMsgHandlerW = ch;
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
		snprintf(s, 50, "Kill %d", Thread);  // make sure the Thread isn't running
		if (WriteLine(s)) return 1;
	}
#endif
	
	int result =  ::LoadCoff(this, Name, &EntryPoint, PackToFlash);

	// failed try as Elf File
	if (result) result = ElfLoad(Name, &EntryPoint, PackToFlash);

	if (result)
	{
		MessageBoxW(NULL, Translate("error loading file"), L"KMotion", MB_OK | MB_SYSTEMMODAL);
		return result;
	}

#ifndef SIMULATE_LOADCOFF
	if (Thread >= 0 && PackToFlash==0)
	{
		// Set the entry point for the thread
		snprintf(s, 50, "EntryPoint%d %X",Thread,EntryPoint);
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
		Pipe(d, (int)strlen(s) + 1 + 8, r, &m);
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
	std::string ErrorMsg;
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
				else {
					i = OPEN_ATTEMPTS;
				}

				if (i==OPEN_ATTEMPTS)
				{
					EntryCount--;
					if (ServerMessDisplayed) return 1;
					ServerMessDisplayed=TRUE;
					DoErrMsg(Translate("Unable to Connect to KMotion Server"));
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

		DoErrMsg(Translate("Unable to Connect to KMotion Server"));
		DoErrMsg(serr_msg);
		exit(1);
	}

	if (ReceivedErrMsg)
	{
		DoErrMsg(Trans.Translate(ErrorMsg));
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

	cmd = MainPathDLL64 + "\\KMotionServer.exe";
	
	if (!CreateProcess (
		NULL,
		cmd.GetBuffer(0), 
		NULL, NULL,
		TRUE, 0,
		NULL, NULL,
		&si, &pi))
	{
		ServerMessDisplayed = true;
		CStringW Errmsg;
		Errmsg.Format("Unable to execute:\r\r" + (CStringW)cmd + Translate("\r\rTry re-installing software or copy this file to the same location as KMotion.exe or Calling Application"));
		DoErrMsg(Errmsg);
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
	return CompileAndLoadCoff(Name, Thread, (char *)NULL, 0);
}

int CKMotionDLL::CompileAndLoadCoff(const char* Name, int Thread, char *Err, int MaxErrLen)
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

int CKMotionDLL::Compile(const char*Name, const char* OutFile, const int BoardType, int Thread, char *Err, int MaxErrLen)
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
	wchar_t             szMsg[100];

	CString Errors;

	if (Thread==0) return 1;

	// Check for #Pragma TI_COMPILER

	FILE *f=fopen(Name, "rt");

	if (f)
	{
		CString s;
		fgets(s.GetBufferSetLength(200), 200, f);
		s.ReleaseBuffer();
		fclose(f);

		RemoveComments(s);
		s.TrimLeft();
		int i = s.Find("#pragma");
		if (i >= 0)
		{
			int k = s.Find("TI_COMPILER",i);
			if (k > i)
			{
				return CompileTI(Name, OutFile, BoardType, Thread, Err, MaxErrLen);
			}
		}
	}


	// Try and locate the TCC67 Compiler
	CString Compiler = MainPathDLL + COMPILER;
	CString OFile=OutFile;

	f=fopen(Compiler,"r");  // try where the KMotionDLL was first

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
				DoErrMsg(Translate("Error Locating TCC67.exe Compiler"));
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
	CString BindTo,IncSrcPath1, IncSrcPath2, IncSrcPath3;

//ExcludeTranslate

	if (BoardType == BOARD_TYPE_KOGNA)
		IncSrcPath1 = "-I \"" + MainPathRoot + "\\DSP_KOGNA\" ";
	else
		IncSrcPath1="-I \"" + MainPathRoot + "\\DSP_KFLOP\" ";

	IncSrcPath2="-I \"" + ExtractPath(Name) + "\" ";

	if (BoardType == BOARD_TYPE_KOGNA)
		BindTo = MainPathRoot + "\\DSP_KOGNA\\DSPKOGNA.out";
	else 
		BindTo = MainPathRoot + "\\DSP_KFLOP\\DSPKFLOP.out";

	cmd.Format(" -text %08X -g -nostdinc " + IncSrcPath1 + IncSrcPath2 + IncSrcPath3 + "-o ",GetLoadAddress(Thread,BoardType));
	cmd = Compiler + cmd;
	cmd += "\"" + OFile + "\" \"" + Name + "\" \"" + BindTo +"\"";
	
	CreateProcess (
		NULL,
		cmd.GetBuffer(0), 
		NULL, NULL,
		TRUE, 0,
		NULL, NULL,
		&si, &pi);
//ResumeTranslate
	
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
			wsprintfW(szMsg, Translate("Error #%d reading compiler output."),GetLastError());
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

	CStringW ErrorsW = Errors;
	if (Err)
	{
		if (MaxErrLen > 0)
		{
			if (ErrorsW.GetLength() >= MaxErrLen-1) // too large to return?
				ErrorsW = ErrorsW.Left(MaxErrLen-1); // yes, trim excess
			wcscpy_s(Err, MaxErrLen, ErrorsW);
		}
	}

	return exitcode;
#else

	if (Err) *Err = 0;
        
	//normalize_slashes()

	if (Thread==0) return 1;

	char command[MAX_LINE +1];
	if(kmx::getCompileCommand(Name, OutFile, GetLoadAddress(Thread,BoardType), BoardType, command, sizeof(command))){
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
#ifndef _KMOTIONX
void CKMotionDLL::RemoveComments(CString &s)
{
	int i = s.Find("//");
	if (i >= 0)
		s = s.Left(i);

	i = s.Find("/*");
	if (i >= 0)
	{
		s = s.Left(s.GetLength() - i);

		int k = s.Find("*/", i);

		if (k > i)
		{   // both found remove section
			s.Delete(i, k - i + 2);
		}
		else // one found - remove remainder
		{
			s = s.Left(i);
		}
	}
}
#endif
int CKMotionDLL::CompileTI(const char * Name, const char * OutFile, const int BoardType, int Thread, wchar_t * Err, int MaxErrLen)
{
#ifndef _KMOTIONX
	SECURITY_ATTRIBUTES sa = { 0 };
	STARTUPINFO         si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	HANDLE              hPipeOutputRead = NULL;
	HANDLE              hPipeOutputWrite = NULL;
	HANDLE              hPipeInputRead = NULL;
	HANDLE              hPipeInputWrite = NULL;
	BOOL                bTest = 0;
	DWORD               dwNumberOfBytesRead = 0;
	wchar_t             szMsg[100];

	CStringW Errors;

	int Opt = 0; // default no optimization
	int MaxSize, BoardMaxSize;

	if (BoardType == BOARD_TYPE_KFLOP)
	{
		MaxSize = MAX_USER_PROG_SIZE_KFLOP;
		BoardMaxSize = 11 * 0x10000;
		if (Thread == 7) MaxSize *= 5;
	}
	else
	{
		MaxSize = MAX_USER_PROG_SIZE_KOGNA;
		BoardMaxSize = 10 * 0x40000;
		if (Thread == 7) MaxSize *= 4;

	}

	FILE *f = fopen(Name, "rt");

	if (f)
	{
		CString s;
		fgets(s.GetBufferSetLength(200), 200, f);
		s.ReleaseBuffer();
		fclose(f);

		RemoveComments(s);
		s.TrimLeft();
		int i = s.Find("#pragma");
		if (i >= 0)
		{
			int k = s.Find("TI_COMPILER", i);
			if (k > i)
			{
				CString sOpt, sMaxSize;
				int r1 = 1, r2 = 1, i1, i2, i3, i0;
				i0 = s.Find("(", k);  // look for (Opt) or (Opt,MaxSize)
				if (i0 > k)
				{
					i1 = s.Find(",", i0);  // look for (Opt) or (Opt,MaxSize)
					if (i1 > i0)
					{
						i2 = s.Find(")", i1);  //  (Opt,MaxSize)
						if (i2 > i1)
						{
							sOpt = s.Mid(i0 + 1, i1 - i0 - 1);
							r1 = sscanf(sOpt, "%d", &Opt);
							sMaxSize = s.Mid(i1 + 1, i2 - i1 - 1);

							i3 = sMaxSize.Find("0x");
							if (i3 >= 0)
							{	// hex value
								sMaxSize = sMaxSize.Right(sMaxSize.GetLength() - i3 - 2);
								r2 = sscanf(sMaxSize, "%x", &MaxSize);
							}
							else
							{	// decimal value  
								sMaxSize = s.Mid(i1 + 1, i2 - i1 - 1);
								r2 = sscanf(sMaxSize, "%d", &MaxSize);
							}
						}
					}
					else
					{
						r1 = 0;
						i1 = s.Find(")", i0);  // (Opt)
						if (i1 > i0)
						{
							sOpt = s.Mid(i0 + 1, i1 - i0 - 1);
							r1 = sscanf(sOpt, "%d", &Opt);
						}
					}
				}


				if (r1 != 1 || r2 != 1 || Opt < 0 || Opt > 3 || MaxSize <= 0 || MaxSize > BoardMaxSize)
				{
					// invalid format pragma TI_COMPILER

					Errors = Translate("Error Line :1: invalid TI_COMPILER pragma \n") + (CStringW) s +
						Translate("\n Expected format:\n#pragma TI_COMPILER\n#pragma TI_COMPILER(opt level 0-3)\n#pragma TI_COMPILER(opt level 0-3, Max Thread Space - hex or decimal)");

					if (MaxSize > BoardMaxSize)
						Errors += "\n\nInvalid Max Thread Space";

					CStringW ErrorsW = Errors;
					wcscpy_s(Err, MaxErrLen, ErrorsW);
					return 1;
				}
			}
		}
	}

	CString CompilerTI;
	if (BoardType == BOARD_TYPE_KFLOP)
	{
		CompilerTI = COMPILERTI_KFLOP;
	}
	else
	{
		CompilerTI = COMPILERTI_KOGNA;
	}


	// Try and locate the Compiler
	CString Compiler = MainPathRoot + CompilerTI;
	CString OFile = OutFile;

	f = fopen(Compiler, "r");  // try where the KMotionDLL was first

	if (f == NULL)
	{
		DoErrMsg(Translate("Error Locating cl6x.exe Compiler"));
		return 1;
	}

	fclose(f);

	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// Create pipe for standard output redirection.
	CreatePipe(&hPipeOutputRead,  // read handle
		&hPipeOutputWrite, // write handle
		&sa,      // security attributes
		10000000      // number of bytes reserved for pipe - 0 default
	);

	// Create pipe for standard input redirection.
	CreatePipe(&hPipeInputRead,  // read handle
		&hPipeInputWrite, // write handle
		&sa,      // security attributes
		10000000      // number of bytes reserved for pipe - 0 default
	);

	// Make child process use hPipeOutputWrite as standard out,
	// and make sure it does not show on screen.
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = hPipeInputRead;
	si.hStdOutput = hPipeOutputWrite;
	si.hStdError = hPipeOutputWrite;

	CString cmd;  // build command line
	CString IncSrcPath1, IncSrcPath2, IncSrcPath3, opt, FileType;

	if (BoardType == BOARD_TYPE_KOGNA)
	{
		FileType = "--abi=eabi";
		IncSrcPath1 = "-i \"" + MainPathRoot + "\\DSP_KOGNA\" ";
	}
	else
	{
		IncSrcPath1 = "-i \"" + MainPathRoot + "\\DSP_KFLOP\" ";
	}

	IncSrcPath2 = "-i \"" + ExtractPath(Name) + "\" ";

	IncSrcPath3 = "-i \"" + MainPathRoot + "\\C Programs\" ";

// must use far data model as User programs call Firmware that uses its Near Data Pointer
	cmd.Format(" %s -k -q -as --diag_suppress=163 --mem_model:data=far "
		+ IncSrcPath1 + IncSrcPath2 + IncSrcPath3 + "-mu -ml3 -mv6710 -o%d", FileType, Opt);
	cmd = Compiler + cmd + " \"" + Name + "\" --obj_directory=\"" + ExtractPath(Name) + "\" --asm_directory=\"" + ExtractPath(Name) + "\"";

	CreateProcess(
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
	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitcode;
	int result = GetExitCodeProcess(pi.hProcess, &exitcode);

	// Now test to capture DOS application output by reading
	// hPipeOutputRead.  Could also write to DOS application
	// standard input by writing to hPipeInputWrite.

	CString cs;
	char *s = cs.GetBuffer(10001);
	dwNumberOfBytesRead = 0;

	bTest = ReadFile(
		hPipeOutputRead,      // handle of the read end of our pipe
		s,					  // address of buffer that receives data
		10000,                // number of bytes to read
		&dwNumberOfBytesRead, // address of number of bytes read
		NULL                  // non-overlapped.
	);

	// do something with data.
	s[dwNumberOfBytesRead] = 0;  // null terminate
	cs.ReleaseBuffer();
	Errors = cs;

	// Close all remaining handles
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hPipeOutputRead);
	CloseHandle(hPipeInputWrite);

	if (!bTest && GetLastError() != ERROR_BROKEN_PIPE) // Note broken pipe just means there was nothing to read
	{
		wsprintfW(szMsg, Translate("Error #%d reading compiler output."), GetLastError());
		DoErrMsg(szMsg);
		return 1;
	}


	if (exitcode == 0)  // compile successful ?
	{
		CStringW LinkMessages;
		exitcode = LinkTI(Compiler, Name, OutFile, BoardType, Thread, LinkMessages.GetBufferSetLength(2000), 2000, MaxSize);
		LinkMessages.ReleaseBuffer();

		// Combine messages
		Errors = Errors + LinkMessages;
	}


	/*----------Console application (CONSPAWN.EXE) code------*/

	if (Err && MaxErrLen > 0)
	{
		if (Errors.GetLength() >= MaxErrLen - 1) // too large to return?
			Errors = Errors.Left(MaxErrLen - 1); // yes, trim excess
		wcscpy_s(Err, MaxErrLen, Errors);
	}


	return exitcode;
#else
	return -1;
#endif
}

int CKMotionDLL::LinkTI(const char * Linker, const char * Name, const char * OutFile, const int BoardType, int Thread, wchar_t * Err, int MaxErrLen, int MaxSize)
{
#ifndef _KMOTIONX
	SECURITY_ATTRIBUTES sa = { 0 };
	STARTUPINFO         si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	HANDLE              hPipeOutputRead = NULL;
	HANDLE              hPipeOutputWrite = NULL;
	HANDLE              hPipeInputRead = NULL;
	HANDLE              hPipeInputWrite = NULL;
	BOOL                bTest = 0;
	DWORD               dwNumberOfBytesRead = 0;
	wchar_t                szMsg[100];

	CStringW Errors;

	CString OFile = OutFile;

	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// Create pipe for standard output redirection.
	CreatePipe(&hPipeOutputRead,  // read handle
		&hPipeOutputWrite, // write handle
		&sa,      // security attributes
		10000000      // number of bytes reserved for pipe - 0 default
	);

	// Create pipe for standard input redirection.
	CreatePipe(&hPipeInputRead,  // read handle
		&hPipeInputWrite, // write handle
		&sa,      // security attributes
		10000000      // number of bytes reserved for pipe - 0 default
	);

	// Make child process use hPipeOutputWrite as standard out,
	// and make sure it does not show on screen.
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = hPipeInputRead;
	si.hStdOutput = hPipeOutputWrite;
	si.hStdError = hPipeOutputWrite;

	CString CompilerTI, LinkTemplate, Symbols, LinkCmd;
	if (BoardType == BOARD_TYPE_KFLOP)
	{
		CompilerTI = COMPILERTI_KFLOP;
		LinkTemplate = LINKTEMPLATE_KFLOP;
		Symbols = SYMBOLS_KFLOP;
		LinkCmd = TILINKCMD_KFLOP;
	}
	else
	{
		CompilerTI = COMPILERTI_KOGNA;
		LinkTemplate = LINKTEMPLATE_KOGNA;
		Symbols = SYMBOLS_KOGNA;
		LinkCmd = TILINKCMD_KOGNA;
	}

	CString cmd;  // build command line
	cmd = (CString)Linker + " -z " + "\"" + MainPathRoot + LinkCmd + "\"";

	CString LinkTemplateData;
	
	FILE *f = fopen(MainPathRoot + LinkTemplate, "rt");
	
	if (!f)
	{
		DoErrMsg(Translate("Unable to open TI Link Template.\n\n") + (CStringW)MainPathRoot + (CStringW)LinkTemplate);
		return 1;
	}

	int n = (int)fread_s(LinkTemplateData.GetBufferSetLength(100000), 100000, 1, 100000, f);
	LinkTemplateData.ReleaseBufferSetLength(n);
	fclose(f);


	CString MapFile, ObjFile = Name;
	n = ObjFile.ReverseFind('.');
	if (n > 0) ObjFile.Delete(n, ObjFile.GetLength() - n);
	MapFile = ObjFile + ".map";
	ObjFile = ObjFile + ".obj";


	// Find IRAM_END in symbols file
	f = fopen(MainPathRoot + Symbols, "rt");
	if (!f)
	{
		DoErrMsg(Translate("Error unable to open DSP Symbol file.\n\n") + (CStringW)MainPathRoot + (CStringW)Symbols);
		return 1;
	}
	CString SymbolsData;
	n = (int)fread_s(SymbolsData.GetBufferSetLength(100000), 100000, 1, 100000, f);
	SymbolsData.ReleaseBufferSetLength(n);
	fclose(f);

	int i0 = SymbolsData.Find("IRAM_END=0x");
	if (i0 == -1)
	{
		DoErrMsg(Translate("Error unable to find IRAM_END sysmbol to determine IRAM size"));
		return 1;
	}

	int i1 = SymbolsData.Find(';',i0+11);
	if (i1 == -1)
	{
		DoErrMsg(Translate("Error unable to find IRAM_END sysmbol to determine IRAM size"));
		return 1;
	}

	CString IramEnd = SymbolsData.Mid(i0+11, i1 - i0);
	int iRamEnd;
	int r = sscanf(IramEnd, "%x", &iRamEnd);
	if (r != 1)
	{
		DoErrMsg(Translate("Error unable to find IRAM_END sysmbol to determine IRAM size"));
		return 1;
	}

//ExcludeTranslate
	CString IR,IRL,TS,TL;
	IR.Format("0x%08x", iRamEnd);
	if (BoardType == BOARD_TYPE_KFLOP)
		IRL.Format("0x%08x", 0x10020000-iRamEnd);
	else
		IRL.Format("0x%08x", 0x11840000 - iRamEnd);
	TS.Format("0x%08x", GetLoadAddress(Thread, BoardType));
	TL.Format("0x%08x", MaxSize);

	LinkTemplateData.Replace("{OBJECTFILE}", "\"" + ObjFile + "\"");
	LinkTemplateData.Replace("{MAPFILE}", "\"" + MapFile + "\"");
	LinkTemplateData.Replace("{OUTPUTFILE}", "\"" + (CString)OutFile + "\"");
	LinkTemplateData.Replace("{IRAMSTART}", IR);
	LinkTemplateData.Replace("{IRAMLENGTH}", IRL);
	LinkTemplateData.Replace("{THREADSTART}", TS);
	LinkTemplateData.Replace("{THREADLENGTH}", TL);
	if (BoardType == BOARD_TYPE_KFLOP)
		LinkTemplateData.Replace("{DSP_KFLOP_PATH}", MainPathRoot + "\\DSP_KFLOP\\");
	else
		LinkTemplateData.Replace("{DSP_KFLOP_PATH}", MainPathRoot + "\\DSP_KOGNA\\");
//ResumeTranslate

	f = fopen(MainPathRoot + LinkCmd, "wt");

	if (!f)
	{
		DoErrMsg(Translate("Unable to open TI Link Command File") + (CStringW)MainPathRoot + (CStringW)LinkCmd);
		return 1;
	}

	n = (int)fwrite(LinkTemplateData,LinkTemplateData.GetLength(), 1, f);
	fclose(f);

	CreateProcess(
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
	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitcode;
	int result = GetExitCodeProcess(pi.hProcess, &exitcode);

	// Now test to capture DOS application output by reading
	// hPipeOutputRead.  Could also write to DOS application
	// standard input by writing to hPipeInputWrite.

	CString cs;
	char *s = cs.GetBuffer(10001);

	bTest = ReadFile(
		hPipeOutputRead,      // handle of the read end of our pipe
		s,					  // address of buffer that receives data
		10000,                // number of bytes to read
		&dwNumberOfBytesRead, // address of number of bytes read
		NULL                  // non-overlapped.
	);


	if (!bTest)
	{
		wsprintfW(szMsg, Translate("Error #%d reading compiler output."), GetLastError());
		DoErrMsg(szMsg);
		return 1;
	}

	// do something with data.
	s[dwNumberOfBytesRead] = 0;  // null terminate
	cs.ReleaseBuffer();
	Errors = cs;


	// Close all remaining handles
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hPipeOutputRead);
	CloseHandle(hPipeInputWrite);

	/*----------Console application (CONSPAWN.EXE) code------*/

	if (Err && MaxErrLen > 0)
	{
		if (Errors.GetLength() >= MaxErrLen - 1) // too large to return?
			Errors = Errors.Left(MaxErrLen - 1); // yes, trim excess
		wcscpy_s(Err, MaxErrLen, Errors);
	}

	return exitcode;
#else
	return -1;
#endif
}

int CKMotionDLL::ValidateC(const char *Name, wchar_t *Err, int MaxErrLen)
{
#ifndef _KMOTIONX
	SECURITY_ATTRIBUTES sa = { 0 };
	STARTUPINFO         si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	HANDLE              hPipeOutputRead = NULL;
	HANDLE              hPipeOutputWrite = NULL;
	HANDLE              hPipeInputRead = NULL;
	HANDLE              hPipeInputWrite = NULL;
	BOOL                bTest = 0;
	DWORD               dwNumberOfBytesRead = 0;
	wchar_t             szMsg[100];



	// Try and locate splint

	CStringW Errors;
	CString Compiler = MainPathRoot + VALIDATOR;

	FILE *f = fopen(Compiler, "r");  // try where the KMotionDLL was irst

	if (f == NULL)
	{
		DoErrMsg(Translate("Error Locating Cppcheck.exe code validator"));
		return 1;
	}
	fclose(f);


	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;




	// Create pipe for standard output redirection.
	CreatePipe(&hPipeOutputRead,  // read handle
		&hPipeOutputWrite, // write handle
		&sa,      // security attributes
		1000000   // number of bytes reserved for pipe - 0 default
	);

	// Create pipe for standard input redirection.
	CreatePipe(&hPipeInputRead,  // read handle
		&hPipeInputWrite, // write handle
		&sa,      // security attributes
		1000000   // number of bytes reserved for pipe - 0 default
	);

	// Make child process use hPipeOutputWrite as standard out,
	// and make sure it does not show on screen.
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = hPipeInputRead;
	si.hStdOutput = hPipeOutputWrite;
	si.hStdError = hPipeOutputWrite;
//ExcludeTranslate

	CString cmd;  // build command line
	CString BindTo, IncSrcPath1, IncSrcPath2, IncSrcPath3, Susppress, opts, templ;

	IncSrcPath1 = "-I \"" + MainPathRoot + "\\DSP_KFLOP\" ";
	IncSrcPath2 = "-I \"" + ExtractPath(Name) + "\" ";
	IncSrcPath3 = "-I \"" + MainPathRoot + "\\C Programs\" ";
	Susppress = "--suppressions-list=\"" + MainPathRoot + VALIDATOR_SUPPRESS + "\" ";
	opts = "--enable=all ";
	templ = "--template=\"{file}, line {line} : {severity} {id}\n{message}\n{code}\n\" ";
	
	cmd = Compiler + " \"" + Name + "\" " + opts + IncSrcPath1 + IncSrcPath2 + IncSrcPath3 + Susppress + templ;

//ResumeTranslate

	CreateProcess(
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


	bool Done = true;
	DWORD Timeout;

	do
	{
		// Wait for CONSPAWN to finish.
		Timeout = WaitForSingleObject(pi.hProcess, 3000);  // timeout in 30 seconds

		Done = Timeout == WAIT_OBJECT_0 || MessageBoxW(NULL, Translate("splint taking a long time for validation.  Continue waiting?"), L"KMotion", MB_YESNO) == IDNO;
	} while (!Done);

	DWORD exitcode;
	int result = GetExitCodeProcess(pi.hProcess, &exitcode);

	if (Timeout != WAIT_OBJECT_0)
	{
		exitcode = 0; // treat stop waiting as success
		TerminateProcess(pi.hProcess, 9999);
	}

	// Now test to capture DOS application output by reading
	// hPipeOutputRead.  Could also write to DOS application
	// standard input by writing to hPipeInputWrite.

	CString cs;
	char *s = cs.GetBuffer(10001);

	bTest = ReadFile(
		hPipeOutputRead,      // handle of the read end of our pipe
		s,					  // address of buffer that receives data
		10000,                // number of bytes to read
		&dwNumberOfBytesRead, // address of number of bytes read
		NULL                  // non-overlapped.
	);


	if (!bTest)
	{
		wsprintfW(szMsg, Translate("Error #%d reading Validation output."), GetLastError());
		DoErrMsg(szMsg);
		return 1;
	}

	// do something with data.
	s[dwNumberOfBytesRead] = 0;  // null terminate
	cs.ReleaseBuffer();
	Errors = cs;

	if (exitcode != 0)
	{
		Errors = Translate("Validation Failed!\n") + Errors;
	}


	if (Timeout != WAIT_OBJECT_0) Errors = Errors + Translate(" Validation timed out");



	// Close all remaining handles
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hPipeOutputRead);
	CloseHandle(hPipeInputWrite);

	/*----------Console application (CONSPAWN.EXE) code------*/

	if (Err && MaxErrLen > 0)
	{
		if (Errors.GetLength() >= MaxErrLen - 1) // too large to return?
			Errors = Errors.Left(MaxErrLen - 1); // yes, trim excess
		wcscpy_s(Err, MaxErrLen, Errors);
	}

	return exitcode;
#else
	return -1;
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
	if (BoardType == BOARD_TYPE_KOGNA)
		return USER_PROG_ADDRESS_KOGNA + (thread - 1) * MAX_USER_PROG_SIZE_KOGNA;
	else if (BoardType == BOARD_TYPE_KFLOP)
		return USER_PROG_ADDRESS_KFLOP + (thread-1) * MAX_USER_PROG_SIZE_KFLOP;
	else
	{
		DoErrMsg(Translate("Error Board Type"));
		return (0xfffffff);
	}
}

// To avoid GUI deadlocks if called from GUI and not essential to determine
// the Board Type set Wait (default) to work on best effort manner


int CKMotionDLL::CheckKMotionVersion(int *type, bool GetBoardTypeOnly, bool Wait) 
{
	int result;
	char BoardVersion[MAX_LINE];
	char CoffVersion[81];
	char OutFile[MAX_PATH];
	char ms[MAX_LINE];

	if (type) *type = BOARD_TYPE_UNKNOWN;

	if (Wait)
		result = WaitToken("CheckKMotionVersion");
	else
		result = KMotionLock("CheckKMotionVersion");

	if (result == KMOTION_LOCKED)  // see if we can get access
	{
		// Get the firmware date from the KMotion Card which
		// will be in PT (Pacific Time)
		ReleaseToken();
		result = WriteLineReadLine("Version",BoardVersion);

		if (result) return result;

		// now get date stamp from firmware .out file
		//PH TODO check for first 4 charachters not just anywhere in string
		//This should solve this. If resultpointer is the same as input the string starts with KFLOP
		//if(strstr(BoardVersion,"KFLOP") == BoardVersion)
		if(strstr(BoardVersion,"KOGNA") != NULL)
		//if (BoardVersion.Find("KOGNA")==0)
		{
			kmx::getDspFile(OutFile, BOARD_TYPE_KOGNA);
			if (type) *type = BOARD_TYPE_KOGNA;
		}
		else
		{
			kmx::getDspFile(OutFile, BOARD_TYPE_KFLOP);
			if (type) *type = BOARD_TYPE_KFLOP;
		}

		if (GetBoardTypeOnly) return 0;

		int result = ExtractCoffVersionString(OutFile,CoffVersion);

		if (result)
		{
			snprintf(ms, MAX_LINE, "Error Extracting Version Information from file" LINE_BREAK LINE_BREAK " %s",OutFile);
			DoErrMsg(ms);
			return 1;
		}

		CoffVersion[strlen(CoffVersion)-1] = '\0'; //remove trailing \n
		

		// check if they match exactly
		if (strcmp(CoffVersion,BoardVersion) == 0) return 0;
		//TODO check for first 4 charachters not just anywhere in string
		if(strstr(BoardVersion,"KFLOP") != NULL)
			snprintf(ms, MAX_LINE, "DSP_KFLOP.out Date Stamp Doesn't match KFLOP Firmware" LINE_BREAK LINE_BREAK
				" Before compiling programs please use Flash/Config Screen and select:" LINE_BREAK
				" Download New Version.  This will install compatible Firmware with" LINE_BREAK
				" this version of software" LINE_BREAK LINE_BREAK
				" %s  KFLOP Firmware" LINE_BREAK
				" %s  DSP_KFLOP.out file",
				BoardVersion,
				CoffVersion);
		else
			snprintf(ms, MAX_LINE, "DSP_KMotion.out Date Stamp Doesn't match KMotion Firmware" LINE_BREAK LINE_BREAK
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
	FILHDR  file_hdr; //PH local variable insted of global
	AOUTHDR o_filehdr; //PH local variable insted of global
	int have_aout = 0;

	*size_text = *size_bss = *size_data = *size_total = 0;
	
	f = fopen(InFile,"rb");

	if (!f) return 1;

	if (fread(&file_hdr, FILHSZ, 1, f) != 1) return 1;

/*-------------------------------------------------------------------------*/
/* MAKE SURE THIS IS REALLY A COFF FILE.                                   */ 
/*-------------------------------------------------------------------------*/
	if (file_hdr.f_magic != MAGIC)
	{
		fclose(f);
		// its, not try as Elf
		return CheckElfSize(InFile, size_text, size_bss, size_data, size_total);
	}

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

#define MAX_SECTIONS 200
#define SHT_PROGBITS 1
#define SHF_ALLOC 2

int CKMotionDLL::CheckElfSize(const char* InFile, int* size_text, int* size_bss, int* size_data, int* size_total)
{
	FILE* f;
	int i;
	unsigned int VersionAddress = 0;
	Elf32_Ehdr file_hdr;                   /* FILE HEADER STRUCTURE              */
	Elf32_Shdr sect_hdr, const_hdr, symbol_hdr, strtab_hdr;
	unsigned int start_text = 0, end_text = 0, start_bss = 0, end_bss = 0, start_data = 0, end_data = 0;
	unsigned int min, max;

	f = fopen(InFile, "rb");

	if (!f) return 1;

	if (fread(&file_hdr, sizeof(file_hdr), 1, f) != 1) return 1;

	// check for ELF file
	char* p = (char*)&file_hdr;
	if (p[1] != 'E' || p[2] != 'L' || p[3] != 'F')
	{
		fclose(f);
		return 1;  // not an elf file
	}

	// find the offset to the string table
	// read in the String section header
	if (fseek(f, file_hdr.e_shoff + file_hdr.e_shstrndx * file_hdr.e_shentsize, SEEK_SET)) return 1;
	if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) return 1;

	int HeaderStringsOffset = sect_hdr.sh_offset;
	int HeaderStringTableSize = sect_hdr.sh_size;

	char* HeaderStringTable = (char*)malloc(HeaderStringTableSize + 1);
	if (fseek(f, HeaderStringsOffset, SEEK_SET)) { delete HeaderStringTable;  return 1; }
	if (fread(HeaderStringTable, HeaderStringTableSize, 1, f) != 1) { delete HeaderStringTable;  return 1; }


	//find 3 section headers
	const_hdr.sh_name = strtab_hdr.sh_name = symbol_hdr.sh_name = 0;

	if (fseek(f, file_hdr.e_shoff, SEEK_SET)) return 1;  // seek to section headers
	for (i = 0; i < file_hdr.e_shnum; i++)
	{
		if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) { delete HeaderStringTable;  return 1; }

		if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".const") == 0)
			const_hdr = sect_hdr;
		else if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".strtab") == 0)
			strtab_hdr = sect_hdr;
		else if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".symtab") == 0)
			symbol_hdr = sect_hdr;

		if (const_hdr.sh_name != 0 && strtab_hdr.sh_name != 0 && symbol_hdr.sh_name != 0) break;
	}

	delete HeaderStringTable;

	if (i == file_hdr.e_shnum) return 1;

	// Read in main String table
	int StringsOffset = strtab_hdr.sh_offset;
	int StringTableSize = strtab_hdr.sh_size;

	char* StringTable = (char*)malloc(StringTableSize + 1);
	if (fseek(f, StringsOffset, SEEK_SET)) { delete StringTable;  return 1; }
	if (fread(StringTable, StringTableSize, 1, f) != 1) { delete StringTable;  return 1; }

	// read in all symbols
	Elf32_Sym* Syms = (Elf32_Sym*)malloc(symbol_hdr.sh_size * sizeof(Elf32_Sym));
	if (fseek(f, symbol_hdr.sh_offset, SEEK_SET)) { delete StringTable; delete Syms;  return 1; }  // seek to symbols
	if (fread(Syms, symbol_hdr.sh_size, 1, f) != 1) { delete StringTable; delete Syms;  return 1; }

	// go through the symbols looking for the Version 
	int count = 0;
	for (i = 0; i < (int)(symbol_hdr.sh_size / sizeof(Elf32_Sym)); i++)
	{
		char* name = StringTable + Syms[i].st_name;
		if (strcmp("__start_.text", name) == 0)  { start_text = Syms[i].st_value; count++; }
		if (strcmp("__stop_.text", name) == 0)  { end_text = Syms[i].st_value; count++; }
		if (strcmp("__start_.bss", name) == 0)  { start_bss = Syms[i].st_value; count++; }
		if (strcmp("__stop_.bss", name) == 0)  { end_bss = Syms[i].st_value; count++; }
		if (strcmp("__start_.data", name) == 0)  { start_data = Syms[i].st_value; count++; }
		if (strcmp("__stop_.data", name) == 0)  { end_data = Syms[i].st_value; count++; }
	}

	delete StringTable;
	delete Syms;
	fclose(f);

	*size_text = end_text - start_text;
	*size_bss = end_bss - start_bss;
	*size_data = end_data - start_data;


	min = 0xffffffff;
	if (start_text != 0 && min > start_text) min = start_text;
	if (start_bss != 0 && min > start_bss) min = start_bss;
	if (start_data != 0 && min > start_data) min = start_data;

	max = 0x0;
	if (end_text != 0 && max < end_text) max = end_text;
	if (end_bss != 0 && max < end_bss) max = end_bss;
	if (end_data != 0 && max < end_data) max = end_data;

	*size_total = max - min;
	return 0;
}



// Find special version string that is pointed to by symbol
//
// VersionAndBuildTime
//
// within the  .const section of the COFF file
//


int CKMotionDLL::ExtractElfVersionString(const char *InFile, char *Version)
{
	FILE *f;
	int i;
	unsigned int VersionAddress = 0;
	Elf32_Ehdr file_hdr;                   /* FILE HEADER STRUCTURE              */
	Elf32_Shdr sect_hdr, const_hdr, symbol_hdr, strtab_hdr;


	f = fopen(InFile, "rb");


	if (!f) return 1;

	if (fread(&file_hdr, sizeof(file_hdr), 1, f) != 1) return 1;

	// find the offset to the string table
	// read in the String section header
	if (fseek(f, file_hdr.e_shoff + file_hdr.e_shstrndx * file_hdr.e_shentsize, SEEK_SET)) return 1;
	if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) return 1;

	int HeaderStringsOffset = sect_hdr.sh_offset;
	int HeaderStringTableSize = sect_hdr.sh_size;

	char *HeaderStringTable = (char *)malloc(HeaderStringTableSize + 1);
	if (fseek(f, HeaderStringsOffset, SEEK_SET)) { delete HeaderStringTable;  return 1; }
	if (fread(HeaderStringTable, HeaderStringTableSize, 1, f) != 1) { delete HeaderStringTable;  return 1; }

	//find 3 section headers
	const_hdr.sh_name = strtab_hdr.sh_name = symbol_hdr.sh_name = 0;

	if (fseek(f, file_hdr.e_shoff, SEEK_SET)) return 1;  // seek to section headers
	for (i = 0; i < file_hdr.e_shnum; i++)
	{
		if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) { delete HeaderStringTable;  return 1; }

		if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".const") == 0)
			const_hdr = sect_hdr;
		else if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".strtab") == 0)
			strtab_hdr = sect_hdr;
		else if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".symtab") == 0)
			symbol_hdr = sect_hdr;

		if (const_hdr.sh_name != 0 && strtab_hdr.sh_name != 0 && symbol_hdr.sh_name != 0) break;
	}

	delete HeaderStringTable;

	if (i == file_hdr.e_shnum) return 1;

	// Read in main String table
	int StringsOffset = strtab_hdr.sh_offset;
	int StringTableSize = strtab_hdr.sh_size;

	char *StringTable = (char *)malloc(StringTableSize + 1);
	if (fseek(f, StringsOffset, SEEK_SET)) { delete StringTable;  return 1; }
	if (fread(StringTable, StringTableSize, 1, f) != 1) { delete StringTable;  return 1; }

	// read in all symbols
	Elf32_Sym *Syms = (Elf32_Sym *)malloc(symbol_hdr.sh_size * sizeof(Elf32_Sym));
	if (fseek(f, symbol_hdr.sh_offset, SEEK_SET)) { delete StringTable; delete Syms;  return 1; }  // seek to symbols
	if (fread(Syms, symbol_hdr.sh_size, 1, f) != 1) { delete StringTable; delete Syms;  return 1; }

	// go through the symbols looking for the Version 
	for (i = 0; i < (int)(symbol_hdr.sh_size / sizeof(Elf32_Sym)); i++)
	{
		if (strcmp(StringTable + Syms[i].st_name, "VersionAndBuildTime") == 0)
		{
			// found it
			// seek to it in .const
			if (fseek(f, Syms[i].st_value - const_hdr.sh_addr + const_hdr.sh_offset, SEEK_SET)) { delete StringTable; delete Syms;  return 1; }
			// read it
			if (fread(Version, 50, 1, f) != 1) { delete StringTable; delete Syms;  return 1; }
			delete StringTable;
			delete Syms;
			return 0;
		}
	}

	delete StringTable;
	delete Syms;
	return 0;
}


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

	// check for ELF file
	char *p = (char*)&file_hdr;
	if (p[1] == 'E' && p[2] == 'L' &&p[3] == 'F')
	{
		fclose(f);
		return ExtractElfVersionString(InFile, Version);
	}

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


int CKMotionDLL::ElfLoad(const char *InFile, unsigned int *EntryPoint, int PackToFlash)
{
	FILE *f;
	int i;
	unsigned int VersionAddress = 0;
	Elf32_Ehdr file_hdr;                   /* FILE HEADER STRUCTURE              */
	Elf32_Shdr sect_hdr, LoadSections[MAX_SECTIONS];
	int iLoadSections = 0;

	f = fopen(InFile, "rb");

	if (!f) return 1;

	if (fread(&file_hdr, sizeof(file_hdr), 1, f) != 1) return 1;

	// check for ELF file
	char *p = (char*)&file_hdr;
	if (p[1] != 'E' || p[2] != 'L' || p[3] != 'F')
	{
		fclose(f);
		return 1;  // not an elf file
	}

	// find the offset to the string table
	// read in the String section header
	if (fseek(f, file_hdr.e_shoff + file_hdr.e_shstrndx * file_hdr.e_shentsize, SEEK_SET)) return 1;
	if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) return 1;

	int HeaderStringsOffset = sect_hdr.sh_offset;
	int HeaderStringTableSize = sect_hdr.sh_size;

	char *HeaderStringTable = (char *)malloc(HeaderStringTableSize + 1);
	if (fseek(f, HeaderStringsOffset, SEEK_SET)) { delete HeaderStringTable;  return 1; }
	if (fread(HeaderStringTable, HeaderStringTableSize, 1, f) != 1) { delete HeaderStringTable;  return 1; }


	*EntryPoint = file_hdr.e_entry; // return the entry point

	//identify all the sections that have data to be placed in memory

	if (fseek(f, file_hdr.e_shoff, SEEK_SET)) return 1;  // seek to section headers
	for (i = 0; i < file_hdr.e_shnum; i++)
	{
		if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) { delete HeaderStringTable;  return 1; }

		if (sect_hdr.sh_type == SHT_PROGBITS && (sect_hdr.sh_flags & SHF_ALLOC) == SHF_ALLOC)
		{
			if (iLoadSections == MAX_SECTIONS)
			{
				MessageBoxW(NULL, Translate("Too many Sections in ELF File"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
				return 1;
			}
			LoadSections[iLoadSections++] = sect_hdr;
		}
	}

	delete HeaderStringTable;
	
	// now load them all
	for (i = 0; i < iLoadSections; i++)
	{
		if (fseek(f, LoadSections[i].sh_offset, SEEK_SET)) return 1;  // seek to data

		// allocate memory for data
		unsigned char *buffer = (unsigned char *)malloc(LoadSections[i].sh_size);
		if (!buffer)
		{
			MessageBoxW(NULL, Translate("Memory Allocation Error for ELF File"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			fclose(f);
			return 1;
		}

		// read the data
		if (fread(buffer, LoadSections[i].sh_size, 1, f) != 1) {delete buffer; fclose(f); return 1; }

		// write it to DSP
		if (!mem_write(buffer, LoadSections[i].sh_size, LoadSections[i].sh_addr, 0)) { delete buffer; fclose(f); return 1; }

		delete buffer;
	}

	return 0;
}

void CKMotionDLL::DoErrMsg(const char* s)
{
	std::wstring t = kmx::strtowstr(s);
	DoErrMsg(t.c_str());
}

void CKMotionDLL::DoErrMsg(const wchar_t *s)
{

	if (!ErrMessageDisplayed)
	{
		ErrMessageDisplayed=true;
		if (ErrMsgHandlerW)
		{
			try
			{
				ErrMsgHandlerW(s);
			}
			catch (...)
			{
				ErrMessageDisplayed=false;
			}
		}
		else
		{
			MessageBoxW(NULL,s,L"KMotion",MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
		}
		ErrMessageDisplayed=false;
	}
}
//PH convenience function
void CKMotionDLL::DoErrMsg(std::wstring s){
	DoErrMsg(s.c_str());
}
//PH convenience function
void CKMotionDLL::DoErrMsg(std::string s){
	DoErrMsg(s.c_str());
}

int CKMotionDLL::GetStatus(MAIN_STATUS& status, bool lock)
{ 
    int word, i,result,n,token;
	char s[2570]; // big enough bufferfor  256 x 9 char hex strings 
	char *sp = s;
	int *p=(int *)&status;

	n = sizeof(status) / sizeof(int);

	memset(p, 0, sizeof(status));  // clear all

	if (!ReadStatus)
	{
		if (!lock)ReleaseToken();  // was already locked?
		return 0;  // if wrong status version exit silently  
	}

	if(lock)
	{
		token = WaitToken( false, 100, "GetStatus");
		if (token != KMOTION_LOCKED) return 1;
	}

	// KMotion is available read the status
	snprintf(s, 2570, "GetStatus");
	if (WriteLine(s))
	{
		ReleaseToken();
		return 1;
	}

	n=sizeof(status)/sizeof(int);
	memset(s,'\0',2570);

	for (i=0; i<n; i++)
	{
		if (*sp == '\0') //string is empty time to read next line
		{
			if (ReadLineTimeOut(s,5000))  // big enough bufferfor  256 x 9 char hex strings 
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
		result = sscanf(sp,"%8x",&word);

		if (result!=1)
		{
			ReleaseToken();
			return 1;
		}

		if (ReadStatus)  // only fill structure if version matches
			*p++ = word;

		if (strlen(sp) >=9)
		{
			sp=sp+9;
		}
		else
		{
			ReleaseToken();
			return 1;
		}

		// check Version if first word
		if (i==0)
		{
			if (n != (status.VersionAndSize & 0xffff))
			{
				ReadStatus = false;

				// update number of words to read to avoid getting out of sync
				n = status.VersionAndSize & 0xffff;
			}
		}
	} 
	ReleaseToken(); 

	if (!ReadStatus)  // Wrong Version?
	{
		// note this may throw an exception and not return in .NET App
		// note this may throw an exception and not return in .NET App
		DoErrMsg(Translate("Error Status Record Size mismatch\r\rStatus updates disabled\r\rFlash Compatible Firmware and restart"));
	}

	return 0;
}


#define FLASHWRITER "\\DSP_KOGNA\\ti_tools\\flash_writer\\sfh_OMAP-L138.exe"
//c:\KMotionSrcKogna\DSP_KOGNA\ti_tools\flash_writer\sfh_OMAP-L138.exe -targettype C6748_LCDK -flashtype NAND -v -p COM10 -flash_noubl C:\KMotionSrcKogna\DSP_KOGNA\Debug\DSPKOGNA.bin
int CKMotionDLL::FlashKognaCOM(const char* Com)
{
#ifndef _KMOTIONX
	SECURITY_ATTRIBUTES sa = { 0 };
	STARTUPINFO         si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	HANDLE              hPipeOutputRead = NULL;
	HANDLE              hPipeOutputWrite = NULL;
	HANDLE              hPipeInputRead = NULL;
	HANDLE              hPipeInputWrite = NULL;
	BOOL                bTest = 0;
	DWORD               dwNumberOfBytesRead = 0;
	wchar_t             szMsg[100];

	CString Errors;



	// Try and locate the TI Flash Writer
	CString FlashWriter = MainPathRoot + FLASHWRITER;

	FILE *f = fopen(FlashWriter, "r");  // try rel where the KMotionDLL is

	if (f == NULL)
	{
		DoErrMsg(Translate("Error Locating ") + FLASHWRITER);
		return 1;
	}
	fclose(f);


	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// Make child process use hPipeOutputWrite as standard out,
	// and allow to show on screen.
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_SHOW;
	si.hStdInput = NULL;
	si.hStdOutput = NULL;
	si.hStdError = hPipeOutputWrite;

	CString cmd;  // build command line
	CString BindTo, IncSrcPath1, IncSrcPath2, IncSrcPath3;

	cmd.Format(" -targettype C6748_LCDK -flashtype NAND -v -p %s -flash_noubl %s\\DSP_KOGNA\\DSPKOGNA.bin", Com, MainPathRoot);
	cmd = FlashWriter + cmd;

	CreateProcess(
		NULL,
		cmd.GetBuffer(0),
		NULL, NULL,
		TRUE, 0,
		NULL, NULL,
		&si, &pi);


	// Wait for CONSPAWN to finish.
	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitcode;
	int result = GetExitCodeProcess(pi.hProcess, &exitcode);

	// Now test to capture DOS application output by reading
	// hPipeOutputRead.  Could also write to DOS application
	// standard input by writing to hPipeInputWrite.

	if (exitcode != 0)
	{
		wsprintfW(szMsg, Translate("Error #%d Executing Flash over COM Port\r\rMake sure %s is present and not in use by another App"), GetLastError(), (CStringW)Com);
		DoErrMsg(szMsg);
		return 1;
	}
	return 0;
#else
	return -1;
#endif
}

std::wstring CKMotionDLL::Translate(std::string s)
{
		return Trans.Translate(s);
}
