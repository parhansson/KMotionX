// Kinematics3Link.h: interface for the CKinematics3Link class.
//
// 3 Link parallel arm pen robot driven by 3 rotary servos (Left, Right, Z).
// Left/Right servo arms swing in the horizontal plane forming a five-bar
// linkage that positions the pen tip in X,Y.  The Z servo arm swings in a
// vertical plane and raises/lowers the pen through a ball jointed link.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Kinematics3Link_H__A31F72C4_58D9_4E11_9A02_7C55E1B93F10__INCLUDED_)
#define AFX_Kinematics3Link_H__A31F72C4_58D9_4E11_9A02_7C55E1B93F10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class CKinematics3Link : public CKinematics
{
public:
	CKinematics3Link();
	virtual ~CKinematics3Link();
	virtual int TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo = false);
	virtual int TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo = false);

private:
	double ClampCos(double x);
	double PlanarTwoLink(double dx, double dy, double L1, double L2, double elbow);
	bool PoseInReach(double x, double y, double z, bool NoGeo = false);
};

#endif // !defined(AFX_Kinematics3Link_H__A31F72C4_58D9_4E11_9A02_7C55E1B93F10__INCLUDED_)
