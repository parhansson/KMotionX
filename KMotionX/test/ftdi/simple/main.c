/*
	To build use the following gcc statement 
	(assuming you have the d2xx library in the /usr/local/lib directory).
	gcc -o setVIDPID main.c -L. -lftd2xx -Wl,-rpath,/usr/local/lib 
*/

#include <stdio.h>
#include <sys/time.h>
#include "../ftd2xx.h"
#include <string.h>
#define timeBeginPeriod(x)
#define timeEndPeriod(x)
#define CONNECT_TIMEOUT 1000
#define ABORT_CHAR 0x03  // ctrl-c clears/aborts any command
#define MAX_BOARDS 16
#define MAX_LINE 2560
#define true TRUE
#define false FALSE
typedef int bool;
#define NO_KMOTION_TIMEOUT FALSE // useful for debugging
enum // KMotionLocked Return Codes
{
	KMOTION_LOCKED=0,        // (and token is locked) if KMotion is available for use
	KMOTION_IN_USE=1,        // if already in use
	KMOTION_NOT_CONNECTED=2, // if error or not able to connect
};

	bool BoardIDAssigned;
	int USB_Loc_ID;
	bool m_Connected;
	char* ErrMsg;
	int Token;
	char m_SaveChars[MAX_LINE+1];
	FT_HANDLE ftHandle;

	int m_FirmwareVersion;
	int NonRespondingCount;
	bool FailMessageAlreadyShown;
	bool SendAbortOnConnect;

int SetLatency(unsigned char);
int Connect();
bool RequestedDeviceAvail();
int FlushInputBuffer();
int KMotionLock();
void ReleaseToken();
int WriteLineWithEcho(const char *);
int WriteLine(const char *);
int WriteLineReadLine(const char *, char *);
int MakeSureConnected();
int ReadLineTimeOutRaw(char *, int);
int ReadLineTimeOut(char *, int);
int ReadBytesAvailable(char *, int , DWORD *, int );
int Failed();
void Disconnect();

void printDevicesInfo(){
	FT_STATUS ftStatus;
	FT_DEVICE_LIST_INFO_NODE *devInfo;
	DWORD numDevs;
	// create the device information list
	ftStatus = FT_CreateDeviceInfoList(&numDevs);
	if (ftStatus == FT_OK) {
	       printf("Number of devices is %d\n",numDevs);
	}
	if (numDevs > 0) {
	       // allocate storage for list based on numDevs
	       devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
	       // get the device information list
	       ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
	       if (ftStatus == FT_OK) {
	    	   	   int i;
	             for (i = 0; i < numDevs; i++) {
	                    printf("Dev %d:\n",i);
	                    printf("  Flags=0x%x\n",devInfo[i].Flags);
	                    printf("  Type=0x%x\n",devInfo[i].Type);
	                    printf("  ID=0x%x\n",devInfo[i].ID);
	                    printf("  LocId=0x%x\n",devInfo[i].LocId);
	                    printf("  SerialNumber=%s\n",devInfo[i].SerialNumber);
	                    printf("  Description=%s\n",devInfo[i].Description);
	                    printf("  ftHandle=0x%x\n",devInfo[i].ftHandle);
	             }
	       }
	       free (devInfo);
	}
}

