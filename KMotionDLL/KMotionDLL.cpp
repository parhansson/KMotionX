// KMotionDLL.cpp : Defines the entry point for the DLL application.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include <vector>
#include "StdAfx.h"

#define KMotionBd 1

#include "PARAMS.H"
#include "VERSION.H"
#include "COFF.H"
#include "CLOAD.H"


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

#ifdef _KMOTIONX

    char* kmotionRoot;
    char* kmotionBin;
    char rootPath[MAX_PATH];
    char resolved_root[MAX_PATH];

    if(!(kmotionBin = getenv("KMOTION_BIN"))){
    		//No KMOTION_ROOT environment variable set.
    		//Assume user is in KMotion/bin directory
        	sprintf(rootPath,"%s",getenv("PWD"));
        	kmotionBin = realpath(rootPath, resolved_root);
        	//printf("%s:%d Environment KMOTION_BIN resolved to %s\n",__FILE__,__LINE__,kmotionBin);
    }
    sprintf(MainPath, "%s", kmotionBin);

    if(!(kmotionRoot = getenv("KMOTION_ROOT"))){
    		//No KMOTION_ROOT environment variable set.
    		//Assume user is in KMotion/bin directory
        	sprintf(rootPath,"%s/..",getenv("PWD"));
        	kmotionRoot = realpath(rootPath, resolved_root);
        	//printf("%s:%d Environment KMOTION_ROOT resolved to %s\n",__FILE__,__LINE__,kmotionRoot);

        	//Save resolved root, used by CoordMotion
        	//This results in Segmentation faults when calling getenv later on
        	//setenv("KMOTION_ROOT", kmotionRoot,0);
    }
    sprintf(MainPathRoot, "%s", kmotionRoot);
    
    strcpy(customCompiler, COMPILER);
    if (OLD_COMPILER != 0)
        strcpy(customOptions, "-g");
    else
        customOptions[0] = 0;
    tcc_vers = OLD_COMPILER ? 16 : 26;

#endif

	PipeMutex = new CMutex(FALSE,"KMotionPipe",0);
	ConsoleHandler=NULL;
	ErrMsgHandler=NULL;
    use_tcp = false;
    remote_tcp = false;
    first_status = true;
    poll_interest = 0;
    HostStatus = 0;
    sprintf(DSPKFLOP, "%s%cDSP_KFLOP%cDSPKFLOP.out",MainPathRoot,PATH_SEPARATOR,PATH_SEPARATOR);
    server_dir[0] = 0;
    compiler_dir[0] = 0;

}

CKMotionDLL::CKMotionDLL(int boardid)
{ 
	_init(boardid);
}

#ifdef _KMOTIONX
CKMotionDLL::CKMotionDLL(int boardid, unsigned int dfltport, const char * url)
{
	_init(boardid);
	use_tcp = true;
	tcp_port = dfltport;
	strncpy(hostname, url, sizeof(hostname)-1);
	hostname[sizeof(hostname)-1] = 0;
}
#endif

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
		    //printf("-1- %ld\n", getThreadId());
			return KMOTION_IN_USE;
		}

		if (Timer.Elapsed_Seconds() > 2.0 * TimeOut_ms * 0.001)
		{
			PipeMutex->Unlock();
		    //printf("-2- %ld\n", getThreadId());
			return KMOTION_IN_USE;
		}

		if (count++)
		{
			Sleep(10);
		}

		result = KMotionLock();

		if (result == KMOTION_IN_USE) {
		    //printf("-3- %ld\n", getThreadId());
			PipeMutex->Unlock();
		}
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

