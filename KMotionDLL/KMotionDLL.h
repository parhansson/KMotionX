// KMotionDLL.h    --  KMotion DLL Interface class
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined (KMOTIONDLL_H)
#define KMOTIONDLL_H



#include <afxmt.h>
#include "../DSP_KFLOP/PC-DSP.h"
#ifdef _KMOTIONX
#include <SocketWrapper.h>
#else
#define KMOTIONDLL_DLL
#endif
#include <vector>

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
  #ifdef KMOTIONDLL_EXPORTS // defined if we are building the KMOTIONDLL DLL (instead of using it)
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

#define OLD_COMPILER 1              // Select default compiler: 1 to use 0.9.16 tcc, else 0 to use later version

#ifdef COMPILER
	#undef COMPILER
#endif


#ifdef _KMOTIONX
    #if OLD_COMPILER
    #define COMPILER "tcc67"        // For now, port of the old compiler (based on 0.9.16) works properly
    #else
    #define COMPILER "c67-tcc"    // Later version (0.9.26) but not yet fully working.
    #endif
    #define SOCK_PATH "/tmp/kmotionsocket"
#else
    #define COMPILER "\\TCC67.exe"
#endif

#define KMOTION_PORT    57777   // Default server socket port (for TCP connections)

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

// Used by SetPollCallback to register change events of interest.  If the appropriate bit is set, and the
// MAIN_STATUS value changes, then the appropriate (virtual) method will be invoked.  Default methods do
// nothing.
enum
{
    POLL_ADC    = 0x00000001,          // ADC value change
    POLL_DAC    = 0x00000002,          // DAC
    POLL_PWM    = 0x00000004,          // PWM
    POLL_POS    = 0x00000008,          // Position
    POLL_DEST   = 0x00000010,          // Destination
    POLL_OUTCHAN= 0x00000020,          // Output Channel
    POLL_MODES  = 0x00000040,          // Input or output modes
    POLL_ENABLE = 0x00000080,          // Axis enables
    POLL_DONE   = 0x00000100,          // Axis 'done'
    POLL_IO     = 0x00000200,          // Digital I/O direction or state
    POLL_THREAD = 0x00000400,          // Thread activity
    POLL_STOP   = 0x00000800,          // Stop coord motion state
    POLL_PCCOMM = 0x00001000,          // PC comms (from kflop)
};

typedef int CONSOLE_HANDLER(const char *buf);
typedef void ERRMSG_HANDLER(const char *ErrMsg);


// Some types to give nicer binding behavior than raw C arrays
typedef std::vector<bool> boolvec;
typedef std::vector<int> intvec;
typedef std::vector<double> floatvec;



// This class is exported from the KMotionDLL.dll
class KMOTIONDLL_API CKMotionDLL {
public:
	CKMotionDLL(int boardid);
	virtual ~CKMotionDLL();
    int BoardID;


	int WriteLineReadLine(const char *s, char *response);
	int WriteLine(const char *s);
	int WriteLineWithEcho(const char *s);
	int ReadLineTimeOut(char *buf, int TimeOut_ms=100);
	int ListLocations(int *nlocations, int *list);
	int WaitToken(bool display_msg=true, int TimeOut_ms=100);
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
	// Default implementation of following calls the C callbacks set above.  Having virtuals
	// makes it nicer to use in a swig target language binding.
    virtual void Console(const char *buf);
    virtual void ErrMsg(const char *buf);
    
	int CheckKMotionVersion(int *type=NULL, bool GetBoardTypeOnly=false);
	int ExtractCoffVersionString(const char *InFile, char *Version);
    int GetStatus(MAIN_STATUS& status, bool lock);
    bool GetLastStatus(MAIN_STATUS& status);
    void SetHostStatus(int host_status, bool poll=true);  // See PC-DSP.h for Global Host Status bit settings.
    void Poll();
    void SetPollCallback(unsigned int interest);  // Set status change interest according to OR'd combo of POLL_* enum above.
	void DoErrMsg(const char *s);
#ifdef _KMOTIONX
    const char * getInstallRoot(void) const { return GetMainPathRoot(); }