unsigned int timeGetTime()
{
	struct timeval now;
	//Note that gettimeofday returns time relative to an epoch while timeGetTime on Windows returns based on system startup time.
	gettimeofday(&now, NULL);
	int v = (now.tv_sec)*1000+(now.tv_usec)/1000 - 1387584000000;
	//printf("timeGetTime=%d %d\n",v, (unsigned int)v);
	return (unsigned int)v;
}
int main(int argc, char *argv[])
{	

	m_Connected=false;
	SendAbortOnConnect=true;
	FailMessageAlreadyShown=false;
	Token=0;
	NonRespondingCount=0;
	m_FirmwareVersion=2;
	USB_Loc_ID=-1;
    BoardIDAssigned=false;

	(void)argc; /* Deliberately unused parameter */
	(void)argv; /* Deliberately unused parameter */

	
	FT_SetVIDPID(1027, 62001);
	//if(Connect()){
	//	printf("Connect failed\n");
	//}
	//printDevicesInfo();

	if(KMotionLock() == KMOTION_LOCKED){
		printf("KMotionLocked\n");
		char response[MAX_LINE+1];
		char request[MAX_LINE+1];
		const char *s = "DefineCS";
		//memcpy(d,&code,4);
		//memcpy(d+4,&BoardID,4);
		strcpy(request,s);
		int result = WriteLineReadLine(request, response);
		if(!result){
			printf("WriteLineReadLine \n\tresult=%d \n\trequest=%s \n\treply=%s \n",result,request,response);

		}
	}
	//printDevicesInfo();
	//Disconnect();

	ReleaseToken();
	if(KMotionLock() == KMOTION_LOCKED){
			printf("KMotionLocked\n");
	} else{

		printf("KMotionLock failed %d\n",KMotionLock());
	}
	ReleaseToken();
	Disconnect();
	return 0;
}


int MakeSureConnected()
{
	if (!m_Connected)
	{
		if (Connect())
		{
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			return 1;
		}
	}
	return 0;
}

int WriteLineWithEcho(const char *s)
{
	FT_STATUS ftStatus;
	DWORD BytesWritten;
	int length;
	char s2[MAX_LINE+1];

	if (MakeSureConnected()){
		printf("%s:%d Failure!\n",__FILE__,__LINE__);
		return 1;
	}
    strcpy(s2,s);
	strcat(s2,"\r"); // Add CR \r

	length = strlen(s2);

	ftStatus = FT_Write(ftHandle, s2, length, &BytesWritten);

	return 0;
}

int WriteLine(const char *s)
{
	char s2[MAX_LINE+1]="\x1b\x01";

	strcat(s2,s);
	return WriteLineWithEcho(s2);
}

int WriteLineReadLine(const char *send, char *response)
{
	if (WriteLine(send))
	{
		printf("%s:%d Failure!\n",__FILE__,__LINE__);
		return 1;
	}
	if (ReadLineTimeOut(response,3000))
	{
		printf("%s:%d Failure!\n",__FILE__,__LINE__);
		return 1;
	}
	response[strlen(response)-2]=0;  // remove the /r /n

	return 0;
}

void ReleaseToken()
{
	Token--;
	if (Token < 0) Token=0; // ADDED THIS LINE TO ENFORCE Token >= 0
}
int KMotionLock()
{
	int result;
	//CString reason;
	//if (!Mutex->Lock(3000)) return KMOTION_NOT_CONNECTED;

	if (!m_Connected)
	{
		// try to connect

		if (!RequestedDeviceAvail())
		{
			NonRespondingCount=0;
			return KMOTION_NOT_CONNECTED;
		}

		if (Connect())
		{
			return KMOTION_NOT_CONNECTED;
		}
	}


	if (Token==0)
	{
		Token++;
		result=KMOTION_LOCKED;
	}
	else
	{
		result=KMOTION_IN_USE;
	}


	return result;
}

