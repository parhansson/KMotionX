// Kinematics.h: interface for the CKinematics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_)
#define AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_


// Generic helper definitions for shared library support http://gcc.gnu.org/wiki/Visibility
#if defined _WIN32 || defined __CYGWIN__
  #define GCODEINTERPRETER_HELPER_API_IMPORT __declspec(dllimport)
  #define GCODEINTERPRETER_HELPER_API_EXPORT __declspec(dllexport)
  #define GCODEINTERPRETER_HELPER_API_LOCAL
#else
  #if __GNUC__ >= 4
    #define GCODEINTERPRETER_HELPER_API_IMPORT __attribute__ ((visibility ("default")))
    #define GCODEINTERPRETER_HELPER_API_EXPORT __attribute__ ((visibility ("default")))
    #define GCODEINTERPRETER_HELPER_API_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define GCODEINTERPRETER_HELPER_API_IMPORT
    #define GCODEINTERPRETER_HELPER_API_EXPORT
    #define GCODEINTERPRETER_HELPER_API_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define KMOTIONDLL_API and KMOTIONDLL_LOCAL.
// GCODEINTERPRETER_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// GCODEINTERPRETER_LOCAL is used for non-api symbols.

#ifdef GCODEINTERPRETER_DLL // defined if KMOTIONDLL is compiled as a DLL
  #ifdef KMOTIONDLL_DLL_EXPORTS // defined if we are building the KMOTIONDLL DLL (instead of using it)
    #define GCODEINTERPRETER_API GCODEINTERPRETER_HELPER_API_EXPORT
  #else
    #define GCODEINTERPRETER_API GCODEINTERPRETER_HELPER_API_IMPORT
  #endif // GCODEINTERPRETER_DLL_EXPORTS
  #define GCODEINTERPRETER_LOCAL GCODEINTERPRETER_HELPER_API_LOCAL
#else // GCODEINTERPRETER_DLL is not defined: this means KMOTIONDLL is a static lib.
  #define GCODEINTERPRETER_API
  #define GCODEINTERPRETER_LOCAL
#endif // GCODEINTERPRETER_DLL


#include "PT2D.h"
#include "PT3D.h"



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ACTUATORS 6




class GCODEINTERPRETER_API CKinematics  
{
public:
	int Solve(double *A, int N);
	int MaxAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double *accel);
	int MaxRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double *rate);
	int MaxRapidRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double *rate);
	int MaxRapidJerkInDirection(double dx, double dy, double dz, double da, double db, double dc, double *jerk);
	int MaxRapidAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double *accel);
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c);
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr);

	int IntersectionTwoCircles(CPT2D c0, double r0, CPT2D c1, double r1, CPT2D *q);

	int ReadGeoTable(const char *name);
	int GeoCorrect(double x, double y, double z, double *cx, double *cy, double *cz);

	
	CKinematics();
	virtual ~CKinematics();

	MOTION_PARAMS m_MotionParams;


	bool GeoTableValid;
	CPT3D *GeoTable;
	CString *Table2;
	int NRows,NCols;
	double GeoSpacingX,GeoSpacingY;
	double GeoOffsetX,GeoOffsetY;  // Machine coordinates of grid point row=0 col=0
};

#endif // !defined(AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_)
