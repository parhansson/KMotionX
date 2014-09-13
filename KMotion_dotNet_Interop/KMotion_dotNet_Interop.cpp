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


netCONSOLE_HANDLER *ConsoleHandler=NULL;    

//KMotion Callbacks
int LocalConsoleHandler(const char *buf)
{
	return ConsoleHandler(buf);
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

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_New(int *handle, int board)
{
	CKMotionDLL *KM; // one instance of the interface class

	KM = new CKMotionDLL(0);
	KM->BoardID = board;
	*handle=(int)KM; 
	return 1L;
}
 
extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_Free(int *handle)
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
	int rslt=KM_dll->WaitToken(false, timeout);
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


extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_SimpleCompileAndLoadCoff(void *handle, int Thread, const char *Name)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->CompileAndLoadCoff(Name,Thread);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_CompileAndLoadCoff(void *handle, int Thread, const char *Name, char **Err, int MaxErrLen)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->CompileAndLoadCoff(Name,Thread,*Err,MaxErrLen);
	return rslt;
}

extern "C" __declspec(dllexport) int __stdcall KM_dotnet_Interop_Compile(void *handle,const int boardtype, int Thread, const char *Name, const char *OutFile, char *Err, int MaxErrLen)
{
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	int rslt=KM_dll->Compile(Name, OutFile, boardtype, Thread, Err, MaxErrLen);
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



extern "C" __declspec(dllexport) 	bool  __stdcall KM_dotnet_Interop_MainStatus_GetStatus(void *handle, bool lock,
			int *versionandsize,
            int adc[24],
            int dac[8],
            int pwm[16],
            double position[8],
            double destination[8],
            unsigned char *outputchan0,
            int *inputmodes,
            int *inputmodes2,
            int *outputmodes,
            int *outputmodes2,
            int *enables,
            int *axisdone,
            int bitsdirection[2],
            int bitsstate[2],
            int *snapbitsdirection0,
            int *snapbitsdirection1,
            int *snapbitsstate0,
            int *snapbitsstate1,
            int *kanalgobitsstateinputs,
            int *kanalogbitsstateoutputs,
            int *runonstartup,
            int *threadactive,
            int *stopimmediatestate,
            double *timestamp,
            int pccomm[8],
            int *virtualbits,
            int *virtualbitsex0)
{
	
	CKMotionDLL *KM_dll=(CKMotionDLL *)handle;
	MAIN_STATUS m ;
	
	 KM_dll->GetStatus(m,lock);
	 *versionandsize = m.VersionAndSize;
	 for(int i = 0; i < 24; i++)
	 {
       adc[i] = m.ADC[i];
	 } 
	 for(int i = 0; i < 8; i++)
	 {
       dac[i] = m.DAC[i];
	 }  	
	 for(int i = 0; i < 16; i++)
	 {
       pwm[i] = m.PWM[i];
	 }   
	 for(int i = 0; i < 8; i++)
	 {
       position[i] = m.Position[i];
	 }  	
	 for(int i = 0; i < 8; i++)
	 {
       destination[i] = m.Dest[i];
	 }   	
	 for(int i = 0; i < 8; i++)
	 {
       outputchan0[i] = m.OutputChan0[i];
	 }  	  
     *inputmodes = m.InputModes;
     *inputmodes2 = m.InputModes2;
     *outputmodes = m.OutputModes;
     *outputmodes2 = m.OutputModes2;
     *enables = m.Enables;
     *axisdone = m.AxisDone; 	
	 for(int i = 0; i < 2; i++)
	 {
       bitsdirection[i] = m.BitsDirection[i];
	 }   
	 for(int i = 0; i < 2; i++)
	 {
       bitsstate[i] = m.BitsState[i];
	 }    
     *snapbitsdirection0 = m.SnapBitsDirection0;
     *snapbitsdirection1 = m.SnapBitsDirection1;
     *snapbitsstate0 = m.SnapBitsState0;
     *snapbitsstate1 = m.SnapBitsState1;
     *kanalgobitsstateinputs = m.KanalogBitsStateInputs;
     *kanalogbitsstateoutputs = m.KanalogBitsStateOutputs;
     *runonstartup = m.RunOnStartUp;
     *threadactive = m.ThreadActive;
     *stopimmediatestate = m.StopImmediateState;
     *timestamp = m.TimeStamp;  
	 for(int i = 0; i < 8; i++)
	{
       pccomm[i] = m.PC_comm[i];
	}
     *virtualbits = m.VirtualBits;
     *virtualbitsex0 = m.VirtualBitsEx0;

       return true;
	}
 
  
//Coordinated Motion Members
//SET MOTION_PARAMS

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_BreakAngle(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->BreakAngle;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_BreakAngle(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->BreakAngle = value;
}


extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CollinearTol(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CollinearTol;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CollinearTol(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams(); 
	p->CollinearTol = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CornerTol(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CornerTol;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CornerTol(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CornerTol = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_FacetAngle(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->FacetAngle;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_FacetAngle(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->FacetAngle = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_TPLookahead(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->TPLookahead;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_TPLookahead(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->TPLookahead = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusA(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->RadiusA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusA(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->RadiusA = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusB(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->RadiusB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusB(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->RadiusB = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusC(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->RadiusC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusC(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->RadiusC = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelX(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelX;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelX(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelX = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelY(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelY;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelY(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelY = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelZ(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelZ(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelA(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelA(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelA = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelB(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelB(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelB = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelC(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxAccelC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelC(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxAccelC = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelX(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelX;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelX(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelX = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelY(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelY;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelY(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelY = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelZ(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelZ(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelA(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelA(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelA = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelB(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelB(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelB = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelC(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxVelC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelC(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxVelC = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchX(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchX;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchX(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchX = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchY(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchY;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchY(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchY = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchZ(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchZ(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchA(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchA(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchA = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchB(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchB(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchB = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchC(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->CountsPerInchC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchC(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->CountsPerInchC = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxLinearLength(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->MaxLinearLength;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxLinearLength(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->MaxLinearLength = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesA(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->DegreesA;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesA(int *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->DegreesA = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesB(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->DegreesB;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesB(int *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->DegreesB = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesC(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	return p->DegreesC;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesC(int *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	MOTION_PARAMS *p=CM_dll->GetMotionParams();
	p->DegreesC = value;
}

 
//Coordinated Motion Rest

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_CoordMotion_New(int *KmotionDLLhandle, int *handle)
{
	
	CKMotionDLL *KM_dll=(CKMotionDLL *)KmotionDLLhandle;
	CCoordMotion *CM;  
	CM = new CCoordMotion(KM_dll); 
	*handle=(int)CM;  
}

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_CoordMotion_Free(int *handle)
{
	CCoordMotion *CM; // one instance of the interface class
	CM= *(CCoordMotion **)handle;
	delete CM;
	*handle=0L; 
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_DownloadInit(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	CM_dll->DownloadInit();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_CheckMotionHalt(int *handle, bool Coord)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->CheckMotionHalt(Coord); 
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ExecutionStop(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->ExecutionStop(); 
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_Get_FeedRateOverride(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetFeedRateOverride(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_FeedRateOverride(int *handle, double value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->SetFeedRateOverride(value); 
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_AxisDestination(int *handle, int axis, double *value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetDestination(axis, value); 
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_AxisPosition(int *handle, int axis, double *value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	return CM_dll->GetPosition(axis, value); 
}
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Abort(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetAbort(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Abort(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	 
    CM_dll->SetAbort(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Clear_Abort(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->ClearAbort(); 
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Halt(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetHalt(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Halt(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->SetHalt(); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Clear_Halt(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->ClearHalt();
}


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_FlushSegments(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->FlushSegments();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_WaitForSegmentsFinished(int *handle, BOOL NoErrorOnDisable)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->WaitForSegmentsFinished(NoErrorOnDisable);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_WaitForMoveXYZABCFinished(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->WaitForMoveXYZABCFinished();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoKMotionCmd(int *handle, const char *s, BOOL FlushBeforeUnbufferedOperation)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DoKMotionCmd(s, FlushBeforeUnbufferedOperation);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoKMotionBufCmd(int *handle, const char *s)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DoKMotionBufCmd(s);
}



 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_MeasurePointAppendToFile(int *handle, const char *name)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->MeasurePointAppendToFile(name);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightTraverse
(int *handle, double x, double y, double z, double a, double b, double c, bool NoCallback)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->StraightTraverse(x, y, z, a, b, c, NoCallback);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ArcFeed
	(int *handle, double DesiredFeedRate_in_per_sec, int plane,
	double first_end, double second_end, 
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->ArcFeed(DesiredFeedRate_in_per_sec, (CANON_PLANE)plane, first_end, second_end, first_axis, second_axis, rotation, axis_end_point, a, b, c, sequence_number, ID);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ArcFeedAccel
	(int *handle, double DesiredFeedRate_in_per_sec, double DesiredAccel, int plane,
	double first_end, double second_end, 
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->ArcFeedAccel(DesiredFeedRate_in_per_sec, DesiredAccel, (CANON_PLANE)plane, first_end, second_end, first_axis, second_axis, rotation, axis_end_point, a, b, c, sequence_number, ID);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightFeed
(int *handle, double DesiredFeedRate_in_per_sec, double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->StraightFeed(DesiredFeedRate_in_per_sec, x, y, z, a, b, c, sequence_number, ID);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightFeedAccel
(int *handle, double DesiredFeedRate_in_per_sec, double DesiredAccel, double x, double y, double z, double a, double b, double c, int sequence_number, int ID)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->StraightFeedAccel(DesiredFeedRate_in_per_sec, DesiredAccel, x, y, z, a, b, c, sequence_number, ID);
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ReadAndSyncCurPositions
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c)
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
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c, bool snap)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->ReadCurAbsPosition(x, y, z, a, b, c, snap);
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetStraightTraverseCallback(int *handle, netSTRAIGHT_TRAVERSE_CALLBACK *p)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    StraightTransverseHandler = p;
    CM_dll->SetStraightTraverseCallback(LocalStraightTransverseHandler); 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetStraightFeedCallback(int *handle, netSTRAIGHT_FEED_CALLBACK *p)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    StraightFeedHandler = p;
    CM_dll->SetStraightFeedCallback(LocalStraightFeedHandler);  
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetArcFeedCallback(int *handle, netARC_FEED_CALLBACK *p)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    ArcFeedHandler = p;
    CM_dll->SetArcFeedCallback(LocalArcFeedHandler);  
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DownloadDoneSegments( int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DownloadDoneSegments();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_OutputSegment(int *handle, int iseg)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->OutputSegment(iseg);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoSpecialCommand(int *handle, int iseg)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DoSpecialCommand(iseg);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoRateAdjustments(int *handle, int i0, int i1)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->DoRateAdjustments(i0, i1);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_GetAxisDefinitions(int *handle, int *x, int *y, int *z, int *a, int *b, int *c)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->GetAxisDefinitions(x, y, z, a, b, c);
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetTPParams(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    CM_dll->SetTPParams();
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Dwell(int *handle, double seconds, int sequence_number)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->Dwell(seconds, sequence_number);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DownloadedSegmentCount(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_nsegs_downloaded;
}
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_TotalDownloadedTime(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_TotalDownloadedTime;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_TimeAlreadyExecuted(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_TimeAlreadyExecuted;
}
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_RemainingTime(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return (CM_dll->m_TotalDownloadedTime) - (CM_dll->m_TimeAlreadyExecuted);
}




extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Simulate(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_Simulate; 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Simulate(int *handle, bool value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	CM_dll->m_Simulate = value;
}
 


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_PreviouslyStopped(int *handle)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
    return CM_dll->m_PreviouslyStopped; 
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_PreviouslyStopped(int *handle, int value)
{
	CCoordMotion *CM_dll=(CCoordMotion *)handle;
	CM_dll->m_PreviouslyStopped = value;
}
 
//RS274 Interpreter Exports

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_New(int *CoordMotionhandle, int *handle)
{
	
	CCoordMotion *CM_dll=(CCoordMotion *)CoordMotionhandle;

	CGCodeInterpreter *GC;  
	GC = new CGCodeInterpreter(CM_dll);
 
	strcpy(GC->ToolFile,"");
	strcpy(GC->SetupFile,"");
	*handle=(int)GC;  
}

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_Free(int *handle)
{
	CGCodeInterpreter *GC; // one instance of the interface class
	GC= *(CGCodeInterpreter **)handle;
	delete GC;
	*handle=0L; 
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_InitializeInterpreter(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->p_setup->origin_index = 0;
	return GC_dll->InitializeInterp() != 0;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_SetOrigin(int *handle, int index, double x, double y, double z, double a, double b, double c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->SetOrigin(index, x, y, z, a, b, c);
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetOrigin(int *handle, int index, double *x, double *y, double *z, double *a, double *b, double *c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetOrigin(index, x, y, z, a, b, c);
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_M_CodeAction(int *handle, int index, int type, double val1, double val2,double val3,double val4,double val5, char *string)
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
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_M_CodeAction(int *handle, int index, int *type, double *val1, double *val2,double *val3,double *val4,double *val5, char **string)
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

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetInitializeOnExecute(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_InitializeOnExecute;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetInitializeOnExecute(int *handle, bool value)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->m_InitializeOnExecute = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetReadToolFile(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ReadToolFile;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetReadToolFile(int *handle, bool value)
{
    CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->m_ReadToolFile = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Axis_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->AA_axis_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Axis_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->AA_axis_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Current_Position(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->AA_current;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Current_Position(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->AA_current = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Origin_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->AA_origin_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Origin_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->AA_origin_offset = value;
}
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Axis_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->BB_axis_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Axis_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->BB_axis_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Current_Position(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->BB_current;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Current_Position(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->BB_current = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Origin_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->BB_origin_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Origin_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->BB_origin_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Axis_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->CC_axis_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Axis_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->CC_axis_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Current_Position(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->CC_current;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Current_Position(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->CC_current = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Origin_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->CC_origin_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Origin_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->CC_origin_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Axis_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->axis_offset_x;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Axis_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->axis_offset_x = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Current_Position(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->current_x;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Current_Position(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_x = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Origin_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->origin_offset_x;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Origin_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->origin_offset_x = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Axis_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->axis_offset_y;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Axis_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->axis_offset_y = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Current_Position(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->current_y;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Current_Position(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_y = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Origin_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->origin_offset_y;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Origin_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->origin_offset_y = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Axis_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->axis_offset_z;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Axis_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->axis_offset_z = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Current_Position(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->current_z;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Current_Position(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_z = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Origin_Offset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->origin_offset_z;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Origin_Offset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->origin_offset_z = value;
}
 

	//Modals
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ControlMode(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->control_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ControlMode(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->control_mode = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LengthUnits(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->length_units;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_LengthUnits(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->length_units = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_OriginIndex(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->origin_index;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_OriginIndex(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->origin_index = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_WorkPlane(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->plane;
} 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_WorkPlane(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->plane = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_PercentFlag(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->percent_flag;
} 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProbeComplete(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->probe_flag;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProbeComplete(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->probe_flag = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProgramX(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->program_x;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProgramX(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->program_x = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProgramY(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->program_y;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProgramY(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->program_y = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SequenceNumber(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->sequence_number;
} 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentLine(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->current_line;
} 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentLine(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_line = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_TraverseSpeed(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->traverse_rate;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_TraverseSpeed(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->traverse_rate = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LineLength(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->line_length;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FileName(int *handle, char *file)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	file = GC_dll->p_setup->filename;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LineText(int *handle, char *line)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	line = GC_dll->p_setup->linetext;
}


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentMotionMode(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->motion_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentMotionMode(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->motion_mode = value;
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_GCode(int *handle, int index)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->active_g_codes[index];
}  
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_MCode(int *handle, int index)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->active_m_codes[index];
}  
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_Setting(int *handle, int index)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->active_settings[index];
} 
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_DistanceMode(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->distance_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_DistanceMode(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->distance_mode = (value == 0) ? MODE_ABSOLUTE : MODE_INCREMENTAL;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FeedMode(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->feed_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FeedMode(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->feed_mode = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FloodCoolantOn(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->flood;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FloodCoolantOn(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->flood = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_MistCoolantOn(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->mist;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_MistCoolantOn(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->mist = value;
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_BlockDelete(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->block_delete;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_BlockDelete(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->block_delete = value;
}



	//Tooling Params 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentToolSlot(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->current_slot;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentToolSlot(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->current_slot = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SelectedToolSlot(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->selected_tool_slot;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SelectedToolSlot(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->selected_tool_slot = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffset(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->tool_length_offset;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffset(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->tool_length_offset = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffsetIndex(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->length_offset_index;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffsetIndex(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->length_offset_index = value;
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusComp(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cutter_radius_compensation;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusComp(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cutter_radius_compensation = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompValue(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cutter_comp_radius;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompValue(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cutter_comp_radius = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompSide(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cutter_comp_side;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompSide(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cutter_comp_side = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_AllowSpindleSpeedOverride(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->speed_override;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_AllowSpindleSpeedOverride(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->speed_override = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SpindleSpeed(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->speed;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SpindleSpeed(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->speed = value;
}
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SpindleDirection(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->spindle_turning;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SpindleDirection(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->spindle_turning = value;
}
 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetTool(int *handle, int id, double length, double diameter)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
 
	 GC_dll->p_setup->tool_table[id].length = length;
	 GC_dll->p_setup->tool_table[id].diameter = diameter;
	 char* tooltable = GC_dll->ToolFile;
	 
} 

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_GetTool(int *handle, int id, double *length, double *diameter)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 *length = GC_dll->p_setup->tool_table[id].length;
	 *diameter = GC_dll->p_setup->tool_table[id].diameter;
	 char* tooltable = GC_dll->ToolFile;
} 

	//Fixed Cycle Params
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_CC(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cycle_cc;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_CC(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_cc = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_I(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cycle_i;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_I(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_i = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_J(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cycle_j;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_J(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_j = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_K(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cycle_k;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_K(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_k = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_L(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cycle_l;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_L(int *handle, double value)
{
 	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->AA_axis_offset = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_P(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cycle_p;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_P(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_p = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Q(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cycle_q;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Q(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_q = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_R(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cycle_r;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_R(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_r = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Z(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->cycle_z;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Z(int *handle, double value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->cycle_z = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_RetractMode(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->p_setup->retract_mode;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_RetractMode(int *handle, int value)
{
	 CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->p_setup->retract_mode = (value == 0) ? R_PLANE : OLD_Z;
}



//Resume Parameters
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CanResume(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_Resume;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CanResume(int *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_Resume = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeZ(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeSafeZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeZ(int *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeSafeZ = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeRelAbs(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeSafeRelAbs;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeRelAbs(int *handle, int value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeSafeRelAbs = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeMoveToSafeZ(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeMoveToSafeZ != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeMoveToSafeZ(int *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeMoveToSafeZ = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseXY(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeTraverseXY != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseXY(int *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeTraverseXY = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeX(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeTraverseSafeX;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeX(int *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeTraverseSafeX = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeY(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeTraverseSafeY;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeY(int *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeTraverseSafeY = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeStartSpindle(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeSafeStartSpindle != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeStartSpindle(int *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeSafeStartSpindle = value;
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeSpindleCWCCW(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeSafeSpindleCWCCW;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeSpindleCWCCW(int *handle, int value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeSafeSpindleCWCCW = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeDoSafeFeedZ(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeDoSafeFeedZ != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeDoSafeFeedZ(int *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeDoSafeFeedZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeFeedSafeZ(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeFeedSafeZ;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeFeedSafeZ(int *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeFeedSafeZ = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeResumeFeedRate(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeResumeFeedRate;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeResumeFeedRate(int *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeResumeFeedRate = value;
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeZFeedRate(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeZFeedRate;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeZFeedRate(int *handle, double value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeZFeedRate = value;
}

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeRestoreFeedRate(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->m_ResumeRestoreFeedRate != 0;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeRestoreFeedRate(int *handle, bool value)
{
     CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	 GC_dll->m_ResumeRestoreFeedRate = value;
}

//Functions


extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_COMPLETE_CALLBACK(int *handle, netG_COMPLETE_CALLBACK *p)
{
    G_CompleteHandler = p;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_STATUS_CALLBACK(int *handle, netG_STATUS_CALLBACK *p)
{
    G_StatusHandler = p;
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_CALLBACK(int *handle, netG_USER_CALLBACK *p)
{
    CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
    G_UserCallbackHandler = p;
	GC_dll->SetUserCallback(&Local_G_USER_CALLBACK); 
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_MCODE_CALLBACK(int *handle, netG_USER_MCODE_CALLBACK *p)
{
    CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
    G_UserMCodeCallbackHandler = p;
	GC_dll->SetUserMCodeCallback(&Local_G_USER_MCODE_CALLBACK); 
}



extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ChangeFixtureNumber(int *handle, int fixture)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ChangeFixtureNumber(fixture);
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesToUserUnits(int *handle, double inches)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->InchesToUserUnits(inches);
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInches(int *handle, double units)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->UserUnitsToInches(units);
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsX(int *handle, double x)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsX(x);
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsY(int *handle, double y)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsY(y);
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsZ(int *handle, double z)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsZ(z);
}
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsA(int *handle, double a)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsA(a);
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsB(int *handle, double b)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsB(b);
}

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsC(int *handle, double c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ConvertAbsToUserUnitsC(c);
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetFeedRate(int *handle, double feedrate)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->SetFeedRate(feedrate);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_InvokeAction(int *handle, int i, BOOL FlushBeforeUnbufferedOperation)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->InvokeAction(i, FlushBeforeUnbufferedOperation);
}

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Interpret(int *handle,
				  int board_type,
		          const char *fname,
			      int start, int end,
				  int restart)
{
 
    CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
 
	return GC_dll->Interpret(board_type, fname, start, end, restart, Local_G_STATUS_CALLBACK, Local_G_COMPLETE_CALLBACK);
}



extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ExecutePC(int *handle, const char *Name)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ExecutePC(Name);
}
  
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_DoExecute(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->DoExecute();
}
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_DoExecuteComplete(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->DoExecuteComplete();
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetHalt(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->Halt();
}
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetHalt(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetHalt();
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetAbort(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->Abort();
}
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetAbort(int *handle)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->GetAbort();
}
 
 extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadAndSyncCurPositions
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	return GC_dll->ReadAndSyncCurPositions(x,y,z,a,b,c);
}
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadCurInterpreterPosition
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c)
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
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c)
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
(int *handle, double x, double y, double z, double a, double b, double c, 
			  double *xp, double *yp, double *zp, double *ap, double *bp, double *cp)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->ConvertAbsoluteToMachine(x, y, z, a, b, c, xp, yp, zp, ap, bp, cp); 
}

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToInterpreterCoord
(int *handle, double x, double y, double z, double a, double b, double c, 
			  double *xp, double *yp, double *zp, double *ap, double *bp, double *cp)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;
	GC_dll->ConvertAbsoluteToInterpreterCoord(x, y, z, a, b, c, xp, yp, zp, ap, bp, cp); 
}


extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetToolFile(void *handle, char *file)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle; 
    size_t length = strlen(file);
	  
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
	return GC_dll->ToolFile;
}


extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetSetupFile(void *handle, char *file)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle; 
	size_t length = strlen(file);

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
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;  
	return GC_dll->SetupFile;
}

extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetVarsFile(void *handle, char *file)
{
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle; 
	size_t length = strlen(file);

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
	CGCodeInterpreter *GC_dll=(CGCodeInterpreter *)handle;  
	return GC_dll->VarsFile;
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
