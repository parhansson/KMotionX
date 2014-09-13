// KMotion_dotNet_Interop.h : main header file for the KMotion_dotNet_Interop DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif
  
#include "resource.h"		// main symbols

#include <iostream> 
#include <fstream>  

//Standard Kmotion Members

typedef int __stdcall netCONSOLE_HANDLER(const char *buf);
typedef void __stdcall netERRMSG_HANDLER(const char *ErrMsg);

extern "C" __declspec(dllexport)	int  __stdcall KM_dotnet_Interop_New(int *handle, int board);
extern "C" __declspec(dllexport)	int  __stdcall KM_dotnet_Interop_Free(int *handle);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_WriteLineReadLine(void *handle, char *s, char **rtrn);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_WriteLine(void *handle, char *s);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_WriteLineWithEcho(void *handle, char *s);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_ReadLineTimeOut(void *handle, char **buf, int TimeOutms);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_WaitToken(void *handle, int timeout);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_KMotionLock(void *handle);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_KMotionLockRecovery(void *handle);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_USBLocation(void *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_ReleaseToken(void *handle);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_Failed(void *handle);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_Disconnect(void *handle);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_CheckForReady(void *handle);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_LoadCoff(void *handle,int Thread, const char *Name);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_LoadCoffPack(void *handle,int Thread, const char *Name, int PackToFlash);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_SimpleCompileAndLoadCoff(void *handle, int Thread, const char *Name);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_CompileAndLoadCoff(void *handle, int Thread, const char *Name, char **Err, int MaxErrLen);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_Compile(void *handle,const int boardtype, int Thread, const char *Name, const char *OutFile, char *Err, int MaxErrLen);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_ServiceConsole(void *handle);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_SetConsoleCallback(void *handle, netCONSOLE_HANDLER *ch);
extern "C" __declspec(dllexport)    int __stdcall KM_dotnet_Interop_SetErrorCallback(void *handle, netERRMSG_HANDLER *ch);

extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_ListLocations(void *handle, int *nlocations, int *list);
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_GetFirmwareVersion(void *handle);  

extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_CheckCoffSize(void *handle, const char *Name, int *size_text, int *size_bss, int *size_data, int *size_total);//
extern "C" __declspec(dllexport) 	int  __stdcall  KM_dotnet_Interop_GetLoadAddress(void *handle, int thread, int BoardType);//
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_ConvertToOut(void *handle, int thread, const char *InFile, char *OutFile, int MaxLength);//

extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_CheckKMotionVersion(void *handle, int *type=NULL, bool GetBoardTypeOnly=false);//
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_ExtractCoffVersionString(void *handle, const char *InFile, char *Version);//


//MAIN_STATUS Members
 
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
            int *virtualbitsex0);





//Coordinated Motion Members
typedef void __stdcall netSTRAIGHT_TRAVERSE_CALLBACK(double x, double y, double z, double a, double b, double c, int sequence_number);

typedef void __stdcall netSTRAIGHT_FEED_CALLBACK(double DesiredFeedRate_in_per_sec,
							   double x, double y, double z, double a, double b, double c, int sequence_number, int ID);

typedef void __stdcall netARC_FEED_CALLBACK(bool ZeroLenAsFullCircles, double DesiredFeedRate_in_per_sec, 
			    int plane,
				double first_end, double second_end, 
		        double first_axis, double second_axis, int rotation,
				double axis_end_point, double a, double b, double c, 
				double first_start, double second_start, double axis_start_point, int sequence_number, int ID);

