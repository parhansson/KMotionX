#include "KMotionDLL.h"


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
#include "Elf.h"
#ifdef _KMOTIONX
#include <algorithm>
#include <cwctype>
#endif

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
	PipeMutex = new CMutex(FALSE, L"KMotionPipe", NULL);

	ConsoleHandler = NULL;
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


int CKMotionDLL::WaitToken(const char* CallerID)
{
	return WaitToken(false, 100000, CallerID);
}

int CKMotionDLL::WaitToken(bool display_msg, int TimeOut_ms, const char *CallerID)
{
	CHiResTimer Timer;
	int result;

//
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

		DoErrMsg(Translate("Can't Connect to KMotion Board") + s);

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

void CKMotionDLL::Console(const wchar_t *buf)
{
    if (ConsoleHandler)
        ConsoleHandler(buf);
}

int CKMotionDLL::LoadCoff(int Thread, const char* Name, int PackToFlash)
{
	CString wName = Name;

	return LoadCoff(Thread, wName, PackToFlash);
}


// Note: ALL User Thread Numbers start with 1

int CKMotionDLL::LoadCoff(int Thread, const wchar_t *Name, int PackToFlash)
{
	CStringA s;
	unsigned int EntryPoint;

	if (Thread==0) return 1;
#ifndef SIMULATE_LOADCOFF
	if (PackToFlash==0 && CheckKMotionVersion()) return 1; 

	if (PackToFlash==0)
	{
		s.Format("Kill %d", Thread);  // make sure the Thread isn't running
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
		s.Format("EntryPoint%d %X",Thread,EntryPoint);
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
		    Console(kmx::strtowstr(cons[i]).c_str());
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
		PipeMutex->Unlock();
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
	return kmx::LaunchServer(NoEthernet);
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

	if (NoEthernet)
		cmd += " -no_ethernet";	
	
	if (!CreateProcess (
		NULL,
		cmd.GetBuffer(0), 
		NULL, NULL,
		TRUE, 0,
		NULL, NULL,
		&si, &pi))
	{
		ServerMessDisplayed = true;
		CString Errmsg;
		Errmsg.Format("Unable to execute:\r\r" + cmd + Translate("\r\rTry re-installing software or copy this file to the same location as KMotion.exe or Calling Application"));
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

int CKMotionDLL::CompileAndLoadCoff(const wchar_t *Name, int Thread)
{
	return CompileAndLoadCoff(Name, Thread, (wchar_t *)NULL, 0);
}


int CKMotionDLL::CompileAndLoadCoff(const wchar_t* Name, int Thread, wchar_t* Err, int MaxErrLen)
{
	int result,BoardType;
	wchar_t OutFile[MAX_PATH];

	if (Thread<=0 || Thread>7) 
	{
		if(Err) {
			CString s;
			s.Format(Translate("Invalid Thread Number %d Valid Range (1-7)"),Thread);
			wcscpy_s(Err, MaxErrLen, s);
		}
		return 1;
	}
	
	ConvertToOut(Thread,Name,OutFile,MAX_PATH);

	if (CheckKMotionVersion(&BoardType)) return 1;

	if (BoardType == BOARD_TYPE_UNKNOWN)  // Can't determine board type?
	{
		if(Err && MaxErrLen > 0) {
			wcscpy_s(Err, MaxErrLen, Translate("Unable to determine Controller Board Type"));
		}
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

int CKMotionDLL::Compile(const wchar_t* Name, const wchar_t* OutFile, const int BoardType, int Thread, wchar_t* Err, int MaxErrLen)
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

	FILE* f;
	_tfopen_s(&f, Name, _T("rt,ccs=UTF-8"));

	if (f)
	{
		CString s;
		fgetws(s.GetBufferSetLength(200), 200, f);
		s.ReleaseBuffer();
		fclose(f);

		RemoveComments(s);
		s.TrimLeft();
		int i = s.Find(L"#pragma");
		if (i >= 0)
		{
			int k = s.Find(L"TI_COMPILER",i);
			if (k > i)
			{
				return CompileTI(Name, OutFile, BoardType, Thread, Err, MaxErrLen);
			}
		}
	}


	// Try and locate the TCC67 Compiler
	CString Compiler = MainPathDLL + COMPILER;
	CString OFile=OutFile;


	_tfopen_s(&f, Compiler, _T("rt,ccs=UTF-8")); // try in the DLL directory first

	if (f==NULL)
	{
		Compiler = MainPath + "\\Release" + COMPILER;  // try in the Release directory next
	
		_tfopen_s(&f, Compiler, _T("rt,ccs=UTF-8"));
		if (f==NULL)
		{
			Compiler = MainPath + "\\Debug" + COMPILER;  // try in the Debug directory next

			_tfopen_s(&f, Compiler, _T("rt,ccs=UTF-8")); 
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
		CStringA ErrorsA;
		char *s = ErrorsA.GetBuffer(10001);
		
		bTest=ReadFile(
			hPipeOutputRead,      // handle of the read end of our pipe
			s,                    // address of buffer that receives data
			10000,                // number of bytes to read
			&dwNumberOfBytesRead, // address of number of bytes read
			NULL                  // non-overlapped.
			);

		
		if (!bTest)
		{
			wsprintf(szMsg, Translate("Error #%d reading compiler output."),GetLastError());
			DoErrMsg(szMsg);
			return 1;
		}

		// do something with data.
		s[dwNumberOfBytesRead] = 0;  // null terminate
		ErrorsA.ReleaseBuffer();
		Errors = ErrorsA;  // convert to wide char
	}


	
	// Close all remaining handles
	CloseHandle (pi.hProcess);
    CloseHandle (pi.hThread);
	CloseHandle (hPipeOutputRead);
	CloseHandle (hPipeInputWrite);
	
	/*----------Console application (CONSPAWN.EXE) code------*/

	if (Err)
	{
		if (MaxErrLen > 0)
		{
			if (Errors.GetLength() >= MaxErrLen-1) // too large to return?
				Errors = Errors.Left(MaxErrLen-1); // yes, trim excess
			wcscpy_s(Err, MaxErrLen, Errors);
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
	      wcscpy_s(Err, MaxErrLen, kmx::strtowstr(strerror(exitCode)));
	    return exitCode;
	}
	if (Err)
	{
	  // The compiler writes narrow (UTF-8) text, read it as bytes and convert to wide
	  std::string output(MaxErrLen > 0 ? MaxErrLen-1 : 0, '\0');
	  size_t len = fread(&output[0], 1, output.size(), out);
	  output.resize(len);
	  wcscpy_s(Err, MaxErrLen, kmx::strtowstr(output));
	  if(len > 0){
			DoErrMsg(Err);
			DoErrMsg(command); //Show command that failed. Lazy me, should concat messages
	    debug("fread: %s\n",output.c_str());
	  }
	}
	exitCode = pclose(out);
	if (exitCode < 0) {
	    exitCode = errno;
	    debug("pclose failed: %s\n",strerror(exitCode));
      if (Err)
        wcscpy_s(Err, MaxErrLen, kmx::strtowstr(strerror(exitCode)));
  }

	return exitCode;
#endif
}
#ifndef _KMOTIONX
void CKMotionDLL::RemoveComments(CString &s)
{
	int i = s.Find(L"//");
	if (i >= 0)
		s = s.Left(i);

	i = s.Find(L"/*");
	if (i >= 0)
	{
		s = s.Left(s.GetLength() - i);

		int k = s.Find(L"*/", i);

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


// Convert all isolated CR to CR LF
int CKMotionDLL::ConvertCRToCRLF(const CString filePath)
{
	try
	{
		// Open the file for reading
		CFile file;
		if (!file.Open(filePath, CFile::modeRead | CFile::typeBinary))
		{
			CString errorMsg;
			errorMsg.Format(L"Failed to open the file: %s", filePath);
			DoErrMsg(errorMsg);
			return 1; // Failed to open the file
		}

		// Get the file size and allocate a buffer
		UINT fileSize = (UINT)file.GetLength();
		char* buffer = new char[fileSize];
		file.Read(buffer, fileSize);
		file.Close();

		// Allocate a temporary buffer for the updated content
		// Assume the worst case where every character is '\r' and needs an additional '\n'
		char* updatedBuffer = new char[fileSize * 2];
		UINT updatedSize = 0;

		// Process the content to replace isolated CR with CR LF
		for (size_t i = 0; i < fileSize; ++i)
		{
			if (buffer[i] == '\r')
			{
				// Check if the next character is not LF
				if (i + 1 >= fileSize || buffer[i + 1] != '\n')
				{
					updatedBuffer[updatedSize++] = '\r';
					updatedBuffer[updatedSize++] = '\n'; // Add LF after CR
				}
				else
				{
					updatedBuffer[updatedSize++] = '\r'; // Keep CR as is
				}
			}
			else
			{
				updatedBuffer[updatedSize++] = buffer[i]; // Copy other characters
			}
		}

		if (updatedSize != fileSize)
		{
			CString errorMsg;
			errorMsg.Format(Translate("File contains line endings not CR LF.  Change Line Endings?\r\r%s"), filePath);
			if (MessageBox(NULL, errorMsg, L"KMotion", MB_YESNO | MB_SYSTEMMODAL) == IDYES)
			{
				// User chose to proceed with the conversion
				// Open the file for writing
				if (!file.Open(filePath, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary))
				{
					delete[] buffer;
					delete[] updatedBuffer;
					MessageBox(NULL, Translate("Failed to open the file for writing"), L"KMotion", MB_OK | MB_SYSTEMMODAL);
					return 2; // Failed to open the file for writing
				}

				// Write the updated content back to the file
				file.Write(updatedBuffer, updatedSize);
				file.Close();
			}
		}

		// Clean up
		delete[] buffer;
		delete[] updatedBuffer;

		return 0; // Success
	}
	catch (...)
	{
		CString errorMsg;
		errorMsg.Format(L"Failed to open the file: %s", filePath);
		DoErrMsg(errorMsg);
		return 3;
	}
}

// Function to remove UTF-8 BOM from a file
int CKMotionDLL::RemoveUTF8BOM(const CString filePath)
{
	// Define the UTF-8 BOM
	unsigned char BOM[3] = { 0xEF, 0xBB, 0xBF };

	// Open the file for reading in binary mode
	FILE* file = _wfopen(filePath, L"rb");
	if (!file)
	{
		CString errorMsg;
		errorMsg.Format(L"Failed to open the file: %s", filePath);
		DoErrMsg(errorMsg);
		return 1;
	}

	// Read the first 3 bytes
	unsigned char buffer[3];
	size_t bytesRead = fread(buffer, 1, 3, file);
	if (bytesRead < 3)
	{
		fclose(file);
		return 0;  // no BOM found
	}

	// Check if the file contains a UTF-8 BOM
	if (memcmp(buffer, BOM, 3) == 0)
	{
		// Get the file size
		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		fseek(file, 3, SEEK_SET); // Skip the BOM

		// Allocate buffer for the rest of the file
		unsigned char* fileContent = new unsigned char[fileSize - 3];
		bytesRead = fread(fileContent, 1, fileSize - 3, file);
		fclose(file);

		if (bytesRead != fileSize - 3)
		{
			delete[] fileContent;
			return 1;
		}

		// Open the file for writing in binary mode
		file = _wfopen(filePath, L"wb");
		if (!file)
		{
			CString errorMsg;
			errorMsg.Format(L"Failed to open the file for writing to remove UTF-8 BOM: %s", filePath);
			DoErrMsg(errorMsg);
			delete[] fileContent;
			return 1;
		}

		// Write the file content without the BOM
		size_t bytesWritten = fwrite(fileContent, 1, bytesRead, file);
		fclose(file);
		delete[] fileContent;

		if (bytesWritten != bytesRead)
		{
			return 1;
		}
	}
	else
	{
		// Close the file if no BOM is found
		fclose(file);
	}
	return 0;
}


// Function to remove UTF-8 BOM from all included files in a given C++ file
int CKMotionDLL::RemoveBOMandIncludedFiles(const CString& FilePath, const CList<CString, CString&>& IncludePaths)
{
	if (RemoveUTF8BOM(FilePath)) return 1;
	if (ConvertCRToCRLF(FilePath)) return 1;

	FILE* cFile = _wfopen(FilePath, L"r");
	if (!cFile)
	{
		CString errorMsg;
		errorMsg.Format(L"Failed to open file: %s", FilePath);
		DoErrMsg(errorMsg);
		return 1;
	}

	char line[1024];
	char includePrefix[] = "#include \"";
	size_t includePrefixLen = strlen(includePrefix);

	while (fgets(line, sizeof(line), cFile))
	{
		if (strncmp(line, includePrefix, includePrefixLen) == 0)
		{
			char* start = line + includePrefixLen;
			char* end = strchr(start, '"');
			if (end)
			{
				*end = '\0';
				CString includedFile = start;

				// Iterate through the include paths to find the full path of the included file
				POSITION pos = IncludePaths.GetHeadPosition();
				while (pos != NULL)
				{
					CString includePath = IncludePaths.GetNext(pos);
					CString fullPath = includePath + L"\\" + includedFile;

					if (_waccess(fullPath, 0) == 0) // Check if the file exists
					{
						if (RemoveBOMandIncludedFiles(fullPath, IncludePaths))
						{
							fclose(cFile);
							return 1;
						}
						break;
					}
				}
			}
		}
	}

	fclose(cFile);
	return 0;
}
#endif
int CKMotionDLL::CompileTI(const wchar_t * Name, const wchar_t * OutFile, const int BoardType, int Thread, wchar_t * Err, int MaxErrLen)
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

	CList <CString, CString&> IncludePaths;

	int Opt = 0; // default no optimization
	int MaxSize, BoardMaxSize;

	if (BoardType == BOARD_TYPE_KFLOP)
	{
		MaxSize = MAX_USER_PROG_SIZE_KFLOP;
		BoardMaxSize = MAX_USER_PROG_SIZE_KFLOP * (7 - Thread) + MAX_USER_PROG_SIZE_KFLOP_7;
		if (Thread == 7) MaxSize = MAX_USER_PROG_SIZE_KFLOP_7;
	}
	else
	{
		MaxSize = MAX_USER_PROG_SIZE_KOGNA;
		BoardMaxSize = MAX_USER_PROG_SIZE_KOGNA * (7 - Thread) + MAX_USER_PROG_SIZE_KOGNA_7;
		if (Thread == 7) MaxSize = MAX_USER_PROG_SIZE_KOGNA_7;
	}


	FILE *f = _wfopen(Name, L"rt");

	if (f)
	{
		CString s;
		fgetws(s.GetBufferSetLength(200), 200, f);
		s.ReleaseBuffer();
		fclose(f);

		RemoveComments(s);
		s.TrimLeft();
		int i = s.Find(L"#pragma");
		if (i >= 0)
		{
			int k = s.Find(L"TI_COMPILER", i);
			if (k > i)
			{
				CString sOpt, sMaxSize;
				int r1 = 1, r2 = 1, i1, i2, i3, i0;
				i0 = s.Find(L"(", k);  // look for (Opt) or (Opt,MaxSize)
				if (i0 > k)
				{
					i1 = s.Find(L",", i0);  // look for (Opt) or (Opt,MaxSize)
					if (i1 > i0)
					{
						i2 = s.Find(L")", i1);  //  (Opt,MaxSize)
						if (i2 > i1)
						{
							sOpt = s.Mid(i0 + 1, i1 - i0 - 1);
							r1 = swscanf(sOpt, L"%d", &Opt);
							sMaxSize = s.Mid(i1 + 1, i2 - i1 - 1);

							i3 = sMaxSize.Find(L"0x");
							if (i3 >= 0)
							{	// hex value
								sMaxSize = sMaxSize.Right(sMaxSize.GetLength() - i3 - 2);
								r2 = swscanf(sMaxSize, L"%x", &MaxSize);
							}
							else
							{	// decimal value  
								sMaxSize = s.Mid(i1 + 1, i2 - i1 - 1);
								r2 = swscanf(sMaxSize, L"%d", &MaxSize);
							}
						}
					}
					else
					{
						r1 = 0;
						i1 = s.Find(L")", i0);  // (Opt)
						if (i1 > i0)
						{
							sOpt = s.Mid(i0 + 1, i1 - i0 - 1);
							r1 = swscanf(sOpt, L"%d", &Opt);
						}
					}
				}


				if (r1 != 1 || r2 != 1 || Opt < 0 || Opt > 3 || MaxSize <= 0 || MaxSize > BoardMaxSize)
				{
					// invalid format pragma TI_COMPILER

					Errors = Translate("Error Line :1: invalid TI_COMPILER pragma \n") + s +
						Translate("\n Expected format:\n#pragma TI_COMPILER\n#pragma TI_COMPILER(opt level 0-3)\n#pragma TI_COMPILER(opt level 0-3, Max Thread Space - hex or decimal)");

					if (MaxSize > BoardMaxSize)
						Errors += "\n\nInvalid Max Thread Space";

					wcscpy_s(Err, MaxErrLen, Errors);
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

	_tfopen_s(&f, Compiler, _T("rt,ccs=UTF-8"));

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
		IncSrcPath1 = MainPathRoot + "\\DSP_KOGNA";
	}
	else
	{
		IncSrcPath1 = MainPathRoot + "\\DSP_KFLOP";
	}

	IncSrcPath2 = ExtractPath(Name);

	IncSrcPath3 = MainPathRoot + "\\C Programs";

	IncludePaths.AddTail(IncSrcPath1);
	IncludePaths.AddTail(IncSrcPath2);
	IncludePaths.AddTail(IncSrcPath3);
	if (RemoveBOMandIncludedFiles(Name, IncludePaths)) return 1;

// must use far data model as User programs call Firmware that uses its Near Data Pointer
	cmd.Format(L" %ls -k -q -as --diag_suppress=163 --mem_model:data=far "
		"-i \"" + IncSrcPath1 + "\" -i \"" + IncSrcPath2 + "\" -i \"" + IncSrcPath3 + "\" -mu -ml3 -mv6710 -o%d", FileType, Opt);
	cmd = Compiler + cmd + " \"" + Name + "\" --obj_directory=\"" + ExtractPath(Name) + "\" --asm_directory=\"" + ExtractPath(Name) + "\"";


	// Check for non-ANCII characters in the command line
	int nUTF8Length = WideCharToMultiByte(CP_UTF8, 0, cmd, cmd.GetLength(), NULL, 0, NULL, NULL);

	if (nUTF8Length != cmd.GetLength())
	{
		DoErrMsg(Translate("Error non-ANCI filenames or folder names not supported by TI Compiler"));
		return 1;
	}



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

	CStringA cs;
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
	Errors = UTF82W(cs, cs.GetLength());


	// Close all remaining handles
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hPipeOutputRead);
	CloseHandle(hPipeInputWrite);

	if (!bTest && GetLastError() != ERROR_BROKEN_PIPE && GetLastError() != ERROR_INVALID_PARAMETER) // Note broken pipe just means there was nothing to read
	{
		wsprintfW(szMsg, Translate("Error #%d reading compiler output."), GetLastError());
		DoErrMsg(szMsg);
		return 1;
	}


	if (exitcode == 0)  // compile successful ?
	{
		CString LinkMessages;
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

int CKMotionDLL::LinkTI(const wchar_t * Linker, const wchar_t * Name, const wchar_t * OutFile, const int BoardType, int Thread, wchar_t * Err, int MaxErrLen, int MaxSize)
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

	CString Errors;

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

	CStringA LinkTemplateDataA;
	CString LinkTemplateData;

	FILE *f;
	_tfopen_s(&f, MainPathRoot + LinkTemplate, _T("rt"));

	if (!f)
	{
		DoErrMsg(Translate("Unable to open TI Link Template.\n\n") + MainPathRoot + LinkTemplate);
		return 1;
	}

	int n = (int)fread_s(LinkTemplateDataA.GetBufferSetLength(100000), 100000, 1, 100000, f);
	LinkTemplateDataA.ReleaseBufferSetLength(n);
	LinkTemplateData = LinkTemplateDataA;
	fclose(f);


	CString MapFile, ObjFile = Name;
	n = ObjFile.ReverseFind('.');
	if (n > 0) ObjFile.Delete(n, ObjFile.GetLength() - n);
	MapFile = ObjFile + ".map";
	ObjFile = ObjFile + ".obj";


	// Find IRAM_END in symbols file
	_tfopen_s(&f, MainPathRoot + Symbols, _T("rt"));

	if (!f)
	{
		DoErrMsg(Translate("Error unable to open DSP Symbol file.\n\n") + MainPathRoot + Symbols);
		return 1;
	}
	CStringA SymbolsData;
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
	int r = swscanf(IramEnd, L"%x", &iRamEnd);
	if (r != 1)
	{
		DoErrMsg(Translate("Error unable to find IRAM_END sysmbol to determine IRAM size"));
		return 1;
	}

//ExcludeTranslate
	CString IR,IRL,TS,TL;
	IR.Format(L"0x%08x", iRamEnd);
	if (BoardType == BOARD_TYPE_KFLOP)
		IRL.Format(L"0x%08x", 0x10020000-iRamEnd);
	else
		IRL.Format(L"0x%08x", 0x11840000 - iRamEnd);
	TS.Format(L"0x%08x", GetLoadAddress(Thread, BoardType));
	TL.Format(L"0x%08x", MaxSize);

	LinkTemplateData.Replace(L"{OBJECTFILE}", "\"" + ObjFile + "\"");
	LinkTemplateData.Replace(L"{MAPFILE}", "\"" + MapFile + "\"");
	LinkTemplateData.Replace(L"{OUTPUTFILE}", "\"" + (CString)OutFile + "\"");
	LinkTemplateData.Replace(L"{IRAMSTART}", IR);
	LinkTemplateData.Replace(L"{IRAMLENGTH}", IRL);
	LinkTemplateData.Replace(L"{THREADSTART}", TS);
	LinkTemplateData.Replace(L"{THREADLENGTH}", TL);
	if (BoardType == BOARD_TYPE_KFLOP)
		LinkTemplateData.Replace(L"{DSP_KFLOP_PATH}", MainPathRoot + "\\DSP_KFLOP\\");
	else
		LinkTemplateData.Replace(L"{DSP_KFLOP_PATH}", MainPathRoot + "\\DSP_KOGNA\\");
//ResumeTranslate

	_tfopen_s(&f, MainPathRoot + LinkCmd, _T("wt"));

	if (!f)
	{
		DoErrMsg(Translate("Unable to open TI Link Command File") + MainPathRoot + LinkCmd);
		return 1;
	}

	LinkTemplateDataA = W2UTF8(LinkTemplateData, LinkTemplateData.GetLength());

	fputs(LinkTemplateDataA, f);
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

	CStringA cs;
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

int CKMotionDLL::ValidateC(const wchar_t *Name, wchar_t *Err, int MaxErrLen, int BoardType)
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


	// check if User program is valid C with a main function (not some include file)
	FILE* f;  
	_tfopen_s(&f, Name, _T("rt,ccs=UTF-8"));

	if (f != NULL)
	{
		bool found = false;
		while (!feof(f))
		{
			CString s;
			fgetws(s.GetBufferSetLength(2000), 2000, f);
			s.ReleaseBuffer();
			RemoveComments(s);
			int i = s.Find(L"main");
			if (i >= 0)
			{
				// remove up to main
				s = s.Mid(i+4);
				// skip over white space
				s = s.TrimLeft();
				if (s.GetLength() >0 && s[0] == '(')
				{
					// found main function
					found = true;
					break;
				}
			}
		}
		fclose(f);
		if (!found)
		{
			DoErrMsg(Translate("No main() function in file.  Only Programs can be Validated not #include files."));
			return 1;
		}
	}


	// Try and locate clang-tidy

	CString Errors;
	CString Compiler = MainPathRoot + VALIDATOR;

	// Check in clang folder
	_tfopen_s(&f, Compiler, _T("rt,ccs=UTF-8"));

	if (f == NULL)
	{
		DoErrMsg(Translate("Error Locating clang-tidy.exe code validator"));
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
	CString BindTo, IncSrcPath1, IncSrcPath2, IncSrcPath3, Susppress, opts, checks;

	// add any addition User Options to the command line
	_tfopen_s(&f, MainPathRoot + VALIDATEOPTIONS, _T("rt,ccs=UTF-8"));

	if (f != NULL)
	{
		fgetws(opts.GetBufferSetLength(2000), 2000, f);
		opts.ReleaseBuffer();
		fclose(f);
	}


	if (BoardType == BOARD_TYPE_KOGNA)
		IncSrcPath1 = "-I\"" + MainPathRoot + "\\DSP_KOGNA\" ";
	else
		IncSrcPath1 = "-I\"" + MainPathRoot + "\\DSP_KFLOP\" ";
	IncSrcPath2 = "-I\"" + ExtractPath(Name) + "\" ";
	IncSrcPath3 = "-I\"" + MainPathRoot + "\\C Programs\" ";
	checks = " --config-file=\"" + MainPathRoot + VALIDATECHECKS + "\" ";

	cmd = Compiler + " \"" + Name + "\"" + checks + opts + " " + IncSrcPath1 + IncSrcPath2 + IncSrcPath3;

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

	CStringA cs;
	char *s = cs.GetBuffer(10001);

	bTest = ReadFile(
		hPipeOutputRead,      // handle of the read end of our pipe
		s,					  // address of buffer that receives data
		10000,                // number of bytes to read
		&dwNumberOfBytesRead, // address of number of bytes read
		NULL                  // non-overlapped.
	);


	if (!bTest && GetLastError() != ERROR_BROKEN_PIPE)
	{
		wsprintfW(szMsg, Translate("Error #%d reading Validation output."), GetLastError());
		DoErrMsg(szMsg);
		return 1;
	}

	// do something with data.
	s[dwNumberOfBytesRead] = 0;  // null terminate
	cs.ReleaseBuffer();

	Errors = UTF82W(cs, cs.GetLength());

	if (exitcode != 0 && Errors == "")
	{
		Errors = Translate("Validation Failed!\n") + Errors;
	}
	
	if (Errors == "")
	{
		Errors = Translate("Validation found no Issues\n");
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
#ifndef _KMOTIONX
CStringA CKMotionDLL::W2UTF8(const wchar_t* pszText, int nLength)
{
	//First call the function to determine how much space we need to allocate
	int nUTF8Length = WideCharToMultiByte(CP_UTF8, 0, pszText, nLength, NULL, 0, NULL, NULL);

	//If the calculated length is zero, then ensure we have at least room for a NULL terminator
	if (nUTF8Length == 0)
		nUTF8Length = 1;

	//Now recall with the buffer to get the converted text
	CStringA sUTF;
	char* pszUTF8Text = sUTF.GetBuffer(nUTF8Length + 1); //include an extra byte because we may be null terminating the string ourselves
	int nCharsWritten = WideCharToMultiByte(CP_UTF8, 0, pszText, nLength, pszUTF8Text, nUTF8Length, NULL, NULL);

	//Ensure we NULL terminate the text if WideCharToMultiByte doesn't do it for us
	if (nLength != -1)
	{
		AFXASSUME(nCharsWritten <= nUTF8Length);
		pszUTF8Text[nCharsWritten] = '\0';
	}

	sUTF.ReleaseBuffer();

	return sUTF;
}


CString CKMotionDLL::UTF82W(const char* pszText, int nLength)
{
	//First call the function to determine how much space we need to allocate
	int nWideLength = MultiByteToWideChar(CP_UTF8, 0, pszText, nLength, NULL, 0);

	//If the calculated length is zero, then ensure we have at least room for a NULL terminator
	if (nWideLength == 0)
		nWideLength = 1;

	//Now recall with the buffer to get the converted text
	CString sWideString;
	wchar_t* pszWText = sWideString.GetBuffer(nWideLength + 1); //include an extra byte because we may be null terminating the string ourselves
	int nCharsWritten = MultiByteToWideChar(CP_UTF8, 0, pszText, nLength, pszWText, nWideLength);

	//Ensure we NULL terminate the text if MultiByteToWideChar doesn't do it for us
	if (nLength != -1)
	{
		AFXASSUME(nCharsWritten <= nWideLength);
		pszWText[nCharsWritten] = '\0';
	}

	sWideString.ReleaseBuffer();

	return sWideString;
}
#endif



void CKMotionDLL::ConvertToOut(int thread, const wchar_t *InFile, wchar_t *OutFile, int MaxLength)
{
#ifndef _KMOTIONX
	CString OFile;
	CString IFile=InFile;

	CString InFileWithCase=InFile;
	CString ThreadString;

	if (thread>0)
	{

		ThreadString.Format(L"(%d).out",thread);

		IFile.MakeLower();

		if (IFile.Right(2)= ".c")
		{
			OFile = InFileWithCase.Left(InFileWithCase.GetLength()-2);
		}
		else if (IFile.Find(L".txt")!=-1)
		{
			OFile = InFileWithCase.Left(InFileWithCase.GetLength()-4);
		}
		else if (IFile.Find(L".cpp")!=-1)
		{
			OFile = InFileWithCase.Left(InFileWithCase.GetLength()-4);
		}
		else
		{
			OFile = InFileWithCase;
		}

		OFile += ThreadString;
	}

	wcsncpy(OutFile,OFile,MaxLength);
#else

	std::wstring OFile;

	if (thread > 0) {
		OFile = InFile;

		const auto pos = OFile.find_last_of(L'.');
		if (pos != std::wstring::npos) {
			std::wstring suffix = OFile.substr(pos);

			std::transform(suffix.begin(), suffix.end(), suffix.begin(),
						[](wchar_t c) {
							return static_cast<wchar_t>(std::towlower(c));
						});

			if (suffix == L".c" ||
				suffix == L".txt" ||
				suffix == L".cpp") {
				OFile.erase(pos);
			}
		}

		OFile += L"(" + std::to_wstring(thread) + L").out";
	}

	if (MaxLength > 0) {
		std::wcsncpy(OutFile, OFile.c_str(), MaxLength - 1);
		OutFile[MaxLength - 1] = L'\0';
	}
#endif
}

CString CKMotionDLL::ExtractPath(CString InFile)
{
	int next_pos=0,pos;

	CString OutFile;

	do
	{
		pos=next_pos;
		next_pos = InFile.Find(PATH_SEPARATOR,pos+1);
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
		if (Wait)
			ReleaseToken();
		else
			PipeCmd(ENUM_ReleaseToken);  // KMotionLock took only the server's token, not PipeMutex
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

		int result = ExtractCoffVersionString(kmx::strtowstr(OutFile).c_str(),CoffVersion);

		if (result)
		{
			snprintf(ms, MAX_LINE, "Error Extracting Version Information from file" LINE_BREAK LINE_BREAK " %s",OutFile);
			DoErrMsg(ms);
			return 1;
		}

		if (char *nl = strchr(CoffVersion, '\n')) *nl = '\0'; //remove trailing \n
		

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

int CKMotionDLL::CheckCoffSize(const wchar_t *InFile, int *size_text, int *size_bss, int *size_data, int *size_total)
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
	
	f = _wfopen(InFile,L"rb");

	if (!f) return 1;

	if (fread(&file_hdr, FILHSZ, 1, f) != 1) {fclose(f); return 1;}

/*-------------------------------------------------------------------------*/
// MAKE SURE THIS IS REALLY A COFF FILE. 
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
		if (fread(&o_filehdr, AOUTSZ, 1, f) != 1) {fclose(f); return 1;}
		have_aout = 1;
	}

	// first read the string table

	if (fseek(f,file_hdr.f_symptr + file_hdr.f_nsyms * SYMESZ,SEEK_SET)) {fclose(f); return 1;}
	if (fread(&str_size, sizeof(str_size), 1, f) != 1) { 
		// No strings.  Since don't support -g compiler option on Linux, a lot of 
		// things can be missing.  We won't find the start/stop symbols either. 
		Coff_str_table = NULL; 
	} 
    else { 
		Coff_str_table = (char *)malloc(str_size); 
 
		if (fread(Coff_str_table, str_size-4, 1, f) != 1) {free(Coff_str_table); fclose(f); return 1;}; 
	} 

	// read/process all the symbols

	// seek back to symbols

	if (file_hdr.f_nsyms) {

		if (fseek(f,file_hdr.f_symptr,SEEK_SET)) {free(Coff_str_table); fclose(f); return 1;};

		for (i=0; i< file_hdr.f_nsyms; i++)
		{
			if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); fclose(f); return 1;};

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
				if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); fclose(f); return 1;};
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

int CKMotionDLL::CheckElfSize(const wchar_t* InFile, int* size_text, int* size_bss, int* size_data, int* size_total)
{
	FILE* f;
	int i;
	unsigned int VersionAddress = 0;
	Elf32_Ehdr file_hdr;                   /* FILE HEADER STRUCTURE              */
	Elf32_Shdr sect_hdr, const_hdr, symbol_hdr, strtab_hdr;
	unsigned int start_text = 0, end_text = 0, start_bss = 0, end_bss = 0, start_data = 0, end_data = 0;
	unsigned int min, max;

	f = _wfopen(InFile, L"rb");

	if (!f) return 1;

	if (fread(&file_hdr, sizeof(file_hdr), 1, f) != 1) {fclose(f); return 1;}

	// check for ELF file
	char* p = (char*)&file_hdr;
	if (p[1] != 'E' || p[2] != 'L' || p[3] != 'F')
	{
		fclose(f);
		return 1;  // not an elf file
	}

	// find the offset to the string table
	// read in the String section header
	if (fseek(f, file_hdr.e_shoff + file_hdr.e_shstrndx * file_hdr.e_shentsize, SEEK_SET)) {fclose(f); return 1;}
	if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) {fclose(f); return 1;}

	int HeaderStringsOffset = sect_hdr.sh_offset;
	int HeaderStringTableSize = sect_hdr.sh_size;

	char* HeaderStringTable = (char*)malloc(HeaderStringTableSize + 1);
	if (fseek(f, HeaderStringsOffset, SEEK_SET)) { delete HeaderStringTable;  fclose(f); return 1; }
	if (fread(HeaderStringTable, HeaderStringTableSize, 1, f) != 1) { delete HeaderStringTable;  fclose(f); return 1; }


	//find 3 section headers
	const_hdr.sh_name = strtab_hdr.sh_name = symbol_hdr.sh_name = 0;

	if (fseek(f, file_hdr.e_shoff, SEEK_SET)) { delete HeaderStringTable;  fclose(f); return 1; };  // seek to section headers
	for (i = 0; i < file_hdr.e_shnum; i++)
	{
		if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) { delete HeaderStringTable;  fclose(f); return 1; }

		if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".const") == 0)
			const_hdr = sect_hdr;
		else if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".strtab") == 0)
			strtab_hdr = sect_hdr;
		else if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".symtab") == 0)
			symbol_hdr = sect_hdr;

		if (const_hdr.sh_name != 0 && strtab_hdr.sh_name != 0 && symbol_hdr.sh_name != 0) break;
	}

	delete HeaderStringTable;

	if (i == file_hdr.e_shnum) {fclose(f); return 1;}

	// Read in main String table
	int StringsOffset = strtab_hdr.sh_offset;
	int StringTableSize = strtab_hdr.sh_size;

	char* StringTable = (char*)malloc(StringTableSize + 1);
	if (fseek(f, StringsOffset, SEEK_SET)) { delete StringTable;  fclose(f); return 1; }
	if (fread(StringTable, StringTableSize, 1, f) != 1) { delete StringTable;  fclose(f); return 1; }

	// read in all symbols
	Elf32_Sym* Syms = (Elf32_Sym*)malloc(symbol_hdr.sh_size * sizeof(Elf32_Sym));
	if (fseek(f, symbol_hdr.sh_offset, SEEK_SET)) { delete StringTable; delete Syms;  fclose(f); return 1; }  // seek to symbols
	if (fread(Syms, symbol_hdr.sh_size, 1, f) != 1) { delete StringTable; delete Syms;  fclose(f); return 1; }

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


int CKMotionDLL::ExtractElfVersionString(const wchar_t *InFile, char *Version)
{
	FILE *f;
	int i;
	unsigned int VersionAddress = 0;
	Elf32_Ehdr file_hdr;                   /* FILE HEADER STRUCTURE              */
	Elf32_Shdr sect_hdr, const_hdr, symbol_hdr, strtab_hdr;


	f = _wfopen(InFile, L"rb");


	if (!f) return 1;

	if (fread(&file_hdr, sizeof(file_hdr), 1, f) != 1) {fclose(f); return 1;}

	// find the offset to the string table
	// read in the String section header
	if (fseek(f, file_hdr.e_shoff + file_hdr.e_shstrndx * file_hdr.e_shentsize, SEEK_SET)) {fclose(f); return 1;}
	if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) {fclose(f); return 1;}

	int HeaderStringsOffset = sect_hdr.sh_offset;
	int HeaderStringTableSize = sect_hdr.sh_size;

	char *HeaderStringTable = (char *)malloc(HeaderStringTableSize + 1);
	if (fseek(f, HeaderStringsOffset, SEEK_SET)) { delete HeaderStringTable;  fclose(f); return 1; }
	if (fread(HeaderStringTable, HeaderStringTableSize, 1, f) != 1) { delete HeaderStringTable; fclose(f); return 1; }

	//find 3 section headers
	const_hdr.sh_name = strtab_hdr.sh_name = symbol_hdr.sh_name = 0;

	if (fseek(f, file_hdr.e_shoff, SEEK_SET)) {delete HeaderStringTable; fclose(f); return 1;}  // seek to section headers
	for (i = 0; i < file_hdr.e_shnum; i++)
	{
		if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) { delete HeaderStringTable;  fclose(f); return 1; }

		if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".const") == 0)
			const_hdr = sect_hdr;
		else if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".strtab") == 0)
			strtab_hdr = sect_hdr;
		else if (strcmp(HeaderStringTable + sect_hdr.sh_name, ".symtab") == 0)
			symbol_hdr = sect_hdr;

		if (const_hdr.sh_name != 0 && strtab_hdr.sh_name != 0 && symbol_hdr.sh_name != 0) break;
	}

	delete HeaderStringTable;

	if (i == file_hdr.e_shnum) {fclose(f); return 1;}

	// Read in main String table
	int StringsOffset = strtab_hdr.sh_offset;
	int StringTableSize = strtab_hdr.sh_size;

	char *StringTable = (char *)malloc(StringTableSize + 1);
	if (fseek(f, StringsOffset, SEEK_SET)) { delete StringTable;  fclose(f); return 1; }
	if (fread(StringTable, StringTableSize, 1, f) != 1) { delete StringTable;  fclose(f); return 1; }

	// read in all symbols
	Elf32_Sym *Syms = (Elf32_Sym *)malloc(symbol_hdr.sh_size * sizeof(Elf32_Sym));
	if (fseek(f, symbol_hdr.sh_offset, SEEK_SET)) { delete StringTable; delete Syms;  fclose(f); return 1; }  // seek to symbols
	if (fread(Syms, symbol_hdr.sh_size, 1, f) != 1) { delete StringTable; delete Syms;  fclose(f); return 1; }

	// go through the symbols looking for the Version 
	for (i = 0; i < (int)(symbol_hdr.sh_size / sizeof(Elf32_Sym)); i++)
	{
		if (strcmp(StringTable + Syms[i].st_name, "VersionAndBuildTime") == 0)
		{
			// found it
			// seek to it in .const
			if (fseek(f, Syms[i].st_value - const_hdr.sh_addr + const_hdr.sh_offset, SEEK_SET)) { delete StringTable; delete Syms;  fclose(f); return 1; }
			// read it
			if (fread(Version, 50, 1, f) != 1) { delete StringTable; delete Syms;  fclose(f); return 1; }
			fclose(f);
			delete StringTable;
			delete Syms;
			return 0;
		}
	}

	delete StringTable;
	delete Syms;
	fclose(f);
	return 0;
}


