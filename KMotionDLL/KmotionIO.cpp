// KMotionIO.cpp: implementation of the CKMotionIO class.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "StdAfx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Ping.h"
#include <fcntl.h>
#include <time.h>
#include <timeapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

enum { CONN_STATE_IDLE, CONN_STATE_CONNECTING, CONN_STATE_COMPLETE };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKMotionIO::CKMotionIO()
{
	ConsoleHandler=NULL;

	Mutex = new CMutex(FALSE,"KMotionIO",NULL);

	m_Connected=false;
	ConnectSocket = NULL;
	SendAbortOnConnect=true;
	FailMessageAlreadyShown=false;
	Token=0;
	NonRespondingCount=0;
	m_FirmwareVersion=2;
	Requested_ID = -1;
	Actual_ID = -1;
	port = 2000;
	m_ConnectThreadState = CONN_STATE_IDLE;

#ifdef _KMOTIONX
    //FT_SetVIDPID(VENDOR, PRODUCT);
    FT_SetVIDPID(1027, 62001);
#endif
}


CKMotionIO::~CKMotionIO()
{
	delete Mutex;
}


SOCKET CKMotionIO::ConnectToKognaSocket(std::wstring* pReason, unsigned long ipAddress, int port)
{
	unsigned char DynoMAC[6] = { 0x8c, 0x1f, 0x64, 0x15, 0xe0, 0x00 }; // dymotion purchased MAC base address
	unsigned char DynoMask[6] = { 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00 }; // high 36 bit mask

	wchar_t ErrorMessage[64]; 		//----------------------
	// Create a SOCKET for connecting to server
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	PMIB_IPNETTABLE pIpNetTable = NULL;
	ULONG Size = 0;
	DWORD dwIpAddr = 0;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		_swprintf_p(ErrorMessage, 64, Translate("WSAStartup failed with error: %d\n"), err);
		if (pReason)
			*pReason = ErrorMessage;
		return NULL;
	}

	// Check if User has specified a specific Board
	// 0x00000000 = 0 = any board found
	// 0x00XXXXXX = High byte 0 asSume USB Address 
	// 0xFF000XXX = High Byte 255 low 2 bytes = Serial Number = Domain KognaXXXX
	// 0x01000000 - 0xDFFFFFFF assume static IP Address if high byte >= 1 and < 224

	if (ipAddress >= 0x01000000 && ipAddress <= 0xDFFFFFFF) // Static ip address?
	{
		return TryConnectToSocket(pReason, ipAddress, port);  // Try it
	}

	if (ipAddress == 0 ||									  // use any Board found
		(ipAddress >= 0xFF000000 && ipAddress <= 0xFF000FFF)) // or use Kogna Serial Number?
	{
		// wait for first complete scan fro Kognas so Apps Expecting to connect on first try succeed
		while (!FirstKognasScanComplete)
			Sleep(1);

		DWORD dwWaitResult = WaitForSingleObject(KognaListMutex, INFINITE);  // no time-out interval
		if (dwWaitResult != WAIT_OBJECT_0) return -1;

		for (int i = 0; i < nKognas; i++)
		{
			if (ipAddress == 0 || Kognas[i].KognaSerialNumber == (ipAddress & 0xFFF))
			{
				ipAddress = Kognas[i].KognaIP;
				SOCKET s = TryConnectToSocket(pReason, ipAddress, port);

				if (s)
				{
					ReleaseMutex(KognaListMutex);
					Actual_ID = ipAddress;
					return s;
				}
			}
		}
		ReleaseMutex(KognaListMutex);
	}
	return NULL;

}

DWORD ConnectThread(LPDWORD lpdwParam)
{
	((CKMotionIO*)lpdwParam)->TryConnectToSocketThread();
	return 0;
}

// because TCP/IP connections requires such a long time to timeout perform
// the connection in a worker Thread and keep returning no connection until
// the worker thread successfully connects