bool RequestedDeviceAvail(){
	FT_STATUS ftStatus;
	FT_DEVICE ftDevice;
	DWORD deviceID;
	int i, numDevs, list[MAX_BOARDS];
	char SerialNumber[16]; 
	char Description[64]; 

	
	// fill the list with -1 because driver
	// leaves the entries for open drivers unchanged

	for (i=0; i<MAX_BOARDS; i++) list[i]=-1;

	numDevs=-1;

	ftStatus = FT_ListDevices(list,&numDevs,FT_LIST_ALL|FT_OPEN_BY_LOCATION);
	
	if (numDevs==0)
	{
		printf("Numdevs = %d\n" ,numDevs);
		return false;
	}

	if (numDevs < 1 || numDevs >= MAX_BOARDS)
	{
		printf("Numdevs = %d\n" ,numDevs);
		return false;
	}

	// go through the list and remove any non-dynomotion boards
	for (i=0; i<numDevs; i++)
	{
		if (list[i] != -1)
		{
			//According to documentation FT_OPEN_BY_LOCATION is not supported on linux
			ftStatus = FT_OpenEx((void *)list[i], FT_OPEN_BY_LOCATION, &ftHandle);
			//ftStatus = FT_OpenEx("KFT13ZAM",FT_OPEN_BY_SERIAL_NUMBER,&ftHandle);

			if(ftStatus != FT_OK)
			{ 
			   // FT_Open failed 
				printf("Open device failed code %d\n",ftStatus);

			   list[i] = -1;  // mark as unusable 
			} 
			else
			{

				ftStatus = FT_GetDeviceInfo( 
					  ftHandle, 
					  &ftDevice, 
					  &deviceID, 
					  SerialNumber, 
					  Description, 
					  NULL 
					  ); 

				if (ftStatus == FT_OK) 
				{
					ftStatus = FT_Close(ftHandle);

					printf("Device found. Description: %s Serialnumber: %s DeviceID 0x%08x\n" ,Description,SerialNumber, deviceID);
				}
			}
		}
	}

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
					//if (list[i]==KMotionLocal.KMotionIO[k].USB_Loc_ID)
					//	break;
				}
				if (k==MAX_BOARDS)
				{
					BoardIDAssigned=true;
					USB_Loc_ID=list[i];  // assign it
				}
			}
		}
		if (!BoardIDAssigned)
		{
			printf("No KMotion devices available\n");
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
		printf("KMotion not found on USB Location %08X\n"
								   "Unable to open device\n",USB_Loc_ID);
		return false;
	}

	return true;
}

int Connect()
{
    
	FT_STATUS ftStatus;
    
	if (NonRespondingCount==2) return 1;
    
	m_SaveChars[0]=0;  // start anew
    
	if (!RequestedDeviceAvail())
	{
		printf("In Connect() RequestedDeviceAvail failed\n");
		return 1;
	}
    
	

#define TIME_TO_TRY_TO_OPEN 3000
    
	// FT_ListDevices OK, number of devices connected is in numDevs
    
	// usually during boot the board comes and goes, since it appeared
	// to be there, try for a while to open it
    
	DWORD t0=timeGetTime();
    
	for (;;)
	{
		//According to documentation FT_OPEN_BY_LOCATION is not supported on linux
		ftStatus = FT_OpenEx((void *)USB_Loc_ID,FT_OPEN_BY_LOCATION,&ftHandle);
		//ftStatus = FT_OpenEx("KFT13ZAM",FT_OPEN_BY_SERIAL_NUMBER,&ftHandle);

		
		if (ftStatus == FT_OK)
		{
			//ftStatus = FT_SetDeadmanTimeout(ftHandle,500);
			//ftStatus = FT_SetUSBParameters(ftHandle, 64, 0);
			//if (ftStatus != FT_OK) {
			//	printf("FT_SetUSBParameters FAILED error code %d\n", ftStatus);
			//}
			//This really does make a mence
			ftStatus = FT_ResetDevice(ftHandle);
			if (ftStatus == FT_OK) {
			   // Set Deadman Timer to 6 seconds

			}
			else {
				printf("FT_ResetDevice FAILED error code %d\n", ftStatus);
			}

			// FT_Open OK, use ftHandle to access device
            
			if (SetLatency(2))
			{
				printf("SetLatency FAILED\n");
				return 1;
			}
            
			
			if (FlushInputBuffer())
			{
				printf("FlushInputBuffer FAILED\n");
				FT_Close(ftHandle);
				return 1;
			}
            
			m_Connected=true;  // All set
            
			return 0;
		}
		else
		{
			// FT_Open failed
            
			if (timeGetTime()-t0 > TIME_TO_TRY_TO_OPEN)
			{
				printf("Unable to open KMotion device\n");
				return 1;
			}
            
			usleep(100*1000);  // delay a bit then loop back and try again
		}
	}
    
	return 0;
}

