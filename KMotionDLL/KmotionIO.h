// KMotionIO.h: interface for the CKMotionIO class.
//
//
//  This is the class that the Server uses to actually talk to the driver
//
//
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined(AFX_KMotionIO_H__BF583FCD_556C_47DA_AE7C_BC3163146ABF__INCLUDED_)
#define AFX_KMotionIO_H__BF583FCD_556C_47DA_AE7C_BC3163146ABF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern CHiResTimer Timer;

#define NO_KMOTION_TIMEOUT false // useful for debugging

typedef int SERVER_CONSOLE_HANDLER (int board, const char *buf);
#define VENDOR 0x0403
#define PRODUCT 0xf231

#ifdef LIB_FTDI
#define FT_OK 0
#endif


class CKMotionIO  
{
public:
	bool FailMessageAlreadyShown;
	bool SendAbortOnConnect;
	int LogToConsole(char *s);
	int HandleDiskIO(char *s);
	int KMotionLock(const char *CallerID);
	int USBLocation();
	int KMotionLockRecovery();
	int MakeSureConnected();
	void ReleaseToken();
	int CheckForReady();
	int Failed();
	int Disconnect();
	int FirmwareVersion();
	int NonRespondingCount;
	int FlushInputBuffer();
	CMutex *Mutex;
	int NumberBytesAvailToRead(int *navail, bool ShowMessage);
	int WriteLineReadLine(const char *send, char *response);
	bool RequestedDeviceAvail(char *reason);
	int ReadLineTimeOut(char *buf, int TimeOutms);
	int ReadLineTimeOutRaw(char *buf, int TimeOutms);
	int SetLatency(uint8_t LatencyTimer);
	int WriteLineWithEcho(const char * s);
	int WriteLine(const char *s);
	int ReadBytesAvailable(char *RxBuffer, int maxbytes, uint32_t *BytesReceived, int timeout_ms);
	int ReadSendNextLine(FILE *fr);
	int ServiceConsole();
	int SetConsoleCallback(SERVER_CONSOLE_HANDLER *ch);
	int Connect();
	CKMotionIO();
	virtual ~CKMotionIO();

	bool BoardIDAssigned;
	int USB_Loc_ID;
	bool m_Connected;
	char ErrMsg[MAX_LINE];

	char m_LastCallerID[256];

protected:
	int Token;
	char m_SaveChars[MAX_LINE+1];
#ifdef LIB_FTDI
	struct ftdi_context *ftdi;
#else
	FT_HANDLE ftHandle;
#endif

	SERVER_CONSOLE_HANDLER *ConsoleHandler;
private:
	int ErrorMessageBox(const char *s);
	int m_FirmwareVersion;


};

#endif // !defined(AFX_KMotionIO_H__BF583FCD_556C_47DA_AE7C_BC3163146ABF__INCLUDED_)