SOCKET CKMotionIO::TryConnectToSocket(std::wstring* pReason, unsigned long ipAddress, int port)
{
	HANDLE Thread = NULL;
	bool wait = false;
	CHiResTimer Timer;

	do
	{
		switch (m_ConnectThreadState)
		{
		case CONN_STATE_IDLE:
			m_ConnectThreadState = CONN_STATE_CONNECTING;
			ipAddress_Thread = ipAddress;
			port_Thread = port;

			// create a worker Thread to scan for adapters in system
			Thread = CreateThread(
				NULL,                        /* no security attributes        */
				100000,                      /* stack size 100K        */
				(LPTHREAD_START_ROUTINE) ::ConnectThread, /* thread function       */
				this,	    			     /* argument to thread function   */
				0,                           /* use default creation flags    */
				NULL);

			if (!Thread)  // if Thread failed remain in IDLE state
				m_ConnectThreadState = CONN_STATE_IDLE;

			wait = true;  // at first trying to connect wait some time to connect
			Sleep(0);
			Timer.Start();
			break;

		case CONN_STATE_CONNECTING:
			if (wait && Timer.Elapsed_Seconds() < 0.1)
				break; // keep looping
			else
				return NULL;  // so far not connected, maybe next time

		case CONN_STATE_COMPLETE:
			// verify requested IP and port hasn't changed aand
			// if so, discard and mark as no connection
			m_ConnectThreadState = CONN_STATE_IDLE;
			if (ipAddress == ipAddress_Thread && port == port_Thread)
				return socket_Thread;
		}
	}
	while (true);
	return NULL;
}



// called by worker thread

void CKMotionIO::TryConnectToSocketThread()
{
	SOCKET ConnectSocket;

	ConnectSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET)
	{
		socket_Thread = NULL;
		m_ConnectThreadState = CONN_STATE_IDLE;
		return;
	}

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	sockaddr_in clientService;
	memset(&clientService, 0, sizeof(clientService));
	clientService.sin_family = AF_INET;
	unsigned int Net_add = htonl(ipAddress_Thread);
	clientService.sin_addr.s_addr = Net_add;
	clientService.sin_port = htons(port);
	char IPAddres[256];
	strcpy(IPAddres, inet_ntoa(clientService.sin_addr));

	//----------------------
	// Connect to server.
	if (connect_with_timeout(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService), 500) == SOCKET_ERROR) 
	{
		socket_Thread = NULL;
		m_ConnectThreadState = CONN_STATE_IDLE;
		return;
	}
	socket_Thread = ConnectSocket;
	m_ConnectThreadState = CONN_STATE_COMPLETE;
}


// from https://stackoverflow.com/questions/2597608/c-socket-connection-timeout
int CKMotionIO::connect_with_timeout(SOCKET sockfd, const struct sockaddr* addr, socklen_t addrlen, unsigned int timeout_ms)
{
	int rc = 0;

	// Set O_NONBLOCK
	unsigned long ulValue = 1;
	ioctlsocket(sockfd, FIONBIO, &ulValue);

	// Start connecting (asynchronously)
	if (connect(sockfd, addr, addrlen) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		// Did connect return an error? If so, we'll fail.
		if ((err != WSAEWOULDBLOCK) && (err != WSAEINPROGRESS)) {
			rc = -1;
		}
		// Otherwise, we'll wait for it to complete.
		else {
			// Set a deadline timestamp 'timeout' ms from now (needed b/c poll can be interrupted)
			CHiResTimer Timer;
			Timer.Start();

			// Wait for the connection to complete.
			do {
				// Wait for connect to complete (or for the timeout deadline)
				if (Timer.Elapsed_Seconds() > timeout_ms * 0.001) 
				{ 
					rc = -1; 
					break; 
				}

				struct pollfd PollConnect[1];
				PollConnect->fd = sockfd;
				PollConnect->events = POLLOUT;

				rc = WSAPoll(PollConnect, 1, 100);
				if (rc > 0) // successful?
				{
					int error = 0; socklen_t len = sizeof(error);
					int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *)&error, &len);
					if (error != 0) rc = -1;
					break;
				}
			}
			// If poll was interrupted, try again.
			while (1);
		}
	}
	// Restore original O_NONBLOCK state
//	if (fcntl(sockfd, F_SETFL, sockfd_flags_before) < 0) return -1;
	// Success
	return rc;
}