int SetLatency(unsigned char LatencyTimer)
{
	FT_STATUS ftStatus;
		
	unsigned char c;

	ftStatus = FT_GetLatencyTimer (ftHandle, &c);
	if(c != LatencyTimer){
		printf("FT_GetLatencyTimer old value %d\n", c);
		ftStatus = FT_SetLatencyTimer(ftHandle,LatencyTimer);
	}
	if (ftStatus == FT_OK) 
	{ 
		if(c != LatencyTimer){
			ftStatus = FT_GetLatencyTimer (ftHandle, &c);
			printf("FT_GetLatencyTimer new value %d\n", c);
		// LatencyTimer set 

		}

		ftStatus = FT_SetChars (ftHandle, '\n', 1, 0,0);

		if (ftStatus == FT_OK) 
		{ 
			// Event set 
			return 0;
		}
		else 
		{ 
			// FT_SetLatencyTimer FAILED!
			printf("Unable to set USB Event Character\n");
			return 1;
		}
	}
	else 
	{ 
		// FT_SetLatencyTimer FAILED!
		printf("Unable to set USB Latency timer\n");
		return 1;
	}
}

int FlushInputBuffer()
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
	if (ftStatus != FT_OK){
		printf("%s:%d Failure!\n",__FILE__,__LINE__);
		return 1;
	}


	// discard any data in the read queue in the driver

	DWORD t0=timeGetTime();
	do
	{
		ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
		if (ftStatus != FT_OK){
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			return 1;
		}

		if (RxBytes > 0)
		{
			if (RxBytes > 400) RxBytes=400;
			ftStatus = FT_Read(ftHandle,RxBuffer,RxBytes,&BytesReceived);
			if (ftStatus != FT_OK){
				printf("%s:%d Failure!\n",__FILE__,__LINE__);
			}
		}
	}
	while (RxBytes > 0 && timeGetTime()-t0 < CONNECT_TIMEOUT);



	if (SendAbortOnConnect)
	{
		// send flush command to DSP

		s[0]=ABORT_CHAR;
		s[1]=0;//'\n';//0;


		ftStatus = FT_Write(ftHandle, s, 1, &BytesWritten);
		if (ftStatus != FT_OK){
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			return 1;
		}
		if (BytesWritten != 1){
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			return 1;
		}

		// wait and be sure chars are transmitted

		t0=timeGetTime();
		do
		{
			ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
			if (ftStatus != FT_OK){
				printf("%s:%d Failure!\n",__FILE__,__LINE__);
				return 1;
			}
			if(RxBytes > 0){
				printf("TX RxBytes=%d\n",RxBytes);
			}
			usleep(10000);
		}
		while (TxBytes != 0 && timeGetTime()-t0 < CONNECT_TIMEOUT);

		if (TxBytes != 0){
			printf("%s:%d Failure TX timeout %d \n",__FILE__,__LINE__,timeGetTime()-t0);
			return 1;
		}


		// wait for a fixed time for the abort acknowledge
		// to come back which is exactly 3 characters ESC C \r

		t0=timeGetTime();

		do
		{
			ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
			//ftStatus=FT_GetQueueStatus(ftHandle,&RxBytes);
			if (ftStatus != FT_OK){
				printf("%s:%d Failure!\n",__FILE__,__LINE__);
				return 1;
			}
			if(RxBytes > 0){
				printf("RX RxBytes=%d\n",RxBytes);
			}
			usleep(10000);
		}
		while (RxBytes < 3 && timeGetTime()-t0 < CONNECT_TIMEOUT);


		if (RxBytes == 0)
		{
			printf("%s:%d Failure RX timeout %d \n",__FILE__,__LINE__,timeGetTime()-t0);
			// KMotion seems to be present but not responding
			// after two attemps flag as non responsive and
			// stop trying

			NonRespondingCount++;

			if (NonRespondingCount == 2)
			{
				printf("KMotion present but not responding\n"
								"Correct problem and restart application\n");
			}
			return 1;
		}

		if (RxBytes != 3){
			printf("%s:%d Failure RX bytes %d timeout %d \n",__FILE__,__LINE__,RxBytes, timeGetTime()-t0);
			return 1;
		}

		NonRespondingCount=0;

		// read the 3 bytes

		ftStatus = FT_Read(ftHandle,RxBuffer,RxBytes,&BytesReceived);

		if (ftStatus != FT_OK){
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			return 1;
		}
		if (BytesReceived != 3){
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			return 1;
		}

		if (RxBuffer[0] != '\x1b'){
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			return 1;
		}
		if (RxBuffer[1] != 'C'){
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			return 1;
		}
		if (RxBuffer[2] != '\r'){
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			return 1;
		}
	}

	// verify there are no transmit or receive characters

	ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
	if (ftStatus != FT_OK){
		printf("%s:%d Failure!\n",__FILE__,__LINE__);
		return 1;
	}
	if (RxBytes != 0){
		printf("%s:%d Failure!\n",__FILE__,__LINE__);
		return 1;
	}
	if (TxBytes != 0){
		printf("%s:%d Failure!\n",__FILE__,__LINE__);
		return 1;
	}

	// OK looks like we are in sync

	return 0;
}


