// KMViaVB.cpp : Defines the entry point for the DLL application.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



// BEWARE: CDecl is used as the calling convention for Visual Basic called routines,
// and all of these routines are called via VB


#include "stdafx.h"
#include "KMViaVB.h"

#include "..\KMotionDll\KMotionDll.h"  // KMotion DLL Header

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

long __stdcall KMViaVB_New(long *handle, long board)
{
	CKMotionDLL *KM; // one instance of the interface class

	KM = new CKMotionDLL(board);
	*handle=(long)KM;
	return 1L;
}


long __stdcall KMViaVB_Free(long *handle)
{
	CKMotionDLL *KM; // one instance of the interface class
	KM=*(CKMotionDLL **)handle;
	delete KM;
	*handle=0L;
	return 1L;
}


long __stdcall KMViaVB_WriteLineReadLine(long *handle, char *s, char **rtrn)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->WriteLineReadLine(s,*rtrn);
	return rslt;
}

long __stdcall KMViaVB_WriteLine(void *handle, char *s)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->WriteLine(s);
	return rslt;
}


long __stdcall KMViaVB_WriteLineWithEcho(void *handle, char *s)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->WriteLineWithEcho(s);
	return rslt;
}

long __stdcall KMViaVB_ReadLineTimeOut(void *handle,char **buf, long TimeOutms)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->ReadLineTimeOut(*buf,TimeOutms);
	return rslt;
}

long __stdcall KMViaVB_WaitToken(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->WaitToken();
	return rslt;
}

long __stdcall KMViaVB_KMotionLock(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->KMotionLock();
	return rslt;
}

long __stdcall KMViaVB_USBLocation(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->USBLocation();
	return rslt;
}

void __stdcall KMViaVB_ReleaseToken(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	KM_dll->ReleaseToken();
	return;
}

long __stdcall KMViaVB_Failed(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->Failed();
	return rslt;
}

long __stdcall KMViaVB_Disconnect(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->Disconnect();
	return rslt;
}

long __stdcall KMViaVB_CheckForReady(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->CheckForReady();
	return rslt;
}

long __stdcall KMViaVB_LoadCoff(void *handle,long Thread, char *Name)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->LoadCoff(Thread,Name,false);
	return rslt;
}


long __stdcall KMViaVB_CompileAndLoadCoff(long *handle,  long Thread, char *Name, char **Err, long MaxErrLen)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->CompileAndLoadCoff(Name,Thread,*Err,MaxErrLen);
	return rslt;
}

long __stdcall KMViaVB_ServiceConsole(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->ServiceConsole();
	return rslt;
}


long __stdcall KMViaVB_SetConsoleCallback(void *handle, VB_CONSOLE_HANDLER *ch)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	VB_ConsoleHandler=ch;
	long rslt=KM_dll->SetConsoleCallback(LocalConsoleHandler);
	return rslt;
}

long __stdcall KMViaVB_ListLocations(void *handle, long *nlocations, long *list)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	long rslt=KM_dll->ListLocations((int *)nlocations,(int *)list);
	return rslt;
}