bool CKMotionIO::RequestedDeviceAvail(std::wstring *Reason)
{
	int i;
	bool TryKogna = Requested_ID == 0 || Requested_ID >  MAX_USB_ID;
	bool TryKFLOP = Requested_ID == 0 || Requested_ID <= MAX_USB_ID;

	Mutex->Lock();

	if (TryKogna)
	{
		if (ConnectSocket)
		{
			Mutex->Unlock();
			return true;
		}
		ConnectSocket = ConnectToKognaSocket(Reason, Requested_ID, port);
		if (ConnectSocket != NULL)
		{
			Mutex->Unlock();
			return true;
		}
	}
	
	if (TryKFLOP) 
	{
		KFLOPListMutex->Lock();  // no time-out interval
		if (1/*dwWaitResult == WAIT_OBJECT_0*/)
		{
			if (nKFLOPs > 0)
			{
				// if USB Location is undefined select the first from
				// the list that is not already taken

				if (Requested_ID == 0)
				{
					for (i = 0; i < (int)nKFLOPs; i++)
					{
						int k;
						// make sure nobody is already using this one
						for (k = 0; k < MAX_BOARDS; k++)
						{
							if (KMotionLocal.KMotionIO[k].m_Connected &&
								KFLOPs[i].LocId == KMotionLocal.KMotionIO[k].Actual_ID)
								break;
						}
						if (k == MAX_BOARDS)
						{
							Actual_ID = KFLOPs[i].LocId;  // assign it
							Mutex->Unlock();
							KFLOPListMutex->Unlock();
							return true;
						}
					}

					Mutex->Unlock();
					KFLOPListMutex->Unlock();
					if (Reason) *Reason = Translate("No KMotion devices available");
					return false;
				}

				// user wants a specific usb location
				// so see if it is available

				for (i = 0; i < (int)nKFLOPs; i++)
				{
					if (KFLOPs[i].LocId == Requested_ID)
						break;
				}

				if (i == nKFLOPs)
				{
					Mutex->Unlock();
					KFLOPListMutex->Unlock();
					if (Reason)
					{
						wchar_t dyn_buf[150];
						swprintf(dyn_buf,150, Translate("KMotion not found on USB Location %08X\r\rUnable to open device").c_str(), Requested_ID);
						*Reason = dyn_buf;
					} 
					return false;
				}
				else // match
				{
					Mutex->Unlock();
					KFLOPListMutex->Unlock();
					return true;
				}
			}
			else
			{
				Mutex->Unlock();
				KFLOPListMutex->Unlock();
				if (Reason) *Reason = Translate("No KMotion devices available");
				return false;
			}
		}
	}

	Mutex->Unlock();
	return false;
}

 
#define CONNECT_TRIES 5

int CKMotionIO::Connect()
{
	std::wstring reason;
	CHiResTimer Timer;

	FT_STATUS ftStatus;

	if (NonRespondingCount==CONNECT_TRIES) return 1;

	m_SaveChars[0]=0;  // start anew

	Mutex->Lock();

	if (!RequestedDeviceAvail(&reason))
	{
		ErrorMessageBox(reason.c_str());
		Mutex->Unlock();
		return 1;
	}


	Timer.Start();

	if((unsigned int)Actual_ID > MAX_USB_ID)
	{
		// Set Socket to non-blocking mode
		// Set socket to blocking mode
		unsigned long ul = 1;  // 0=Nonblocking mode disabled
		int nRet = ioctlsocket(ConnectSocket, FIONBIO, (unsigned long*)&ul);
		if (nRet == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = NULL;
			Mutex->Unlock();
			return 1;
		}

		m_Connected=true;  // All set

		if (FlushInputBufferKogna())
		{
			Mutex->Unlock();
			return 1;
		}

		Mutex->Unlock();
		return 0;
	}


	
	#define TIME_TO_TRY_TO_OPEN 3.0

	// FT_ListDevices OK, number of devices connected is in numDevs

	// usually during boot the board comes and goes, since it appeared 
	// to be there, try for a while to open it
	for (;;) 
	{
#ifdef _WIN64		
		ftStatus = FT_OpenEx((void *)((__int64)Actual_ID),FT_OPEN_BY_LOCATION,&ftHandle);
#else
		ftStatus = FT_OpenEx((void *)Actual_ID,FT_OPEN_BY_LOCATION,&ftHandle);
#endif
		
		if (ftStatus == FT_OK) 
		{
			// FT_Open OK, use ftHandle to access device

			if (SetLatency(2))
			{
				Mutex->Unlock();
				return 1;
			}

			// Set read timeout of 5sec, write timeout of 5sec 
			ftStatus = FT_SetTimeouts(ftHandle, 5000, 5000); 
			if (ftStatus != FT_OK)
			{
				FT_Close(ftHandle);
				Mutex->Unlock();
				return 1;
			}

			if (FlushInputBuffer())
			{
				FT_Close(ftHandle);
				Mutex->Unlock();
				return 1;
			}

			m_Connected=true;  // All set

			Mutex->Unlock();
			return 0;
		}
		else 
		{
			// FT_Open failed

			if (Timer.Elapsed_Seconds() > TIME_TO_TRY_TO_OPEN)
			{
				ErrorMessageBox(Translate("Unable to open KMotion device"));
				Mutex->Unlock();
				return 1;
			}

			Sleep(100);  // delay a bit then loop back and try again
		}
	}

	Mutex->Unlock();
	return 0;
}

#define CONNECT_TIMEOUT 1.0

