// Kinematics5AxisTableAC.h: interface for the CKinematics5AxisTableAC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KINEMATICS5AXISTABLEAC_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
#define AFX_KINEMATICS5AXISTABLEAC_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

class CKinematics5AxisTableAC : public CKinematics
{
public:
    CKinematics5AxisTableAC();
    virtual ~CKinematics5AxisTableAC();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);
	int InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo = false);

	void Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
								double *xp,double *yp,double *zp);

        double CTableAZeroZZero;
        double ASaddleYCenterpoint;
        double ASaddleZCenterpoint;
        double CTableAZeroXCenterpoint;
        double CTableAZeroYCenterpoint;
};

#endif // !defined(AFX_KINEMATICS5AXISTABLEAC_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
