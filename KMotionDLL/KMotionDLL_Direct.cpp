// KMotionDLL_Direct.cpp : Defines the entry point for the DLL application.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"

#define KMotionBd 1  // used within coff loader

#include "PARAMS.h"
#include "VERSION.h"
#include "COFF.h"
#include "CLOAD.h"




CKMotionDLL_Direct::CKMotionDLL_Direct()
{ 
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
			KMotionLocal.KMotionIO[i].USB_Loc_ID == BoardID)
				return i;   // found previously assigned matching Location use it     
	}

	for (i=0; i<MAX_BOARDS; i++)
	{
		if (KMotionLocal.KMotionIO[i].BoardIDAssigned)
		{
			if (KMotionLocal.KMotionIO[i].USB_Loc_ID == BoardID)
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

	if (i==MAX_BOARDS)
	{
		AfxMessageBox("Fatal Error: Too Many Board IDs used",MB_ICONSTOP|MB_OK);
		KMotionLocal.KMotionIO[i].Mutex->Unlock(); 
		exit(1);
	}

	KMotionLocal.KMotionIO[i].BoardIDAssigned = true;  
	KMotionLocal.KMotionIO[i].USB_Loc_ID = BoardID;  	// assign the ID

	KMotionLocal.KMotionIO[0].Mutex->Unlock(); 
	return i;
}


int CKMotionDLL_Direct::ListLocations(int *nlocations, int *list)
{
	FT_HANDLE ftHandle;
	FT_DEVICE ftDevice;
	DWORD deviceID;
	char SerialNumber[16]; 
	char Description[64]; 
	FT_STATUS ftStatus;
	int i;

	for (i=0; i<MAX_BOARDS; i++) list[i]=-1;

	*nlocations=-1;

	ftStatus = FT_ListDevices(list,nlocations,FT_LIST_ALL|FT_OPEN_BY_LOCATION);
	
	if ((ftStatus == FT_OK || *nlocations!=-1) &&  *nlocations <= MAX_BOARDS)
	{
		// go through the list and remove any non-dynomotion boards
		for (i=0; i<*nlocations; i++)
		{
			if (list[i] != -1)
			{
				ftStatus = FT_OpenEx((void *)list[i], FT_OPEN_BY_LOCATION, &ftHandle); 

				if(ftStatus != FT_OK)
				{ 
				   // FT_Open failed 
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
						FT_Close(ftHandle);

						if (strstr(Description,"KFLOP")== NULL &&
							strstr(Description,"KMotion")== NULL &&
							strstr(Description,"Dynomotion")== NULL)
						{
							// remove it from the list
							for (int k=i+1; k<*nlocations; k++)
								list[k-1] = list[k];  // shift up
							(*nlocations)--;
							i--; // redo this slot since it was deleted and things shifted up
						}
					}
				}
			}
		}

		// note ListDevices fails if all devices are open,
		// so if it changed the nlocations and it is reasonable
		// assume it is valid

		int k=0;

		// go through and if there were any -1 that indicates
		// that device is in use (connected) search and substitute
		// with any assigned and connected boards

		for (i=0; i<*nlocations; i++)
		{
			if (list[i] == -1)
			{
				for (;k<MAX_BOARDS;k++)
				{
					if (KMotionLocal.KMotionIO[k].BoardIDAssigned &&
						KMotionLocal.KMotionIO[k].m_Connected &&
						KMotionLocal.KMotionIO[k].USB_Loc_ID != -1)
					{
						list[i]=KMotionLocal.KMotionIO[k++].USB_Loc_ID;
						break;
					}
				}
			}
		}

		return 0;
	}
	else 
	{
		// FT_ListDevices failed
		*nlocations=0;
		return 1;
	}
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

int CKMotionDLL_Direct::KMotionLock(int board)
{
	return KMotionLocal.KMotionIO[board].KMotionLock();
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

const char * CKMotionDLL_Direct::GetErrMsg(int board)
{
	return KMotionLocal.KMotionIO[board].ErrMsg;
}

void CKMotionDLL_Direct::ClearErrMsg(int board)
{
	KMotionLocal.KMotionIO[board].ErrMsg="";
}
