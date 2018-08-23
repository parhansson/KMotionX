// KinematicsGeppettoExtrude.h: interface for the CKinematicsGeppetto class with Extruder Axis.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KinematicsGeppettoExtrude_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
#define AFX_KinematicsGeppettoExtrude_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

class CKinematicsGeppettoExtrude : public CKinematics  
{
public:
	CPT3D Act0Center;
	CPT3D Act1Center;
	CPT3D Act2Center;
	CPT3D Act3Center;
	CPT3D Act4Center;
	CPT3D Act5Center;
	double Act0Off;
	double Act1Off;
	double Act2Off;
	double Act3Off;
	double Act4Off;
	double Act5Off;
	CKinematicsGeppettoExtrude();
	virtual ~CKinematicsGeppettoExtrude();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);
	virtual int ReadGeoTable(const char *name);
	int GeoCorrect(double x, double y, double z, double a, double b, double c,
		double *rx, double *ry, double *rz, double *ra, double *rb, double *rc);

	void Rotate(double xc,double yc,double zc,double z,double a,double b,double *xp,double *yp,double *zp);
	void Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
								double *xp,double *yp,double *zp);

	virtual int GetSoftLimits(double *xm, double *xp, double *ym, double *yp, double *zm, double *zp,
		double *am, double *ap, double *bm, double *bp, double *cm, double *cp);

};

#endif // !defined(AFX_KinematicsGeppettoExtrude_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