// Reads a line and if it gets a line with no timeout
// check if it is a user message (indicated by esc char
// at the beginning.  If so print it to the Console
// repeat until we get a normal line to be returned
// to the caller
int ReadLineTimeOut(char *buf, int TimeOutms)
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

				// if next character is 1, 2, or 3
				// handle it as a write file command

				if (buf[1] >=1 && buf[1] <=3)
				{
					// it is a disk command
					printf("%s:%d TODO HandleDiskIO\n",__FILE__,__LINE__);
					//HandleDiskIO(buf+1);
				}
				else
				{
					// Send to console
					printf("%s:%d TODO LogToConsole\n",__FILE__,__LINE__);
					//LogToConsole(buf+1);
				}

				Done=false;
			}
		}
	}
	while (!Done);

	return result;
}

int ReadLineTimeOutRaw(char *buf, int TimeOutms)
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
			usleep(1 *1000);
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
				printf("%s:%d Failure!\n",__FILE__,__LINE__);
				return result;
			}

			if (NBytesRead > 0)
			{
				for (i=0; i<(int)NBytesRead; i++)
				{
					if (i+TotalBytes > MAX_LINE)
					{
						printf("KMotion Received Line too long\n");
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
			printf("%s:%d ReadLineTimeOutRaw timeout!\n",__FILE__,__LINE__);
			return 2;  // return with timeout indication
		}

	}
	FailMessageAlreadyShown=false;  // if we successfully received something show any subsequent error
	return 0;
}

int ReadBytesAvailable(char *RxBuffer, int maxbytes, DWORD *BytesReceived, int timeout_ms)
{
	FT_STATUS ftStatus;
	DWORD EventDWord;
	DWORD RxBytes;
	DWORD TxBytes;

	ftStatus=FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
	//ftStatus=FT_GetQueueStatus(ftHandle,&RxBytes);

	if (ftStatus != FT_OK)
	{
		printf("%s:%d Failure!\n",__FILE__,__LINE__);
		Failed();
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
			printf("%s:%d Failure!\n",__FILE__,__LINE__);
			Failed();
			return 1;
		}
	}

	return 0;
}
void Disconnect(){
	if(m_Connected){
		FT_Purge(ftHandle,FT_PURGE_RX|FT_PURGE_TX);
		FT_ResetDevice(ftHandle);
		m_Connected=false;
		FT_Close(ftHandle);
	}
}

int Failed()
{
	m_Connected=false;

	FT_Close(ftHandle);

	if (!FailMessageAlreadyShown)
		printf("Read Failed - Auto Disconnect\n");

	FailMessageAlreadyShown=true;

	return 0;
}

