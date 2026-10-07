// Kinematics2AxisRobot.cpp: implementation of the CKinematics2AxisRobot class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Kinematics2AxisRobot.h"

#define sqr(x) ((x)*(x))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematics2AxisRobot::CKinematics2AxisRobot()
{
	m_MotionParams.MaxLinearLength = 0.25;  // limit the segment lengs for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1.0;       // limit the increase in Rapid HW FRO
	m_MotionParams.UseOnlyLinearSegments=true;

	//Assign Linkage Values
	L = 11.0;
	R1 = 4.0;
	S = 3.0;
	R2 = 4.0;
	R3 = 4.0;
	L1 = 7.0;
	L2 = L - L1;
}

CKinematics2AxisRobot::~CKinematics2AxisRobot()
{

}

int CKinematics2AxisRobot::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double* Acts, bool NoGeo)
{
	//Define Intermediate Variables
	double A1, H, A2, Y1, Y2, Y3, X1, X2, H1, A3, L3, A4, M2;

	// adjust xy to be zero at neutral home position
	x = -x + L - S;
	y += R3;
	
	// find angles for each motor
	GeoCorrect(x,y,z,&x,&y,&z);

	//Calculate values necessary to solve for motor 2
	A1 = atan(y/(x + S));												//pythagorean theorem to solve for A1
	H = y / (sin(A1));													//pythagorean theorem to find hypotenuse of same angle
	A2 = acos((sqr(H) + sqr(R1) - sqr(L)) / (2 * H * R1));				//law of Cosines to find A2

	M2 = A1 + A2;
	double r1 = M2*180/PI;												//Motor position 2 is the sum of the solved angles

	//Calculate values necessary to solve for motor 1
	Y1 = sin(M2) * R1;													//Calculate Y height of Motor 2 Linkage
	Y2 = L1 / L * (y - Y1) + Y1;										//Find height of center link on L using a ratio of Y and Y1
	Y3 = y - Y2;														//Find Y height of small trangle
	X1 = sqrt(sqr(L2) - sqr(Y3));										//pythagorean theorem to solve for X1
	X2 = x - X1;														//Find base of triangle between M1 and top linkage
	H1 = sqrt(sqr(Y2)+ sqr(X2));										//Find hypotenuse of same angle
	double T = (sqr(R2) + sqr(H1) - sqr(R3)) / (2 * R2 * H1);
	if (T > 1.0 || T < -1.0) 
		return 1;
	A3 = acos(T);			//Law of Cos to find angle A3
	L3 = sqrt(sqr(Y2) + sqr(S + X2));									//Find length of line L3
	T = (sqr(H1) + sqr(S) - sqr(L3)) / (2 * H1 * S);
	if (T > 1.0 || T < -1.0) 
		return 1;
	A4 = acos(T);				//Law cos Cos to find angle A4

	double r0 = 180 - (A4*180/PI)-(A3*180/PI); 							//Motor position 1

	//Calculate Motor 3 Position
	double r2 = atan(Y3 / X1) * 180 / PI;

	Acts[0] = r0*m_MotionParams.CountsPerInchX;
	Acts[1] = r1*m_MotionParams.CountsPerInchY;
	Acts[2] = (z-r2)*m_MotionParams.CountsPerInchZ;

	Acts[3] = a*m_MotionParams.CountsPerInchA;
	Acts[4] = b*m_MotionParams.CountsPerInchB;
	Acts[5] = c*m_MotionParams.CountsPerInchC;

	return 0;
}


// perform Inversion to go the other way

int CKinematics2AxisRobot::TransformActuatorstoCAD(double* Acts, double* xr, double* yr, double* zr, double* ar, double* br, double* cr, bool NoGeo)
{
	return InvertTransformCADtoActuators(Acts, xr, yr, zr, ar, br, cr);
}