int CKMotionDLL::ExtractCoffVersionString(const wchar_t *InFile, char *Version)
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

	f = _wfopen(InFile, L"rb");

	if (!f) return 1;

	if (fread(&file_hdr, FILHSZ, 1, f) != 1) {fclose(f); return 1;}

	if (fread(&o_filehdr, sizeof(o_filehdr), 1, f) != 1) {fclose(f); return 1;}

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
		if (fread(&sect_hdr, SCNHSZ, 1, f) != 1) {fclose(f); return 1;}

		if (strcmp(".const",sect_hdr.s_name)==0) break;  // found it?
	}

	if (i==file_hdr.f_nscns) {fclose(f); return 1;}


	// now read the string table

	if (fseek(f,file_hdr.f_symptr + file_hdr.f_nsyms * SYMESZ,SEEK_SET)) {fclose(f); return 1;}
	if (fread(&str_size, sizeof(int), 1, f) != 1) {fclose(f); return 1;}

	Coff_str_table = (char *)malloc(str_size);

	if (fread(Coff_str_table, str_size-4, 1, f) != 1) {free(Coff_str_table); fclose(f); return 1; }

	// read/process all the symbols

	// seek back to symbols

	if (fseek(f,file_hdr.f_symptr,SEEK_SET)) {free(Coff_str_table); fclose(f); return 1; }

	for (i=0; i< file_hdr.f_nsyms; i++)
	{
		if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); fclose(f); return 1; }

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
			if (fread(&csym, SYMESZ, 1, f) != 1) {free(Coff_str_table); fclose(f); return 1; }
			i++;
		}
	}


	free(Coff_str_table); 

	if (VersionAddress==0) {fclose(f); return 1;}

	// compute file offset for the string

	int offset = sect_hdr.s_scnptr +         // file pointer to data
		(VersionAddress - sect_hdr.s_paddr); // plus offset - physical address


	if (fseek(f,offset,SEEK_SET)) {fclose(f); return 1;}
	
	for (i=0; i<80; i++)
	{
		if (fread(Version+i, sizeof(char), 1, f) != 1) {fclose(f); return 1;}
		if (Version[i]==0) break;
	}

	fclose(f);

	if (i==80) return 1;
	
	return 0;
}


