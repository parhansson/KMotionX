/*
Copyright © 2011 Brad Murry
All rights reserved.

BSD License:

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


KMotion_dotNet_Interop.cpp
Implementation for Export DLL
*/


#include "stdafx.h" 
#include "KMotion_dotNet_Interop.h" 
#include "KMotionDll.h"  // KMotion DLL Header
#include "CoordMotion.h"
#include "GCodeInterpreter.h"
#include "..\GCodeInterpreter\GCodeInterpreter.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
 
 
#define GET_VALUE(source_value, start, length) ((source_value >> start) & ((1 << length) - 1))
#define SET_VALUE(source_value, index, new_val) (new_val > 0 ? (source_value | (1 << (index - 1))) : (source_value & (~(1 << (index - 1)))))
#define LENGTH(T) (sizeof(T)/sizeof(*(T)))

BEGIN_MESSAGE_MAP(CKMotion_dotNet_InteropApp, CWinApp)
END_MESSAGE_MAP()


// CKMotion_dotNet_InteropApp construction

//Controller Callback
CKMotion_dotNet_InteropApp::CKMotion_dotNet_InteropApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


netCONSOLE_HANDLER* ConsoleHandler = NULL;
netCONSOLE_HANDLER_WIDE* ConsoleHandlerW = NULL;

//KMotion Callbacks
int LocalConsoleHandler(const char* buf)
{
	return ConsoleHandler(buf);
}
int LocalConsoleHandlerW(const wchar_t* buf)
{
	return ConsoleHandlerW(buf);
}

netERRMSG_HANDLER *ErrorHandler=NULL;    

//KMotion Callbacks
void LocalErrorHandler(const char *buf)
{
	return ErrorHandler(buf);
}


netSTRAIGHT_TRAVERSE_CALLBACK *StraightTransverseHandler=NULL;
void LocalStraightTransverseHandler(double x, double y, double z, double a, double b, double c, int sequence_number)
{
 StraightTransverseHandler(x, y, z, a, b, c, sequence_number);
}