void CKMotionDLL::ErrMsg(const char *buf)
{
    if (ErrMsgHandler)
        ErrMsgHandler(buf);
    else
		AfxMessageBox(buf, MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);

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
	strcpy(d+8,s); 

	Pipe(d, strlen(s)+1+8 ,r, &m);

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


void CKMotionDLL::Terminate(int exit_code)
{
    // Default implementation just calls ::exit()
    ::exit(exit_code);
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
			    #define OPEN_ATTEMPTS 10
			    if (!use_tcp) {
				    // pipe won't open try to launch server
				    LaunchServer();
					Sleep(100);
				
				    for (i=0; i<OPEN_ATTEMPTS; i++) // try for a few secs
				    {
					    if (OpenPipe())
						    break;
					
					    Sleep(500);
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
#ifdef _KMOTIONX
		            throw std::system_error(ENXIO, std::system_category(), "Connect");
#else
                    Terminate(1);
#endif
				}
			}
		}

#ifdef _KMOTIONX	// Until we support tcp/ip under windows, use the old protocol (no length prefix)
        msglen = n;
		PipeFile.Write(&msglen, sizeof(msglen));   // Send the request length prefix
#endif
		PipeFile.Write(s, n);           // Send the request
		
		for (;;)
		{
#ifndef _KMOTIONX
			*m = PipeFile.Read(r, MAX_LINE + 1);     // Get the response
#else
		    len = 0;
		    while (len < sizeof(msglen))
		        len += PipeFile.Read((char *)(&msglen)+len, sizeof(msglen)-len);
			if (msglen > MAX_LINE + 1)
		        throw std::system_error(E2BIG, std::system_category(), "Read");
		    len = 0;
		    while (len < msglen)
			    len += PipeFile.Read(r+len, msglen-len);     // Get the response
            *m = len;
#endif
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
#ifndef _KMOTIONX
	catch (CFileException)
#else
	catch (std::system_error & serr)
#endif
	{
		EntryCount--;
		CFExcept = true;
		PipeMutex->Unlock();
#ifndef _KMOTIONX
		serr_msg = "Unable to Connect to KMotion Server";
#else
		serr_msg = serr.what();
#endif
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

		DoErrMsg(serr_msg);
		Terminate(1);
	}
	
	if (ReceivedErrMsg)
	{
		DoErrMsg(ErrorMsg);
	}
	
	return 0;
 }



void CKMotionDLL::SetServerDir(const char * dir)
{
    if (dir) {
        strncpy(server_dir, dir, sizeof(server_dir)-1);
        server_dir[sizeof(server_dir)-1] = 0;
    }
    else
        server_dir[0] = 0;
}

const char * CKMotionDLL::GetServerDir()
{
#ifndef _KMOTIONX
    return server_dir;  // May be empty string, which means in the normal DOS search path.
#else
    // Not DOS, so use either override path (from SetServerDir) or MainPath obtained from environ variable.
    if (server_dir[0])
        return server_dir;
    else
        return MainPath;
#endif
}

int CKMotionDLL::LaunchServer()
{
	char                cmd[sizeof(server_dir)+32];

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

	strcpy(cmd, GetServerDir());

    if (cmd[0])
        strcat(cmd, "\\");
	strcat(cmd, "KMotionServer.exe");
	
	if (!CreateProcess (
		NULL,
		cmd, 
		NULL, NULL,
		TRUE, 0,
		NULL, NULL,
		&si, &pi))
	{
		ServerMessDisplayed = true;
		DoErrMsg("Unable to execute:\r\rKMotionServer.exe\r\r"
			"Try re-installing software or copy this file to the same location as KMotion.exe");
		Terminate(1);
	}

	
	// Now that handles have been inherited, close it to be safe.
	// You don't want to read or write to them accidentally.
	CloseHandle(hPipeOutputWrite);
	CloseHandle(hPipeInputRead);
#elif defined(__APPLE__)
	//The daemon is currently not supported on Apple
    #ifdef _DEAMON
    //sprintf(command, "%s/%s", MainPath,"KMotionServer");
	printf("%s:%d Launch KMotionServer first!\n",__FILE__,__LINE__);
	PipeMutex->Unlock();
	Terminate(1);
    #else
    sprintf(cmd, "%s/%s", GetServerDir(),"KMotionServer &");
    system(cmd);
    #endif

#else
    #ifdef _DEAMON
    sprintf(cmd, "%s/%s", GetServerDir(),"KMotionServer");
    #else
    sprintf(cmd, "%s/%s", GetServerDir(),"KMotionServer &");
    #endif
    printf("%s:%d Launching KMotionServer %s\n",__FILE__,__LINE__, cmd);
    system(cmd);
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
		//TODO MaxErrLen should be honored to avoid segementation fault.
		if(Err) sprintf(Err,"Invalid Thread Number %d Valid Range (1-7)",Thread);
		return 1;
	}
	
	ConvertToOut(Thread,Name,OutFile,MAX_PATH);

	if (CheckKMotionVersion(&BoardType)) 
	{
		if(Err) sprintf(Err,"Board type mismatch");
	    return 1;
	}

	// Compile the C File

	result = Compile(Name,OutFile,BoardType,Thread,Err,MaxErrLen);
    if (Err && MaxErrLen) 
        Err[MaxErrLen-1]='\0'; 
	if (result)
	    return result;

	// Download the .out File

	return LoadCoff(Thread, OutFile);
}

