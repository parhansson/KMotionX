// KMotionDLL.h    --  KMotion DLL Interface class
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined (KMOTIONDLL_H)
#define KMOTIONDLL_H



#include <afxmt.h>
#include "../DSP_KFLOP/PC-DSP.h"
#include <SocketWrapper.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the KMOTIONDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// KMOTIONDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

// Generic helper definitions for shared library support http://gcc.gnu.org/wiki/Visibility
#if defined _WIN32 || defined __CYGWIN__
  #define KMOTIONDLL_HELPER_DLL_IMPORT __declspec(dllimport)
  #define KMOTIONDLL_HELPER_DLL_EXPORT __declspec(dllexport)
  #define KMOTIONDLL_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define KMOTIONDLL_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define KMOTIONDLL_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define KMOTIONDLL_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define KMOTIONDLL_HELPER_DLL_IMPORT
    #define KMOTIONDLL_HELPER_DLL_EXPORT
    #define KMOTIONDLL_HELPER_DLL_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define KMOTIONDLL_API and KMOTIONDLL_LOCAL.
// KMOTIONDLL_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// KMOTIONDLL_LOCAL is used for non-api symbols.

#ifdef KMOTIONDLL_DLL // defined if KMOTIONDLL is compiled as a DLL
  #ifdef KMOTIONDLL_DLL_EXPORTS // defined if we are building the KMOTIONDLL DLL (instead of using it)
    #define KMOTIONDLL_API KMOTIONDLL_HELPER_DLL_EXPORT
  #else
    #define KMOTIONDLL_API KMOTIONDLL_HELPER_DLL_IMPORT
  #endif // KMOTIONDLL_DLL_EXPORTS
  #define KMOTIONDLL_LOCAL KMOTIONDLL_HELPER_DLL_LOCAL
#else // KMOTIONDLL_DLL is not defined: this means KMOTIONDLL is a static lib.
  #define KMOTIONDLL_API
  #define KMOTIONDLL_LOCAL
#endif // KMOTIONDLL_DLL
#pragma warning ( disable : 4251 )


#define MAX_LINE 2560

#define MAX_BOARDS 16

#ifdef _WINDOWS
#define COMPILER "\\TCC67.exe"
#else
#define COMPILER "c67-tcc"
#endif

enum 
{
    KMOTION_OK=0,
    KMOTION_TIMEOUT=1,
    KMOTION_READY=2,
    KMOTION_ERROR=3,
};

enum 
{
    BOARD_TYPE_UNKNOWN=0,
    BOARD_TYPE_KMOTION=1,
    BOARD_TYPE_KFLOP=2,
};

enum // KMotionLocked Return Codes
{
	KMOTION_LOCKED=0,        // (and token is locked) if KMotion is available for use
	KMOTION_IN_USE=1,        // if already in use
	KMOTION_NOT_CONNECTED=2, // if error or not able to connect
};

typedef int CONSOLE_HANDLER(const char *buf);
typedef void ERRMSG_HANDLER(const char *ErrMsg);




// This class is exported from the KMotionDLL.dll
class KMOTIONDLL_API CKMotionDLL {
public:
	CKMotionDLL(int boardid);
	virtual ~CKMotionDLL();
    int BoardID;


	int WriteLineReadLine(const char *s, char *response);
	int WriteLine(const char *s);
	int WriteLineWithEcho(const char *s);
	int ReadLineTimeOut(char *buf, int TimeOutms=1000000);
	int ListLocations(int *nlocations, int *list);
	int WaitToken(bool display_msg=true, int TimeOut_ms=1000000);
	int KMotionLock();
	int USBLocation();
	int KMotionLockRecovery();
	void ReleaseToken();
	int Failed();
	int Disconnect();
	int FirmwareVersion();
	int CheckForReady();

	// Note: ALL User Thread Numbers start with 1
	
	
	int LoadCoff(int Thread, const char *Name, int PackToFlash=0); //PackToFlash 0=normal,1=NewVersion,2=bootload
	int CompileAndLoadCoff(const char *Name, int Thread);
	int CompileAndLoadCoff(const char *Name, int Thread, char *Err, int MaxErrLen);
	int Compile(const char *Name, const char *OutFile, const int board_type, int Thread, char *Err, int MaxErrLen);
	int CheckCoffSize(const char *Name, int *size_text, int *size_bss, int *size_data, int *size_total);  // return size of sections and total (including padding)
	unsigned int GetLoadAddress(int thread, int BoardType);
	void ConvertToOut(int thread, const char *InFile, char *OutFile, int MaxLength);

	int ServiceConsole();

	int SetConsoleCallback(CONSOLE_HANDLER *ch);
	int SetErrMsgCallback(ERRMSG_HANDLER *eh);
	int CheckKMotionVersion(int *type=NULL, bool GetBoardTypeOnly=false);
	int /*CKMotionDLL::*/ExtractCoffVersionString(const char *InFile, char *Version);
    int GetStatus(MAIN_STATUS& status, bool lock);
	void DoErrMsg(const char *s);


private:

	CMutex *PipeMutex;
	bool PipeOpen;
	bool ServerMessDisplayed;

	CONSOLE_HANDLER *ConsoleHandler;  
	ERRMSG_HANDLER *ErrMsgHandler;  

	int PipeCmd(int code);
	int PipeCmdStr(int code, const char *s);
	int Pipe(const char *s, int n, char *r, int *m);
	int LaunchServer();

	CString ExtractPath(CString InFile);

#ifdef _WINDOWS
	CFile PipeFile;
#else
	//int PipeFile;
	SocketWrapper PipeFile;
	CString MainPathDLL;
	CString MainPath;
	CString MainPathRoot;
#endif

};

#endif
