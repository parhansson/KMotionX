// KMViaVB.h
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

typedef long __stdcall VB_CONSOLE_HANDLER(const char *buf);

long __stdcall KMViaVB_New(HANDLE64 handle, long board);
long __stdcall KMViaVB_Free(HANDLE64 handle);
long __stdcall KMViaVB_WriteLineReadLine(HANDLE64 handle, char *s, char **rtrn);
long __stdcall KMViaVB_WriteLine(HANDLE64 handle, char *s);
long __stdcall KMViaVB_WriteLineWithEcho(HANDLE64 handle, char *s);
long __stdcall KMViaVB_ReadLineTimeOut(HANDLE64 handle,char **buf, long TimeOutms);
long __stdcall KMViaVB_WaitToken(HANDLE64 handle);
long __stdcall KMViaVB_KMotionLock(HANDLE64 handle);
long __stdcall KMViaVB_USBLocation(HANDLE64 handle);
void __stdcall KMViaVB_ReleaseToken(HANDLE64 handle);
long __stdcall KMViaVB_Failed(HANDLE64 handle);
long __stdcall KMViaVB_Disconnect(HANDLE64 handle);
long __stdcall KMViaVB_CheckForReady(HANDLE64 handle);
long __stdcall KMViaVB_LoadCoff(HANDLE64 handle,long Thread, char *Name, long PackToFlash);
long __stdcall KMViaVB_CompileAndLoadCoff(HANDLE64 handle, long Thread, char *Name, long MaxErrLen, char **Err);
long __stdcall KMViaVB_ServiceConsole(HANDLE64 handle);
long __stdcall KMViaVB_SetConsoleCallback(HANDLE64 handle, VB_CONSOLE_HANDLER *ch);

