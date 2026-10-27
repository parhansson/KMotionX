// Kinematics5AxisGimbalAB.cpp: implementation of the CKinematics5AxisGimbalAB class.
//
// Kienematics for Gimble head with A B Axis (should really be called A C but client CAD software used A B for some reason)
//
// A axis rotates about X axis when B axis=0
// B axis rotates about Z
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Kinematics5AxisGimbalAB.h"

#define sqr(x) ((x)*(x))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematics5AxisGimbalAB::CKinematics5AxisGimbalAB()
{
	m_InvGuess[2] = 5.0;   // cold guess for the base iterative inverse
	PivotToChuckLength = 6.0; // distance from Gimbal point to chuck ie 0 length tool

	m_MotionParams.MaxLinearLength = 0.05;  // limit the segment lengths for nonlinear systems
	m_MotionParams.MaxAngularChange = 0.5;  // limit the segment angle change for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1.0;       // limit the increase in Rapid HW FRO
	m_MotionParams.UseOnlyLinearSegments=true;
	m_MotionParams.DoRapidsAsFeeds=true;
}

CKinematics5AxisGimbalAB::~CKinematics5AxisGimbalAB()
{

}

// map AB as AC (for some reason customer prefers it)
int CKinematics5AxisGimbalAB::RemapForNonStandardAxes(double *x, double *y, double *z, double *a, double *b, double *c)
{
	*c = *b;
	*b = 0.0;

	return 0;
}



// Rotate a point x,y,z about a rotation point xc,yc,zc 
// angle a first (about X Axis) then angle b (about Y axis) then angle c (about Z Axis) units of degrees

void CKinematics5AxisGimbalAB::Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
								double *xp,double *yp,double *zp)
{
	double ar=a*PI/180.0;
	double br=b*PI/180.0;
	double cr=c*PI/180.0;
	
	// first rotate about x axis a degrees (changes only y and z)
	double xa = x;
	double ya = yc + (y-yc)*cos(ar) - (z-zc)*sin(ar);
	double za = zc + (y-yc)*sin(ar) + (z-zc)*cos(ar);

	// rotate about y axis b degrees(changes only x and z)
	double xb = xc + (xa-xc)*cos(br) - (za-zc)*sin(br);
	double yb = ya;
	double zb = zc + (xa-xc)*sin(br) + (za-zc)*cos(br);

	// rotate about z axis c degrees(changes only x and y)
	*xp = xc + (xb-xc)*cos(cr) - (yb-yc)*sin(cr);
	*yp = yc + (xb-xc)*sin(cr) + (yb-yc)*cos(cr);
	*zp = zb;
}

// Kienematics for Gimble head with A B Axis (should really be called A C but client CAD software used A B for some reason)
//
// A axis rotates about X axis when B axis=0
// B axis rotates about Z


int CKinematics5AxisGimbalAB::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo)
{
	double TCP_Rotated_x,TCP_Rotated_y,TCP_Rotated_z;
	// find lengths of each actuator

	// Determine where the TCP rotated point would be relative to origin when oriented at the desired angles.
	Rotate3(0, 0, 0, m_MotionParams.TCP_X, m_MotionParams.TCP_Y, (m_MotionParams.TCP_Z + PivotToChuckLength), a, 0, b, &TCP_Rotated_x, &TCP_Rotated_y, &TCP_Rotated_z);

	// Translate from TCP to end effector origin
	x += TCP_Rotated_x - m_MotionParams.TCP_X;
	y += TCP_Rotated_y - m_MotionParams.TCP_Y;
	z += TCP_Rotated_z - (m_MotionParams.TCP_Z + PivotToChuckLength);

	if (!NoGeo) GeoCorrect(x,y,z,&x,&y, &z);

	Acts[0] = x*m_MotionParams.CountsPerInchX;
	Acts[1] = y*m_MotionParams.CountsPerInchY;
	Acts[2] = z*m_MotionParams.CountsPerInchZ;
	Acts[3] = a*m_MotionParams.CountsPerInchA;
	Acts[4] = b*m_MotionParams.CountsPerInchB;
	Acts[5] = c*m_MotionParams.CountsPerInchC;

	return 0;
}





// perform Inversion to go the other way

int CKinematics5AxisGimbalAB::TransformActuatorstoCAD(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	return InvertTransformCADtoActuators(Acts, xr, yr, zr, ar, br, cr, NoGeo);
}

