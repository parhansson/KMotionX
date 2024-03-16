#include "StdAfx.h"
#include "KinematicsScara.h"

#define sqr(x) ((x)*(x))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematicsScara::CKinematicsScara()
{
	Length_1 = 7.874; // X axis length
	Length_2 = 7.874; // Y axis length

	SqrLength_1 = sqr(Length_1); //X*X
	SqrLength_2 = sqr(Length_2); //Y*Y

								 // copied from gepetto
	m_MotionParams.MaxLinearLength = 0.05;  // limit the segment lengths for nonlinear systems
	m_MotionParams.MaxAngularChange = 0.5;  // limit the segment angle change for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1.0;       // limit the increase in Rapid HW FRO
	m_MotionParams.UseOnlyLinearSegments = true;
}

CKinematicsScara::~CKinematicsScara()
{

}

int CKinematicsScara::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo)
{
	double _X = x - (Length_1 + Length_2); // shift the coordinates to match the systems
	double _Y = y;

	double distance = sqrt(sqr(_X) + sqr(_Y));  // distance from (x0,y0) to (x2,y2)
												// to simplify calculations
	double distance2 = sqr(_X) + sqr(_Y);

	//the angle between the vector from (0,0) to (x2,y2) and the axis X
	double angle = acos(CorrectAngle(_X / distance));

	// looking for angles in a triangle by the consensus theorem
	double angle1 = acos(CorrectAngle((distance2 + SqrLength_1 - SqrLength_2) / (2 * distance * Length_1)));
	double angle3 = acos(CorrectAngle((distance2 + SqrLength_2 - SqrLength_1) / (2 * distance * Length_2)));
	double angle2 = PI - angle1 - angle3;

	double angleMotor1 = angle + angle1; // angle of rotation of the 1st engine
	double a1 = 180 - angleMotor1 * 180 / PI; //angle of rotation of 1 engine in degrees
	double a2 = 180 - angle2 * 180 / PI; //angle of rotation of 2 engine in degrees
	double a3 = -a1 - a2 + a; //we hold the axis A parallel to the X axis

	Acts[0] = a1 * m_MotionParams.CountsPerInchX;
	Acts[1] = a2 * m_MotionParams.CountsPerInchY;
	Acts[2] = z * m_MotionParams.CountsPerInchZ;

	Acts[3] = a3 * m_MotionParams.CountsPerInchA;
	Acts[4] = b * m_MotionParams.CountsPerInchB;
	Acts[5] = c * m_MotionParams.CountsPerInchC;

	return 0;
}

int CKinematicsScara::TransformActuatorstoCAD(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	double a1 = Acts[0] / m_MotionParams.CountsPerInchX;
	double a2 = Acts[1] / m_MotionParams.CountsPerInchY;
	double a3 = Acts[3] / m_MotionParams.CountsPerInchA;

	double angleMotor1 = PI - (a1)* PI / 180;
	double angleMotor2 = PI - (a2)* PI / 180;

	// distance from (0,0) to (x2,y2)
	double distance = sqrt(SqrLength_1 + SqrLength_2 - 2 * Length_1 * Length_2 * cos(angleMotor2));
	// to simplify calculations
	double distance2 = SqrLength_1 + SqrLength_2 - 2 * Length_1 * Length_2 * cos(angleMotor2);

	double t;
	// 1st angle of the triangle
	if (distance != 0)
		t = (SqrLength_1 + distance2 - SqrLength_2) / (2 * Length_1 * distance);
	else
		t = 0;
	double angle1 = acos(CorrectAngle(t));

	// angle between the vector from (0,0) to (x2,y2) and the X axis
	double angle = angleMotor1 - angle1;

	double a = a1 + a2 + a3;


	// find the coordinates (x2,y2) - the end point
	*xr = distance * cos(angle) + (Length_1 + Length_2);
	*yr = distance * sin(angle);

	*zr = Acts[2] / m_MotionParams.CountsPerInchZ;
	*ar = a;
	*br = Acts[4] / m_MotionParams.CountsPerInchB;
	*cr = Acts[5] / m_MotionParams.CountsPerInchC;

	return 0;
}

// Check for out of range for ACos
double CKinematicsScara::CorrectAngle(double _angle)
{
	if (_angle > 1)
		return 1;
	if (_angle < -1)
		return -1;
	return _angle;
}