int CKMotionIO::FlushInputBufferKogna()
{
	char s[2560];
	CHiResTimer Timer;

	if (SendAbortOnConnect)
	{
		// send flush command to DSP

		s[0] = ABORT_CHAR;
		s[1] = 0;

		if (SendSocketNonBlock(s, 1))  return 1;  // send it

		// wait for a fixed time for the abort acknowledge
		// to come back which is exactly 3 characters ESC C \r

		Timer.Start();
		do
		{
			if (ReadLineTimeOutRaw(s, 100))  
				return 1;
			if (Timer.Elapsed_Seconds() > CONNECT_TIMEOUT)
				return 1;
		} while ((s[0] != 0x1b || s[1] != 'C' || s[2] != '\r' || s[3] != '\n' || s[4] != 0));
	}

	// OK looks like we are in sync
	return 0;
}

int CKMotionIO::NumberBytesAvailToRead(int *navail, bool ShowMessage)
{
	FT_STATUS ftStatus;
	DWORD EventDWord;
	DWORD RxBytes;
	DWORD TxBytes;

	Mutex->Lock();

	*navail = (int)strlen(m_SaveChars);  // take into account any already read in
	
	if ((unsigned int)Actual_ID > MAX_USB_ID)
	{
		unsigned long bytes_available=0;

		if (ioctlsocket(ConnectSocket, FIONREAD, &bytes_available) == SOCKET_ERROR)
		{
			if (ShowMessage)
				Failed();
			else
				m_Connected = false;

			Mutex->Unlock();
			return 1;
		}
		*navail += (int)bytes_available;
		Mutex->Unlock();
		return 0;
	}
	else
	{
	ftStatus = FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);

	if (ftStatus != FT_OK) 
	{
		if (ShowMessage)
			Failed();
		else
			m_Connected=false;

		Mutex->Unlock();
		return 1;
	}
	else
	{
		*navail+=(int)RxBytes;
		Mutex->Unlock();
		return 0;
	}
}
}


int CKMotionIO::ReadBytesAvailable(char *RxBuffer, int maxbytes, DWORD *BytesReceived, int timeout_ms)
{
	FT_STATUS ftStatus;
	DWORD EventDWord;
	DWORD RxBytes;
	DWORD TxBytes;
	timeout_ms = 100;
	int Offset = 0;

	Mutex->Lock();

	if((unsigned int)Actual_ID > MAX_USB_ID)
	{
		int nRet = 0;

		do  // loop until no more data available or buffer full
		{
			nRet = recv(ConnectSocket, RxBuffer + Offset, maxbytes - Offset - 1, 0);
			if(nRet != SOCKET_ERROR)
			{
				Offset += nRet;
			}
			if(Offset >= maxbytes - 1)
			{
				Offset = maxbytes - 1;
				break;
			}
			if (nRet == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
				{
					nRet = 0;
				}
				else
				{
					wchar_t ErrMsg[1024];
					_swprintf_p(ErrMsg, 1024, Translate("SOCKET ERROR: %d"), error);
					ErrorMessageBox(ErrMsg);
					closesocket(ConnectSocket);
					ConnectSocket = NULL;
					Failed();
					Mutex->Unlock();
					return 1;
				}
			}
		}
		while (nRet > 0 && maxbytes - Offset - 1 > 0);
		RxBuffer[Offset] = 0;
		*BytesReceived = Offset;
	}
	else
	{
		ftStatus = FT_GetStatus(ftHandle, &RxBytes, &TxBytes, &EventDWord);

		if (ftStatus != FT_OK)
		{
			Failed();
			Mutex->Unlock();
			return 1;
		}

		if ((int)RxBytes > maxbytes) RxBytes = maxbytes - 1; // leave room for null

		RxBuffer[0] = 0;  // set buf empty initially
		*BytesReceived = 0;

		if (RxBytes > 0)
		{
			ftStatus = FT_Read(ftHandle, RxBuffer, RxBytes, BytesReceived);
			if (ftStatus == FT_OK)
			{
				RxBuffer[*BytesReceived] = 0;  // null terminate
			}
			else
			{
				Failed();
				Mutex->Unlock();
				return 1;
			}
		}
	}

	Mutex->Unlock();
	return 0;
}



// Call this to check if a "Ready" response came back
// 
// Note:There is a possibility that other DSP threads are
// printing to the console screen
//
// First check if any input is available
// if not, return instantly (KMOTION_OK)
// if so, then a whole line might not be available
// but should be in transmission and be in very soon,
// so wait up to a short while to get in an entire line
// if the line doesn't come in for some reason return (KMOTION_TIMEOUT)
// check if the line should go to the console (esc)
// if so print it and repeat the above as long as 
// there is still input available
// if the line is not console data, check for "Ready",
// if it is "Ready" send it to the console and return KMOTION_READY;
// otherwise send it to the console

