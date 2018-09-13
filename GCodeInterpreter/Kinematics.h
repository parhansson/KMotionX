// Kinematics.h: interface for the CKinematics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_)
#define AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_

#ifndef _KMOTIONX
#define GCODEINTERPRETER_DLL
#endif


// Now we use the generic helper definitions above to define GCODEINTERPRETER_API and GCODEINTERPRETER_LOCAL.
// GCODEINTERPRETER_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// GCODEINTERPRETER_LOCAL is used for non-api symbols.

#ifdef GCODEINTERPRETER_DLL // defined if KMOTIONDLL is compiled as a DLL
  #ifdef GCODEINTERPRETER_EXPORTS // defined if we are building the KMOTIONDLL DLL (instead of using it)
    #define GCODEINTERPRETER_API DLL_HELPER_API_EXPORT
  #else
    #define GCODEINTERPRETER_API DLL_HELPER_API_IMPORT
  #endif // GCODEINTERPRETER_DLL_EXPORTS
  #define GCODEINTERPRETER_LOCAL DLL_HELPER_API_LOCAL
#else // GCODEINTERPRETER_DLL is not defined: this means KMOTIONDLL is a static lib.
  #define GCODEINTERPRETER_API
  #define GCODEINTERPRETER_LOCAL
#endif // GCODEINTERPRETER_DLL


#include "PT2D.h"
#include "PT3D.h"



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ACTUATORS 8




class GCODEINTERPRETER_API CKinematics  
{
public:
	int Solve(double *A, int N);
	int MaxAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel);
	int MaxRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate);
	int MaxRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double *rate);
	int MaxRapidRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate);
	int MaxRapidJerkInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *jerk);
	int MaxRapidAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel);
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double u, double v, double *Acts, bool NoGeo = false);
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, double *u, double *v, bool NoGeo = false);
	virtual int ComputeAnglesOption(int is);
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);
	virtual int RemapForNonStandardAxes(double *x, double *y, double *z, double *a, double *b, double *c);

	int IntersectionTwoCircles(CPT2D c0, double r0, CPT2D c1, double r1, CPT2D *q);

	virtual int ReadGeoTable(const char *name);
	virtual int GeoCorrect(double x, double y, double z, double *cx, double *cy, double *cz);
	virtual int GetSoftLimits(double *xm, double *xp, double *ym, double *yp, double *zm, double *zp,
		double *am, double *ap, double *bm, double *bp, double *cm, double *cp, double *um, double *up, double *vm, double *vp) {
		return 0;
	}
	
	CKinematics();
	virtual ~CKinematics();

	MOTION_PARAMS m_MotionParams;


	bool GeoTableValid;
	CPT3D *GeoTable;
	//C String *Table2;
	int NRows,NCols;
	double GeoSpacingX,GeoSpacingY;
	double GeoOffsetX,GeoOffsetY;  // Machine coordinates of grid point row=0 col=0
};

#endif // !defined(AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_)