int CKMotionDLL::ElfLoad(const wchar_t *InFile, unsigned int *EntryPoint, int PackToFlash)
{
	FILE *f;
	int i;
	unsigned int VersionAddress = 0;
	Elf32_Ehdr file_hdr;                   /* FILE HEADER STRUCTURE              */
	Elf32_Shdr sect_hdr, LoadSections[MAX_SECTIONS];
	int iLoadSections = 0;

	f = _wfopen(InFile, L"rb");

	if (!f) return 1;

	if (fread(&file_hdr, sizeof(file_hdr), 1, f) != 1) {fclose(f); return 1;}

	// check for ELF file
	char *p = (char*)&file_hdr;
	if (p[1] != 'E' || p[2] != 'L' || p[3] != 'F')
	{
		fclose(f);
		return 1;  // not an elf file
	}

	// find the offset to the string table
	// read in the String section header
	if (fseek(f, file_hdr.e_shoff + file_hdr.e_shstrndx * file_hdr.e_shentsize, SEEK_SET)) {fclose(f); return 1;}
	if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) {fclose(f); return 1;}

	int HeaderStringsOffset = sect_hdr.sh_offset;
	int HeaderStringTableSize = sect_hdr.sh_size;

	char *HeaderStringTable = (char *)malloc(HeaderStringTableSize + 1);
	if (fseek(f, HeaderStringsOffset, SEEK_SET)) { delete HeaderStringTable;  fclose(f); return 1; }
	if (fread(HeaderStringTable, HeaderStringTableSize, 1, f) != 1) { delete HeaderStringTable;  fclose(f); return 1; }


	*EntryPoint = file_hdr.e_entry; // return the entry point

	//identify all the sections that have data to be placed in memory

	if (fseek(f, file_hdr.e_shoff, SEEK_SET)) { delete HeaderStringTable; fclose(f); return 1;}  // seek to section headers
	for (i = 0; i < file_hdr.e_shnum; i++)
	{
		if (fread(&sect_hdr, sizeof(sect_hdr), 1, f) != 1) { delete HeaderStringTable;  fclose(f); return 1; }

		if (sect_hdr.sh_type == SHT_PROGBITS && (sect_hdr.sh_flags & SHF_ALLOC) == SHF_ALLOC)
		{
			if (iLoadSections == MAX_SECTIONS)
			{
				MessageBoxW(NULL, Translate("Too many Sections in ELF File"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
				delete HeaderStringTable;  
				fclose(f);
				return 1;
			}
			LoadSections[iLoadSections++] = sect_hdr;
		}
	}

	delete HeaderStringTable;
	
	// now load them all
	for (i = 0; i < iLoadSections; i++)
	{
		if (fseek(f, LoadSections[i].sh_offset, SEEK_SET)) {fclose(f); return 1;}  // seek to data

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

	fclose(f);
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
		if (ErrMsgHandler)
		{
			try
			{
				ErrMsgHandler(s);
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

int CKMotionDLL::GetStatus(MAIN_STATUS& status, bool lock)
{ 
    int word, i,result,n,token;
	CStringA s; 
	int *p=(int *)&status;

	n = sizeof(status) / sizeof(int);

	memset(p, 0, sizeof(status));  // clear all

	// Only release the token if we took it here. With lock false the caller holds it and releases it
	if (!ReadStatus)
	{
		return 0;  // if wrong status version exit silently  
	}

	if(lock)
	{
		token = WaitToken( false, 100, "GetStatus");
		if (token != KMOTION_LOCKED) return 1;
	}

	// KMotion is available read the status
	s.Format("GetStatus");  
	if (WriteLine(s))
	{
		if (lock) ReleaseToken();
		return 1;
	}


	s.Empty();

	for (i=0; i<n; i++)
	{
		if (s.IsEmpty())
		{
			if (ReadLineTimeOut(s.GetBuffer(2570),5000))  // big enough bufferfor  256 x 9 char hex strings 
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
		result = sscanf(s.GetBuffer(0),"%8X", &word);

		if (result!=1)
		{
			if (lock) ReleaseToken();
			return 1;
		}

		if (ReadStatus)  // only fill structure if version matches
			*p++ = word;

		if (s.GetLength() >=9)
		{
			s.Delete(0,9);
		}
		else
		{
			if (lock) ReleaseToken();
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
	if (lock) ReleaseToken(); 

	if (!ReadStatus)  // Wrong Version?
	{
		// note this may throw an exception and not return in .NET App
		// note this may throw an exception and not return in .NET App
		DoErrMsg(Translate("Error Status Record Size mismatch\r\rStatus updates disabled\r\rFlash Compatible Firmware and restart"));
	}

	return 0;
}


int CKMotionDLL::FlashKognaCOM(const char* Com)
{
	CString ComW = Com;
	return FlashKognaCOM(ComW);
}
#define FLASHWRITER "\\DSP_KOGNA\\ti_tools\\flash_writer\\sfh_OMAP-L138.exe"
//c:\KMotionSrcKogna\DSP_KOGNA\ti_tools\flash_writer\sfh_OMAP-L138.exe -targettype C6748_LCDK -flashtype NAND -v -p COM10 -flash_noubl C:\KMotionSrcKogna\DSP_KOGNA\Debug\DSPKOGNA.bin
int CKMotionDLL::FlashKognaCOM(const wchar_t* Com)
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

	FILE *f = _wfopen(FlashWriter, L"r");  // try rel where the KMotionDLL is

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

	cmd.Format(L" -targettype C6748_LCDK -flashtype NAND -v -p %ls -flash_noubl %ls\\DSP_KOGNA\\DSPKOGNA.bin", Com, MainPathRoot);
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
		wsprintfW(szMsg, Translate("Error #%d Executing Flash over COM Port\r\rMake sure %ls is present and not in use by another App"), GetLastError(), Com);
		DoErrMsg(szMsg);
		return 1;
	}
	return 0;
#else
	// The TI flash writer (sfh_OMAP-L138.exe) is only available for Windows
	DoErrMsg(Translate("Flashing a Kogna over a COM Port is not supported on this platform"));
	return 1;
#endif
}

CString CKMotionDLL::Translate(CString s)
{
		return Trans.Translate(s);
}
