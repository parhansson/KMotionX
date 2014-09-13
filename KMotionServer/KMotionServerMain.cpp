// KMotionServerMain.cpp 
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h" 
#include "MainFrm.h"
#include <afxtempl.h>


#include "..\KMotionDLL\HiResTimer.h" 
#include "..\KMotionDLL\KMotionDLL.h" 
#include "..\KMotionDLL\FTD2xx.h"
#include "..\KMotionDLL\KMotionIO.h" 
#include "..\KMotionDLL\KMotionDLL_Direct.h" 
 

extern CMainFrame *TheFrame;


#define BUFSIZE 4096
#define PIPE_TIMEOUT 10000

CKMotionDLL_Direct KMotionDLL;

void InstanceThread(LPVOID); 
void GetAnswerToRequest(char *chRequest, DWORD nInBytes, char *chReply, DWORD *cbReplyBytes, HANDLE hPipe);
int ConsoleHandler(int board, const char *buf);
 
int nClients = 0; 

HANDLE ConsolePipeHandle[MAX_BOARDS];

CList<CString,CString> ConsoleList[MAX_BOARDS];


void MyErrExit(char *s)
{
	MessageBox(NULL,s,"KMotion Server",MB_ICONSTOP|MB_OK|MB_SYSTEMMODAL);
	exit(1);
}


 
void ServerMain(LPVOID lpvParam) 
{ 
   BOOL fConnected; 
   DWORD dwThreadId; 
   HANDLE hPipe, hThread; 
   LPTSTR lpszPipename = "\\\\.\\pipe\\kmotionpipe"; 

   for (int i=0; i<MAX_BOARDS; i++) ConsolePipeHandle[i]=NULL;
 
// The main loop creates an instance of the named pipe and 
// then waits for a client to connect to it. When the client 
// connects, a thread is created to handle communications 
// with that client, and the loop is repeated. 
 
   for (;;) 
   { 
      hPipe = CreateNamedPipe( 
          lpszPipename,             // pipe name 
          PIPE_ACCESS_DUPLEX,       // read/write access 
          PIPE_TYPE_MESSAGE |       // message type pipe 
          PIPE_READMODE_MESSAGE |   // message-read mode 
          PIPE_WAIT,                // blocking mode 
          PIPE_UNLIMITED_INSTANCES, // max. instances  
          BUFSIZE,                  // output buffer size 
          BUFSIZE,                  // input buffer size 
          PIPE_TIMEOUT,             // client time-out 
          NULL);                    // no security attribute 

      if (hPipe == INVALID_HANDLE_VALUE) 
          MyErrExit("CreatePipe"); 
 
      // Wait for the client to connect; if it succeeds, 
      // the function returns a nonzero value. If the function returns 
      // zero, GetLastError returns ERROR_PIPE_CONNECTED. 
 
      fConnected = ConnectNamedPipe(hPipe, NULL) ? 
         TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
 
      if (fConnected) 
      { 

		  nClients++;

      // Create a thread for this client. 
         hThread = CreateThread( 
            NULL,              // no security attribute 
            0,                 // default stack size 
            (LPTHREAD_START_ROUTINE) InstanceThread, 
            (LPVOID) hPipe,    // thread parameter 
            0,                 // not suspended 
            &dwThreadId);      // returns thread ID 

         if (hThread == NULL) 
            MyErrExit("CreateThread"); 
 
      } 
      else 
        // The client could not connect, so close the pipe. 
         CloseHandle(hPipe); 
   } 
   return; 
} 
 
VOID InstanceThread(LPVOID lpvParam) 
{ 
   CHAR chRequest[BUFSIZE]; 
   CHAR chReply[BUFSIZE]; 
   DWORD cbBytesRead, cbReplyBytes, cbWritten; 
   BOOL fSuccess; 
   HANDLE hPipe; 
 
// The thread's parameter is a handle to a pipe instance. 
 
	hPipe = (HANDLE) lpvParam; 

	while (1) 
	{ 
		// Read client requests from the pipe. 
		fSuccess = ReadFile( 
			 hPipe,        // handle to pipe 
			 chRequest,    // buffer to receive data 
			 BUFSIZE,      // size of buffer 
			 &cbBytesRead, // number of bytes read 
			 NULL);        // not overlapped I/O 

      if (! fSuccess || cbBytesRead == 0) 
         break; 
      
	  GetAnswerToRequest(chRequest, cbBytesRead, chReply, &cbReplyBytes, hPipe); 
	   
   // Write the reply to the pipe. 
      fSuccess = WriteFile( 
         hPipe,        // handle to pipe 
         chReply,      // buffer to write from 
         cbReplyBytes, // number of bytes to write 
         &cbWritten,   // number of bytes written 
         NULL);        // not overlapped I/O 

      if (! fSuccess || cbReplyBytes != cbWritten) break; 
  } 
 
// Flush the pipe to allow the client to read the pipe's contents 
// before disconnecting. Then disconnect the pipe, and close the 
// handle to this pipe instance. 
 
   FlushFileBuffers(hPipe); 
   DisconnectNamedPipe(hPipe); 
   CloseHandle(hPipe); 


   if (--nClients <= 0) exit(0);              // nobody left - terminate server
   if (KMotionDLL.nInstances() < 2) exit(0);  // nobody left - terminate server
} 
 


