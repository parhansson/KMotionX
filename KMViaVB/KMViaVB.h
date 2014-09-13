// KMViaVB.h
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

typedef long __stdcall VB_CONSOLE_HANDLER(const char *buf);

long __stdcall KMViaVB_New(long *handle, long board);
long __stdcall KMViaVB_Free(long *handle);
long __stdcall KMViaVB_WriteLineReadLine(long *handle, char *s, char **rtrn);
long __stdcall KMViaVB_WriteLine(void *handle, char *s);
long __stdcall KMViaVB_WriteLineWithEcho(void *handle, char *s);
long __stdcall KMViaVB_ReadLineTimeOut(void *handle,char **buf, long TimeOutms);
long __stdcall KMViaVB_WaitToken(void *handle);
long __stdcall KMViaVB_KMotionLock(void *handle);
long __stdcall KMViaVB_USBLocation(void *handle);
void __stdcall KMViaVB_ReleaseToken(void *handle);
long __stdcall KMViaVB_Failed(void *handle);
long __stdcall KMViaVB_Disconnect(void *handle);
long __stdcall KMViaVB_CheckForReady(void *handle);
long __stdcall KMViaVB_LoadCoff(long *handle,long Thread, char *Name);
long __stdcall KMViaVB_CompileAndLoadCoff(long *handle, long Thread, char *Name, long MaxErrLen, char **Err);
long __stdcall KMViaVB_ServiceConsole(void *handle);
long __stdcall KMViaVB_SetConsoleCallback(void *handle, VB_CONSOLE_HANDLER *ch);