//ExcludeTranslate
int CKMotionIO::CheckForReady()
{
	char buf[257];
	char *beg;
	char copy[257];
	int nbytes, result;
	bool DetectedError=false;

	for (;;) 
	{
		if (m_Connected)
		{
			if (!NumberBytesAvailToRead(&nbytes, true) && nbytes>0)
			{
				result = ReadLineTimeOutRaw(buf, 100);
				
				if (result==0)
				{
					
					// check if first char is an ESC

					if (buf[0] == 0x1b)
						beg=buf+1;
					else
						beg=buf;
				
					strncpy(copy,beg,255);
					_strupr(copy);

					// check for "Error"
					if (strstr(copy, "ERROR") != NULL)
					{
						// error, send to console

						DetectedError = true;
					}

					
					// check for "Ready"
					if (strcmp(beg,"Ready\r\n"))  
					{
						// no, strange send to console

						LogToConsole(beg);
					}
					else
					{
						// yes! send to console and return Ready

						LogToConsole(beg);
						if (DetectedError)
							return KMOTION_ERROR;  // a line contained the word Translate("error")
						else
							return KMOTION_READY;  
					}
				}
				else
				{
					return KMOTION_TIMEOUT;  // not a whole line for some reason
				}
			}
			else
			{
				return KMOTION_OK;  // no data, return OK, not Ready
			}
		}
		else
		{
			return KMOTION_ERROR;  // not connected return error
		}
	}

	return result;
}
//ResumeTranslate




// Reads a line and if it gets a line with no timeout
// check if it is a user message (indicated by esc char
// at the beginning.  If so print it to the Console
// repeat until we get a normal line to be returned
// to the caller


int CKMotionIO::ReadLineTimeOut(char *buf, int TimeOutms)
{
	int result;
	bool Done;

	do 
	{
		Done=true;
		result = ReadLineTimeOutRaw(buf, TimeOutms);
		
		if (result==0)
		{
			// check if first char is an ESC

			if (buf[0] == 0x1b)
			{
				// it is!  
				
				// if next character is 1-7
				// handle it as a write file command
				
				if (buf[1] >=1 && buf[1] <=7)
				{
					// it is a disk command
					HandleDiskIO(buf+1);
				}
				else
				{
					// Send to console
					LogToConsole(buf+1);
				}
				
				Done=false;
			}
		}
	}
	while (!Done);

	return result;
}

int CKMotionIO::ReadLineTimeOutRaw(char *buf, int TimeOutms)
{
	BOOL Done=FALSE;
	int TotalBytes, result;
	DWORD NBytesRead;
	char *p;
	char ReadBuffer[MAX_LINE+10];
	int i,freespace;
	bool FirstTime=true;
	static bool ErrorDisplayed=false;
	CHiResTimer Timer;


	if (ErrorDisplayed) return 1;

	Mutex->Lock();

	strcpy(buf,m_SaveChars);
	TotalBytes=(int)strlen(buf);
	m_SaveChars[0]=0; // remember we used them

	Timer.Start();

	while (!Done)
	{ 
		if (FirstTime)
			FirstTime=false;
		else
		{
			timeBeginPeriod(1);
			Sleep(1);
			timeEndPeriod(1);
		}

		for (i=0; i<200; i++)
		{
			ReadBuffer[i]=0;
		}

		freespace = MAX_LINE-TotalBytes-1;

		if (freespace > 0)
		{
			result = ReadBytesAvailable(ReadBuffer, freespace, &NBytesRead, 0);

			if (result)
			{
				Mutex->Unlock();
				return result;
			}
			
			if (NBytesRead > 0) 
			{
				for (i=0; i<(int)NBytesRead; i++)
				{
					if (i+TotalBytes > MAX_LINE)
					{
						ErrorMessageBox(Translate("KMotion Received Line too long"));
						Mutex->Unlock();
						return 1;
					}
					buf[i+TotalBytes] = ReadBuffer[i];
				}
			}

			TotalBytes += NBytesRead;

			//Successfully received data.  Zero terminate string.
			buf[TotalBytes] = 0;
		
		}
		// check if we got a terminating character  \n 


		p = strchr(buf,'\n');


		if (p)
		{
			// check if we read extra characters
			if ((p-buf) < (int)(strlen(buf)-1))
			{
				// yes, save them for next time
				strcpy(m_SaveChars,p+1);
				// now zero terminate first string
				// after the \r
				*(p+1) = 0;
			}

			Done=TRUE;
		}

		if (!NO_KMOTION_TIMEOUT && !Done && Timer.Elapsed_Seconds() * 1000.0 > TimeOutms)
		{
			Mutex->Unlock();
			return 2;  // return with timeout indication
		}

	}
	FailMessageAlreadyShown=false;  // if we successfully received something show any subsequent error
	Mutex->Unlock();
	return 0;
}


