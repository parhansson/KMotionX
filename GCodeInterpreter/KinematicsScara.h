// KinematicsScara.h: interface for the CKinematicsScara class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KinematicsScara_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
#define AFX_KinematicsScara_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

class CKinematicsScara : public CKinematics  
{
public:
	CKinematicsScara();
	virtual ~CKinematicsScara();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
	double CorrectAngle(double _angle);
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);

	void Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
								double *xp,double *yp,double *zp);

	double PivotToChuckLength;

	double Length_1; // X axis length
	double Length_2; // Y axis length

	double SqrLength_1; //X*X
	double SqrLength_2; //Y*Y

};

#endif // !defined(AFX_KinematicsScara_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
