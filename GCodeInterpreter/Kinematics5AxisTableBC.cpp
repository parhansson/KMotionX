// Kinematics5AxisTableBC.cpp: implementation of the CKinematics5AxisTableBC class.
//
// Kienematics for 2 axis table - B and C axis
//
// B axis rotates parallel to Y
// C axis rotates parallel to Z axis when B axis=0
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Kinematics5AxisTableBC.h"

#define sqr(x) ((x)*(x))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematics5AxisTableBC::CKinematics5AxisTableBC()
{
	m_InvGuess[2] = 5.0;   // cold guess for the base iterative inverse
    CTableBZeroZZero = 0; //Machine Z coordinate of C table face when B=0
    CTableBZeroXCenterpoint = 0; //Machine X coordinate of C rotation axis when B=0
    CTableBZeroYCenterpoint = 0; //Machine Y coordinate of C rotation axis when B=0
    BSaddleXCenterpoint = 0.123; //Machine X coordinate of B rotation axis
                                    //Set B to 90,-90 and measure the centerpoint and give result in machine coordinates
                                    //Ideally this is equal to CTableBZeroXCenterpoint, but reality is rarely so nice
    BSaddleZCenterpoint = 4.000; //Machine Z coordinate of B rotation axis
                                    //Use CTableBZeroZZero and the measurements from BSaddleXCenterpoint (compensated for
                                    //tool centerpoint) to define an arc to find the center of rotation in machine coordinate Z

	m_MotionParams.MaxLinearLength = 0.05;  // limit the segment lengths for nonlinear systems
	m_MotionParams.MaxAngularChange = 0.5;  // limit the segment angle change for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1.0;       // limit the increase in Rapid HW FRO
	m_MotionParams.UseOnlyLinearSegments=true;
	m_MotionParams.DoRapidsAsFeeds=true;
}

CKinematics5AxisTableBC::~CKinematics5AxisTableBC()
{

}


// Rotate a point x,y,z about a rotation point xc,yc,zc 
// angle a first (about X Axis) then angle b (about Y axis) then angle c (about Z Axis) units of degrees

void CKinematics5AxisTableBC::Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
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

// Kienematics for for 2 axis table - B and C axis
//
// B axis rotates parallel to Y
// C axis rotates parallel to Z axis when B axis=0


int CKinematics5AxisTableBC::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo)
{
    double CP_Rotated_C_x,CP_Rotated_C_y,CP_Rotated_C_z;
    double CP_Rotated_BC_x,CP_Rotated_BC_y,CP_Rotated_BC_z;

    // Determine where the commanded XYZ point will be after C rotation
    Rotate3(CTableBZeroXCenterpoint, CTableBZeroYCenterpoint, CTableBZeroZZero, x, y, z, 0, 0, c, &CP_Rotated_C_x, &CP_Rotated_C_y, &CP_Rotated_C_z);
    // Determine where the commanded XYZ point will be after B rotation
    Rotate3(BSaddleXCenterpoint, CTableBZeroYCenterpoint, BSaddleZCenterpoint, CP_Rotated_C_x, CP_Rotated_C_y, CP_Rotated_C_z, 0, b, 0, &CP_Rotated_BC_x, &CP_Rotated_BC_y, &CP_Rotated_BC_z);

    // Translate XYZ target for B rotation
    x = CP_Rotated_BC_x;
    y = CP_Rotated_BC_y;
    z = CP_Rotated_BC_z;

    // Translate from TCP to end effector origin
    //x += CP_Rotated_BC_x - b_x;
    //y += CP_Rotated_BC_y - b_y;
    //z += CP_Rotated_BC_z - b_z;

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

int CKinematics5AxisTableBC::TransformActuatorstoCAD(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	return InvertTransformCADtoActuators(Acts, xr, yr, zr, ar, br, cr, NoGeo);
}

