// Kinematics5AxisTableAB.cpp: implementation of the CKinematics5AxisTableAB class.
// thanks to Tom & Daniele
// Kienematics for 2 axis table - A and B axis
//
// A axis rotates parallel to X
// B axis rotates parallel to Y axis when A axis=0
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Kinematics5AxisTableAB.h"

#define sqr(x) ((x)*(x))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematics5AxisTableAB::CKinematics5AxisTableAB()
{
    BTableAZeroZZero = 0; //Machine Z coordinate of B table face when A=0
    BTableAZeroXCenterpoint = 0; //Machine X coordinate of B rotation axis when A=0
    BTableAZeroYCenterpoint = 0; //Machine Y coordinate of B rotation axis when A=0
    ASaddleYCenterpoint = 0; //Machine Y coordinate of A rotation axis
                                    //Set A to 90,-90 and measure the centerpoint and give result in machine coordinates
                                    //Ideally this is equal to BTableAZeroYCenterpoint, but reality is rarely so nice
    ASaddleZCenterpoint = 0; //Machine Z coordinate of A rotation axis
                                    //Use BTableAZeroZZero and the measurements from ASaddleXCenterpoint (compensated for
                                    //tool centerpoint) to define an arc to find the center of rotation in machine coordinate Z

	m_MotionParams.MaxLinearLength = 0.05;  // limit the segment lengths for nonlinear systems
//	m_MotionParams.MaxAngularChange = 0.5;  // limit the segment angle change for nonlinear systems
	m_MotionParams.MaxAngularChange = 2.0;  // tktk limit the segment angle change for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1.0;       // limit the increase in Rapid HW FRO
	m_MotionParams.UseOnlyLinearSegments=true;
	m_MotionParams.DoRapidsAsFeeds=true;
}

CKinematics5AxisTableAB::~CKinematics5AxisTableAB()
{

}

int CKinematics5AxisTableAB::Initialize()
{
	// Check if parameters exist in Kinematics.txt, if so update them and ask for confirmation value window yes/no, no abort
	GetParameter("BTableAZeroZZero", &BTableAZeroZZero);
	//CString str1;
	//str1.Format("Value BTableAZeroZZero is: %g \n", BTableAZeroZZero);
	//if (AfxMessageBox(str1, MB_YESNO) == IDNO) return -1;
	GetParameter("BTableAZeroXCenterpoint", &BTableAZeroXCenterpoint);
	//CString str2;
	//str2.Format("Value BTableAZeroXCenterpoint is: %g \n", BTableAZeroXCenterpoint);
	//if (AfxMessageBox(str2, MB_YESNO) == IDNO) return -1;
	GetParameter("BTableAZeroYCenterpoint", &BTableAZeroYCenterpoint);
	//CString str3;
	//str3.Format("Value BTableAZeroYCenterpoint is: %g \n", BTableAZeroYCenterpoint);
	//if (AfxMessageBox(str3, MB_YESNO) == IDNO) return -1;
	GetParameter("ASaddleYCenterpoint", &ASaddleYCenterpoint);
	//CString str4;
	//str4.Format("Value ASaddleYCenterpoint is: %g \n", ASaddleYCenterpoint);
	//if (AfxMessageBox(str4, MB_YESNO) == IDNO) return -1;
	GetParameter("ASaddleZCenterpoint", &ASaddleZCenterpoint);
	//CString str5;
	//str5.Format("Value ASaddleZCenterpoint is: %g \n", ASaddleZCenterpoint);
	//if (AfxMessageBox(str1 + str2 + str3 + str4 + str5, MB_YESNO) == IDNO) return -1;
	return 0;
}


// Rotate a point x,y,z about a rotation point xc,yc,zc 
// angle a first (about X Axis) then angle b (about Y axis) then angle c (about Z Axis) units of degrees

void CKinematics5AxisTableAB::Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
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