// special prefix tells DSP not to echo

int CKMotionIO::WriteLine(const char *s)
{
	char s2[MAX_LINE+1]="\x1b\x01";

	strcat(s2,s);
	return WriteLineWithEcho(s2);
}


int CKMotionIO::WriteLineWithEcho(const char *s)
{
	FT_STATUS ftStatus;
	int length;
	char s2[MAX_LINE+1];   
	DWORD BytesWritten;

	if (MakeSureConnected()) return 1;
    strcpy(s2,s);
	strcat(s2,"\r"); // Add CR

	length = (int)strlen(s2);

	Mutex->Lock();
	
	if((unsigned int)Actual_ID > MAX_USB_ID)
	{
		if (SendSocketNonBlock(s2, length)) return 1;
	}
	else
	{
		ftStatus = FT_Write(ftHandle, s2, length, &BytesWritten);
	}

	Mutex->Unlock();

	return 0;
}


int CKMotionIO::SendSocketNonBlock(char *s2, int length)
{
	DWORD BytesWritten;

	do
	{
		BytesWritten = send(ConnectSocket, s2, length, 0);  // non blocking call

		if (BytesWritten == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				wchar_t ErrMsg[1024];
				_swprintf_p(ErrMsg, 1024, Translate("SOCKET ERROR: %d"), error);
				ErrorMessageBox(ErrMsg);
				closesocket(ConnectSocket);
				ConnectSocket = NULL;
				Failed();
				Mutex->Unlock();
				return 1;
			}
		}
		else
		{
			length -= BytesWritten;
			s2 += BytesWritten;

			if (length == 0) break;

			Sleep(0);
		}

	} while (true);
	
	return 0;
}
int CKMotionIO::SetLatency(UCHAR LatencyTimer)
{
	FT_STATUS ftStatus;
		
	unsigned char c;

	
	Mutex->Lock();

	ftStatus = FT_GetLatencyTimer (ftHandle, &c);

	ftStatus = FT_SetLatencyTimer(ftHandle,LatencyTimer);
	
	if (ftStatus == FT_OK) 
	{ 
		ftStatus = FT_GetLatencyTimer (ftHandle, &c);

		// LatencyTimer set 

		ftStatus = FT_SetChars (ftHandle, '\n', 1, 0,0);

		if (ftStatus == FT_OK) 
		{ 
			// Event set 

			Mutex->Unlock();
			return 0;
		}
		else 
		{ 
			// FT_SetLatencyTimer FAILED!
			ErrorMessageBox(Translate("Unable to set USB Event Character"));
			Mutex->Unlock();
			return 1;
		}
	}
	else 
	{ 
		// FT_SetLatencyTimer FAILED!
		ErrorMessageBox(Translate("Unable to set USB Latency timer"));
		Mutex->Unlock();
		return 1;
	}
}


int CKMotionIO::WriteLineReadLine(const char *send, char *response)
{
	Mutex->Lock();
	if (WriteLine(send))
	{
		Mutex->Unlock();
		return 1;
	}

	if (ReadLineTimeOut(response,1000000))
	{
		Mutex->Unlock();
		return 1;
	}
	
	response[(int)strlen(response)-2]=0;  // remove the /r /n

	Mutex->Unlock();

	return 0;
}

