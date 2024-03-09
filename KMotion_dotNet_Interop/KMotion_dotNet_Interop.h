// KMotion_dotNet_Interop.h : main header file for the KMotion_dotNet_Interop DLL
//

#pragma once

#ifndef __PureC__

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif
  
#include "resource.h"		// main symbols

#include <iostream> 
#include <fstream> 

#endif

#include "PC-DSP.h"

#if _WIN64
#define HANDLE64 __int64
#else
#define HANDLE64 int
#endif

//Standard Kmotion Members

typedef int __stdcall netCONSOLE_HANDLER(const char* buf);
typedef int __stdcall netCONSOLE_HANDLER_WIDE(const wchar_t* buf);
typedef void __stdcall netERRMSG_HANDLER(const char *ErrMsg);
typedef void __stdcall netERRMSG_HANDLER_WIDE(const wchar_t *ErrMsg);

extern "C" __declspec(dllexport)	int  __stdcall KM_dotnet_Interop_New(HANDLE64 *handle, int board);
extern "C" __declspec(dllexport)	int  __stdcall KM_dotnet_Interop_Free(HANDLE64 *handle);
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
extern "C" __declspec(dllexport) 	int  __stdcall KM_dotnet_Interop_Compile(void *handle,const int boardtype, int Thread, const char *Name, char **Err, int MaxErrLen);
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



