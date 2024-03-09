// KMViaVB.cpp : Defines the entry point for the DLL application.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



// BEWARE: CDecl is used as the calling convention for Visual Basic called routines,
// and all of these routines are called via VB


#include "stdafx.h"
#include "KMViaVB.h"

#include "..\KMotionDll\KMotionDll.h"  // KMotion DLL Header

#define HANDLE64 _int64

// translate call to __stdcall otherwise stack will be wrong for VB
VB_CONSOLE_HANDLER *VB_ConsoleHandler=NULL;   

int LocalConsoleHandler(const char *buf)
{
	return VB_ConsoleHandler(buf);
}


long __stdcall KMViaVB_Test1()
{
	return 0xDEAD;
}
long __stdcall KMViaVB_Test2(long jj)
{
	return 0xDEAD + jj;
}
long __stdcall KMViaVB_Test3(long *jj)
{
	return 0xDEAD + (*jj)++;
}

long __stdcall KMViaVB_New(HANDLE64 *handle, long board)
{
	CKMotionDLL *KM; // one instance of the interface class

	KM = new CKMotionDLL(board);
	*handle=(HANDLE64)KM;
	return 1L;
}


long __stdcall KMViaVB_Free(HANDLE64 handle)
{
	CKMotionDLL *KM; // one instance of the interface class
	KM=*(CKMotionDLL **)handle;
	delete KM;
	*(HANDLE64 *)handle=0L;
	return 1L;
}


long __stdcall KMViaVB_WriteLineReadLine(HANDLE64 handle, char *s, char **rtrn)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->WriteLineReadLine(s,*rtrn);
	return rslt;
}

long __stdcall KMViaVB_WriteLine(HANDLE64 handle, char *s)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->WriteLine(s);
	return rslt;
}


long __stdcall KMViaVB_WriteLineWithEcho(HANDLE64 handle, char *s)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->WriteLineWithEcho(s);
	return rslt;
}

long __stdcall KMViaVB_ReadLineTimeOut(HANDLE64 handle,char **buf, long TimeOutms)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->ReadLineTimeOut(*buf,TimeOutms);
	return rslt;
}

long __stdcall KMViaVB_WaitToken(HANDLE64 handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->WaitToken();
	return rslt;
}

long __stdcall KMViaVB_KMotionLock(HANDLE64 handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->KMotionLock();
	return rslt;
}

long __stdcall KMViaVB_USBLocation(HANDLE64 handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->USBLocation();
	return rslt;
}

void __stdcall KMViaVB_ReleaseToken(HANDLE64 handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	KM_dll->ReleaseToken();
	return;
}

long __stdcall KMViaVB_Failed(HANDLE64 handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->Failed();
	return rslt;
}

long __stdcall KMViaVB_Disconnect(HANDLE64 handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->Disconnect();
	return rslt;
}

long __stdcall KMViaVB_CheckForReady(HANDLE64 handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->CheckForReady();
	return rslt;
}

long __stdcall KMViaVB_LoadCoff(HANDLE64 handle,long Thread, char *Name, long PackToFlash)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->LoadCoff(Thread,Name,PackToFlash);
	return rslt;
}


long __stdcall KMViaVB_CompileAndLoadCoff(HANDLE64 handle,  long Thread, char *Name, wchar_t **Err, long MaxErrLen)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->CompileAndLoadCoff(Name,Thread,*Err,MaxErrLen);
	return rslt;
}

long __stdcall KMViaVB_ServiceConsole(HANDLE64 handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->ServiceConsole();
	return rslt;
}


long __stdcall KMViaVB_SetConsoleCallback(HANDLE64 handle, VB_CONSOLE_HANDLER *ch)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	VB_ConsoleHandler=ch;
	long rslt=KM_dll->SetConsoleCallback(LocalConsoleHandler);
	return rslt;
}

long __stdcall KMViaVB_ListLocations(HANDLE64 handle, long *nlocations, long *list)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->ListLocations((int *)nlocations,(int *)list);
	return rslt;
}