void GetAnswerToRequest(char *chRequest, DWORD nInBytes, char *chReply, DWORD *cbReplyBytes, HANDLE hPipe)
{
	int code, BoardID, board, TimeOutms, result=0, nLocations, List[256];

	memcpy(&code, chRequest,4);

	if (code!=ENUM_ListLocations)  // all commands require a board to be mapped, except this command
	{
		memcpy(&BoardID, chRequest+4,4);
		board=KMotionDLL.MapBoardToIndex(BoardID);
	}

	chReply[0]=DEST_NORMAL;

	switch (code)
	{

	case ENUM_WriteLineReadLine:	// Send Code, board, string -- Get Dest byte, Result (int) and string
		result = KMotionDLL.WriteLineReadLine(board, chRequest+8, chReply+1+4);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes = 1+4+strlen(chReply+1+4)+1; // Dest byte, Result int, string, null char
		break;

	case ENUM_WriteLine:	
		result = KMotionDLL.WriteLine(board, chRequest+8);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_WriteLineWithEcho:	
		result = KMotionDLL.WriteLineWithEcho(board, chRequest+8);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_ReadLineTimeOut:	// Send Code, board, timeout -- Dest byte, Get Result (int), and string
		memcpy(&TimeOutms, chRequest+8,4);
		result = KMotionDLL.ReadLineTimeOut(board, chReply+1+4 ,TimeOutms);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes = 1+4+strlen(chReply+1+4)+1; // Dest byte, Result int, string, null char
		break;

	case ENUM_ListLocations:		// Send Code -- Get Dest, Result (int), nlocations (int), List (ints)
		result = KMotionDLL.ListLocations(&nLocations, List);
		memcpy(chReply+1, &result,4);
		memcpy(chReply+1+4, &nLocations,4);
		memcpy(chReply+1+8, List, nLocations*4);
		*cbReplyBytes = 1+4+4+4*nLocations; // Dest byte, Result int, string, null char
		break;

	case ENUM_Failed:	
		result = KMotionDLL.Failed(board);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_Disconnect:	
		result = KMotionDLL.Disconnect(board);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_FirmwareVersion:	
		result = KMotionDLL.FirmwareVersion(board);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_CheckForReady:	
		result = KMotionDLL.CheckForReady(board);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_KMotionLock:	
		result = KMotionDLL.KMotionLock(board);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_USBLocation:	
		result = KMotionDLL.USBLocation(board);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_KMotionLockRecovery:	
		result = KMotionDLL.KMotionLockRecovery(board);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_ReleaseToken:	
		KMotionDLL.ReleaseToken(board);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_ServiceConsole:	
		result = KMotionDLL.ServiceConsole(board);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	case ENUM_SetConsole:
		// remember which pipe is associated with the console handler for the board
		ConsolePipeHandle[board] = hPipe;  
		result = KMotionDLL.SetConsoleCallback(board,ConsoleHandler);
		memcpy(chReply+1, &result,4);
		*cbReplyBytes=1+4;
		break;

	default: MyErrExit("Bad Request Code");
	}

	// before we send the answer back check if this pipe
	// is the message handler for the board and there are
	// messages in the queue 

	if (code!=ENUM_ListLocations && hPipe == ConsolePipeHandle[board])
	{
		int nSent=0;

		while (!ConsoleList[board].IsEmpty() && nSent++<10)
		{
			DWORD cbReplyBytes, cbBytesRead, cbWritten; 
			unsigned char Reply;
			BOOL fSuccess; 
			CString s = ConsoleList[board].RemoveHead();

			s.Insert(0,DEST_CONSOLE);

			cbReplyBytes = s.GetLength()+1+1;  // + Term Null + DEST code

			// Write the message to the pipe. 
			fSuccess = WriteFile( 
				 hPipe,        // handle to pipe 
				 s.GetBuffer(0),      // buffer to write from 
				 cbReplyBytes, // number of bytes to write 
				 &cbWritten,   // number of bytes written 
				 NULL);        // not overlapped I/O 

			if (! fSuccess || cbReplyBytes != cbWritten) break; 


			

		   // Read back 1 byte ack 0xAA from Console Handler
			
			fSuccess = ReadFile( 
				 hPipe,        // handle to pipe 
				 &Reply,    // buffer to receive data 
				 1,      // size of buffer 
				 &cbBytesRead, // number of bytes read 
				 NULL);        // not overlapped I/O 

			if (! fSuccess || cbBytesRead != 1 || Reply != 0xAA) 
				break; 
      
		}
	}

	// check if we have an error message to send back

	if (code!=ENUM_ListLocations && !KMotionDLL.GetErrMsg(board)[0]==0)
	{
		DWORD cbReplyBytes, cbBytesRead, cbWritten; 
		unsigned char Reply;
		BOOL fSuccess; 
		CString s = KMotionDLL.GetErrMsg(board);

		s.Insert(0,DEST_ERRMSG);

		cbReplyBytes = s.GetLength()+1+1;  // + Term Null + DEST code

		// Write the message to the pipe. 
		fSuccess = WriteFile( 
			 hPipe,        // handle to pipe 
			 s.GetBuffer(0),      // buffer to write from 
			 cbReplyBytes, // number of bytes to write 
			 &cbWritten,   // number of bytes written 
			 NULL);        // not overlapped I/O 

		if (fSuccess && cbReplyBytes == cbWritten)
		{
		   // Read back 1 byte ack 0xAA from Console Handler
			
			fSuccess = ReadFile( 
				 hPipe,        // handle to pipe 
				 &Reply,    // buffer to receive data 
				 1,      // size of buffer 
				 &cbBytesRead, // number of bytes read 
				 NULL);        // not overlapped I/O 
		}

		KMotionDLL.ClearErrMsg(board);
	}
}


int ConsoleHandler(int board, const char *buf)
{
	// check if there is a console handler for
	// this board

	if (ConsolePipeHandle[board])
	{
		// there is, add the message to the list

		ConsoleList[board].AddTail(buf);
	}
	return 0;
}