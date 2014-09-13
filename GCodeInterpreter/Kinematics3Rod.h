// Kinematics3Rod.h: interface for the CKinematics3Rod class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KINEMATICS3ROD_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
#define AFX_KINEMATICS3ROD_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class CKinematics3Rod : public CKinematics  
{
public:
	CPT3D Act0Center;
	CPT3D Act1Center;
	CPT3D Act2Center;
	double Act0Off;
	double Act1Off;
	double Act2Off;
	CKinematics3Rod();
	virtual ~CKinematics3Rod();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c);
};

#endif // !defined(AFX_KINEMATICS3ROD_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
