// Kinematics5AxisTableAC.cpp: implementation of the CKinematics5AxisTableAC class.
//
// Kienematics for 2 axis table - A and C axis
//
// A axis rotates parallel to X
// C axis rotates parallel to Z axis when A axis=0
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Kinematics5AxisTableAC.h"

#define sqr(x) ((x)*(x))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematics5AxisTableAC::CKinematics5AxisTableAC()
{
	m_InvGuess[2] = 5.0;   // cold guess for the base iterative inverse
    CTableAZeroZZero = 0; //Machine Z coordinate of C table face when A=0
    CTableAZeroXCenterpoint = 0; //Machine X coordinate of C rotation axis when A=0
    CTableAZeroYCenterpoint = 0; //Machine Y coordinate of C rotation axis when A=0
    ASaddleYCenterpoint = 0; //Machine Y coordinate of A rotation axis
                                    //Set A to 90,-90 and measure the centerpoint and give result in machine coordinates
                                    //Ideally this is equal to CTableAZeroYCenterpoint, but reality is rarely so nice
    ASaddleZCenterpoint = 1.023622047; //Machine Z coordinate of A rotation axis
                                    //Use CTableAZeroZZero and the measurements from ASaddleXCenterpoint (compensated for
                                    //tool centerpoint) to define an arc to find the center of rotation in machine coordinate Z

	m_MotionParams.MaxLinearLength = 0.05;  // limit the segment lengths for nonlinear systems
	m_MotionParams.MaxAngularChange = 0.5;  // limit the segment angle change for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1.0;       // limit the increase in Rapid HW FRO
	m_MotionParams.UseOnlyLinearSegments=true;
	m_MotionParams.DoRapidsAsFeeds=true;
}

CKinematics5AxisTableAC::~CKinematics5AxisTableAC()
{

}


// Rotate a point x,y,z about a rotation point xc,yc,zc 
// angle a first (about X Axis) then angle b (about Y axis) then angle c (about Z Axis) units of degrees

void CKinematics5AxisTableAC::Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
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

// Kienematics for for 2 axis table - A and C axis
//
// A axis rotates parallel to X
// C axis rotates parallel to Z axis when A axis=0


int CKinematics5AxisTableAC::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo)
{
    double CP_Rotated_C_x,CP_Rotated_C_y,CP_Rotated_C_z;
    double CP_Rotated_AC_x,CP_Rotated_AC_y,CP_Rotated_AC_z;

	if (m_MotionParams.TCP_Active)
	{
		x -= m_MotionParams.TCP_X;
		y -= m_MotionParams.TCP_Y;
		z -= m_MotionParams.TCP_Z;

		// Determine where the commanded XYZ point will be after C rotation
		Rotate3(CTableAZeroXCenterpoint, CTableAZeroYCenterpoint, CTableAZeroZZero, x, y, z, 0, 0, -c, &CP_Rotated_C_x, &CP_Rotated_C_y, &CP_Rotated_C_z);
		// Determine where the commanded XYZ point will be after A rotation
		Rotate3(CTableAZeroXCenterpoint, ASaddleYCenterpoint, ASaddleZCenterpoint, CP_Rotated_C_x, CP_Rotated_C_y, CP_Rotated_C_z, -a, 0, 0, &CP_Rotated_AC_x, &CP_Rotated_AC_y, &CP_Rotated_AC_z);

		// Translate XYZ target for AC rotation
		x = CP_Rotated_AC_x + m_MotionParams.TCP_X;
		y = CP_Rotated_AC_y + m_MotionParams.TCP_Y;
		z = CP_Rotated_AC_z + m_MotionParams.TCP_Z;
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

int CKinematics5AxisTableAC::TransformActuatorstoCAD(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	return InvertTransformCADtoActuators(Acts, xr, yr, zr, ar, br, cr, NoGeo);
}

