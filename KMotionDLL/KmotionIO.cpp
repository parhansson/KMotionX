// KMotionIO.cpp: implementation of the CKMotionIO class.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "StdAfx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKMotionIO::CKMotionIO()
{
	ConsoleHandler=NULL;

	Mutex = new CMutex(FALSE,"KMotionIO",NULL);

	m_Connected=false;
	SendAbortOnConnect=true;
	FailMessageAlreadyShown=false;
	Token=0;
	NonRespondingCount=0;
	m_FirmwareVersion=2;
	USB_Loc_ID=-1;
    BoardIDAssigned=false;

#ifdef _KMOTIONX
    //FT_SetVIDPID(VENDOR, PRODUCT);
    FT_SetVIDPID(1027, 62001);
#endif
}


CKMotionIO::~CKMotionIO()
{
	delete Mutex;
}



bool CKMotionIO::RequestedDeviceAvail(char *Reason)
{
	FT_DEVICE_LIST_INFO_NODE *devInfo;
	FT_STATUS ftStatus;
	DWORD numDevs;  
	int i;

	Mutex->Lock();

	// create the device information list
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	
	if (ftStatus == FT_OK) 
	{ 
		if (numDevs > 0) 
		{ 
			// allocate storage for list based on numDevs 
			devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs); // get the device information list 
			// get the device information list
			ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);

			for (i=0; i<(int)numDevs; i++)
			{
				if (strstr(devInfo[i].Description,"KFLOP")== NULL &&
					strstr(devInfo[i].Description,"KMotion")== NULL &&
					strstr(devInfo[i].Description,"Dynomotion")== NULL)
				{
					// remove it from the list
					for (int k=i+1; k<(int)numDevs; k++)
						devInfo[k-1] = devInfo[k];  // shift up
					
					numDevs--;
					i--; // redo this slot since it was deleted and things shifted up
				}
			}

			// if USB Location is undefined select the first from
			// the list that is not already taken

			if (!BoardIDAssigned)
			{
				for (i=0; i<(int)numDevs && !BoardIDAssigned; i++)
				{
					int k;
					// make sure nobody is already using this one
					for (k=0; k<MAX_BOARDS; k++)
					{
						if (devInfo[i].LocId==KMotionLocal.KMotionIO[k].USB_Loc_ID)
							break;
					}
					if (k==MAX_BOARDS)
					{
						BoardIDAssigned=true;
						USB_Loc_ID=devInfo[i].LocId;  // assign it
					}
				}

				if (!BoardIDAssigned)
				{
					Mutex->Unlock();
					if (Reason) strcpy(Reason,"No KMotion devices available");
					return false;
				}
			}

			// user wants a specific usb location
			// so see if it is available

			for (i=0; i<(int)numDevs; i++)
			{
				if (devInfo[i].LocId==USB_Loc_ID)
					break;
			}

			if (i==numDevs)
			{
				Mutex->Unlock();
				if (Reason) sprintf(Reason,"KMotion not found on USB Location %08X\r\r"
										   "Unable to open device",USB_Loc_ID);
				return false;
			}


			delete (devInfo);
		}
		else
		{
			Mutex->Unlock();
			if (Reason) strcpy(Reason,"No KMotion devices available");
			return false;
		}
	}

	Mutex->Unlock();
	return true;
}



 
#define CONNECT_TRIES 5


