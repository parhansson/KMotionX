

// KMotionDLL_Direct.cpp : Defines the entry point for the DLL application.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "StdAfx.h"

#define KMotionBd 1  // used within coff loader

#include "PARAMS.h"
#include "VERSION.h"
#include "COFF.h"
#include "CLOAD.h"
#include "HiResTimer.h"
#include "Ping.h"



CKMotionDLL_Direct::CKMotionDLL_Direct()
{ 
}

void CKMotionDLL_Direct::FindKognas(void)
{
	::FindKognas();
	::FindKFLOPs();
}



// Maps a specified Board Identifiers to a KMotionIO Index (or object)
//
// A board identifier may be any of the following:
//
//    #1  a USB location ID such as 0x00000321
//    #2  0 .. -15 which maps to the first through the 15th 
//        currently available device (which has not been opened
//        by location)
//
// We maintain within each KMotionIO object whether it has been assigned
// and if so, which Board Identifier and which USB location it is assigned to
//
// If a new Board Identifier is encountered the first available KMotionIO
// object is assigned to it

int CKMotionDLL_Direct::MapBoardToIndex(int BoardID)
{
	int i;

	if (BoardID <= 0 && BoardID >= -15)
	{
		// simply use the BoardID as the object
		return -BoardID;
	}

	// if requested BoardID is specified as a USB location
	// first scan all objects to see if one is already connected
	// to that USB location

	for (i=0; i<MAX_BOARDS; i++)
	{
		if (KMotionLocal.KMotionIO[i].BoardIDAssigned &&
			KMotionLocal.KMotionIO[i].m_Connected &&
			KMotionLocal.KMotionIO[i].Actual_ID == BoardID)
				return i;   // found previously assigned matching Location use it     
	}

	for (i=0; i<MAX_BOARDS; i++)
	{
		if (KMotionLocal.KMotionIO[i].BoardIDAssigned)
		{
			if (KMotionLocal.KMotionIO[i].Actual_ID == BoardID)
				return i;   // found previously assigned matching object, return it     
		}
	}

	// BoardID never before encountered

	KMotionLocal.KMotionIO[0].Mutex->Lock();  // better lock while assigning objects

	// find an available object to handle it

	for (i=0; i<MAX_BOARDS; i++)
	{
		if (!KMotionLocal.KMotionIO[i].BoardIDAssigned) break;
	}

	if (i == MAX_BOARDS)
	{
		MessageBoxW(NULL, Translate("Fatal Error: Too Many Board IDs used"), L"KMotion", MB_ICONSTOP | MB_OK);
		KMotionLocal.KMotionIO[i].Mutex->Unlock();
		exit(1);
	}

	KMotionLocal.KMotionIO[i].BoardIDAssigned = true;  
	KMotionLocal.KMotionIO[i].Actual_ID = BoardID;  	// assign the ID

	KMotionLocal.KMotionIO[0].Mutex->Unlock(); 
	return i;
}



int CKMotionDLL_Direct::ListLocations(int *nlocations, int *list)
{
	int i;

	list[0]=-1;
	*nlocations=0;


	KognaListMutex->Lock();  // no time-out interval
	if (1/*dwWaitResult == WAIT_OBJECT_0*/)
	{
		// go through the list and copy Kogna IPs to User's list
		for (i = 0; i < nKognas; i++)
		{
			list[(*nlocations)++] = Kognas[i].KognaIP;				// Add Kogna IP to list
			list[(*nlocations)++] = 0xFF000000 | Kognas[i].KognaSerialNumber;	// Add additional to list by Serial Number
		}

		KognaListMutex->Unlock();
	}


	KFLOPListMutex->Lock();  // no time-out interval
	if (1/*dwWaitResult == WAIT_OBJECT_0*/)
	{
		// go through the list and copy Kogna IPs to User's list
		for (unsigned int i = 0; i < nKFLOPs; i++)
		{
			list[(*nlocations)++] = KFLOPs[i].LocId;				// Add KFLOP IP to list
		}

		KFLOPListMutex->Unlock();
	}


	return 0;
}



int CKMotionDLL_Direct::SetRequested_ID(int board, unsigned int Board_ID)
{
	return KMotionLocal.KMotionIO[board].Requested_ID = Board_ID;
}

int CKMotionDLL_Direct::WriteLineReadLine(int board, const char *s, char *response)
{
	return KMotionLocal.KMotionIO[board].WriteLineReadLine(s, response);
}

int CKMotionDLL_Direct::WriteLine(int board, const char *s)
{
	return KMotionLocal.KMotionIO[board].WriteLine(s);
}

int CKMotionDLL_Direct::WriteLineWithEcho(int board, const char *s)
{
	return KMotionLocal.KMotionIO[board].WriteLineWithEcho(s);
}

int CKMotionDLL_Direct::ReadLineTimeOut(int board, char *buf, int TimeOutms)
{
	return KMotionLocal.KMotionIO[board].ReadLineTimeOut(buf,TimeOutms);
}


int CKMotionDLL_Direct::Failed(int board)
{
	return KMotionLocal.KMotionIO[board].Failed();
}

int CKMotionDLL_Direct::Disconnect(int board)
{
	return KMotionLocal.KMotionIO[board].Disconnect();
}

int CKMotionDLL_Direct::FirmwareVersion(int board)
{
	return KMotionLocal.KMotionIO[board].FirmwareVersion();
}

int CKMotionDLL_Direct::CheckForReady(int board)
{
	return KMotionLocal.KMotionIO[board].CheckForReady();
}

int CKMotionDLL_Direct::KMotionLock(int board, const char *CallerID)
{
	return KMotionLocal.KMotionIO[board].KMotionLock(CallerID);
}

int CKMotionDLL_Direct::USBLocation(int board)
{
	return KMotionLocal.KMotionIO[board].USBLocation();
}

int CKMotionDLL_Direct::KMotionLockRecovery(int board)
{
	return KMotionLocal.KMotionIO[board].KMotionLockRecovery();
}

void CKMotionDLL_Direct::ReleaseToken(int board)
{
	KMotionLocal.KMotionIO[board].ReleaseToken();
}

int  CKMotionDLL_Direct::ServiceConsole(int board)
{
	return KMotionLocal.KMotionIO[board].ServiceConsole();
}

int CKMotionDLL_Direct::SetConsoleCallback(int board, SERVER_CONSOLE_HANDLER *ch)
{
	KMotionLocal.KMotionIO[board].SetConsoleCallback(ch);
	return 0;
}

int CKMotionDLL_Direct::nInstances()
{
	return share;
}

const wchar_t * CKMotionDLL_Direct::GetErrMsg(int board)
{
	return KMotionLocal.KMotionIO[board].ErrMsg.c_str();
}

void CKMotionDLL_Direct::ClearErrMsg(int board)
{
	KMotionLocal.KMotionIO[board].ErrMsg=L"";
}