netSTRAIGHT_FEED_CALLBACK *StraightFeedHandler=NULL;
void LocalStraightFeedHandler(double DesiredFeedRate_in_per_sec,
							   double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
{
 StraightFeedHandler(DesiredFeedRate_in_per_sec, x, y, z, a, b, c, sequence_number,ID);
}


netARC_FEED_CALLBACK *ArcFeedHandler=NULL;
void LocalArcFeedHandler(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, 
			    CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c, 
				double first_start, double second_start, double axis_start_point, int sequence_number, int ID)
{
 ArcFeedHandler(ZeroLenAsFullCircles, DesiredFeedRate_in_per_sec, plane,
first_end, second_end,first_axis,second_axis,rotation,axis_end_point,a, b, c, first_start,second_start,axis_start_point,sequence_number,ID);
}

 
//Interpreter Callbacks
netG_COMPLETE_CALLBACK *G_CompleteHandler = NULL;
void  Local_G_COMPLETE_CALLBACK(int status, int lineno, int sequence_number, const char *err)
{
  G_CompleteHandler(status, lineno, sequence_number, err);
}

netG_STATUS_CALLBACK *G_StatusHandler = NULL;
void Local_G_STATUS_CALLBACK(int line_no, const char *msg)
{
  G_StatusHandler(line_no, msg);
}

netG_USER_CALLBACK *G_UserCallbackHandler = NULL;
int Local_G_USER_CALLBACK(const char *msg)
{
  return G_UserCallbackHandler(msg);
}

netG_USER_MCODE_CALLBACK *G_UserMCodeCallbackHandler = NULL;
int Local_G_USER_MCODE_CALLBACK(int mCode)
{
  return G_UserMCodeCallbackHandler(mCode);
}

netSTRAIGHT_TRAVERSE_CALLBACK *InterpreterStraightTransverseHandler=NULL;
void LocalInterpreterStraightTransverseHandler(double x, double y, double z, double a, double b, double c, int sequence_number)
{
 InterpreterStraightTransverseHandler(x, y, z, a, b, c, sequence_number);
}

netSTRAIGHT_FEED_CALLBACK *InterpreterStraightFeedHandler=NULL;
void LocalInterpreterStraightFeedHandler(double DesiredFeedRate_in_per_sec,
							   double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
{
 InterpreterStraightFeedHandler(DesiredFeedRate_in_per_sec, x, y, z, a, b, c, sequence_number,ID);
}


netARC_FEED_CALLBACK *InterpreterArcFeedHandler=NULL;
void LocalInterpreterArcFeedHandler(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, 
			    CANON_PLANE plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c, 
				double first_start, double second_start, double axis_start_point, int sequence_number, int ID)
{
 InterpreterArcFeedHandler(ZeroLenAsFullCircles, DesiredFeedRate_in_per_sec, plane,
first_end, second_end,first_axis,second_axis,rotation,axis_end_point, a, b, c, first_start,second_start,axis_start_point,sequence_number,ID);
}


//Standard Kmotion Members

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_New(HANDLE64 *handle, int board)
{
	CKMotionDLL *KM; // one instance of the interface class

	KM = new CKMotionDLL(board);
	KM->BoardID = board;
	*handle=(HANDLE64)KM; 
	return 1L;
}
 
extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_Free(HANDLE64 *handle)
{
	CKMotionDLL *KM; // one instance of the interface class
	KM=*(CKMotionDLL **)handle;
	delete KM;
	*handle=0L;
	return 1L;
}


extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_WriteLineReadLine(void *handle, char *s, char **rtrn)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->WriteLineReadLine(s,*rtrn);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_WriteLine(void *handle, char *s)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->WriteLine(s);
	return rslt;
}


extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_WriteLineWithEcho(void *handle, char *s)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->WriteLineWithEcho(s);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_ReadLineTimeOut(void *handle, char **buf, int TimeOutms)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->ReadLineTimeOut(*buf,TimeOutms);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_WaitToken(void *handle, int timeout)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->WaitToken(false, timeout, "dotNETWaitToken");
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_KMotionLock(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->KMotionLock();
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_KMotionLockRecovery(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->KMotionLockRecovery();
	return rslt;
}
extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_USBLocation(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->USBLocation();
	return rslt;
}

extern "C" __declspec(dllexport) void __stdcall KM_dotnet_Interop_ReleaseToken(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	KM_dll->ReleaseToken();
	return;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_Failed(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->Failed();
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_Disconnect(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->Disconnect();
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_CheckForReady(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->CheckForReady();
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_LoadCoff(void *handle, int Thread, const char *Name)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->LoadCoff(Thread,Name, 0);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_LoadCoffPack(void *handle, int Thread, const char *Name, int PackToFlash)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->LoadCoff(Thread,Name, PackToFlash);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_SimpleCompileAndLoadCoff(void* handle, int Thread, const char* Name)
{
	CKMotionDLL* KM_dll = (CKMotionDLL*)handle;
	int rslt = KM_dll->CompileAndLoadCoff(Name, Thread);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_CompileAndLoadCoff(void *handle, int Thread, const char *Name, char **Err, int MaxErrLen)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->CompileAndLoadCoff(Name,Thread,*Err,MaxErrLen);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_Compile(void *handle,const int boardtype, int Thread, const char *Name, char **Err, int MaxErrLen)
{
	CString OutFile;
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	KM_dll->ConvertToOut(Thread, Name, OutFile.GetBuffer(MAX_PATH), MAX_PATH);
	OutFile.ReleaseBuffer();
	int rslt=KM_dll->Compile(Name, OutFile, boardtype, Thread, *Err, MaxErrLen);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_ServiceConsole(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->ServiceConsole();
	return rslt;
}


extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_SetConsoleCallback(void *handle, netCONSOLE_HANDLER *ch)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	ConsoleHandler=ch;
	int rslt=KM_dll->SetConsoleCallback(LocalConsoleHandler);
	return rslt;
}
 
extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_SetErrorCallback(void *handle, netERRMSG_HANDLER *ch)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	ErrorHandler=ch;
	int rslt=KM_dll->SetErrMsgCallback(LocalErrorHandler);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_ListLocations(void *handle, int *nlocations, int *list)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->ListLocations((int *)nlocations,(int *)list);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_GetFirmwareVersion(void *handle)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->FirmwareVersion();
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_CheckCoffSize(void *handle, const char *Name, int *size_text, int *size_bss, int *size_data, int *size_total)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->CheckCoffSize(Name, size_text, size_bss, size_data, size_total);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_GetLoadAddress(void *handle, int thread, int BoardType)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->GetLoadAddress(thread, BoardType);
	return rslt;
}

extern "C" __declspec(dllexport) void __stdcall KM_dotnet_Interop_ConvertToOut(void *handle, int thread, const char *InFile, char *OutFile, int MaxLength)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	KM_dll->ConvertToOut(thread, InFile, OutFile, MaxLength);
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_CheckKMotionVersion(void *handle, int *type, bool GetBoardTypeOnly)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->CheckKMotionVersion(type, GetBoardTypeOnly);
	return rslt;
}

extern "C" __declspec(dllexport) void __stdcall KM_dotnet_Interop_ExtractCoffVersionString(void *handle, const char *InFile, char *Version)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	KM_dll->ExtractCoffVersionString(InFile, Version);
}

extern "C" __declspec(dllexport) 	bool  __stdcall KM_dotnet_Interop_MainStatus_GetStatus(void* handle, bool lock, MAIN_STATUS * m)
{
	CKMotionDLL* KM_dll = (CKMotionDLL*)handle;
	KM_dll->GetStatus(*m, lock);
	return true;
}
  
//Coordinated Motion Members
//SET MOTION_PARAMS

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_BreakAngle(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->BreakAngle;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_BreakAngle(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->BreakAngle = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CollinearTol(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CollinearTol;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CollinearTol(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams(); 
	p->CollinearTol = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CornerTol(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CornerTol;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CornerTol(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CornerTol = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_FacetAngle(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->FacetAngle;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_FacetAngle(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->FacetAngle = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_TPLookahead(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->TPLookahead;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_TPLookahead(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->TPLookahead = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusA(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->RadiusA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusA(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->RadiusA = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusB(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->RadiusB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusB(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->RadiusB = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusC(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->RadiusC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusC(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->RadiusC = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelX(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelX;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelX(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelX = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelY(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelY;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelY(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelY = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelZ(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelZ(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelA(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelA(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelA = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelB(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelB(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelB = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelC(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelC(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelC = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelX(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelX;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelX(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelX = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelY(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelY;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelY(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelY = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelZ(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelZ(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelA(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelA(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelA = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelB(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelB(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelB = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelC(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelC(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelC = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchX(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchX;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchX(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchX = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchY(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchY;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchY(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchY = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchZ(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchZ(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchA(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchA(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchA = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchB(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchB(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchB = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchC(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchC(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchC = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxLinearLength(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxLinearLength;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxLinearLength(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxLinearLength = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxRapidFRO(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxRapidFRO;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxRapidFRO(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxRapidFRO = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_UseOnlyLinearSegments(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->UseOnlyLinearSegments;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_UseOnlyLinearSegments(HANDLE64 *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->UseOnlyLinearSegments = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DoRapidsAsFeeds(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->DoRapidsAsFeeds;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DoRapidsAsFeeds(HANDLE64 *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->DoRapidsAsFeeds = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_ArcsToSegs(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->ArcsToSegs;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_ArcsToSegs(HANDLE64 *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->ArcsToSegs = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesA(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->DegreesA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesA(HANDLE64 *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->DegreesA = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesB(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->DegreesB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesB(HANDLE64 *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->DegreesB = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesC(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->DegreesC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesC(HANDLE64 *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->DegreesC = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_TCP_Active(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->TCP_Active;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_TCP_Active(HANDLE64 *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->TCP_Active = value;
}

 
//Coordinated Motion Rest

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_CoordMotion_New(int *KmotionDLLhandle, HANDLE64 *handle)
{
	
	CKMotionDLL *KM_dll=(CKMotionDLL *)KmotionDLLhandle;
	CCoordMotion *CM;  
	CM = new CCoordMotion(KM_dll); 
	*handle=(HANDLE64)CM;  
}

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_CoordMotion_Free(HANDLE64 *handle)
{
	CCoordMotion *CM; // one instance of the interface class
	CM= *(CCoordMotion **)handle;
	delete CM;
	*handle=0L; 
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_DownloadInit(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	CM_dll->DownloadInit();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_CheckMotionHalt(HANDLE64 *handle, bool Coord)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->CheckMotionHalt(Coord); 
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ExecutionStop(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->ExecutionStop(); 
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_Get_FeedRateOverride(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetFeedRateOverride(); 
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_Get_FeedRateRapidOverride(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetFeedRateRapidOverride(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_FeedRateOverride(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->SetFeedRateOverride(value); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_FeedRateRapidOverride(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->SetFeedRateRapidOverride(value); 
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_Get_SpindleRateOverride(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetSpindleRateOverride(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_SpindleRateOverride(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->SetSpindleRateOverride(value); 
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_Get_HardwareFRORange(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetHardwareFRORange(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_HardwareFRORange(HANDLE64 *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->SetHardwareFRORange(value); 
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_AxisDestination(HANDLE64 *handle, int axis, double *value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetDestination(axis, value); 
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_AxisPosition(HANDLE64 *handle, int axis, double *value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	return CM_dll->GetPosition(axis, value); 
}
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Abort(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetAbort(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Abort(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	 
    CM_dll->SetAbort(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Clear_Abort(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->ClearAbort(); 
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Halt(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetHalt(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Halt(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->SetHalt(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Clear_Halt(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->ClearHalt();
}


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_FlushSegments(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->FlushSegments();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_WaitForSegmentsFinished(HANDLE64 *handle, BOOL NoErrorOnDisable)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->WaitForSegmentsFinished(NoErrorOnDisable);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_WaitForMoveXYZABCFinished(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->WaitForMoveXYZABCFinished();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoKMotionCmd(HANDLE64 *handle, const char *s, BOOL FlushBeforeUnbufferedOperation)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DoKMotionCmd(s, FlushBeforeUnbufferedOperation);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoKMotionBufCmd(HANDLE64 *handle, const char *s)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DoKMotionBufCmd(s);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_KinematicsReadGeoTable(HANDLE64 *handle, const char *filename)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	return CM_dll->Kinematics->ReadGeoTable(filename);
}



 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_MeasurePointAppendToFile(HANDLE64 *handle, const char *name)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->MeasurePointAppendToFile(name);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightTraverse
(HANDLE64 *handle, double x, double y, double z, double a, double b, double c, bool NoCallback)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->StraightTraverse(x, y, z, a, b, c, NoCallback);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ArcFeed
	(HANDLE64 *handle, double DesiredFeedRate_in_per_sec, int plane,
	double first_end, double second_end, 
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->ArcFeed(DesiredFeedRate_in_per_sec, (CANON_PLANE)plane, first_end, second_end, first_axis, second_axis, rotation, axis_end_point, a, b, c, sequence_number, ID);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ArcFeedAccel
	(HANDLE64 *handle, double DesiredFeedRate_in_per_sec, double DesiredAccel, int plane,
	double first_end, double second_end, 
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->ArcFeedAccel(DesiredFeedRate_in_per_sec, DesiredAccel, (CANON_PLANE)plane, first_end, second_end, first_axis, second_axis, rotation, axis_end_point, a, b, c, sequence_number, ID);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightFeed
(HANDLE64 *handle, double DesiredFeedRate_in_per_sec, double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->StraightFeed(DesiredFeedRate_in_per_sec, x, y, z, a, b, c, sequence_number, ID);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightFeedAccel
(HANDLE64 *handle, double DesiredFeedRate_in_per_sec, double DesiredAccel, double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->StraightFeedAccel(DesiredFeedRate_in_per_sec, DesiredAccel, x, y, z, a, b, c, sequence_number, ID);
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ReadAndSyncCurPositions
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	
    int ret = CM_dll->ReadCurAbsPosition(&CM_dll->current_x,&CM_dll->current_y,&CM_dll->current_z,
		&CM_dll->current_a,&CM_dll->current_b,&CM_dll->current_c);
 
    *x = CM_dll->current_x;
    *y = CM_dll->current_y;
    *z = CM_dll->current_z;
    *a = CM_dll->current_a;
    *b = CM_dll->current_b;
    *c = CM_dll->current_c;

    return ret;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ReadCurAbsPosition
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c, bool snap)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->ReadCurAbsPosition(x, y, z, a, b, c, snap);
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetStraightTraverseCallback(HANDLE64 *handle, netSTRAIGHT_TRAVERSE_CALLBACK *p)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    StraightTransverseHandler = p;
    CM_dll->SetStraightTraverseCallback(LocalStraightTransverseHandler); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetStraightFeedCallback(HANDLE64 *handle, netSTRAIGHT_FEED_CALLBACK *p)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    StraightFeedHandler = p;
    CM_dll->SetStraightFeedCallback(LocalStraightFeedHandler);  
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetArcFeedCallback(HANDLE64 *handle, netARC_FEED_CALLBACK *p)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    ArcFeedHandler = p;
    CM_dll->SetArcFeedCallback(LocalArcFeedHandler);  
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DownloadDoneSegments( HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DownloadDoneSegments();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_OutputSegment(HANDLE64 *handle, int iseg)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->OutputSegment(iseg);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoSpecialCommand(HANDLE64 *handle, int iseg)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DoSpecialCommand(iseg);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoRateAdjustments(HANDLE64 *handle, int i0, int i1)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DoRateAdjustments(i0, i1);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_GetAxisDefinitions(HANDLE64 *handle, int *x, int *y, int *z, int *a, int *b, int *c)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetAxisDefinitions(x, y, z, a, b, c);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_SetAxisDefinitions(HANDLE64 *handle, int x, int y, int z, int a, int b, int c)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->SetAxisDefinitions(x, y, z, a, b, c);
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetTPParams(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->SetTPParams();
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Dwell(HANDLE64 *handle, double seconds, int sequence_number)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->Dwell(seconds, sequence_number);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DownloadedSegmentCount(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_nsegs_downloaded;
}
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_TotalDownloadedTime(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_TotalDownloadedTime;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_TimeAlreadyExecuted(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_TimeAlreadyExecuted;
}
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_RemainingTime(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return (CM_dll->m_TotalDownloadedTime) - (CM_dll->m_TimeAlreadyExecuted);
}




extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Simulate(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_Simulate; 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Simulate(HANDLE64 *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	CM_dll->m_Simulate = value;
}
 


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_PreviouslyStopped(HANDLE64 *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_PreviouslyStopped; 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_PreviouslyStopped(HANDLE64 *handle, int value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	CM_dll->m_PreviouslyStopped = value;
}
 
//RS274 Interpreter Exports

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_New(int *CoordMotionhandle, HANDLE64 *handle)
{
	
	CCoordMotion *CM_dll=(CCoordMotion *)CoordMotionhandle;

	CGCodeInterpreter *GC;  
	GC = new CGCodeInterpreter(CM_dll);
 
	strcpy(GC->ToolFile,"");
	strcpy(GC->SetupFile,"");
	*handle=(HANDLE64)GC;  
}

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_Free(HANDLE64 *handle)
{
	CGCodeInterpreter *GC; // one instance of the interface class
	GC= *(CGCodeInterpreter **)handle;
	delete GC;
	*handle=0L; 
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_InitializeInterpreter(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->p_setup->origin_index = 0;
	return GC_dll->InitializeInterp() != 0;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_SetOrigin(HANDLE64 *handle, int index, double x, double y, double z, double a, double b, double c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->SetOrigin(index, x, y, z, a, b, c);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetOrigin(HANDLE64 *handle, int index, double *x, double *y, double *z, double *a, double *b, double *c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetOrigin(index, x, y, z, a, b, c);
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_M_CodeAction(HANDLE64 *handle, int index, int type, double val1, double val2,double val3,double val4,double val5, char *string)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
    GC_dll->McodeActions[index].Action = type;
    GC_dll->McodeActions[index].dParams[0] = val1;
	GC_dll->McodeActions[index].dParams[1] = val2;
	GC_dll->McodeActions[index].dParams[2] = val3;
	GC_dll->McodeActions[index].dParams[3] = val4;
	GC_dll->McodeActions[index].dParams[4] = val5;
	strncpy(GC_dll->McodeActions[index].String,string, 255); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_M_CodeAction(HANDLE64 *handle, int index, int *type, double *val1, double *val2,double *val3,double *val4,double *val5, char **string)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	*type = GC_dll->McodeActions[index].Action;
	*val1 = GC_dll->McodeActions[index].dParams[0];
	*val2 = GC_dll->McodeActions[index].dParams[1];
	*val3 = GC_dll->McodeActions[index].dParams[2];
	*val4 = GC_dll->McodeActions[index].dParams[3];
	*val5 = GC_dll->McodeActions[index].dParams[4];
	strncpy(*string,GC_dll->McodeActions[index].String, 255); 
}

//Setup Parameters
	//Acutator Origins, Positions and Offsets

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetInterpretThreadID(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll = (CGCodeInterpreter *)handle;
	return GC_dll->m_InterpretThreadID;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetInvokeThreadID(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll = (CGCodeInterpreter *)handle;
	return GC_dll->m_InvokeThreadID;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetCurrentWin32ThreadID()
{
	return GetCurrentThreadId();
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetInitializeOnExecute(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll = (CGCodeInterpreter *)handle;
	return GC_dll->m_InitializeOnExecute;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetInitializeOnExecute(HANDLE64 *handle, bool value)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->m_InitializeOnExecute = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetReadToolFile(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ReadToolFile;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetReadToolFile(HANDLE64 *handle, bool value)
{
    CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->m_ReadToolFile = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Axis_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->AA_axis_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Axis_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->AA_axis_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Current_Position(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->AA_current;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Current_Position(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->AA_current = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Origin_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->AA_origin_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Origin_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->AA_origin_offset = value;
}
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Axis_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->BB_axis_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Axis_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->BB_axis_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Current_Position(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->BB_current;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Current_Position(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->BB_current = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Origin_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->BB_origin_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Origin_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->BB_origin_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Axis_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->CC_axis_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Axis_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->CC_axis_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Current_Position(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->CC_current;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Current_Position(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->CC_current = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Origin_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->CC_origin_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Origin_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->CC_origin_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Axis_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->axis_offset_x;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Axis_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->axis_offset_x = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Current_Position(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->current_x;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Current_Position(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_x = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Origin_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->origin_offset_x;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Origin_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->origin_offset_x = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Axis_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->axis_offset_y;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Axis_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->axis_offset_y = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Current_Position(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->current_y;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Current_Position(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_y = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Origin_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->origin_offset_y;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Origin_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->origin_offset_y = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Axis_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->axis_offset_z;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Axis_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->axis_offset_z = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Current_Position(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->current_z;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Current_Position(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_z = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Origin_Offset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->origin_offset_z;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Origin_Offset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->origin_offset_z = value;
}
 

	//Modals
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ControlMode(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->control_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ControlMode(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->control_mode = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LengthUnits(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->length_units;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_LengthUnits(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->length_units = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_OriginIndex(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->origin_index;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_OriginIndex(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->origin_index = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_WorkPlane(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->plane;
} 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_WorkPlane(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->plane = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_PercentFlag(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->percent_flag;
} 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProbeComplete(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->probe_flag;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProbeComplete(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->probe_flag = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProgramX(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->program_x;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProgramX(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->program_x = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProgramY(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->program_y;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProgramY(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->program_y = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SequenceNumber(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->sequence_number;
} 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentLine(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->current_line;
} 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentLine(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_line = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_TraverseSpeed(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->traverse_rate;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_TraverseSpeed(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->traverse_rate = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LineLength(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->line_length;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FileName(HANDLE64 *handle, char *file)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	file = GC_dll->p_setup->filename;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LineText(HANDLE64 *handle, char *line)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	line = GC_dll->GetRealTimeState()->linetext;
}


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentMotionMode(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->motion_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentMotionMode(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->motion_mode = value;
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_GCode(HANDLE64 *handle, int index)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->active_g_codes[index];
}  
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_MCode(HANDLE64 *handle, int index)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->active_m_codes[index];
}  
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_Setting(HANDLE64 *handle, int index)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->active_settings[index];
} 
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_DistanceMode(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->distance_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_DistanceMode(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->distance_mode = (value == 0) ? MODE_ABSOLUTE : MODE_INCREMENTAL;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FeedMode(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->feed_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FeedMode(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->feed_mode = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FloodCoolantOn(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->flood;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FloodCoolantOn(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->flood = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_MistCoolantOn(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->mist;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_MistCoolantOn(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->mist = value;
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_BlockDelete(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->block_delete;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_BlockDelete(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->block_delete = value;
}



	//Tooling Params 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentToolSlot(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->current_slot;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentToolSlot(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_slot = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SelectedToolSlot(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->selected_tool_slot;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SelectedToolSlot(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->selected_tool_slot = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolXOffset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->tool_xoffset;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolYOffset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->tool_yoffset;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffset(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->tool_length_offset;
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolXOffset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->tool_xoffset = value;
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolYOffset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->tool_yoffset = value;
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffset(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->tool_length_offset = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffsetIndex(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->length_offset_index;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffsetIndex(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->length_offset_index = value;
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusComp(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cutter_radius_compensation;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusComp(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cutter_radius_compensation = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompValue(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cutter_comp_radius;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompValue(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cutter_comp_radius = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompSide(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cutter_comp_side;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompSide(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cutter_comp_side = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_AllowSpindleSpeedOverride(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->speed_override;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_AllowSpindleSpeedOverride(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->speed_override = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SpindleSpeed(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->speed;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SpindleSpeed(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->speed = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FeedRate(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->feed_rate;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FeedRate(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->feed_rate = value;
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SpindleDirection(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->spindle_turning;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SpindleDirection(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->spindle_turning = value;
}
 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetTool(HANDLE64 *handle, int index, int slot, int id, 
																							  double length, double diameter, double xoffset, double yoffset)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
 
	 GC_dll->p_setup->tool_table[index].slot = slot;
	 GC_dll->p_setup->tool_table[index].id = id;
	 GC_dll->p_setup->tool_table[index].length = length;
	 GC_dll->p_setup->tool_table[index].diameter = diameter;
	 GC_dll->p_setup->tool_table[index].xoffset = xoffset;
	 GC_dll->p_setup->tool_table[index].yoffset = yoffset;
} 

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_GetTool(HANDLE64 *handle, int index, int *slot, int *id, 
																							  double *length, double *diameter, double *xoffset, double *yoffset)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 *slot = GC_dll->GetRealTimeState()->tool_table[index].slot;
	 *id = GC_dll->GetRealTimeState()->tool_table[index].id;
	 *length = GC_dll->GetRealTimeState()->tool_table[index].length;
	 *diameter = GC_dll->GetRealTimeState()->tool_table[index].diameter;
	 *xoffset = GC_dll->GetRealTimeState()->tool_table[index].xoffset;
	 *yoffset = GC_dll->GetRealTimeState()->tool_table[index].yoffset;
} 

	//Fixed Cycle Params
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_CC(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cycle_cc;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_CC(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_cc = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_I(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cycle_i;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_I(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_i = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_J(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cycle_j;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_J(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_j = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_K(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cycle_k;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_K(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_k = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_L(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cycle_l;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_L(HANDLE64 *handle, double value)
{
 	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->AA_axis_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_P(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cycle_p;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_P(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_p = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Q(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cycle_q;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Q(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_q = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_R(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cycle_r;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_R(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_r = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Z(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->cycle_z;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Z(HANDLE64 *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_z = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_RetractMode(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->retract_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_RetractMode(HANDLE64 *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->retract_mode = (value == 0) ? R_PLANE : OLD_Z;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_block_p_flag(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->block1.p_flag==ON;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_block_p_number(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->block1.p_number;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_block_q_flag(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->block1.q_flag==ON;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_block_q_number(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetRealTimeState()->block1.q_number;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_block_r_flag(int* handle)
{
	CGCodeInterpreter* GC_dll = (CGCodeInterpreter*)handle;
	return GC_dll->GetRealTimeState()->block1.r_flag == ON;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_block_r_number(int* handle)
{
	CGCodeInterpreter* GC_dll = (CGCodeInterpreter*)handle;
	return GC_dll->GetRealTimeState()->block1.r_number;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_block_d_flag(int* handle)
{
	CGCodeInterpreter* GC_dll = (CGCodeInterpreter*)handle;
	return GC_dll->GetRealTimeState()->block1.d_flag == ON;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_block_d_number(int* handle)
{
	CGCodeInterpreter* GC_dll = (CGCodeInterpreter*)handle;
	return GC_dll->GetRealTimeState()->block1.d_number;
}



//Resume Parameters
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CanResume(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_Resume;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CanResume(HANDLE64 *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_Resume = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeZ(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeSafeZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeZ(HANDLE64 *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeSafeZ = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeRelAbs(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeSafeRelAbs;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeRelAbs(HANDLE64 *handle, int value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeSafeRelAbs = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeMoveToSafeZ(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeMoveToSafeZ != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeMoveToSafeZ(HANDLE64 *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeMoveToSafeZ = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseXY(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeTraverseXY != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseXY(HANDLE64 *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeTraverseXY = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeX(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeTraverseSafeX;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeX(HANDLE64 *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeTraverseSafeX = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeY(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeTraverseSafeY;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeY(HANDLE64 *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeTraverseSafeY = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeStartSpindle(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeSafeStartSpindle != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeStartSpindle(HANDLE64 *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeSafeStartSpindle = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeSpindleCWCCW(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeSafeSpindleCWCCW;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeSpindleCWCCW(HANDLE64 *handle, int value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeSafeSpindleCWCCW = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeDoSafeFeedZ(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeDoSafeFeedZ != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeDoSafeFeedZ(HANDLE64 *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeDoSafeFeedZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeFeedSafeZ(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeFeedSafeZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeFeedSafeZ(HANDLE64 *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeFeedSafeZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeResumeFeedRate(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeResumeFeedRate;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeResumeFeedRate(HANDLE64 *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeResumeFeedRate = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeZFeedRate(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeZFeedRate;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeZFeedRate(HANDLE64 *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeZFeedRate = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeRestoreFeedRate(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeRestoreFeedRate != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeRestoreFeedRate(HANDLE64 *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeRestoreFeedRate = value;
}


// Parameter methods

extern "C" __declspec(dllexport) double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Parameter(HANDLE64* handle, int index)
{
	CGCodeInterpreter* GC_dll = (CGCodeInterpreter*)handle;
	if (index >= 0 && index < RS274NGC_MAX_PARAMETERS)
	{
		return GC_dll->GetRealTimeState()->parameters[index];
	}
	return 0;
}

extern "C" __declspec(dllexport) bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Parameter(HANDLE64* handle, int index, double value)
{
	CGCodeInterpreter* GC_dll = (CGCodeInterpreter*)handle;
	if (index >= 0 && index < RS274NGC_MAX_PARAMETERS)
	{
		GC_dll->p_setup->parameters[index] = value;
		return true;
	}
	return false;
}


//Functions


extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_COMPLETE_CALLBACK(HANDLE64 *handle, netG_COMPLETE_CALLBACK *p)
{
    G_CompleteHandler = p;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_STATUS_CALLBACK(HANDLE64 *handle, netG_STATUS_CALLBACK *p)
{
    G_StatusHandler = p;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_CALLBACK(HANDLE64 *handle, netG_USER_CALLBACK *p)
{
    CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
    G_UserCallbackHandler = p;
	GC_dll->SetUserCallback(&Local_G_USER_CALLBACK); 
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_MCODE_CALLBACK(HANDLE64 *handle, netG_USER_MCODE_CALLBACK *p)
{
    CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
    G_UserMCodeCallbackHandler = p;
	GC_dll->SetUserMCodeCallback(&Local_G_USER_MCODE_CALLBACK); 
}



extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ChangeFixtureNumber(HANDLE64 *handle, int fixture)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ChangeFixtureNumber(fixture);
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesToUserUnits(HANDLE64 *handle, double inches)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->InchesToUserUnits(inches);
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInches(HANDLE64 *handle, double units)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->UserUnitsToInches(units);
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsX(HANDLE64 *handle, double x)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsX(x);
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsY(HANDLE64 *handle, double y)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsY(y);
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsZ(HANDLE64 *handle, double z)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsZ(z);
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsA(HANDLE64 *handle, double a)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsA(a);
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsB(HANDLE64 *handle, double b)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsB(b);
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsC(HANDLE64 *handle, double c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsC(c);
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetFeedRate(HANDLE64 *handle, double feedrate)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->SetFeedRate(feedrate);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_InvokeAction(HANDLE64 *handle, int i, BOOL FlushBeforeUnbufferedOperation)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->InvokeAction(i, FlushBeforeUnbufferedOperation);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Interpret(HANDLE64 *handle,
				  int board_type,
		          const char *fname,
			      int start, int end,
				  int restart)
{
 
    CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
 
	return GC_dll->Interpret(board_type, fname, start, end, restart, Local_G_STATUS_CALLBACK, Local_G_COMPLETE_CALLBACK);
}



extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ExecutePC(HANDLE64 *handle, const char *Name)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ExecutePC(Name);
}
  
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_DoExecute(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->DoExecute();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_DoExecuteComplete(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->DoExecuteComplete();
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetHalt(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->Halt();
}
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetHalt(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetHalt();
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetAbort(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->Abort();
}
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetAbort(HANDLE64 *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetAbort();
}
 
 extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadAndSyncCurPositions
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ReadAndSyncCurPositions(x,y,z,a,b,c);
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadCurInterpreterPosition
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	double dx;
	double dy;
	double dz;
	double da;
	double db;
	double dc;

	GC_dll->CoordMotion->ReadCurAbsPosition(&dx, &dy, &dz, &da, &db, &dc); 
	GC_dll->ConvertAbsoluteToInterpreterCoord(dx, dy, dz, da, db, dc, &dx, &dy, &dz, &da, &db, &dc); 

	*x = dx;
	*y = dy;
	*z = dz;
	*a = da;
	*b = db;
	*c = dc;
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadCurMachinePosition
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	double dx;
	double dy;
	double dz;
	double da;
	double db;
	double dc;

	GC_dll->CoordMotion->ReadCurAbsPosition(&dx, &dy, &dz, &da, &db, &dc); 
	GC_dll->ConvertAbsoluteToMachine(dx, dy, dz, da, db, dc, &dx, &dy, &dz, &da, &db, &dc); 
	
	*x = dx;
	*y = dy;
	*z = dz;
	*a = da;
	*b = db;
	*c = dc;
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToMachine
(HANDLE64 *handle, double x, double y, double z, double a, double b, double c, 
			  double *xp, double *yp, double *zp, double *ap, double *bp, double *cp)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->ConvertAbsoluteToMachine(x, y, z, a, b, c, xp, yp, zp, ap, bp, cp); 
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToInterpreterCoord
(HANDLE64 *handle, double x, double y, double z, double a, double b, double c, 
			  double *xp, double *yp, double *zp, double *ap, double *bp, double *cp)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->ConvertAbsoluteToInterpreterCoord(x, y, z, a, b, c, xp, yp, zp, ap, bp, cp); 
}


extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetToolFile(void *handle, char *file)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle; 
    size_t length = (int)strlen(file);
	  
	for(size_t i = 0; i < 260; i++)
	{
	  GC_dll->ToolFile[i]= '\0';
	}
  
	for(size_t i = 0; i < length; i++)
	{
	  GC_dll->ToolFile[i] = file[i]; 
	} 
}
extern "C" __declspec(dllexport) 	char*   __stdcall KM_dotnet_Interop_GCodeInterpreter_GetToolFile(void *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	char* s = GC_dll->ToolFile;
	ULONG ulSize = (int)strlen(s) + sizeof(char);
	char* pszReturn = NULL;

	pszReturn = (char*)::CoTaskMemAlloc(ulSize);
	// Copy the contents of s
	// to the memory pointed to by pszReturn.
	strcpy(pszReturn, s);
	return pszReturn;
}


extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetSetupFile(void *handle, char *file)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle; 
	size_t length = (int)strlen(file);

	for(int i = 0; i < 260; i++)
	{
	  GC_dll->SetupFile[i]= '\0';
	}
  
	for(size_t i = 0; i < length; i++)
	{
	  GC_dll->SetupFile[i] = file[i]; 
	}  
}

extern "C" __declspec(dllexport) 	char*   __stdcall KM_dotnet_Interop_GCodeInterpreter_GetSetupFile(void *handle)
{
	CGCodeInterpreter* GC_dll = (CGCodeInterpreter*)handle;
	char* s = GC_dll->SetupFile;
	ULONG ulSize = (int)strlen(s) + sizeof(char);
	char* pszReturn = NULL;

	pszReturn = (char*)::CoTaskMemAlloc(ulSize);
	// Copy the contents of s
	// to the memory pointed to by pszReturn.
	strcpy(pszReturn, s);
	return pszReturn;
}

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetVarsFile(void *handle, char *file)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle; 
	size_t length = (int)strlen(file);

	for(int i = 0; i < 260; i++)
	{
	  GC_dll->VarsFile[i]= '\0';
	}
  
	for(size_t i = 0; i < length; i++)
	{
	  GC_dll->VarsFile[i] = file[i]; 
	}  
}

extern "C" __declspec(dllexport) 	char*   __stdcall KM_dotnet_Interop_GCodeInterpreter_GetVarsFile(void *handle)
{
	CGCodeInterpreter* GC_dll = (CGCodeInterpreter*)handle;
	char* s = GC_dll->VarsFile;
	ULONG ulSize = (int)strlen(s) + sizeof(char);
	char* pszReturn = NULL;

	pszReturn = (char*)::CoTaskMemAlloc(ulSize);
	// Copy the contents of s
	// to the memory pointed to by pszReturn.
	strcpy(pszReturn, s);
	return pszReturn;
}








// The one and only CKMotion_dotNet_InteropApp object

CKMotion_dotNet_InteropApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x14FF0ECB, 0x96A7, 0x44BF, { 0x90, 0x39, 0x44, 0x77, 0x6C, 0x28, 0x38, 0x28 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CKMotion_dotNet_InteropApp initialization

BOOL CKMotion_dotNet_InteropApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();

	return TRUE;
}

// DllGetClassObject - Returns class factory

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
}


// DllCanUnloadNow - Allows COM to unload DLL

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}


// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll())
		return SELFREG_E_CLASS;

	return S_OK;
}


// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll(FALSE))
		return SELFREG_E_CLASS;

	return S_OK;
}