extern "C" __declspec(dllexport) 	bool  __stdcall KM_dotnet_Interop_MainStatus_GetStatus(void* handle, bool lock, MAIN_STATUS * m);


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
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_BreakAngle(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_BreakAngle(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CollinearTol(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CollinearTol(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CornerTol(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CornerTol(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_FacetAngle(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_FacetAngle(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_TPLookahead(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_TPLookahead(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusA(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusA(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusB(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusB(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_RadiusC(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_RadiusC(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelX(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelX(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelY(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelY(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelZ(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelZ(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelA(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelA(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelB(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelB(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxAccelC(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxAccelC(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelX(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelX(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelY(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelY(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelZ(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelZ(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelA(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelA(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelB(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelB(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxVelC(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxVelC(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchX(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchX(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchY(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchY(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchZ(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchZ(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchA(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchA(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchB(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchB(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_CountsPerInchC(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_CountsPerInchC(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxLinearLength(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxLinearLength(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_MaxRapidFRO(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_MaxRapidFRO(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesA(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesA(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesB(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesB(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_DegreesC(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_DegreesC(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Get_TCP_Active(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_MOTION_PARAMS_Set_TCP_Active(HANDLE64 *handle, bool value);
 
//Function Calls
extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_CoordMotion_New(int *KmotionDLLhandle, HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_CoordMotion_Free(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_DownloadInit(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_CheckMotionHalt(HANDLE64 *handle, bool Coord);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ExecutionStop(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_Get_FeedRateOverride(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_Get_FeedRateRapidOverride(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_FeedRateOverride(HANDLE64 *handle, double value);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_FeedRateRapidOverride(HANDLE64 *handle, double value);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_Get_SpidleRateOverride(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_SpindleRateOverride(HANDLE64 *handle, double value);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_AxisDestination(HANDLE64 *handle, int axis, double *value);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Get_AxisPosition(HANDLE64 *handle, int axis, double *value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Abort(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Abort(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Clear_Abort(HANDLE64 *handle);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Halt(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Halt(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Clear_Halt(HANDLE64 *handle);


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_FlushSegments(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_WaitForSegmentsFinished(HANDLE64 *handle, BOOL NoErrorOnDisable = FALSE);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_WaitForMoveXYZABCFinished(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoKMotionCmd(HANDLE64 *handle, const char *s, BOOL FlushBeforeUnbufferedOperation);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoKMotionBufCmd(HANDLE64 *handle, const char *s);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_KinematicsReadGeoTable(HANDLE64 *handle, const char *filename);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_MeasurePointAppendToFile(HANDLE64 *handle, const char *name);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightTraverse
(HANDLE64 *handle, double x, double y, double z, double a, double b, double c, bool NoCallback=false);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ArcFeed
	(HANDLE64 *handle, double DesiredFeedRate_in_per_sec, int plane,
	double first_end, double second_end, 
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ArcFeedAccel
	(HANDLE64 *handle, double DesiredFeedRate_in_per_sec, double DesiredAccel, int plane,
	double first_end, double second_end, 
	double first_axis, double second_axis, int rotation,
	double axis_end_point, double a, double b, double c, int sequence_number, int ID);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightFeed
(HANDLE64 *handle, double DesiredFeedRate_in_per_sec, double x, double y, double z, double a, double b, double c, int sequence_number, int ID);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_StraightFeedAccel
(HANDLE64 *handle, double DesiredFeedRate_in_per_sec, double DesiredAccel, double x, double y, double z, double a, double b, double c, int sequence_number, int ID);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ReadAndSyncCurPositions
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_ReadCurAbsPosition
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c, bool snap=false);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetStraightTraverseCallback(HANDLE64 *handle, netSTRAIGHT_TRAVERSE_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetStraightFeedCallback(HANDLE64 *handle, netSTRAIGHT_FEED_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetArcFeedCallback(HANDLE64 *handle, netARC_FEED_CALLBACK *p);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DownloadDoneSegments(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_OutputSegment(HANDLE64 *handle, int iseg);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoSpecialCommand(HANDLE64 *handle, int iseg);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DoRateAdjustments(HANDLE64 *handle, int i0, int i1);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_GetAxisDefinitions(HANDLE64 *handle, int *x, int *y, int *z, int *a, int *b, int *c);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_SetAxisDefinitions(HANDLE64 *handle, int x, int y, int z, int a, int b, int c);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_SetTPParams(HANDLE64 *handle);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_Dwell(HANDLE64 *handle, double seconds, int sequence_number);


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_CoordMotion_DownloadedSegmentCount(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_TotalDownloadedTime(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_TimeAlreadyExecuted(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_CoordMotion_RemainingTime(HANDLE64 *handle);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_CoordMotion_Get_Simulate(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_CoordMotion_Set_Simulate(HANDLE64 *handle, bool value);
 

//RS274 Interpreter Exports
//Callbacks
typedef void __stdcall netG_COMPLETE_CALLBACK(int status, int lineno, int sequence_number, const char *err);
typedef void __stdcall netG_STATUS_CALLBACK(int line_no, const char *msg);
typedef int __stdcall netG_USER_CALLBACK(const char *msg);
typedef int __stdcall netG_USER_MCODE_CALLBACK(int mCode);
//Ctor..Dtor
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_New(int *CoordMotionhandle, HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Free(HANDLE64 *handle);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_InitializeInterpreter(HANDLE64 *handle);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_SetOrigin(HANDLE64 *handle, int index, double x, double y, double z, double a, double b, double c);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetOrigin(HANDLE64 *handle, int index, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_M_CodeAction(HANDLE64 *handle, int index, int type, double val1, double val2,double val3,double val4,double val5, char *string);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_M_CodeAction(HANDLE64 *handle, int index, int *type, double *val1, double *val2,double *val3,double *val4,double *val5, char **string);


//Setup Parameters
//Acutator Origins, Positions and Offsets

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetInterpretThreadID(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetInvokeThreadID(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_GetCurrentThreadID(HANDLE64 *handle);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetInitializeOnExecute(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetInitializeOnExecute(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetReadToolFile(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetReadToolFile(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Axis_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Axis_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Current_Position(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Current_Position(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_A_Origin_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_A_Origin_Offset(HANDLE64 *handle, double value);
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Axis_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Axis_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Current_Position(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Current_Position(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_B_Origin_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_B_Origin_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Axis_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Axis_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Current_Position(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Current_Position(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_C_Origin_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_C_Origin_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Axis_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Axis_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Current_Position(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Current_Position(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_X_Origin_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_X_Origin_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Axis_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Axis_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Current_Position(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Current_Position(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Y_Origin_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Y_Origin_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Axis_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Axis_Offset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Current_Position(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Current_Position(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Z_Origin_Offset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Z_Origin_Offset(HANDLE64 *handle, double value);
 

	//Modals
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ControlMode(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ControlMode(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LengthUnits(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_LengthUnits(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_OriginIndex(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_OriginIndex(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_WorkPlane(HANDLE64 *handle); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_WorkPlane(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_PercentFlag(HANDLE64 *handle); 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProbeComplete(HANDLE64 *handle); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProbeComplete(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProgramX(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProgramX(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ProgramY(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ProgramY(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SequenceNumber(HANDLE64 *handle); 

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentLine(HANDLE64 *handle); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentLine(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_TraverseSpeed(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_TraverseSpeed(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LineLength(HANDLE64 *handle); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FileName(HANDLE64 *handle, char *file);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_LineText(HANDLE64 *handle, char *line);


extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentMotionMode(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentMotionMode(HANDLE64 *handle, int value);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_GCode(HANDLE64 *handle, int index);  
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_MCode(HANDLE64 *handle, int index);  
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Active_Setting(HANDLE64 *handle, int index);  
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_DistanceMode(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_DistanceMode(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FeedMode(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FeedMode(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FloodCoolantOn(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FloodCoolantOn(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_MistCoolantOn(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_MistCoolantOn(HANDLE64 *handle, int value);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_BlockDelete(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_BlockDelete(HANDLE64 *handle, int value);



	//Tooling Params 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CurrentToolSlot(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CurrentToolSlot(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SelectedToolSlot(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SelectedToolSlot(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolXOffset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolXOffset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolYOffset(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolYOffset(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ToolLengthOffsetIndex(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ToolLengthOffsetIndex(HANDLE64 *handle, int value);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusComp(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusComp(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompValue(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompValue(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_RadiusCompSide(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_RadiusCompSide(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_AllowSpindleSpeedOverride(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_AllowSpindleSpeedOverride(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SpindleSpeed(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SpindleSpeed(HANDLE64 *handle, double value);
 
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_FeedRate(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_FeedRate(HANDLE64 *handle, double value);
 
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_SpindleDirection(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_SpindleDirection(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetTool(HANDLE64 *handle, int index, int slot, int id, 
																							  double length, double diameter, double xoffset, double yoffset); 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_GetTool(HANDLE64 *handle, int index, int *slot, int *id, 
																							  double *length, double *diameter, double *xoffset, double *yoffset);

	//Fixed Cycle Params
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_CC(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_CC(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_I(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_I(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_J(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_J(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_K(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_K(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_L(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_L(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_P(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_P(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Q(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Q(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_R(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_R(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_Z(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_Z(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Cycle_RetractMode(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Cycle_RetractMode(HANDLE64 *handle, int value);



//Resume Parameters
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_CanResume(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_CanResume(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeZ(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeZ(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeRelAbs(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeRelAbs(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeMoveToSafeZ(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeMoveToSafeZ(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseXY(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseXY(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeX(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeX(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeTraverseSafeY(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeTraverseSafeY(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeStartSpindle(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeStartSpindle(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeSafeSpindleCWCCW(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeSafeSpindleCWCCW(HANDLE64 *handle, int value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeDoSafeFeedZ(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeDoSafeFeedZ(HANDLE64 *handle, bool value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeFeedSafeZ(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeFeedSafeZ(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeResumeFeedRate(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeResumeFeedRate(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeZFeedRate(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeZFeedRate(HANDLE64 *handle, double value);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_ResumeRestoreFeedRate(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_ResumeRestoreFeedRate(HANDLE64 *handle, bool value);


// GCode Parameter methods 

extern "C" __declspec(dllexport)  double __stdcall KM_dotnet_Interop_GCodeInterpreter_Get_Parameter(HANDLE64 * handle, int index);
extern "C" __declspec(dllexport)  bool __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_Parameter(HANDLE64 * handle, int index, double value);


//Functions
 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_COMPLETE_CALLBACK(HANDLE64 *handle, netG_COMPLETE_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_STATUS_CALLBACK(HANDLE64 *handle, netG_STATUS_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_CALLBACK(HANDLE64 *handle, netG_USER_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_Set_G_USER_MCODE_CALLBACK(HANDLE64 *handle, netG_USER_MCODE_CALLBACK *p);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetStraightTraverseCallback(HANDLE64 *handle, netSTRAIGHT_TRAVERSE_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetStraightFeedCallback(HANDLE64 *handle, netSTRAIGHT_FEED_CALLBACK *p);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetArcFeedCallback(HANDLE64 *handle, netARC_FEED_CALLBACK *p);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ChangeFixtureNumber(HANDLE64 *handle, int fixture);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesToUserUnits(HANDLE64 *handle, double inches);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesOrDegToUserUnitsA(HANDLE64 *handle, double input);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesOrDegToUserUnitsB(HANDLE64 *handle, double input);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_InchesOrDegToUserUnitsC(HANDLE64 *handle, double input);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInches(HANDLE64 *handle, double units);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInchesOrDegA(HANDLE64 *handle, double input);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInchesOrDegB(HANDLE64 *handle, double input);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_UserUnitsToInchesOrDegC(HANDLE64 *handle, double input);

extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsX(HANDLE64 *handle, double x);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsY(HANDLE64 *handle, double y);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsZ(HANDLE64 *handle, double z);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsA(HANDLE64 *handle, double a);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsB(HANDLE64 *handle, double b);
extern "C" __declspec(dllexport) 	double __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsToUserUnitsC(HANDLE64 *handle, double c);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_InvokeAction(HANDLE64 *handle, int i, BOOL FlushBeforeUnbufferedOperation=TRUE);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_Interpret(HANDLE64 *handle,
				  int board_type,
		          const char *fname,
			      int start, int end,
				  int restart);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ExecutePC(HANDLE64 *handle, const char *Name);
  
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_DoExecute(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_DoExecuteComplete(HANDLE64 *handle);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetHalt(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetHalt(HANDLE64 *handle);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_SetAbort(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_GetAbort(HANDLE64 *handle);

extern "C" __declspec(dllexport) 	int __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadAndSyncCurPositions
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadCurInterpreterPosition
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ReadCurMachinePosition
(HANDLE64 *handle, double *x, double *y, double *z, double *a, double *b, double *c);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToMachine
(HANDLE64 *handle, double x, double y, double z, double a, double b, double c, 
			  double *xp, double *yp, double *zp, double *ap, double *bp, double *cp);

extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_ConvertAbsoluteToInterpreterCoord
(HANDLE64 *handle, double x, double y, double z, double a, double b, double c, 
			  double *xp, double *yp, double *zp, double *ap, double *bp, double *cp);

extern "C" __declspec(dllexport) 	bool __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_Get_Simulate(HANDLE64 *handle);
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_Set_Simulate(HANDLE64 *handle, bool value);
 
extern "C" __declspec(dllexport) 	void __stdcall KM_dotnet_Interop_GCodeInterpreter_CoordMotion_SetTPParams(HANDLE64 *handle);



extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetToolFile(void *handle, char *file); 
extern "C" __declspec(dllexport) 	char*   __stdcall KM_dotnet_Interop_GCodeInterpreter_GetToolFile(void *handle);


extern "C" __declspec(dllexport) 	void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetSetupFile(void *handle, char *file);
extern "C" __declspec(dllexport) 	char*   __stdcall KM_dotnet_Interop_GCodeInterpreter_GetSetupFile(void *handle);

extern "C" __declspec(dllexport)    void  __stdcall KM_dotnet_Interop_GCodeInterpreter_SetVarsFile(void *handle, char *file);
extern "C" __declspec(dllexport)    char*   __stdcall KM_dotnet_Interop_GCodeInterpreter_GetVarsFile(void *handle);

#ifndef __PureC__

class CKMotion_dotNet_InteropApp : public CWinApp
{
public:
	CKMotion_dotNet_InteropApp(); 
	
// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

#endif