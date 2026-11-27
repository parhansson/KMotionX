// Kinematics5AxisTableAGimbalB.cpp: implementation of the CKinematics5AxisTableAGimbalB class.
//
// Kienematics for Table with A Axis and Tool Gimble head with B Axis 
//
// A axis always rotates about X axis
// B axis always rotates about Y axis
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Kinematics5AxisTableAGimbalB.h"

#define sqr(x) ((x)*(x))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematics5AxisTableAGimbalB::CKinematics5AxisTableAGimbalB()
{
	m_InvGuess[2] = 5.0;   // cold guess for the base iterative inverse
	PivotToChuckLength = 6.0; // distance from Gimbal point to chuck ie 0 length tool

	m_MotionParams.MaxLinearLength = 0.05;  // limit the segment lengths for nonlinear systems
	m_MotionParams.MaxAngularChange = 0.5;  // limit the segment angle change for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1.0;       // limit the increase in Rapid HW FRO
	m_MotionParams.UseOnlyLinearSegments=true;
	m_MotionParams.DoRapidsAsFeeds=true;
}

CKinematics5AxisTableAGimbalB::~CKinematics5AxisTableAGimbalB()
{

}


int CKinematics5AxisTableAGimbalB::Initialize()
{
	// Check if parameters exist in Kinematics.txt, if so update them
	GetParameter(L"PivotToChuckLength", &PivotToChuckLength);
	return 0;
}



// Rotate a point x,y,z about a rotation point xc,yc,zc 
// angle a first (about X Axis) then angle b (about Y axis) then angle c (about Z Axis) units of degrees

void CKinematics5AxisTableAGimbalB::Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
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

// Kienematics for Table with A Axis and Tool Gimble head with B Axis 
//
// A axis always rotates about X axis
// B axis always rotates about Y axis

int CKinematics5AxisTableAGimbalB::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo)
{
	double Xt, Yt, Zt, Xa, Ya, Za, ToolXR, ToolZR, ChuckXR, ChuckZR, Dummy;


	if (m_MotionParams.TCP_Active)
	{
		//Find CAD Tool Tip Location from coordinate passed in (Xt, Yt, Zt)
		Xt = x - m_MotionParams.TCP_X;  // remove tool offset
		Yt = y - m_MotionParams.TCP_Y;
		Zt = z - m_MotionParams.TCP_Z;

		//Perform A rotation to obtain (Xa, Ya, Za)
		Rotate3(0, 0, 0, Xt, Yt, Zt, a, 0, 0, &Xa, &Ya, &Za);	

		//Determine ChuckXR ChuckZR by rotating PivotChuckLength by Angle B 
		Rotate3(0, 0, 0, 0, 0, PivotToChuckLength, 0, -b, 0, &ChuckXR, &Dummy, &ChuckZR);

		//Determine Rotated Tool offset ToolXR ToolZR by rotating ToolX ToolZ by Angle B
		Rotate3(0, 0, 0, m_MotionParams.TCP_X, m_MotionParams.TCP_Y, m_MotionParams.TCP_Z, 0, -b, 0, &ToolXR, &Dummy, &ToolZR);

		x = Xa + ToolXR + ChuckXR;
		y = Ya;
		z = Za + ToolZR + ChuckZR - PivotToChuckLength;
	}

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

int CKinematics5AxisTableAGimbalB::TransformActuatorstoCAD(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	return InvertTransformCADtoActuators(Acts, xr, yr, zr, ar, br, cr, NoGeo);
}

