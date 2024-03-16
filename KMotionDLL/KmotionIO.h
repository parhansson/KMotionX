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
#define MAX_USB_ID 0xffffff // Anything more than this is considered an IP Address 1.0.0.0 or more

typedef int SERVER_CONSOLE_HANDLER (int board, const char *buf);

//PH TODO handle kogna sockets
typedef int SOCKET;

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
	bool RequestedDeviceAvail(std::wstring *reason);
	int ReadLineTimeOut(char *buf, int TimeOutms);
	int ReadLineTimeOutRaw(char *buf, int TimeOutms);
	int SendSocketNonBlock(char* s2, int length);
	int SetLatency(uint8_t LatencyTimer);
	int WriteLineWithEcho(const char * s);
	int WriteLine(const char *s);
	int ReadBytesAvailable(char *RxBuffer, int maxbytes, uint32_t *BytesReceived, int timeout_ms);
	int ReadSendNextLine(FILE *fr);
	int ServiceConsole();
	int SetConsoleCallback(SERVER_CONSOLE_HANDLER *ch);
	int Connect();
	int FlushInputBufferKogna();
	CKMotionIO();
	virtual ~CKMotionIO();
	SOCKET ConnectToKognaSocket(std::wstring *pReason, unsigned long ipAddress, int port);
	SOCKET TryConnectToSocket(std::wstring *pReason, unsigned long ipAddress, int port);
	void TryConnectToSocketThread();
	int connect_with_timeout(SOCKET sockfd, const sockaddr* addr, socklen_t addrlen, unsigned int timeout_ms);
	CHiResTimer Timer;
	

	bool BoardIDAssigned;
	unsigned int Requested_ID;
	unsigned int Actual_ID;
	int port;

	bool m_Connected;
	std::wstring ErrMsg;

	std::string m_LastCallerID;

protected:
	int Token;
	char m_SaveChars[2*MAX_LINE+1];
#ifdef LIB_FTDI
	struct ftdi_context *ftdi;
#else
	FT_HANDLE ftHandle;
#endif
	SOCKET ConnectSocket;

	SERVER_CONSOLE_HANDLER *ConsoleHandler;
private:
	int ErrorMessageBox(const wchar_t *s);
	int m_FirmwareVersion;
	int m_ConnectThreadState;
	unsigned long ipAddress_Thread;
	SOCKET socket_Thread;
	int port_Thread; 
};

#endif // !defined(AFX_KMotionIO_H__BF583FCD_556C_47DA_AE7C_BC3163146ABF__INCLUDED_)