int CKMotionIO::Connect()
{
	char reason[256];

	FT_STATUS ftStatus;

	if (NonRespondingCount==CONNECT_TRIES) return 1;

	m_SaveChars[0]=0;  // start anew

	Mutex->Lock();

	if (!RequestedDeviceAvail(reason))
	{
		ErrorMessageBox(reason);
		Mutex->Unlock();
		return 1;
	}


	
	#define TIME_TO_TRY_TO_OPEN 3000

	// FT_ListDevices OK, number of devices connected is in numDevs

	// usually during boot the board comes and goes, since it appeared 
	// to be there, try for a while to open it

	DWORD t0=timeGetTime();

	for (;;) 
	{
		ftStatus = FT_OpenEx((void *)USB_Loc_ID,FT_OPEN_BY_LOCATION,&ftHandle);

		
		if (ftStatus == FT_OK) 
		{
			// FT_Open OK, use ftHandle to access device

			if (SetLatency(2))
			{
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

			if (timeGetTime()-t0 > TIME_TO_TRY_TO_OPEN)
			{
				ErrorMessageBox("Unable to open KMotion device");
				Mutex->Unlock();
				return 1;
			}

			Sleep(100);  // delay a bit then loop back and try again
		}
	}

	Mutex->Unlock();
	return 0;
}

int CKMotionIO::NumberBytesAvailToRead(int *navail, bool ShowMessage)
{
	FT_STATUS ftStatus;
	DWORD EventDWord;
	DWORD RxBytes;
	DWORD TxBytes;

	*navail = strlen(m_SaveChars);  // take into account any already read in
	
	Mutex->Lock();
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


int CKMotionIO::ReadBytesAvailable(char *RxBuffer, int maxbytes, DWORD *BytesReceived, int timeout_ms)
{
	FT_STATUS ftStatus;
	DWORD EventDWord;
	DWORD RxBytes;
	DWORD TxBytes;

	Mutex->Lock();
	ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);

	if (ftStatus != FT_OK)
	{
		Failed();
		Mutex->Unlock();
		return 1;
	}

	if ((int)RxBytes > maxbytes) RxBytes = maxbytes-1; // leave room for null

	RxBuffer[0]=0;  // set buf empty initially
	*BytesReceived=0;

	if (RxBytes > 0) 
	{
		ftStatus = FT_Read(ftHandle,RxBuffer,RxBytes,BytesReceived);
		if (ftStatus == FT_OK) 
		{
			RxBuffer[*BytesReceived]=0;  // null terminate
		}
		else 
		{
			Failed();
			Mutex->Unlock();
			return 1;
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
					if (strstr(copy,"ERROR")!=NULL)
					{
						// error, send to console

						DetectedError=true;
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
							return KMOTION_ERROR;  // a line contained the word "error"
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
	char ReadBuffer[MAX_LINE];
	int i,freespace;
	bool FirstTime=true;
	static bool ErrorDisplayed=false;


	if (ErrorDisplayed) return 1;

	Mutex->Lock();

	strcpy(buf,m_SaveChars);
	TotalBytes=strlen(buf);
	m_SaveChars[0]=0; // remember we used them

	DWORD t0=timeGetTime();

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
						ErrorMessageBox("KMotion Received Line too long");
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

		if (!NO_KMOTION_TIMEOUT && !Done && (int)(timeGetTime()-t0) > TimeOutms) 
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
	DWORD BytesWritten;
	int length;
	char s2[MAX_LINE+1];   

	if (MakeSureConnected()) return 1;
    strcpy(s2,s);
	strcat(s2,"\r"); // Add CR

	length = strlen(s2);

	Mutex->Lock();
	ftStatus = FT_Write(ftHandle, s2, length, &BytesWritten);

	Mutex->Unlock();

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
			ErrorMessageBox("Unable to set USB Event Character");
			Mutex->Unlock();
			return 1;
		}
	}
	else 
	{ 
		// FT_SetLatencyTimer FAILED!
		ErrorMessageBox("Unable to set USB Latency timer");
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

	if (ReadLineTimeOut(response,3000))
	{
		Mutex->Unlock();
		return 1;
	}
	
	response[strlen(response)-2]=0;  // remove the /r /n

	Mutex->Unlock();

	return 0;
}

#define CONNECT_TIMEOUT 1000

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

	ftStatus = FT_Purge(ftHandle,FT_PURGE_RX|FT_PURGE_TX);
	if (ftStatus != FT_OK) 	return 1;


	// discard any data in the read queue in the driver

	DWORD t0=timeGetTime();
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
	while (RxBytes > 0 && timeGetTime()-t0 < CONNECT_TIMEOUT);



	if (SendAbortOnConnect)
	{
		// send flush command to DSP

		s[0]=ABORT_CHAR;
		s[1]=0;


		ftStatus = FT_Write(ftHandle, s, 1, &BytesWritten);
		if (ftStatus != FT_OK) 
			return 1;
		if (BytesWritten != 1) 
			return 1;


		// wait and be sure chars are transmitted

		t0=timeGetTime();
		do
		{
			ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
			if (ftStatus != FT_OK) 
				return 1;
		}
		while (TxBytes != 0 && timeGetTime()-t0 < CONNECT_TIMEOUT);

		if (TxBytes != 0) return 1;


		// wait for a fixed time for the abort acknowledge
		// to come back which is exactly 3 characters ESC C \r

		t0=timeGetTime();

		do
		{
			ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
			if (ftStatus != FT_OK) 
				return 1;
		}
		while (RxBytes < 3 && timeGetTime()-t0 < CONNECT_TIMEOUT);


		if (RxBytes == 0)
		{
			// KMotion seems to be present but not responding
			// after several attemps flag as non responsive and
			// stop trying

			NonRespondingCount++;

			if (NonRespondingCount == CONNECT_TRIES)
			{
				ErrorMessageBox("KMotion present but not responding\r\r"
								"Correct problem and restart application");
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
	
	m_Connected=false;
	
	FT_Close(ftHandle);
	
	if (!FailMessageAlreadyShown)
		ErrorMessageBox("Read Failed - Auto Disconnect");
	
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
		return USB_Loc_ID;
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
	int board = this - KMotionLocal.KMotionIO;

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
			m_LastCallerID[0] = '\0';
		else
			strcpy(m_LastCallerID, CallerID);

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
	m_LastCallerID[0] = '\0';
	Token--;
	if (Token < 0) Token=0; // ADDED THIS LINE TO ENFORCE Token >= 0
	Mutex->Unlock();
}

int CKMotionIO::LogToConsole(char *s)
{
	int board = this - KMotionLocal.KMotionIO;

	if (ConsoleHandler)
		ConsoleHandler(board,s);

	return 0;
}

int CKMotionIO::HandleDiskIO(char *s)
{
	static FILE *f=NULL;
	static FILE *fr=NULL;

	int len = strlen(s);

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

			int n = strlen(s);

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

int CKMotionIO::ErrorMessageBox(const char *s)
{
	strcpy(ErrMsg,s);
	return 0;
}
