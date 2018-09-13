// KinematicsGeppettoExtrude.cpp: implementation of the CKinematicsGeppettoExtrude class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KinematicsGeppettoExtrude.h"

#define sqr(x) ((x)*(x))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematicsGeppettoExtrude::CKinematicsGeppettoExtrude()
{
#define SpindleHeight 0.75
	Act0Center.x = -26.8;
	Act0Center.y = 13.375;
	Act0Center.z = SpindleHeight;

	Act1Center.x = 25;
	Act1Center.y = 11.5;
	Act1Center.z = SpindleHeight;

	Act2Center.x = 0;
	Act2Center.y = -18.7;
	Act2Center.z = SpindleHeight;

	Act3Center.x = -24.5;
	Act3Center.y = 12.5;
	Act3Center.z = SpindleHeight;

	Act4Center.x = 22.5;
	Act4Center.y = 10.5;
	Act4Center.z = SpindleHeight;

	Act5Center.x = 0;
	Act5Center.y = -16.3;
	Act5Center.z = SpindleHeight;

	// when actuator position is zero this is how long the rod is



#define START_HEIGHT 0.0
#define Z_BOTTOM 3.8
#define Z_TOTAL 12.0

#define X0 (0.3)
#define Y0 (1.4)
#define Z0 (Z_TOTAL)

#define X1 (0.3)
#define Y1 (1.4)
#define Z1 (Z_TOTAL)

#define X2 (0.3)
#define Y2 (1.4)
#define Z2 (Z_TOTAL)

#define X3 (-2.5)
#define Y3 (-1.0)
#define Z3 (4.2)

#define X4 (0.5)
#define Y4 (-0.5)
#define Z4 (Z_BOTTOM)

#define X5 (0.5)
#define Y5 (-0.5)
#define Z5 (Z_BOTTOM)



	Act0Off = sqrt(sqr(Act0Center.x-X0) + sqr(Act0Center.y-Y0) + sqr(Act0Center.z-Z0));  
	Act1Off = sqrt(sqr(Act1Center.x-X1) + sqr(Act1Center.y-Y1) + sqr(Act1Center.z-Z1));
	Act2Off = sqrt(sqr(Act2Center.x-X2) + sqr(Act2Center.y-Y2) + sqr(Act2Center.z-Z2));
	Act3Off = sqrt(sqr(Act3Center.x-X3) + sqr(Act3Center.y-Y3) + sqr(Act3Center.z-Z3));  
	Act4Off = sqrt(sqr(Act4Center.x-X4) + sqr(Act4Center.y-Y4) + sqr(Act4Center.z-Z4));
	Act5Off = sqrt(sqr(Act5Center.x-X5) + sqr(Act5Center.y-Y5) + sqr(Act5Center.z-Z5));

	m_MotionParams.MaxLinearLength = 0.05;  // limit the segment lengths for nonlinear systems
	m_MotionParams.MaxAngularChange = 0.5;  // limit the segment angle change for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1.0;       // limit the increase in Rapid HW FRO
	m_MotionParams.UseOnlyLinearSegments=true;
	m_MotionParams.DoRapidsAsFeeds=true;
}

CKinematicsGeppettoExtrude::~CKinematicsGeppettoExtrude()
{

}

// Rotate a point directly above a rotation point a and b angles(degrees)

void CKinematicsGeppettoExtrude::Rotate(double xc,double yc,double zc,double z,double a,double b,
								double *xp,double *yp,double *zp)
{
	double ar=a*PI/180.0;
	double br=b*PI/180.0;
	
	// first rotate about x axis a degrees (changes only y and z)
	double ya = yc - (z-zc)*sin(ar);
	double za = zc + (z-zc)*cos(ar);

	// rotate about y axis b degrees(changes only x and z)
	double xb = xc + (za-zc)*sin(br);
	double zb = zc + (za-zc)*cos(br);

	*xp = xb;
	*yp = ya;
	*zp = zb;
}

// Rotate a point x,y,z about a rotation point xc,yc,zc 
// angle a first (about X Axis) then angle b (about Y axis) then angle c (about Z Axis) units of degrees