// CoordMotion MOTION_PARAMS
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_BreakAngle(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_BreakAngle(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CollinearTol(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CollinearTol(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CornerTol(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CornerTol(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_FacetAngle(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_FacetAngle(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_TPLookahead(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_TPLookahead(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusA(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusA(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusB(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusB(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusC(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusC(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelX(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelX(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelY(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelY(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelZ(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelZ(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelA(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelA(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelB(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelB(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelC(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelC(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelX(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelX(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelY(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelY(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelZ(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelZ(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelA(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelA(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelB(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelB(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelC(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelC(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchX(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchX(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchY(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchY(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchZ(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchZ(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchA(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchA(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchB(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchB(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchC(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchC(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxLinearLength(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxLinearLength(int *handle, double value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesA(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesA(int *handle, bool value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesB(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesB(int *handle, bool value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesC(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesC(int *handle, bool value);
 
//Function Calls
extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_CoordMotion_New(int *KmotionDLLhandle, int *handle);
extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_CoordMotion_Free(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_DownloadInit(int *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_CheckMotionHalt(int *handle, bool Coord);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ExecutionStop(int *handle);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_Get_FeedRateOverride(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_FeedRateOverride(int *handle, double value);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_AxisDestination(int *handle, int axis, double *value);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_AxisPosition(int *handle, int axis, double *value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Abort(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Abort(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Clear_Abort(int *handle);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Halt(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Halt(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Clear_Halt(int *handle);


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_FlushSegments(int *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_WaitForSegmentsFinished(int *handle, BOOL NoErrorOnDisable = FALSE);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_WaitForMoveXYZABCFinished(int *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoKMotionCmd(int *handle, const char *s, BOOL FlushBeforeUnbufferedOperation);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoKMotionBufCmd(int *handle, const char *s);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_MeasurePointAppendToFile(int *handle, const char *name);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightTraverse
(int *handle, double x, double y, double z, double a, double b, double c, bool NoCallback=false);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ArcFeed
	(int *handle, double DesiredFeedRate_in_per_sec, int plane,
	double first_end, double second_end, 
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ArcFeedAccel
	(int *handle, double DesiredFeedRate_in_per_sec, double DesiredAccel, int plane,
	double first_end, double second_end, 
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightFeed
(int *handle, double DesiredFeedRate_in_per_sec, double x, double y, double z, double a, double b, double c, int sequence_number, int ID);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightFeedAccel
(int *handle, double DesiredFeedRate_in_per_sec, double DesiredAccel, double x, double y, double z, double a, double b, double c, int sequence_number, int ID);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ReadAndSyncCurPositions
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ReadCurAbsPosition
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c, bool snap=false);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetStraightTraverseCallback(int *handle, netSTRAIGHT_TRAVERSE_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetStraightFeedCallback(int *handle, netSTRAIGHT_FEED_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetArcFeedCallback(int *handle, netARC_FEED_CALLBACK *p);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DownloadDoneSegments(int *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_OutputSegment(int *handle, int iseg);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoSpecialCommand(int *handle, int iseg);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoRateAdjustments(int *handle, int i0, int i1);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_GetAxisDefinitions(int *handle, int *x, int *y, int *z, int *a, int *b, int *c);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetTPParams(int *handle);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Dwell(int *handle, double seconds, int sequence_number);


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DownloadedSegmentCount(int *handle);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_TotalDownloadedTime(int *handle);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_TimeAlreadyExecuted(int *handle);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_RemainingTime(int *handle);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Simulate(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Simulate(int *handle, bool value);
 

//RS274 Interpreter Exports
//Callbacks
typedef void __stdcall netG_COMPLETE_CALLBACK(int status, int lineno, int sequence_number, const char *err);
typedef void __stdcall netG_STATUS_CALLBACK(int line_no, const char *msg);
typedef int __stdcall netG_USER_CALLBACK(const char *msg);
typedef int __stdcall netG_USER_MCODE_CALLBACK(int mCode);
//Ctor..Dtor
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_New(int *CoordMotionhandle, int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Free(int *handle);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_InitializeInterpreter(int *handle);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_SetOrigin(int *handle, int index, double x, double y, double z, double a, double b, double c);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetOrigin(int *handle, int index, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_M_CodeAction(int *handle, int index, int type, double val1, double val2,double val3,double val4,double val5, char *string);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_M_CodeAction(int *handle, int index, int *type, double *val1, double *val2,double *val3,double *val4,double *val5, char **string);


//Setup Parameters
//Acutator Origins, Positions and Offsets

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetInitializeOnExecute(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetInitializeOnExecute(int *handle, bool value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetReadToolFile(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetReadToolFile(int *handle, bool value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Axis_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Axis_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Current_Position(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Current_Position(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Origin_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Origin_Offset(int *handle, double value);
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Axis_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Axis_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Current_Position(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Current_Position(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Origin_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Origin_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Axis_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Axis_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Current_Position(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Current_Position(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Origin_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Origin_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Axis_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Axis_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Current_Position(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Current_Position(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Origin_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Origin_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Axis_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Axis_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Current_Position(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Current_Position(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Origin_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Origin_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Axis_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Axis_Offset(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Current_Position(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Current_Position(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Origin_Offset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Origin_Offset(int *handle, double value);
 

	//Modals
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ControlMode(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ControlMode(int *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LengthUnits(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_LengthUnits(int *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_OriginIndex(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_OriginIndex(int *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_WorkPlane(int *handle); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_WorkPlane(int *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_PercentFlag(int *handle); 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProbeComplete(int *handle); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProbeComplete(int *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProgramX(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProgramX(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProgramY(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProgramY(int *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SequenceNumber(int *handle); 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentLine(int *handle); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentLine(int *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_TraverseSpeed(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_TraverseSpeed(int *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LineLength(int *handle); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FileName(int *handle, char *file);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LineText(int *handle, char *line);


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentMotionMode(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentMotionMode(int *handle, int value);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_GCode(int *handle, int index);  
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_MCode(int *handle, int index);  
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_Setting(int *handle, int index);  
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_DistanceMode(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_DistanceMode(int *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FeedMode(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FeedMode(int *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FloodCoolantOn(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FloodCoolantOn(int *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_MistCoolantOn(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_MistCoolantOn(int *handle, int value);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_BlockDelete(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_BlockDelete(int *handle, int value);



	//Tooling Params 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentToolSlot(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentToolSlot(int *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SelectedToolSlot(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SelectedToolSlot(int *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffset(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffset(int *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffsetIndex(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffsetIndex(int *handle, int value);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusComp(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusComp(int *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompValue(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompValue(int *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompSide(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompSide(int *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_AllowSpindleSpeedOverride(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_AllowSpindleSpeedOverride(int *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SpindleSpeed(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SpindleSpeed(int *handle, double value);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SpindleDirection(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SpindleDirection(int *handle, int value);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetTool(int *handle, int id, double length, double diameter); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_GetTool(int *handle, int id, double *length, double *diameter);

	//Fixed Cycle Params
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_CC(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_CC(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_I(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_I(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_J(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_J(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_K(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_K(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_L(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_L(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_P(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_P(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Q(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Q(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_R(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_R(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Z(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Z(int *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_RetractMode(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_RetractMode(int *handle, int value);



//Resume Parameters
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CanResume(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CanResume(int *handle, bool value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeZ(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeZ(int *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeRelAbs(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeRelAbs(int *handle, int value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeMoveToSafeZ(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeMoveToSafeZ(int *handle, bool value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseXY(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseXY(int *handle, bool value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeX(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeX(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeY(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeY(int *handle, double value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeStartSpindle(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeStartSpindle(int *handle, bool value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeSpindleCWCCW(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeSpindleCWCCW(int *handle, int value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeDoSafeFeedZ(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeDoSafeFeedZ(int *handle, bool value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeFeedSafeZ(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeFeedSafeZ(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeResumeFeedRate(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeResumeFeedRate(int *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeZFeedRate(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeZFeedRate(int *handle, double value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeRestoreFeedRate(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeRestoreFeedRate(int *handle, bool value);

//Functions
 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_COMPLETE_CALLBACK(int *handle, netG_COMPLETE_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_STATUS_CALLBACK(int *handle, netG_STATUS_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_CALLBACK(int *handle, netG_USER_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_MCODE_CALLBACK(int *handle, netG_USER_MCODE_CALLBACK *p);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetStraightTraverseCallback(int *handle, netSTRAIGHT_TRAVERSE_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetStraightFeedCallback(int *handle, netSTRAIGHT_FEED_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetArcFeedCallback(int *handle, netARC_FEED_CALLBACK *p);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ChangeFixtureNumber(int *handle, int fixture);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesToUserUnits(int *handle, double inches);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesOrDegToUserUnitsA(int *handle, double input);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesOrDegToUserUnitsB(int *handle, double input);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesOrDegToUserUnitsC(int *handle, double input);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInches(int *handle, double units);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInchesOrDegA(int *handle, double input);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInchesOrDegB(int *handle, double input);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInchesOrDegC(int *handle, double input);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsX(int *handle, double x);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsY(int *handle, double y);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsZ(int *handle, double z);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsA(int *handle, double a);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsB(int *handle, double b);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsC(int *handle, double c);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_InvokeAction(int *handle, int i, BOOL FlushBeforeUnbufferedOperation=TRUE);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Interpret(int *handle,
				  int board_type,
		          const char *fname,
			      int start, int end,
				  int restart);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ExecutePC(int *handle, const char *Name);
  
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_DoExecute(int *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_DoExecuteComplete(int *handle);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetHalt(int *handle);
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetHalt(int *handle);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetAbort(int *handle);
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetAbort(int *handle);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadAndSyncCurPositions
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadCurInterpreterPosition
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadCurMachinePosition
(int *handle, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToMachine
(int *handle, double x, double y, double z, double a, double b, double c, 
			  double *xp, double *yp, double *zp, double *ap, double *bp, double *cp);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToInterpreterCoord
(int *handle, double x, double y, double z, double a, double b, double c, 
			  double *xp, double *yp, double *zp, double *ap, double *bp, double *cp);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_Get_Simulate(int *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_Set_Simulate(int *handle, bool value);
 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetTPParams(int *handle);



extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetToolFile(void *handle, char *file); 
extern "C" __declspec(dllexport) 	char*   __stdcall KM_dotnet_Interop_GCodeInterpreter_GetToolFile(void *handle);


extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetSetupFile(void *handle, char *file);
extern "C" __declspec(dllexport) 	char*   __stdcall KM_dotnet_Interop_GCodeInterpreter_GetSetupFile(void *handle);

extern "C" __declspec(dllexport)    void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetVarsFile(void *handle, char *file);
extern "C" __declspec(dllexport)    char*   __stdcall KM_dotnet_Interop_GCodeInterpreter_GetVarsFile(void *handle);

class CKMotion_dotNet_InteropApp : public CWinApp
{
public:
	CKMotion_dotNet_InteropApp(); 
	
// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