int CKMotionIO::FlushInputBuffer()
{
	FT_STATUS ftStatus;
	DWORD RxBytes;
	DWORD TxBytes;
	DWORD BytesReceived;
	DWORD BytesWritten;
	DWORD EventDWord;
	char s[10];
	char RxBuffer[500];
	CHiResTimer Timer;

	ftStatus = FT_Purge(ftHandle,FT_PURGE_RX|FT_PURGE_TX);
	if (ftStatus != FT_OK) 	return 1;


	// discard any data in the read queue in the driver

	Timer.Start();
	do
	{
		ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
		if (ftStatus != FT_OK) 
			return 1;

		if (RxBytes > 0)
		{
			if (RxBytes > 400) RxBytes=400;
			ftStatus = FT_Read(ftHandle,RxBuffer,RxBytes,&BytesReceived);
		}
	}
	while (RxBytes > 0 && Timer.Elapsed_Seconds() < CONNECT_TIMEOUT);



	if (SendAbortOnConnect)
	{
		CHiResTimer Timer;
		// send flush command to DSP

		s[0]=ABORT_CHAR;
		s[1]=0;


		ftStatus = FT_Write(ftHandle, s, 1, &BytesWritten);
		if (ftStatus != FT_OK) 
			return 1;
		if (BytesWritten != 1) 
			return 1;


		// wait and be sure chars are transmitted

		Timer.Start();
		do
		{
			ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
			if (ftStatus != FT_OK) 
				return 1;
		}
		while (TxBytes != 0 && Timer.Elapsed_Seconds() < CONNECT_TIMEOUT);

		if (TxBytes != 0) return 1;


		// wait for a fixed time for the abort acknowledge
		// to come back which is exactly 3 characters ESC C \r

		Timer.Start();

		do
		{
			ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
			if (ftStatus != FT_OK) 
				return 1;
		}
		while (RxBytes < 3 && Timer.Elapsed_Seconds() < CONNECT_TIMEOUT);


		if (RxBytes == 0)
		{
			// KMotion seems to be present but not responding
			// after several attemps flag as non responsive and
			// stop trying

			NonRespondingCount++;

			if (NonRespondingCount == CONNECT_TRIES)
			{
				ErrorMessageBox(Translate("KMotion present but not responding\r\rCorrect problem and restart application"));
			}
			return 1;
		}
		
		if (RxBytes != 3) return 1;

		NonRespondingCount=0;

		// read the 3 bytes

		ftStatus = FT_Read(ftHandle,RxBuffer,RxBytes,&BytesReceived);
		
		if (ftStatus != FT_OK) return 1; 
		if (BytesReceived != 3) return 1; 

		if (RxBuffer[0] != '\x1b') return 1; 
		if (RxBuffer[1] != 'C') return 1; 
		if (RxBuffer[2] != '\r') return 1; 
	}

	// verify there are no transmit or receive characters

	ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
	if (ftStatus != FT_OK) return 1;
	if (RxBytes != 0) return 1; 
	if (TxBytes != 0) return 1; 
	
	// OK looks like we are in sync

	return 0;
}





int CKMotionIO::Failed()
{
	Mutex->Lock();
	
	m_Connected = false;
	
	if(ConnectSocket)
	{
		closesocket(ConnectSocket);
		ConnectSocket = NULL;
	}
	if(ftHandle != 0)
	{
		FT_Close(ftHandle);
		ftHandle = 0;
	}
	
	if (!FailMessageAlreadyShown)
	{
		ReleaseToken();  // if we failed Release Token if Locked
		ErrorMessageBox(Translate("Read Failed - Auto Disconnect"));
	}
	
	FailMessageAlreadyShown=true;
	
	Mutex->Unlock();
	return 0;
}


int CKMotionIO::Disconnect()
{
	Mutex->Lock();
	
	m_Connected=false;
	
	FT_Close(ftHandle);
	
	Mutex->Unlock();
	return 0;
}


int CKMotionIO::FirmwareVersion()
{
	return m_FirmwareVersion;
}


// if connected, returns the USB location code
// associated with this object


int CKMotionIO::USBLocation()
{
	if (m_Connected)
		return Actual_ID;
	else
		return -1;
}


// returns 0 KMOTION_LOCKED (and token is locked) if KMotion is available for use
// returns 1 KMOTION_IN_USE if already in use
// returns 2 KMOTION_NOT_CONNECTED if not able to connect

// stronger than a Mutex lock because it protects against the same thread

int CKMotionIO::KMotionLock(const char *CallerID)
{
	int result;
	int board = (int)(this - KMotionLocal.KMotionIO);

	if (!Mutex->Lock(3000)) return KMOTION_NOT_CONNECTED;

	if (!m_Connected)
	{
		// try to connect

		if (!RequestedDeviceAvail(NULL))
		{
			NonRespondingCount=0;
			Mutex->Unlock();  // no such device available
			return KMOTION_NOT_CONNECTED; 
		}
		
		if (Connect())
		{
			Mutex->Unlock();  // couldn't connect 
			return KMOTION_NOT_CONNECTED; 
		}
	}


	if (Token==0)
	{
		Token++;
		if (CallerID==NULL)
			m_LastCallerID = "";
		else if (CallerID[0] == 0)
			m_LastCallerID = "Unspecified";
		else m_LastCallerID = CallerID;

		result=KMOTION_LOCKED;
	}
	else
	{
		result=KMOTION_IN_USE;
	}

	Mutex->Unlock();

	return result;
}


// Special routine to connect to KMotion board
// without aborting out of the recovery Bootloader

int CKMotionIO::KMotionLockRecovery()
{
	int result;

	SendAbortOnConnect=false;
	result = KMotionLock("KMotionLockRecovery");
	SendAbortOnConnect=true;
	
	return result;
}


void CKMotionIO::ReleaseToken()
{
	Mutex->Lock();
	m_LastCallerID = "";
	Token--;
	if (Token < 0) Token=0; // ADDED THIS LINE TO ENFORCE Token >= 0
	Mutex->Unlock();
}