void CKinematicsGeppettoExtrude::Rotate3(double xc,double yc,double zc,double x,double y,double z,double a,double b,double c,
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

int CKinematicsGeppettoExtrude::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo)
{
	double x_0,y_0,z_0,x_1,y_1,z_1,x_2,y_2,z_2,x_3,y_3,z_3,x_4,y_4,z_4,x_5,y_5,z_5, TCP_Rotated_x,TCP_Rotated_y,TCP_Rotated_z;
	// find lengths of each actuator

	// Determine where the TCP rotated point would be relative to origin when oriented at the desired angles.
	Rotate3(0, 0, 0, m_MotionParams.TCP_X, m_MotionParams.TCP_Y, m_MotionParams.TCP_Z, a, b, c, &TCP_Rotated_x, &TCP_Rotated_y, &TCP_Rotated_z);

	// Translate from TCP to end effector origin
	x += TCP_Rotated_x - m_MotionParams.TCP_X;
	y += TCP_Rotated_y - m_MotionParams.TCP_Y;
	z += TCP_Rotated_z - m_MotionParams.TCP_Z;

	if (!NoGeo) GeoCorrect(x,y,z,a,b,c,&x,&y, &z, &a, &b, &c);

	// Find where connection points should be
	Rotate3(x,y,z,x+X0,y+Y0,z+Z0,a,b,c,&x_0,&y_0,&z_0);
	Rotate3(x,y,z,x+X1,y+Y1,z+Z1,a,b,c,&x_1,&y_1,&z_1);
	Rotate3(x,y,z,x+X2,y+Y2,z+Z2,a,b,c,&x_2,&y_2,&z_2);
	Rotate3(x,y,z,x+X3,y+Y3,z+Z3,a,b,c,&x_3,&y_3,&z_3);
	Rotate3(x,y,z,x+X4,y+Y4,z+Z4,a,b,c,&x_4,&y_4,&z_4);
	Rotate3(x,y,z,x+X5,y+Y5,z+Z5,a,b,c,&x_5,&y_5,&z_5);

	double r0 = sqrt(sqr(x_0 - Act0Center.x) + sqr(y_0 - Act0Center.y) + sqr(z_0 - Act0Center.z)) - Act0Off;
	double r1 = sqrt(sqr(x_1 - Act1Center.x) + sqr(y_1 - Act1Center.y) + sqr(z_1 - Act1Center.z)) - Act1Off;
	double r2 = sqrt(sqr(x_2 - Act2Center.x) + sqr(y_2 - Act2Center.y) + sqr(z_2 - Act2Center.z)) - Act2Off;
	double r3 = sqrt(sqr(x_3 - Act3Center.x) + sqr(y_3 - Act3Center.y) + sqr(z_3 - Act3Center.z)) - Act3Off;
	double r4 = sqrt(sqr(x_4 - Act4Center.x) + sqr(y_4 - Act4Center.y) + sqr(z_4 - Act4Center.z)) - Act4Off;
	double r5 = sqrt(sqr(x_5 - Act5Center.x) + sqr(y_5 - Act5Center.y) + sqr(z_5 - Act5Center.z)) - Act5Off;

	Acts[0] = r0*m_MotionParams.CountsPerInchX;
	Acts[1] = r1*m_MotionParams.CountsPerInchY;
	Acts[2] = r2*m_MotionParams.CountsPerInchZ;
	Acts[3] = r3*m_MotionParams.CountsPerInchA;
	Acts[4] = r4*m_MotionParams.CountsPerInchB;
	Acts[5] = r5*m_MotionParams.CountsPerInchC;

	return 0;
}

int CKinematicsGeppettoExtrude::InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
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

int CKinematicsGeppettoExtrude::TransformActuatorstoCAD(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	return InvertTransformCADtoActuators(Acts, xr, yr, zr, ar, br, cr, NoGeo);
}

//
// Bilinear interpolation in 2D based on NxM grid points
//
// Assumes that we are mapping an ideal X,Y CAD space to an Adjusted Space that
// needs to be commanded for the system to actually be at that ideal location.
//
// Each entry in the table consists of the 
//
// The origin in CAD space is the center of the GRID  
//
// File Format for correction Table is
//
// NRows,NCols
// GeoSpacingX, GeoSpacingY
// Row, Col, AdjustedX, AdjustedY
// Row, Col, AdjustedX, AdjustedY
// Row, Col, AdjustedX, AdjustedY
// Row, Col, AdjustedX, AdjustedY
// Row, Col, AdjustedX, AdjustedY
// .
// .
// .
// .