// Kinematics for for 2 axis table - A and B axis
//
// A axis rotates parallel to X
// B axis rotates parallel to Y axis when A axis=0


int CKinematics5AxisTableAB::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo)
{
    double BP_Rotated_B_x,BP_Rotated_B_y,BP_Rotated_B_z;
    double BP_Rotated_AB_x,BP_Rotated_AB_y,BP_Rotated_AB_z;

	if (m_MotionParams.TCP_Active)
	{
		x -= m_MotionParams.TCP_X;
		y -= m_MotionParams.TCP_Y;
		z -= m_MotionParams.TCP_Z;

		// Determine where the commanded XYZ point will be after B rotation
		Rotate3(BTableAZeroXCenterpoint, BTableAZeroYCenterpoint, BTableAZeroZZero, x, y, z, 0, b, 0, &BP_Rotated_B_x, &BP_Rotated_B_y, &BP_Rotated_B_z);
		// Determine where the commanded XYZ point will be after A rotation
		Rotate3(BTableAZeroXCenterpoint, ASaddleYCenterpoint, ASaddleZCenterpoint, BP_Rotated_B_x, BP_Rotated_B_y, BP_Rotated_B_z, -a, 0, 0, &BP_Rotated_AB_x, &BP_Rotated_AB_y, &BP_Rotated_AB_z);

		// Translate XYZ target for AB rotation
		x = BP_Rotated_AB_x + m_MotionParams.TCP_X;
		y = BP_Rotated_AB_y + m_MotionParams.TCP_Y;
		z = BP_Rotated_AB_z + m_MotionParams.TCP_Z;
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

int CKinematics5AxisTableAB::InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	double Tol=1e-6;
	double d=0.1;					// should be linear over this range
	double x=0,y=0,z=5,a=0,b=0,c=0; // initial guess at answer
	double Acts0[MAX_ACTUATORS],ActsX[MAX_ACTUATORS],ActsY[MAX_ACTUATORS];
	double ActsZ[MAX_ACTUATORS],ActsA[MAX_ACTUATORS],ActsB[MAX_ACTUATORS],ActsC[MAX_ACTUATORS];
	double A[6*7];

	double ex,ey,ez,ea,eb,ec;

	for (int i=0; i<100; i++)
	{
		// measure sensitivity
		// 
		// assume majority is simply x -> Act0, y -> Act1, etc..

		TransformCADtoActuators(x,  y,  z,  a,  b,  c,  Acts0, NoGeo);
		TransformCADtoActuators(x+d,y,  z,  a,  b,  c,  ActsX, NoGeo);
		TransformCADtoActuators(x,  y+d,z,  a,  b,  c,  ActsY, NoGeo);
		TransformCADtoActuators(x,  y,  z+d,a,  b,  c,  ActsZ, NoGeo);
		TransformCADtoActuators(x,  y,  z  ,a+d,b,  c,  ActsA, NoGeo);
		TransformCADtoActuators(x,  y,  z  ,a,  b+d,c,  ActsB, NoGeo);
		TransformCADtoActuators(x,  y,  z  ,a,  b,  c+d,ActsC, NoGeo);


		//   | x | | RX0  RY0  RZ0 |    | R0 |
		//   | y | | RX1  RY1  RZ1 | =  | R1 |
		//   | z | | RX2  RY2  RZ2 |    | R2 |


		A[0*7+0] = (ActsX[0]-Acts0[0])/d;  // changes due to x
		A[1*7+0] = (ActsX[1]-Acts0[1])/d;
		A[2*7+0] = (ActsX[2]-Acts0[2])/d;
		A[3*7+0] = (ActsX[3]-Acts0[3])/d;
		A[4*7+0] = (ActsX[4]-Acts0[4])/d;
		A[5*7+0] = (ActsX[5]-Acts0[5])/d;

		A[0*7+1] = (ActsY[0]-Acts0[0])/d;  // changes due to y
		A[1*7+1] = (ActsY[1]-Acts0[1])/d;
		A[2*7+1] = (ActsY[2]-Acts0[2])/d;
		A[3*7+1] = (ActsY[3]-Acts0[3])/d;
		A[4*7+1] = (ActsY[4]-Acts0[4])/d;
		A[5*7+1] = (ActsY[5]-Acts0[5])/d;

		A[0*7+2] = (ActsZ[0]-Acts0[0])/d;  // changes due to Z
		A[1*7+2] = (ActsZ[1]-Acts0[1])/d;
		A[2*7+2] = (ActsZ[2]-Acts0[2])/d;
		A[3*7+2] = (ActsZ[3]-Acts0[3])/d;
		A[4*7+2] = (ActsZ[4]-Acts0[4])/d;
		A[5*7+2] = (ActsZ[5]-Acts0[5])/d;

		A[0*7+3] = (ActsA[0]-Acts0[0])/d;  // changes due to A
		A[1*7+3] = (ActsA[1]-Acts0[1])/d;
		A[2*7+3] = (ActsA[2]-Acts0[2])/d;
		A[3*7+3] = (ActsA[3]-Acts0[3])/d;
		A[4*7+3] = (ActsA[4]-Acts0[4])/d;
		A[5*7+3] = (ActsA[5]-Acts0[5])/d;

		A[0*7+4] = (ActsB[0]-Acts0[0])/d;  // changes due to B
		A[1*7+4] = (ActsB[1]-Acts0[1])/d;
		A[2*7+4] = (ActsB[2]-Acts0[2])/d;
		A[3*7+4] = (ActsB[3]-Acts0[3])/d;
		A[4*7+4] = (ActsB[4]-Acts0[4])/d;
		A[5*7+4] = (ActsB[5]-Acts0[5])/d;

		A[0*7+5] = (ActsC[0]-Acts0[0])/d;  // changes due to C
		A[1*7+5] = (ActsC[1]-Acts0[1])/d;
		A[2*7+5] = (ActsC[2]-Acts0[2])/d;
		A[3*7+5] = (ActsC[3]-Acts0[3])/d;
		A[4*7+5] = (ActsC[4]-Acts0[4])/d;
		A[5*7+5] = (ActsC[5]-Acts0[5])/d;

		A[0*7+6] = Acts[0]-Acts0[0];   // desired changes
		A[1*7+6] = Acts[1]-Acts0[1];
		A[2*7+6] = Acts[2]-Acts0[2];
		A[3*7+6] = Acts[3]-Acts0[3];
		A[4*7+6] = Acts[4]-Acts0[4];
		A[5*7+6] = Acts[5]-Acts0[5];

		Solve(A,6);  // solve simultaneous eqs

		// corrections in CAD space

		ex = A[0*7+6];
		ey = A[1*7+6];
		ez = A[2*7+6];
		ea = A[3*7+6];
		eb = A[4*7+6];
		ec = A[5*7+6];

		// Done if all within Tolerance

		if (fabs(ex) < Tol &&
			fabs(ey) < Tol &&
			fabs(ez) < Tol &&
			fabs(ea) < Tol &&
			fabs(eb) < Tol &&
			fabs(ec) < Tol)
		{
			*xr = x;
			*yr = y;
			*zr = z;
			*ar = a;
			*br = b;
			*cr = c;

			return 0;
		}

		// make a correction

		x += ex;
		y += ey;
		z += ez;
		a += ea;
		b += eb;
		c += ec;
	}

	// it never converges, return whatever we have

	*xr = x;
	*yr = y;
	*zr = z;
	*ar = a;
	*br = b;
	*cr = c;

	return 1;
}




// perform Inversion to go the other way

int CKinematics5AxisTableAB::TransformActuatorstoCAD(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	return InvertTransformCADtoActuators(Acts, xr, yr, zr, ar, br, cr, NoGeo);
}

