/*
Copyright (c) 2014, P.Hansson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
 */
// KMotionIO.cpp: implementation of the CKMotionIO class.


#include "StdAfx.h"
#include <stdlib.h>
#include <ftdi.h>

int _ftdi_usb_close(ftdi_context *ftdi){
#ifdef __APPLE__
  return ftdi_usb_close(ftdi);
#else
  //Still needed on Ubuntu
  //Wrapper method due to bug in old libftdi driver
  //http://developer.intra2net.com/mailarchive/html/libftdi/2009/msg00214.html
	int ret = 0;
	if (ftdi->usb_dev != NULL){
		ret = ftdi_usb_close(ftdi);
		ftdi->usb_dev = NULL;
  }
  return ret;
#endif
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKMotionIO::CKMotionIO()
{
	ConsoleHandler=NULL;

	Mutex = new CMutex(FALSE,"KMotionIO",0);

	m_Connected=false;
	SendAbortOnConnect=true;
	FailMessageAlreadyShown=false;
	Token=0;
	NonRespondingCount=0;
	m_FirmwareVersion=2;
	USB_Loc_ID=-1;
  BoardIDAssigned=false;

  if ((ftdi = ftdi_new()) == 0)
  {
    log_info("ftdi_new failed.");
    exit(EXIT_FAILURE);
  }
}


CKMotionIO::~CKMotionIO()
{
  _ftdi_usb_close(ftdi);
	ftdi_free(ftdi);
	delete Mutex;
}

bool CKMotionIO::RequestedDeviceAvail(char *Reason)
{
	int ftStatus, i, numDevs, list[MAX_BOARDS];
	struct ftdi_device_list *devlist, *curdev;
	bool error;
	char Manufacturer[128];
	//char SerialNumber[16];
	char Description[64]; 

	error = false;
	devlist = NULL;

	Mutex->Lock();

	// fill the list with -1 because driver
	// leaves the entries for open drivers unchanged

	for (i=0; i<MAX_BOARDS; i++) list[i]=-1;
	numDevs = ftStatus = ftdi_usb_find_all(ftdi, &devlist, VENDOR, PRODUCT );
	if (ftStatus < 0)
	{
		if (Reason) sprintf(Reason,"Failed to enumerate USB devices. %s(%d)",ftdi_get_error_string(ftdi), ftStatus);
		error = true;
	}
	else if (numDevs == 0)
	{
    error = true;
		if (Reason) strcpy(Reason,"No KMotion devices available");
	}
  else if (numDevs > MAX_BOARDS)
  {
    error = true;
    if (Reason) strcpy(Reason,"Too many KMotion devices available");
  }

	if(error){
	  if(devlist != NULL){
      ftdi_list_free(&devlist);
	  }
		Mutex->Unlock();
		return false;

	}
	// go through the list and remove any non-dynomotion boards
	i = 0;
	for (curdev = devlist; curdev != NULL; i++)
	{
		if ((ftStatus = ftdi_usb_get_strings(ftdi, curdev->dev, Manufacturer, 128, Description, 128, NULL, 0)) < 0)
		{
			log_info("ftdi_usb_get_strings failed: %d (%s)",ftStatus, ftdi_get_error_string(ftdi));
		   // FT_Open failed
		   list[i] = -1;  // mark as unusable
		}
		else
		{
		  debug("%s\n%s",Manufacturer, Description);
			if (strstr(Description,"KFLOP")!= NULL ||
				strstr(Description,"KMotion")!= NULL ||
				strstr(Description,"Dynomotion")!= NULL)
			{
				//save index
				list[i] = i;
			}
		}
		curdev = curdev->next;
	}

	ftdi_list_free(&devlist);

	// if USB Location is undefined select the first from
	// the list that is not already taken
	if (!BoardIDAssigned)
	{
		for (i=0; i<numDevs && !BoardIDAssigned; i++)
		{
			if (list[i] != -1)
			{
				int k;
				// make sure nobody is already using this one
				for (k=0; k<MAX_BOARDS; k++)
				{
					if (list[i]==KMotionLocal.KMotionIO[k].USB_Loc_ID)
						break;
				}
				if (k==MAX_BOARDS)
				{
					BoardIDAssigned=true;
					USB_Loc_ID=list[i];  // assign it
					debug("Board location %d", USB_Loc_ID);
				}
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

	for (i=0; i<numDevs; i++)
	{
		if (list[i]==USB_Loc_ID)
			break;
	}

	if (i==numDevs)
	{
		Mutex->Unlock();
		if (Reason) sprintf(Reason,"KMotion not found on USB Location %08X\n"
								   "Unable to open device",USB_Loc_ID);
		return false;
	}

	Mutex->Unlock();
	return true;
}


#define CONNECT_TRIES 5


int CKMotionIO::Connect()
{
	char reason[256];

	int ftStatus;

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

	uint32_t t0=timeGetTime();

	for (;;) 
	{
		ftStatus = ftdi_usb_open_desc_index(ftdi, VENDOR, PRODUCT, NULL, NULL, USB_Loc_ID);
		
		if (ftStatus < FT_OK)
		{
		  log_info("ftdi_usb_open_desc_index failed: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
		  // FT_Open failed
			if (timeGetTime()-t0 > TIME_TO_TRY_TO_OPEN)
			{
			  ErrorMessageBox("Unable to open KMotion device");
				Mutex->Unlock();
				return 1;
			}

			Sleep(100);  // delay a bit then loop back and try again
		}
		else
		{
			// FT_Open OK, use ftHandle to access device

			if (SetLatency(2))
			{
				//Close handle
				if(_ftdi_usb_close(ftdi) < FT_OK){
				  log_info("_ftdi_usb_close failed: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
				}
				Mutex->Unlock();
				return 1;
			}

			
			if (FlushInputBuffer())
			{
				log_info("FAIL: FlushInputBuffer for device %d",USB_Loc_ID);
				//This is the key to why it works on second attempt
				//After usb_close has executed once the device works forever.
				//The ftdi device is not closed when killing the KMotionServer
				//Try to recover from this action when initialising this class.
        if(_ftdi_usb_close(ftdi) < FT_OK){
          log_info("_ftdi_usb_close failed: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
        }
				Mutex->Unlock();
				return 1;
			}
			m_Connected=true;  // All set

			Mutex->Unlock();
			return 0;
		}
	}

	Mutex->Unlock();
	return 0;
}

int CKMotionIO::NumberBytesAvailToRead(int *navail, bool ShowMessage)
{
	/*
	int ftStatus;
	uint32_t EventDWord;
	uint32_t RxBytes;
	uint32_t TxBytes;

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
	*/
	return 0;
}


int CKMotionIO::ReadBytesAvailable(char *RxBuffer, int maxbytes, uint32_t *BytesReceived, int timeout_ms)
{
	int ftStatus;
	int RxBytes;

	Mutex->Lock();
/*
	ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);

	if (ftStatus != FT_OK)
	{
		Failed();
		Mutex->Unlock();
		return 1;
	}
	if ((int)RxBytes > maxbytes) RxBytes = maxbytes-1; // leave room for null
*/
	RxBytes = maxbytes-1; // leave room for null
	RxBuffer[0]=0;  // set buf empty initially
	*BytesReceived=0;
/*
	if (RxBytes > 0) 
	{
*/

		*BytesReceived = ftStatus = ftdi_read_data(ftdi,(unsigned char *)RxBuffer,RxBytes);
		if (ftStatus >= FT_OK)
		{
		  RxBuffer[*BytesReceived]=0;  // null terminate
		}
		else 
		{
			log_info("FAIL:ftdi_read_data status: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
			Failed();
			Mutex->Unlock();
			return 1;
		}
/*
	}
*/
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
	//int nbytes;
	int result;
	bool DetectedError=false;

	for (;;) 
	{
		if (m_Connected)
		{
		  result = ReadLineTimeOutRaw(buf, 100);
			//if (!NumberBytesAvailToRead(&nbytes, true) && nbytes>0)
			if(strlen(buf)>0) // If no data is read this is actually a timeout. Not very good to always wait for timeout
			{
				//result = ReadLineTimeOutRaw(buf, 100);
				
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
	bool Done=false;
	int TotalBytes, result;
	uint32_t NBytesRead;
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

	uint32_t t0=timeGetTime();

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

			Done=true;
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
	int ftStatus;
	int length;
	char s2[MAX_LINE+1];   

	if (MakeSureConnected()) return 1;
    strcpy(s2,s);
	strcat(s2,"\r"); // Add CR

	length = strlen(s2);

	Mutex->Lock();
	ftStatus = ftdi_write_data(ftdi,(unsigned char *)s2,length);
	Mutex->Unlock();

	return 0;
}

int CKMotionIO::SetLatency(uint8_t LatencyTimer)
{
	int ftStatus;
		
	unsigned char c;

	
	Mutex->Lock();

	ftStatus = ftdi_get_latency_timer(ftdi,&c );
	if(c != LatencyTimer){
		log_info("ftdi_get_latency_timer old value %d", c);
		ftStatus = ftdi_set_latency_timer(ftdi,LatencyTimer );

		if (ftStatus < FT_OK)
		{
			// FT_SetLatencyTimer FAILED!
			ErrorMessageBox("Unable to set USB Latency timer");
			Mutex->Unlock();
			return 1;
		}
	}
	// LatencyTimer set


	ftStatus = ftdi_set_event_char(ftdi,'\n',1);

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


int CKMotionIO::WriteLineReadLine(const char *send, char *response)
{
	Mutex->Lock();
	if (WriteLine(send))
	{
		Mutex->Unlock();
		debug("WriteLine failed.");
		return 1;
	}

	if (ReadLineTimeOut(response,3000))
	{
		Mutex->Unlock();
		//TODO this fails occasionally
		debug("ReadLineTimeOut failed.");
		return 1;
	}
	//TODO handle if response len < 2
	response[strlen(response)-2]=0;  // remove the /r /n

	Mutex->Unlock();

	return 0;
}

#define CONNECT_TIMEOUT 1000
#define CONNECT_TIMEOUT_USEC CONNECT_TIMEOUT*1000

int CKMotionIO::FlushInputBuffer()
{
	int ftStatus;
	int BytesReceived = 0;
	int BytesWritten;
	char s[64];
	unsigned char RxBuffer[500];

	// discard any data in the read queue in the driver
  ftStatus = ftdi_usb_purge_buffers(ftdi);
	if (ftStatus < FT_OK){
		log_info("ftdi_usb_purge_buffers failed: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
		return 1;
	}


	uint32_t t0=timeGetTime();
#ifdef __APPLE__
	//On OS X we need this ugly hack.
	//Even if KMotionServer is shutdown, sending abort char only works on first connect since device is connected or power cycled.
	//If sending abort is skipped on subsequent attempts everything seems to work anyway.
	//However if anything is else is written to the device before sending abort it starts working as expected.
	//My suspicion after looking (into libusb code) is that the device is cached somehow,
	//and not resetted(buffers?) or handshaked properly in the underlying USB driver (IOUSBFamily)

	//A dummy write seems to kickstart the whole thing

	strcpy(s,"\x1b\x01ReadBit0\r"); // write message no echo, Use a message that KFlop understands not to confuse it
  //We don't care about error handling here since things seems to start working regardless of the result here.
	//It just needs to be done.

	ftdi_write_data(ftdi,(unsigned char *)s,1);
  do
  {
      BytesReceived = ftdi_read_data(ftdi,RxBuffer, sizeof(RxBuffer) / sizeof(char));
  }
  while (BytesReceived > 0 && timeGetTime()-t0 < CONNECT_TIMEOUT);

#endif

	if (SendAbortOnConnect)
	{
		// send flush command to DSP

		s[0]=ABORT_CHAR;
		s[1]=0;

    BytesWritten = ftStatus = ftdi_write_data(ftdi,(unsigned char *)s,1);
    if (ftStatus < FT_OK){
      log_info("FAIL: ftdi_write_data status: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
      return 1;
    }
    if (BytesWritten != 1){
      log_info("FAIL:bytes written expected 1 actual value: %d", BytesWritten);
      return 1;
    }

    // wait for a fixed time for the abort acknowledge
    // to come back which is exactly 3 characters ESC C \r
    do
    {
      BytesReceived = ftStatus = ftdi_read_data(ftdi,RxBuffer,3);
      usleep(CONNECT_TIMEOUT_USEC/10);
    }
    while (BytesReceived == 0 && timeGetTime()-t0 < CONNECT_TIMEOUT);



		if (BytesReceived == 0)
		{
			// KMotion seems to be present but not responding
			// after several attemps flag as non responsive and
			// stop trying
			log_info("Abort not responding attempt %d", NonRespondingCount);

			NonRespondingCount++;

			if (NonRespondingCount == CONNECT_TRIES)
			{
				ErrorMessageBox("KMotion present but not responding\n"
								"Correct problem and restart application");
			}
			return 1;
		}

		NonRespondingCount=0;

		if (BytesReceived != 3){log_info("Expected bytes is 3. Received bytes %d", BytesReceived); return 1;}

		if (RxBuffer[0] != '\x1b'){log_info("Received byte[0] 1b != %x", RxBuffer[0]); return 1;}
		if (RxBuffer[1] != 'C'){log_info("Received byte[1] C != %c", RxBuffer[1]); return 1;}
		if (RxBuffer[2] != '\r'){log_info("Received byte[2] \\r != %c", RxBuffer[2]); return 1;}

	}

	// verify there are no transmit or receive characters

	// OK looks like we are in sync

	return 0;
}





int CKMotionIO::Failed()
{
	Mutex->Lock();
	
	m_Connected=false;
	
    if (_ftdi_usb_close(ftdi) < 0)
    {
      log_info("unable to close ftdi device: (%s)", ftdi_get_error_string(ftdi));
    }
	
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
	
    if (_ftdi_usb_close(ftdi) < 0)
    {
      log_info("unable to close ftdi device: (%s)", ftdi_get_error_string(ftdi));
    }
	
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
	char reason[256];
	if (!Mutex->Lock(3000)) return KMOTION_NOT_CONNECTED;

	if (!m_Connected)
	{
		// try to connect

		if (!RequestedDeviceAvail(reason))
		{
		  debug("KMOTION_NOT_CONNECTED %s", reason);
			NonRespondingCount=0;
			Mutex->Unlock();  // no such device available
			return KMOTION_NOT_CONNECTED; 
		}
		
		if (Connect())
		{
			Mutex->Unlock();  // couldn't connect 
			debug("KMOTION_NOT_CONNECTED");
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
	  debug("KMOTION_IN_USE");
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
	debug("LogToConsole %s",s);
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
	int nbytes;
	int timeout;
	char buf[MAX_LINE];

	if (KMotionLock("Service Console") == KMOTION_LOCKED)  // quick check if it is available
	{
		//if (!NumberBytesAvailToRead(&nbytes, false) && nbytes>0)
	  //Instead of making one call with a long time out we first make a call with a short time out,
	  //if data is read we need to check if we had a timeout and ask again with a greater value.
	  timeout = ReadLineTimeOutRaw(buf,0);  // likely we will get a timeout sometimes
	  nbytes = strlen(buf);

	  if(nbytes>0)
		{
			// some data in the buffer
	    if(timeout){
          //if previos call timed out we can try again with a longer timout adding read bytes to buffer pointer
          timeout = ReadLineTimeOutRaw(buf + nbytes ,100);
	    }


			if (!timeout)
			{
				if (buf[0]==0x1b)                       // skip over esc if there is one
					LogToConsole(buf+1);
				else
					LogToConsole(buf);
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