int CKMotionIO::LogToConsole(char *s)
{
	int board = (int)(this - KMotionLocal.KMotionIO);

	if (ConsoleHandler)
		ConsoleHandler(board,s);

	return 0;
}

int CKMotionIO::HandleDiskIO(char *s)
{
	static FILE *f=NULL;
	static FILE *fr=NULL;

	int len = (int)strlen(s);

	if (len > 2) s[len-2]=0;  // strip off the CR LF

// esc code 1 = fopen wt
// esc code 2 = fprintf
// esc code 3 = fclose write
// esc code 4 = fopen rt
// esc code 5 = read next
// esc code 6 = fclose read
// esc code 7 = fopen wt

	if (s[0] == 1)  // file open
	{
		if (f) fclose(f);
		f=fopen(s+1,"wt");
	}
	else if (s[0] == 2)  // file write
	{
		if (len > 2)
		{
			s[len-2]='\n';
			s[len-1]=0;
			if (f) fputs(s+1,f);
		}
	}
	else if (s[0] == 3)  // file close
	{
		if (f) fclose(f);
		f=NULL;
	}
	else if (s[0] == 4)  // file open read
	{
		if (fr) fclose(fr);
		fr=fopen(s+1,"rt");

		if (fr)
		{
			return ReadSendNextLine(fr);
		}
		else
		{
			return WriteLine("ReadDiskData 2 0");  // flag as file open error
		}
	}
	else if (s[0] == 5)  // file read
	{
		ReadSendNextLine(fr);
	}
	else if (s[0] == 6)  // file close
	{
		if (fr) fclose(fr);
		fr=NULL;
	}
	else if (s[0] == 7)  // file open append text mode
	{
		if (f) fclose(f);
		f = fopen(s + 1, "at");
	}

	return 0;
}

int CKMotionIO::ReadSendNextLine(FILE *fr)
{
	char s[1024];
	char w[256];

	if (fr)
	{
		if (feof(fr))  // eof?
		{
			return WriteLine("ReadDiskData 3 0");  //signal eof
		}
		else
		{
			if (fgets(s,1024,fr)==NULL)
			{
				return WriteLine("ReadDiskData 3 0");  //signal eof
			}

			char *src, *dst;  // remove any CR LF characters
			for (src = dst = s; *src != '\0'; src++) 
			{
				*dst = *src;
				if (*dst != '\n' && *dst != '\r') dst++;
			}
			*dst = '\0';

			int n = (int)strlen(s);

			Mutex->Lock();

			sprintf(w,"ReadDiskData 1 %d",n);
			if (WriteLine(w))
			{
				Mutex->Unlock();
				return 1;
			}


			int i,k=0;  // reset bytes/line
			for (i=0; i<n; i++)
			{
				sprintf(w+3*k,"%02X ",s[i]);  // append hex code
				k++;
				if (k==80) // full line? 
				{
					w[3*k-1]=0;  // null terminate
					if (WriteLine(w))  //send the line
					{
						Mutex->Unlock();
						return 1;
					}
					k=0;
				}
			}
			
			if (k>0) // any partial line remaining? 
			{
				w[3*k-1]=0;  // null terminate
				if (WriteLine(w))  //send the line
				{
					Mutex->Unlock();
					return 1;
				}
			}
			Mutex->Unlock();
		}
	}
	else
	{
		return WriteLine("ReadDiskData 2 0");  //signal error
	}
	return 0;
}


// Print any data to the console

int CKMotionIO::ServiceConsole()
{
	int nbytes, timeout;
	char b[MAX_LINE];

	if (KMotionLock("Service Console") == KMOTION_LOCKED)  // quick check if it is available
	{
		if (!NumberBytesAvailToRead(&nbytes, false) && nbytes>0)
		{
			// some data in the buffer

			timeout = ReadLineTimeOutRaw(b,100);

			if (!timeout)
			{
				if (b[0]==0x1b)                       // skip over esc if there is one    
					LogToConsole(b+1);
				else
					LogToConsole(b);
			}
		}

		ReleaseToken();
	}
	return 0;
}


int CKMotionIO::MakeSureConnected()
{
	if (!m_Connected)
	{
		if (Connect()) 
		{
			return 1;
		}
	}
	return 0;
}


int CKMotionIO::SetConsoleCallback(SERVER_CONSOLE_HANDLER *ch)
{
	ConsoleHandler = ch;

	return 0;
}

// save the error message to be piped back to caller

int CKMotionIO::ErrorMessageBox(const wchar_t *s)
{
	ErrMsg = s;
	return 0;
}