#ifdef _KMOTIONX
void CKMotionDLL::SetCustomCompiler(const char * compiler, const char * options, int tcc_minor_version)
{
    if (compiler)
        strncpy(customCompiler, compiler, sizeof(customCompiler));
    else
        strcpy(customCompiler, COMPILER);
    if (options)
        strncpy(customOptions, options, sizeof(customOptions));
    else
        customOptions[0] = 0;
    if (tcc_minor_version)
        tcc_vers = tcc_minor_version;
}
#endif


void CKMotionDLL::SetCompilerDir(const char * dir)
{
    if (dir) {
        strncpy(compiler_dir, dir, sizeof(compiler_dir)-1);
        compiler_dir[sizeof(compiler_dir)-1] = 0;
    }
    else
        compiler_dir[0] = 0;
}

const char * CKMotionDLL::GetCompilerDir()
{
    FILE *f;
    
    if (compiler_dir[0])
        return compiler_dir;

#ifndef _KMOTIONX
    
    // Find and cache the compiler location
	CString Compiler = MainPathDLL + COMPILER;

	f=fopen(Compiler,"r");  // try where the KMotionDLL was irst

	if (f==NULL)
	{
		Compiler = MainPath + "\\Release" + COMPILER;
	
		f=fopen(Compiler,"r");  // try in the released directory next
		if (f==NULL)
		{
			Compiler = MainPath + "\\Debug" + COMPILER;
			f=fopen(Compiler,"r");  // try in the debug directory next
			if (f!=NULL)
		        strcpy(compiler_dir, MainPath + "\\Debug");
		}
		else
		    strcpy(compiler_dir, MainPath + "\\Release");
	}
	else
	    strcpy(compiler_dir, MainPath);
#else
	char Compiler[MAX_PATH +1];

    strcpy(Compiler, customCompiler);
	if (Compiler[0] == '/') {
	    // Custom compiler is absolute, break it into dir and file
	    strcpy(compiler_dir, Compiler);
	    *strrchr(compiler_dir, '/') = 0;
	    // Now custom compiler is file name part only...
	    strcpy(customCompiler, Compiler + strlen(compiler_dir) + 1);
	    // Have fixed path, so return it.
	    return compiler_dir;
	}

    f=fopen(Compiler,"r");  // try if compiler is in pwd
    if (f==NULL)
    {
	    sprintf(Compiler, "%s/%s", MainPath, customCompiler);
	    f=fopen(Compiler,"r");  // try in the released directory next
	    if (f==NULL)
	    {
	        if (tcc_vers < 26)
		        sprintf(Compiler, "%s/TCC67/%s", MainPathRoot, customCompiler);
		    else
		        sprintf(Compiler, "%s/KMotionX/tcc-0.9.26/%s", MainPathRoot, customCompiler);
		    f=fopen(Compiler,"r");  // try in the released directory next
		    if (f!=NULL) {
		        *strrchr(Compiler, '/') = 0;
                strcpy(compiler_dir, Compiler);
            }
	    }
	    else
            strcpy(compiler_dir, MainPathRoot);
    }
    else
        strcpy(compiler_dir, ".");  // It's in PWD
#endif
	if (f)
	    fclose(f);
	return compiler_dir;
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
	CString Compiler = CString(GetCompilerDir()) + COMPILER;
	CString OFile=OutFile;

	FILE *f=fopen(Compiler,"r");  // try where the KMotionDLL was first

	if (f==NULL)
	{
		DoErrMsg("Error Locating TCC67.exe Compiler");
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
		BindTo = DSPKFLOP;
;


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

    if (Err)
        *Err = 0;
        
	//normalize_slashes()

	if (Thread==0) return 1;
	char Compiler[MAX_PATH +1];
	int add_dot = 0;
    strcpy(Compiler, customCompiler);
	if (Compiler[0] == '/') {
	    FILE *f=fopen(Compiler,"r");  // try if compiler is on path
	    if (f==NULL)
	    {
		    DoErrMsg("Error Locating custom compiler (given absolute path)");
		    return 1;
	    }
	}
	else {
	    FILE *f=fopen(Compiler,"r");  // try if compiler is in pwd

	    if (f==NULL)
	    {
		    sprintf(Compiler, "%s/%s", MainPath, customCompiler);
		    f=fopen(Compiler,"r");  // try in the released directory next
		    if (f==NULL)
		    {
		        if (tcc_vers < 26)
			        sprintf(Compiler, "%s/TCC67/%s", MainPathRoot, customCompiler);
			    else
			        sprintf(Compiler, "%s/KMotionX/tcc-0.9.26/%s", MainPathRoot, customCompiler);
			    f=fopen(Compiler,"r");  // try in the released directory next
			    if (f==NULL)
			    {
				    DoErrMsg("Error Locating c67-tcc Compiler");
				    return 1;
			    }
		    }
	    }
	    else
	        // In in pwd, then add './' so that shell command can find it here.
	        add_dot = 1;
	    fclose(f);
    }

	char IncSrcPath1[MAX_PATH +1];
	char IncSrcPath2[MAX_PATH +1];
	char BindTo[MAX_PATH +1];
	char path[MAX_PATH +1];

	if (BoardType == BOARD_TYPE_KMOTION){
		sprintf(IncSrcPath1,"-I\"%s%cDSP_KMotion\"", MainPathRoot, PATH_SEPARATOR) ;
		sprintf(BindTo, "\"%s%cDSP_KMotion%cDSPKMotion.out\"", MainPathRoot,PATH_SEPARATOR,PATH_SEPARATOR);
	} else {
		sprintf(IncSrcPath1,"-I\"%s%cDSP_KFLOP\"", MainPathRoot, PATH_SEPARATOR) ;
		strcpy(BindTo, DSPKFLOP);
	}
	ExtractPath(Name,path);
	sprintf(IncSrcPath2,"-I\"%s\"", path) ;


	char command[MAX_LINE +1];

    if (tcc_vers < 26)
 	    snprintf(command, sizeof(command), "%s%s -text %08X %s -nostdinc %s %s -o \"%s\" \"%s\" %s 2>&1",
 	        add_dot ? "./" : "",
			Compiler,
			GetLoadAddress(Thread,BoardType),
			customOptions,
			IncSrcPath1,
			IncSrcPath2,
			OutFile,
			Name,
			BindTo);
    else
	    snprintf(command, sizeof(command), "%s%s -Wl,-Ttext,%08X %s -Wl,--oformat,coff -static -nostdinc -nostdlib %s %s -o \"%s\" \"%s\" %s 2>&1",
 	        add_dot ? "./" : "",
			Compiler,
			GetLoadAddress(Thread,BoardType),
			customOptions,
			IncSrcPath1,
			IncSrcPath2,
			OutFile,
			Name,
			BindTo);

	//Original TCC67 Windows version shipped with KMotion
	//tcc -text 80050000 -g -nostdinc -I./DSP_KFLOP -I./ -o Gecko3Axis.out Gecko3Axis.c ./DSP_KFLOP/DSP_KFLOP.out
	// -text replaced by -Wl,-Ttext,address
	//http://manpages.ubuntu.com/manpages/lucid/man1/tcc.1.html

	//compile with debug flag is currently not supported -g
	//c67-tcc -Wl,-Ttext,80050000 -Wl,--oformat,coff -static -nostdinc -nostdlib -I./ -o ~/Desktop/Gecko3AxisOSX.out Gecko3Axis.c DSPKFLOP.out
	debug("%s\n",command);
	//int exitCode = system(command);
	int exitCode = 0;
	FILE * out = popen(command, "r");
	if (!out) {
	    exitCode = errno;
	    strcpy(Err, strerror(exitCode));
	    return exitCode;
	}
	size_t len = fread(Err, 1, MaxErrLen-1, out);
	Err[len] = 0;
	exitCode = pclose(out);
	if (exitCode < 0) {
	    exitCode = errno;
	    strcpy(Err, strerror(exitCode));
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

	sprintf(ThreadString, "(%d).out",thread);

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


void CKMotionDLL::SetDSPKFLOP(const char * filename)
{
    strncpy(DSPKFLOP, filename, sizeof(DSPKFLOP)-1);
    DSPKFLOP[sizeof(DSPKFLOP)-1] = 0;
}


int CKMotionDLL::CheckKMotionVersion(int *type, bool GetBoardTypeOnly) 
{
	int result;
	char BoardVersion[MAX_LINE];
	char CoffVersion[81];
	char OutFile[MAX_PATH];
	char ms[MAX_LINE];

	if (type) *type = BOARD_TYPE_UNKNOWN;

	if (KMotionLock() == KMOTION_LOCKED)  // see if we can get access
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
			strcpy(OutFile,DSPKFLOP);
			if (type) *type = BOARD_TYPE_KFLOP;
		}
		else
		{
			sprintf(OutFile,"%s%cDSP_KMotion%cDSPKMotion.out",MainPathRoot,PATH_SEPARATOR,PATH_SEPARATOR);
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
				" %s  .out file",
				BoardVersion,
				CoffVersion);
		else
			sprintf(ms,"DSP_KMotion.out Date Stamp Doesn't match KMotion Firmware" LINE_BREAK LINE_BREAK
				" Before compiling programs please use Flash/Config Screen and select:" LINE_BREAK
				" Download New Version.  This will install compatible Firmware with" LINE_BREAK
				" this version of software" LINE_BREAK LINE_BREAK
				" %s  KMotion Firmware" LINE_BREAK
				" %s  .out file",
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
		token = WaitToken( false, 100);
		if (token != KMOTION_LOCKED) return 1;
	}

	// KMotion is available read the status
	sprintf(s,"GetStatus %x", HostStatus);
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

bool CKMotionDLL::GetLastStatus(MAIN_STATUS& status)
{
    if (first_status)
        return false;   // Not yet available
    status = last_status;
    return true;
}

void CKMotionDLL::SetHostStatus(int host_status, bool poll)
{
    HostStatus = host_status;
    if (poll)
        Poll();
}



template<class T, class F = T>
struct make_vec : public std::vector<T>
{
    make_vec(int n, F * values) : std::vector<T>(n)
    { 
        for (int i = 0; i < n; ++i)
            (*this)[i] = values[i];
    }
    virtual ~make_vec() {}
};


void CKMotionDLL::Poll()
{
    // Poll (using GetStatus) and call virtual functions to handle any changes.  This is mainly
    // intended for Python etc. bindings, since it is more efficient for the app to be notified
    // when things change, rather than having the high-level code have to go through the entire
    // status struct looking for changes.
    MAIN_STATUS m; 
    if (!GetStatus(m, true)) {
        unsigned int changes = first_status ? 0xFFFFFFFF : 0;
        if (!first_status) {
            changes |= memcmp(m.ADC, last_status.ADC, sizeof(m.ADC)) ? POLL_ADC : 0;
            changes |= memcmp(m.DAC, last_status.DAC, sizeof(m.DAC)) ? POLL_DAC : 0;
            changes |= memcmp(m.PWM, last_status.PWM, sizeof(m.PWM)) ? POLL_PWM : 0;
            changes |= memcmp(m.Position, last_status.Position, sizeof(m.Position)) ? POLL_POS : 0;
            changes |= memcmp(m.Dest, last_status.Dest, sizeof(m.Dest)) ? POLL_DEST : 0;
            changes |= memcmp(m.OutputChan0, last_status.OutputChan0, sizeof(m.OutputChan0)) ? POLL_OUTCHAN : 0;
            changes |= memcmp(&m.InputModes, &last_status.InputModes, sizeof(m.InputModes)*4) ? POLL_MODES : 0;
            changes |= m.Enables != last_status.Enables ? POLL_ENABLE : 0;
            changes |= m.AxisDone != last_status.AxisDone ? POLL_DONE : 0;
            changes |= memcmp(&m.BitsDirection[0], &last_status.BitsDirection[0], sizeof(m.BitsDirection[0])*10) ? POLL_IO : 0;
            changes |= m.ThreadActive != last_status.ThreadActive ? POLL_THREAD : 0;
            changes |= m.StopImmediateState != last_status.StopImmediateState ? POLL_STOP : 0;
            changes |= m.PC_comm[0] != last_status.PC_comm[0] ? POLL_PCCOMM : 0;
        }
        changes &= poll_interest;
        first_status = false;
        if (changes & POLL_ADC)
            ChangedADC(make_vec<int>(sizeof(m.ADC)/sizeof(m.ADC[0]), m.ADC));
        if (changes & POLL_DAC)
            ChangedDAC(make_vec<int>(sizeof(m.DAC)/sizeof(m.DAC[0]), m.DAC));
        if (changes & POLL_PWM)
            ChangedPWM(make_vec<int>(sizeof(m.PWM)/sizeof(m.PWM[0]), m.PWM));
        if (changes & POLL_POS)
            ChangedPos(make_vec<double>(sizeof(m.Position)/sizeof(m.Position[0]), m.Position));
        if (changes & POLL_DEST)
            ChangedDest(make_vec<double>(sizeof(m.Dest)/sizeof(m.Dest[0]), m.Dest));
        if (changes & POLL_OUTCHAN)
            ChangedOutChan(make_vec<int, unsigned char>(sizeof(m.OutputChan0)/sizeof(m.OutputChan0[0]), m.OutputChan0));
        if (changes & POLL_MODES)
            ChangedMode(m.InputModes, m.InputModes2, m.OutputModes, m.OutputModes2);
        if (changes & POLL_ENABLE)
            ChangedEnable(m.Enables);
        if (changes & POLL_DONE)
            ChangedDone(m.AxisDone);
        if (changes & POLL_IO) {
            // Populate bool vector so it is indexed by I/O bit number.
            boolvec outdir(KSTEP_OPTO_IN_BITS, false);   // FIXME: not best choice of constant, but we do bits up to KAnalog.
            boolvec state(KSTEP_OPTO_IN_BITS, false);
            for (unsigned int i = 0; i < KSTEP_OPTO_IN_BITS; ++i) {
                unsigned int j = 1u << (i & 0x1F);
                if (i < 32) {
                    outdir[i] = (m.BitsDirection[0] & j) != 0;
                    state[i] = (m.BitsState[0] & j) != 0;
                }
                else if (i < 64) {
                    outdir[i] = (m.BitsDirection[1] & j) != 0;
                    state[i] = (m.BitsState[1] & j) != 0;
                }
                else if (i < 96) {
                    outdir[i] = (m.SnapBitsDirection0 & j) != 0;
                    state[i] = (m.SnapBitsState0 & j) != 0;
                }
                else if (i < 128) {
                    outdir[i] = (m.SnapBitsDirection1 & j) != 0;
                    state[i] = (m.SnapBitsState1 & j) != 0;
                }
                else if (i < 128+16) {
                    state[i] = (m.KanalogBitsStateInputs & j) != 0;
                }
                else if (i < KSTEP_OPTO_IN_BITS) {
                    outdir[i] = true;
                    state[i] = (m.KanalogBitsStateOutputs & j) != 0;
                }
            }
            ChangedIO(outdir, state);
        }
        if (changes & POLL_THREAD)
            ChangedThread(m.ThreadActive);
        if (changes & POLL_STOP)
            ChangedStop(m.StopImmediateState);
        if (changes & POLL_PCCOMM)
            ChangedPCComm(make_vec<int>(sizeof(m.PC_comm)/sizeof(m.PC_comm[0]), m.PC_comm));
        last_status = m;
    }
}


void CKMotionDLL::SetPollCallback(unsigned int interest)
{
    poll_interest = interest;
}

void CKMotionDLL::SetMainPathRoot(const char * dir)
{
#ifdef _KMOTIONX
    strncpy(MainPathRoot, dir, sizeof(MainPathRoot)-1);
    MainPathRoot[sizeof(MainPathRoot)-1] = 0;
#else
    MainPathRoot = dir;
#endif
}

const char* CKMotionDLL::GetMainPathRoot() const
{
	return MainPathRoot;
}