int CKinematicsGeppettoExtrude::ReadGeoTable(const char *name)
{
	double X, Y, Z, c;
	int row, col;

	GeoTableValid = false;

	if (name[0] == 0) return 0; // passing in no file turns off geocorrection

	FILE *f = fopen(name, "rt");
	char message[MAX_PATH + 64];
	if (!f)
	{
		sprintf(message, "Unable to open Geometric Correction File : %s", name);
		AfxMessageBox(message);
		return 1;
	}

	int result = fscanf(f, "%d,%d", &NRows, &NCols);

	if (result != 2 || NRows < 2 || NRows > 4000 || NCols < 2 || NCols > 4000)
	{
		fclose(f);
		sprintf(message, "Invalid Geometric Correction File (NRows and NCols) : %s", name);
		AfxMessageBox(message);
		return 1;
	}

	result = fscanf(f, "%lf,%lf", &GeoSpacingX, &GeoSpacingY);

	if (result != 2)
	{
		fclose(f);
		sprintf(message, "Invalid Geometric Correction File (GeoSpacingX and GeoSpacingY) : %s", name);
		AfxMessageBox(message);
		return 1;
	}

	result = fscanf(f, "%lf,%lf", &GeoOffsetX, &GeoOffsetY);

	if (result != 2)
	{
		fclose(f);
		sprintf(message, "Invalid Geometric Correction File (GeoOffsetX and GeoOffsetY) : %s", name);
		AfxMessageBox(message);
		return 1;
	}

	if (GeoTable) delete[] GeoTable;
	GeoTable = new CPT3D[NRows*NCols];

	for (int i = 0; i<NRows*NCols; i++)
	{
		result = fscanf(f, "%d,%d,%lf,%lf,%lf,%lf", &row, &col, &X, &Y, &Z, &c);

		if (result != 6 || row < 0 || row >= NRows || col < 0 || col >= NCols)
		{
			fclose(f);
			sprintf(message, "Invalid Geometric Correction File (invalid data value): %s", name);
			AfxMessageBox(message);
			return 1;
		}

		GeoTable[row*NCols + col].x = X;
		GeoTable[row*NCols + col].y = Y;
		GeoTable[row*NCols + col].z = Z;
		GeoTable[row*NCols + col].c = c;
	}

	fclose(f);

	GeoTableValid = true;
	return 0;
}


// for now the z just has an offset to make the x,y, plane at z=0 flat


int CKinematicsGeppettoExtrude::GeoCorrect(double x, double y, double z, double a, double b, double c, 
											double *rx, double *ry, double *rz, double *ra, double *rb, double *rc)
{
	if (!GeoTableValid) return 1;

	int col = (int)floor((x - GeoOffsetX) / GeoSpacingX);
	int row = (int)floor((y - GeoOffsetY) / GeoSpacingY);

	// stay within table

	if (col < 0) col = 0;
	if (col >= NCols - 1) col = NCols - 2;
	if (row < 0) row = 0;
	if (row >= NRows - 1) row = NRows - 2;

	double GridX = col * GeoSpacingX + GeoOffsetX;
	double GridY = row * GeoSpacingY + GeoOffsetY;

	if (GeoSpacingX == 0.0 || GeoSpacingY == 0.0) return 1;

	double fx = (x - GridX) / GeoSpacingX;
	double fy = (y - GridY) / GeoSpacingY;

	double xBL = GeoTable[row*NCols + col].x;
	double yBL = GeoTable[row*NCols + col].y;
	double zBL = GeoTable[row*NCols + col].z;
	double cBL = GeoTable[row*NCols + col].c;
	double xBR = GeoTable[row*NCols + col + 1].x;
	double yBR = GeoTable[row*NCols + col + 1].y;
	double zBR = GeoTable[row*NCols + col + 1].z;
	double cBR = GeoTable[row*NCols + col + 1].c;
	double xTL = GeoTable[(row + 1)*NCols + col].x;
	double yTL = GeoTable[(row + 1)*NCols + col].y;
	double zTL = GeoTable[(row + 1)*NCols + col].z;
	double cTL = GeoTable[(row + 1)*NCols + col].c;
	double xTR = GeoTable[(row + 1)*NCols + col + 1].x;
	double yTR = GeoTable[(row + 1)*NCols + col + 1].y;
	double zTR = GeoTable[(row + 1)*NCols + col + 1].z;
	double cTR = GeoTable[(row + 1)*NCols + col + 1].c;

	double xb = xBL + (xBR - xBL) * fx;
	double yb = yBL + (yBR - yBL) * fx;
	double zb = zBL + (zBR - zBL) * fx;
	double cb = cBL + (cBR - cBL) * fx;

	double xt = xTL + (xTR - xTL) * fx;
	double yt = yTL + (yTR - yTL) * fx;
	double zt = zTL + (zTR - zTL) * fx;
	double ct = cTL + (cTR - cTL) * fx;


	*rx = xb + (xt - xb) * fy;
	*ry = yb + (yt - yb) * fy;
	*rz = zb + (zt - zb) * fy + z;
	*rc = cb + (ct - cb) * fy + c;

	*ra = a;
	*rb = 0.65 * x + b;  // tkt fudge factor degrees per inch for mysterious tilt

	return 0;
}

int CKinematicsGeppettoExtrude::GetSoftLimits(double *xm, double *xp, double *ym, double *yp, double *zm, double *zp,
	                                   double *am, double *ap, double *bm, double *bp, double *cm, double *cp)
{
	*xm = -5;
	*xp = 5.5;
	*ym = -5;
	*yp = 5;
	*zm = -0.5;
	*zp = 5;
	*am = -25;
	*ap = 25;
	*bm = -25;
	*bp = 25;
	*cm = -25;
	*cp = 25;
	return 0;
}


