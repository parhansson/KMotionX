// Kinematics.h: interface for the CKinematics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_)
#define AFX_KINEMATICS_H__F0E3BA96_734F_4D32_85DD_8B2FA813C991__INCLUDED_


#ifdef GCODEINTERPRETER_EXPORTS
#define GCODEINTERPRETER_API __declspec(dllexport)
#else
#define GCODEINTERPRETER_API __declspec(dllimport)
#endif


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