	// Use to override default compiler executable.  options controls whether -g (and other) option supplied.
	// tcc_minor_version should be set to e.g. 26 for tcc version 0.9.26 (controls options), or 0 to
	// not change the version.
	// If compiler is absolute path, then that exact compiler is used.  Otherwise, it should just be the

	// name of the compiler without any path, and it will be searched for in standard locations.  If an abs
	// path is provided, it is a short-cut to calling SetCompilerDir() with the directory component.
	// If NULL, compiler is set back to default.  In this case, SetCompilerDir(NULL) should be called to
	// reset the compiler location directory back to default as well.
	void SetCustomCompiler(const char * compiler = NULL, const char * options = NULL, int tcc_minor_version = 0); 
	
	// Alternative ctor to connect via TCP.  If url is NULL, assumes localhost, else is a host name.
	CKMotionDLL(int boardid, unsigned int dfltport, const char * url = NULL);
	
#endif
    // Virtuals ('callbacks') for changed status.  Default implementations do nothing.  To use, derive from this
    // class, call SetPollCallback() to register interest in one or more change events, then call Poll() as often
    // as required to retrieve status and dispatch these functions.  Parameters give the new state (previous state
    // can be obtained with GetLastStatus() if necessary).  Array parameters are vectorized, as are the I/O bits.
    virtual void ChangedADC(const intvec & ADCVec) {}
    virtual void ChangedDAC(const intvec & DACVec) {}
    virtual void ChangedPWM(const intvec & PWMVec) {}
    virtual void ChangedPos(const floatvec & PositionVec) {}
    virtual void ChangedDest(const floatvec & DestVec) {}
    virtual void ChangedOutChan(const intvec & OutputChanVec) {}
    virtual void ChangedMode(int InputModes, int InputModes2, int OutputModes, int OutputModes2) {}
    virtual void ChangedEnable(int Enables) {}
    virtual void ChangedDone(int AxisDone) {}
    virtual void ChangedIO(const boolvec & OutDirVec, const boolvec & StateVec) {}
    virtual void ChangedThread(int ThreadActive) {}
    virtual void ChangedStop(int StopImmediateState) {}
    virtual void ChangedPCComm(const intvec & PC_comm) {}

    // Support multiple DSPKFLOP.out files.  Default .out file is in <installdir>/DSP_KFLOP/DSPKFLOP.out
    // but this function can change that.  Caller is responsible for pointing to a valid .out file,
    // which must be an absolute path.
    void SetDSPKFLOP(const char * filename);
    const char * GetDSPKFLOP(void) const { return DSPKFLOP; }
    
    // For special applications, allow changing the locations of where things are found.
    // The Get* members are not 'const' since they may do some caching.
    void SetMainPathRoot(const char * dir);
    const char * GetMainPathRoot(void) const;
    void SetCompilerDir(const char * dir);
    const char * GetCompilerDir(void);
    void SetServerDir(const char * dir);
    const char * GetServerDir(void);

private:

	CMutex *PipeMutex;
	bool PipeOpen;
	bool ServerMessDisplayed;

protected:
	CONSOLE_HANDLER *ConsoleHandler;  
	ERRMSG_HANDLER *ErrMsgHandler;  

private:
	int OpenPipe();
	int PipeCmd(int code);
	int PipeCmdStr(int code, const char *s);
	int Pipe(const char *s, int n, char *r, int *m);
	int LaunchServer();

    void _init(int boardid);

	void ExtractPath(const char *InFile, char *path);
#ifdef _KMOTIONX
	SocketWrapper PipeFile;
	char MainPath[256];
	char MainPathRoot[256];
	char customCompiler[256];
	char customOptions[256];
	int tcc_vers;
    
#else
	CFile PipeFile;
#endif
    bool use_tcp;
    bool remote_tcp;
    unsigned int tcp_port;
    char hostname[257];
    unsigned int poll_interest;
    bool first_status;
    MAIN_STATUS last_status;
    int HostStatus;
    char DSPKFLOP[257];
    char compiler_dir[257];
    char server_dir[257];
};

#endif
