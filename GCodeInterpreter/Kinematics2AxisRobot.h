// Kinematics2AxisRobot.h: interface for the Kinematics2AxisRobot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Kinematics2AxisRobot_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
#define AFX_Kinematics2AxisRobot_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

class CKinematics2AxisRobot : public CKinematics  
{
public:

	//Linkage Definitions
	double L, R1, S, R2, R3, L1, L2;

	CKinematics2AxisRobot();
	virtual ~CKinematics2AxisRobot();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double* Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double* Acts, double* x, double* y, double* z, double* a, double* b, double* c, bool NoGeo = false);
};

#endif // !defined(AFX_Kinematics2AxisRobot_H__876A0A72_6EC3_48D0_9040_60AE3DA2F3C7__INCLUDED_)
