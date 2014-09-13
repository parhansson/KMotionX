// Kinematics.cpp: implementation of the CKinematics class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define sqr(x) ((x)*(x))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKinematics::CKinematics()
{
	m_MotionParams.BreakAngle = 30.0;
	m_MotionParams.TPLookahead = 3.0;
	m_MotionParams.MaxAccelC = 1.0;
	m_MotionParams.MaxAccelB = 1.0;
	m_MotionParams.MaxAccelA = 1.0;
	m_MotionParams.MaxAccelX = 1.0;
	m_MotionParams.MaxAccelY = 1.0;
	m_MotionParams.MaxAccelZ = 1.0;
	m_MotionParams.MaxVelC = 1.0;
	m_MotionParams.MaxVelB = 1.0;
	m_MotionParams.MaxVelA = 1.0;
	m_MotionParams.MaxVelX = 1.0;
	m_MotionParams.MaxVelY = 1.0;
	m_MotionParams.MaxVelZ = 1.0;

	m_MotionParams.MaxRapidJerkC = 10.0;
	m_MotionParams.MaxRapidJerkB = 10.0;
	m_MotionParams.MaxRapidJerkA = 10.0;
	m_MotionParams.MaxRapidJerkX = 10.0;
	m_MotionParams.MaxRapidJerkY = 10.0;
	m_MotionParams.MaxRapidJerkZ = 10.0;
	m_MotionParams.MaxRapidAccelC = 1.0;
	m_MotionParams.MaxRapidAccelB = 1.0;
	m_MotionParams.MaxRapidAccelA = 1.0;
	m_MotionParams.MaxRapidAccelX = 1.0;
	m_MotionParams.MaxRapidAccelY = 1.0;
	m_MotionParams.MaxRapidAccelZ = 1.0;
	m_MotionParams.MaxRapidVelC = 1.0;
	m_MotionParams.MaxRapidVelB = 1.0;
	m_MotionParams.MaxRapidVelA = 1.0;
	m_MotionParams.MaxRapidVelX = 1.0;
	m_MotionParams.MaxRapidVelY = 1.0;
	m_MotionParams.MaxRapidVelZ = 1.0;

	m_MotionParams.CountsPerInchC = 100.0;
	m_MotionParams.CountsPerInchB = 100.0;
	m_MotionParams.CountsPerInchA = 100.0;
	m_MotionParams.CountsPerInchX = 100.0;
	m_MotionParams.CountsPerInchY = 100.0;
	m_MotionParams.CountsPerInchZ = 100.0;
	m_MotionParams.MaxLinearLength = 1e30;  //Infinity for default case
	m_MotionParams.CollinearTol = 0.0002;
	m_MotionParams.CornerTol = 0.0002;
	m_MotionParams.FacetAngle = 0.5;
	m_MotionParams.UseOnlyLinearSegments=false;
	m_MotionParams.DegreesA=false;
	m_MotionParams.DegreesB=false;
	m_MotionParams.DegreesC=false;

	m_MotionParams.SoftLimitNegX=
	m_MotionParams.SoftLimitNegY=
	m_MotionParams.SoftLimitNegZ=
	m_MotionParams.SoftLimitNegA=
	m_MotionParams.SoftLimitNegB=
	m_MotionParams.SoftLimitNegC=1e-30;

	m_MotionParams.SoftLimitPosX=
	m_MotionParams.SoftLimitPosY=
	m_MotionParams.SoftLimitPosZ=
	m_MotionParams.SoftLimitPosA=
	m_MotionParams.SoftLimitPosB=
	m_MotionParams.SoftLimitPosC=1e30;

	GeoTableValid=false;
	GeoTable = NULL;

}

CKinematics::~CKinematics()
{
	if (GeoTable) delete [] GeoTable;
}

int CKinematics::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts)
{
	GeoCorrect(x,y,z,&x,&y,&z);

	Acts[0] = x*m_MotionParams.CountsPerInchX;
	Acts[1] = y*m_MotionParams.CountsPerInchY;
	Acts[2] = z*m_MotionParams.CountsPerInchZ;
	Acts[3] = a*m_MotionParams.CountsPerInchA;
	Acts[4] = b*m_MotionParams.CountsPerInchB;
	Acts[5] = c*m_MotionParams.CountsPerInchC;

	return 0;
}

int CKinematics::InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr)
{
	double Tol=1e-6;
	double d=0.1;					// should be linear over this range
	double x=0,y=0,z=0,a=0,b=0,c=0; // initial guess at answer
	double Acts0[MAX_ACTUATORS],ActsX[MAX_ACTUATORS],ActsY[MAX_ACTUATORS];
	double ActsZ[MAX_ACTUATORS],ActsA[MAX_ACTUATORS],ActsB[MAX_ACTUATORS],ActsC[MAX_ACTUATORS];
	double A[3*4];

	double ex,ey,ez,ea,eb,ec;

	for (int i=0; i<100; i++)
	{
		// measure sensitivity
		// 
		// assume majority is simply x -> Act0, y -> Act1, etc..

		TransformCADtoActuators(x,  y,  z,  a,  b,  c,  Acts0);
		TransformCADtoActuators(x+d,y,  z,  a,  b,  c,  ActsX);
		TransformCADtoActuators(x,  y+d,z,  a,  b,  c,  ActsY);
		TransformCADtoActuators(x,  y,  z+d,a,  b,  c,  ActsZ);
		TransformCADtoActuators(x,  y,  z  ,a+d,b,  c,  ActsA);
		TransformCADtoActuators(x,  y,  z  ,a,  b+d,c,  ActsB);
		TransformCADtoActuators(x,  y,  z  ,a,  b,  c+d,ActsC);


		//   | x | | RX0  RY0  RZ0 |    | R0 |
		//   | y | | RX1  RY1  RZ1 | =  | R1 |
		//   | z | | RX2  RY2  RZ2 |    | R2 |


		A[0*4+0] = (ActsX[0]-Acts0[0])/d;  // changes due to x
		A[1*4+0] = (ActsX[1]-Acts0[1])/d;
		A[2*4+0] = (ActsX[2]-Acts0[2])/d;

		A[0*4+1] = (ActsY[0]-Acts0[0])/d;  // changes due to y
		A[1*4+1] = (ActsY[1]-Acts0[1])/d;
		A[2*4+1] = (ActsY[2]-Acts0[2])/d;

		A[0*4+2] = (ActsZ[0]-Acts0[0])/d;  // changes due to z
		A[1*4+2] = (ActsZ[1]-Acts0[1])/d;
		A[2*4+2] = (ActsZ[2]-Acts0[2])/d;

		A[0*4+3] = Acts[0]-Acts0[0];   // desired changes
		A[1*4+3] = Acts[1]-Acts0[1];
		A[2*4+3] = Acts[2]-Acts0[2];

		Solve(A,3);  // solve simultaneous eqs

		// corrections in CAD space

		ex = A[0*4+3];
		ey = A[1*4+3];
		ez = A[2*4+3];
		ea = d*(Acts[3] - Acts0[3])/(ActsA[3] - Acts0[3]);
		eb = d*(Acts[4] - Acts0[4])/(ActsB[4] - Acts0[4]);
		ec = d*(Acts[5] - Acts0[5])/(ActsC[5] - Acts0[5]);

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



int CKinematics::TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c)
{
	if (GeoTableValid)
	{
		// with GeoTable we must perform iterative inversion
		return InvertTransformCADtoActuators(Acts, x, y, z, a, b, c);
	}
	else
	{
		// with no GeoTable use simple linear method
		*x = Acts[0] / m_MotionParams.CountsPerInchX;
		*y = Acts[1] / m_MotionParams.CountsPerInchY;
		*z = Acts[2] / m_MotionParams.CountsPerInchZ;
		*a = Acts[3] / m_MotionParams.CountsPerInchA;
		*b = Acts[4] / m_MotionParams.CountsPerInchB;
		*c = Acts[5] / m_MotionParams.CountsPerInchC;

		return 0;
	}
}

int CKinematics::MaxRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double *rate)
{
	double Max,FeedRateToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, &m_MotionParams, &pure_angle);

	// limit speeds based on proportion in that direction

	if (pure_angle)
	{
		if (fda>0 && m_MotionParams.MaxVelA < FeedRateToUse * fda/d) FeedRateToUse = m_MotionParams.MaxVelA * d/fda;
		if (fdb>0 && m_MotionParams.MaxVelB < FeedRateToUse * fdb/d) FeedRateToUse = m_MotionParams.MaxVelB * d/fdb;
		if (fdc>0 && m_MotionParams.MaxVelC < FeedRateToUse * fdc/d) FeedRateToUse = m_MotionParams.MaxVelC * d/fdc;
	}
	else
	{
		if (fdx>0 && m_MotionParams.MaxVelX < FeedRateToUse * fdx/d) FeedRateToUse = m_MotionParams.MaxVelX * d/fdx;
		if (fdy>0 && m_MotionParams.MaxVelY < FeedRateToUse * fdy/d) FeedRateToUse = m_MotionParams.MaxVelY * d/fdy;
		if (fdz>0 && m_MotionParams.MaxVelZ < FeedRateToUse * fdz/d) FeedRateToUse = m_MotionParams.MaxVelZ * d/fdz;
		if (fda>0)
		{
			Max = m_MotionParams.MaxVelA;
			if (m_MotionParams.DegreesA)
			{
				Max *= m_MotionParams.RadiusA * PI/180.0;  // transform max rotation speed to linear speed 
				fda *= m_MotionParams.RadiusA * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < FeedRateToUse * fda/d) FeedRateToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxVelB;
			if (m_MotionParams.DegreesB)
			{
				Max *= m_MotionParams.RadiusB * PI/180.0;  // transform max rotation speed to linear speed 
				fdb *= m_MotionParams.RadiusB * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < FeedRateToUse * fdb/d) FeedRateToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxVelC;
			if (m_MotionParams.DegreesC)
			{
				Max *= m_MotionParams.RadiusC * PI/180.0;  // transform max rotation speed to linear speed 
				fdc *= m_MotionParams.RadiusC * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < FeedRateToUse * fdc/d) FeedRateToUse = Max * d/fdc;
		}
	}

	*rate = FeedRateToUse;

	return 0;
}

int CKinematics::MaxRapidRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double *rate)
{
	BOOL pure_angle;
	double Max,FeedRateToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, &m_MotionParams, &pure_angle);

	// limit speeds based on proportion in that direction

	if (pure_angle)
	{
		if (fda>0 && m_MotionParams.MaxRapidVelA < FeedRateToUse * fda/d) FeedRateToUse = m_MotionParams.MaxRapidVelA * d/fda;
		if (fdb>0 && m_MotionParams.MaxRapidVelB < FeedRateToUse * fdb/d) FeedRateToUse = m_MotionParams.MaxRapidVelB * d/fdb;
		if (fdc>0 && m_MotionParams.MaxRapidVelC < FeedRateToUse * fdc/d) FeedRateToUse = m_MotionParams.MaxRapidVelC * d/fdc;
	}
	else
	{
		if (fdx>0 && m_MotionParams.MaxRapidVelX < FeedRateToUse * fdx/d) FeedRateToUse = m_MotionParams.MaxRapidVelX * d/fdx;
		if (fdy>0 && m_MotionParams.MaxRapidVelY < FeedRateToUse * fdy/d) FeedRateToUse = m_MotionParams.MaxRapidVelY * d/fdy;
		if (fdz>0 && m_MotionParams.MaxRapidVelZ < FeedRateToUse * fdz/d) FeedRateToUse = m_MotionParams.MaxRapidVelZ * d/fdz;
		if (fda>0)
		{
			Max = m_MotionParams.MaxRapidVelA;
			if (m_MotionParams.DegreesA)
			{
				Max *= m_MotionParams.RadiusA * PI/180.0;  // transform max rotation speed to linear speed 
				fda *= m_MotionParams.RadiusA * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < FeedRateToUse * fda/d) FeedRateToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxRapidVelB;
			if (m_MotionParams.DegreesB)
			{
				Max *= m_MotionParams.RadiusB * PI/180.0;  // transform max rotation speed to linear speed 
				fdb *= m_MotionParams.RadiusB * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < FeedRateToUse * fdb/d) FeedRateToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxRapidVelC;
			if (m_MotionParams.DegreesC)
			{
				Max *= m_MotionParams.RadiusC * PI/180.0;  // transform max rotation speed to linear speed 
				fdc *= m_MotionParams.RadiusC * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < FeedRateToUse * fdc/d) FeedRateToUse = Max * d/fdc;
		}
	}
	*rate = FeedRateToUse;

	return 0;
}


int CKinematics::MaxAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double *accel)
{
	double Max,AccelToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, &m_MotionParams, &pure_angle);

	// limit accel based on proportion in that direction
	if (pure_angle)
	{
		if (fda>0 && m_MotionParams.MaxAccelA < AccelToUse * fda/d) AccelToUse = m_MotionParams.MaxAccelA * d/fda;
		if (fdb>0 && m_MotionParams.MaxAccelB < AccelToUse * fdb/d) AccelToUse = m_MotionParams.MaxAccelB * d/fdb;
		if (fdc>0 && m_MotionParams.MaxAccelC < AccelToUse * fdc/d) AccelToUse = m_MotionParams.MaxAccelC * d/fdc;
	}
	else
	{
		if (fdx>0 && m_MotionParams.MaxAccelX < AccelToUse * fdx/d) AccelToUse = m_MotionParams.MaxAccelX * d/fdx;
		if (fdy>0 && m_MotionParams.MaxAccelY < AccelToUse * fdy/d) AccelToUse = m_MotionParams.MaxAccelY * d/fdy;
		if (fdz>0 && m_MotionParams.MaxAccelZ < AccelToUse * fdz/d) AccelToUse = m_MotionParams.MaxAccelZ * d/fdz;
		if (fda>0)
		{
			Max = m_MotionParams.MaxAccelA;
			if (m_MotionParams.DegreesA)
			{
				Max *= m_MotionParams.RadiusA * PI/180.0;   // transform max rotation speed to linear speed 
				fda *= m_MotionParams.RadiusA * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < AccelToUse * fda/d) AccelToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxAccelB;
			if (m_MotionParams.DegreesB)
			{
				Max *= m_MotionParams.RadiusB * PI/180.0;   // transform max rotation speed to linear speed 
				fdb *= m_MotionParams.RadiusB * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < AccelToUse * fdb/d) AccelToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxAccelC;
			if (m_MotionParams.DegreesC)
			{
				Max *= m_MotionParams.RadiusC * PI/180.0;   // transform max rotation speed to linear speed 
				fdc *= m_MotionParams.RadiusC * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < AccelToUse * fdc/d) AccelToUse = Max * d/fdc;
		}
	}

	*accel = AccelToUse;

	return 0;
}


int CKinematics::MaxRapidAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double *accel)
{
	double Max,AccelToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, &m_MotionParams, &pure_angle);

	// limit accel based on proportion in that direction
	if (pure_angle)
	{
		if (fda>0 && m_MotionParams.MaxRapidAccelA < AccelToUse * fda/d) AccelToUse = m_MotionParams.MaxRapidAccelA * d/fda;
		if (fdb>0 && m_MotionParams.MaxRapidAccelB < AccelToUse * fdb/d) AccelToUse = m_MotionParams.MaxRapidAccelB * d/fdb;
		if (fdc>0 && m_MotionParams.MaxRapidAccelC < AccelToUse * fdc/d) AccelToUse = m_MotionParams.MaxRapidAccelC * d/fdc;
	}
	else
	{
		if (fdx>0 && m_MotionParams.MaxRapidAccelX < AccelToUse * fdx/d) AccelToUse = m_MotionParams.MaxRapidAccelX * d/fdx;
		if (fdy>0 && m_MotionParams.MaxRapidAccelY < AccelToUse * fdy/d) AccelToUse = m_MotionParams.MaxRapidAccelY * d/fdy;
		if (fdz>0 && m_MotionParams.MaxRapidAccelZ < AccelToUse * fdz/d) AccelToUse = m_MotionParams.MaxRapidAccelZ * d/fdz;
		if (fda>0)
		{
			Max = m_MotionParams.MaxRapidAccelA;
			if (m_MotionParams.DegreesA)
			{
				Max *= m_MotionParams.RadiusA * PI/180.0;   // transform max rotation speed to linear speed 
				fda *= m_MotionParams.RadiusA * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < AccelToUse * fda/d) AccelToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxRapidAccelB;
			if (m_MotionParams.DegreesB)
			{
				Max *= m_MotionParams.RadiusB * PI/180.0;   // transform max rotation speed to linear speed 
				fdb *= m_MotionParams.RadiusB * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < AccelToUse * fdb/d) AccelToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxRapidAccelC;
			if (m_MotionParams.DegreesC)
			{
				Max *= m_MotionParams.RadiusC * PI/180.0;   // transform max rotation speed to linear speed 
				fdc *= m_MotionParams.RadiusC * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < AccelToUse * fdc/d) AccelToUse = Max * d/fdc;
		}
	}

	*accel = AccelToUse;

	return 0;
}

int CKinematics::MaxRapidJerkInDirection(double dx, double dy, double dz, double da, double db, double dc, double *jerk)
{
	double Max,JerkToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, &m_MotionParams, &pure_angle);

	// limit Jerk based on proportion in that direction
	if (pure_angle)
	{
		if (fda>0 && m_MotionParams.MaxRapidJerkA < JerkToUse * fda/d) JerkToUse = m_MotionParams.MaxRapidJerkA * d/fda;
		if (fdb>0 && m_MotionParams.MaxRapidJerkB < JerkToUse * fdb/d) JerkToUse = m_MotionParams.MaxRapidJerkB * d/fdb;
		if (fdc>0 && m_MotionParams.MaxRapidJerkC < JerkToUse * fdc/d) JerkToUse = m_MotionParams.MaxRapidJerkC * d/fdc;
	}
	else
	{
		if (fdx>0 && m_MotionParams.MaxRapidJerkX < JerkToUse * fdx/d) JerkToUse = m_MotionParams.MaxRapidJerkX * d/fdx;
		if (fdy>0 && m_MotionParams.MaxRapidJerkY < JerkToUse * fdy/d) JerkToUse = m_MotionParams.MaxRapidJerkY * d/fdy;
		if (fdz>0 && m_MotionParams.MaxRapidJerkZ < JerkToUse * fdz/d) JerkToUse = m_MotionParams.MaxRapidJerkZ * d/fdz;
		if (fda>0)
		{
			Max = m_MotionParams.MaxRapidJerkA;
			if (m_MotionParams.DegreesA)
			{
				Max *= m_MotionParams.RadiusA * PI/180.0;   // transform max rotation speed to linear speed 
				fda *= m_MotionParams.RadiusA * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < JerkToUse * fda/d) JerkToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxRapidJerkB;
			if (m_MotionParams.DegreesB)
			{
				Max *= m_MotionParams.RadiusB * PI/180.0;   // transform max rotation speed to linear speed 
				fdb *= m_MotionParams.RadiusB * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < JerkToUse * fdb/d) JerkToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxRapidJerkC;
			if (m_MotionParams.DegreesC)
			{
				Max *= m_MotionParams.RadiusC * PI/180.0;   // transform max rotation speed to linear speed 
				fdc *= m_MotionParams.RadiusC * PI/180.0;   // transform rotational dist to linear distance 
			}
			if (Max < JerkToUse * fdc/d) JerkToUse = Max * d/fdc;
		}
	}

	*jerk = JerkToUse;

	return 0;
}




// from : http://mcraefamily.com/MathHelp/GeometryConicSectionCircleIntersection.htm


int CKinematics::IntersectionTwoCircles(CPT2D c0, double r0, CPT2D c1, double r1, CPT2D *q)
{
	double d2 = sqr(c1.x-c0.x) + sqr(c1.y-c0.y);  
	double K = 0.25 * sqrt((sqr(r0+r1)-d2)*(d2-sqr(r0-r1)));

	q->x = 0.5 * (c1.x+c0.x) + 0.5 * (c1.x-c0.x)*(sqr(r0)-sqr(r1))/d2 - 2.0 * (c1.y-c0.y)*K/d2;
	q->y = 0.5 * (c1.y+c0.y) + 0.5 * (c1.y-c0.y)*(sqr(r0)-sqr(r1))/d2 + 2.0 * (c1.x-c0.x)*K/d2;

	return 0;
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

int CKinematics::ReadGeoTable(const char *name)
{
	double X,Y,Z;
	int row,col;

	GeoTableValid=false;

	if (name[0]==0) return 0; // passing in no file turns off geocorrection
	
	FILE *f = fopen(name,"rt");

	if (!f)
	{
		AfxMessageBox((CString)"Unable to open Geometric Correction File : " + name);
		return 1;
	}

	int result = fscanf(f,"%d,%d",&NRows,&NCols);
		
	if (result != 2 || NRows < 2 || NRows > 4000 || NCols < 2 || NCols > 4000)
	{
		fclose(f);
		AfxMessageBox((CString)"Invalid Geometric Correction File (NRows and NCols) : " + name);
		return 1;
	}

	result = fscanf(f,"%lf,%lf",&GeoSpacingX,&GeoSpacingY);
		
	if (result != 2)
	{
		fclose(f);
		AfxMessageBox((CString)"Invalid Geometric Correction File (GeoSpacingX and GeoSpacingY) : " + name);
		return 1;
	}

	result = fscanf(f,"%lf,%lf",&GeoOffsetX,&GeoOffsetY);
		
	if (result != 2)
	{
		fclose(f);
		AfxMessageBox((CString)"Invalid Geometric Correction File (GeoOffsetX and GeoOffsetY) : " + name);
		return 1;
	}

	if (GeoTable) delete [] GeoTable;
	GeoTable = new CPT3D[NRows*NCols];

	for (int i=0; i<NRows*NCols; i++)
	{
		result = fscanf(f,"%d,%d,%lf,%lf,%lf",&row,&col,&X,&Y,&Z);

		if (result != 5 || row < 0 || row >= NRows || col < 0 || col >= NCols)
		{
			fclose(f);
			AfxMessageBox((CString)"Invalid Geometric Correction File (invalid data value) : " + name);
			return 1;
		}

		GeoTable[row*NCols+col].x = X;
		GeoTable[row*NCols+col].y = Y;
		GeoTable[row*NCols+col].z = Z;
	}

	fclose(f);
	
	GeoTableValid=true;
	return 0;
}


// for now the z just has an offset to make the x,y, plane at z=0 flat


int CKinematics::GeoCorrect(double x, double y, double z, double *cx, double *cy, double *cz)
{
	if (!GeoTableValid) return 1;

	int col = (int)floor((x-GeoOffsetX)/GeoSpacingX);
	int row = (int)floor((y-GeoOffsetY)/GeoSpacingY);

	// stay within table

	if (col < 0) col=0;
	if (col >= NCols-1) col = NCols-2;
	if (row < 0) row=0;
	if (row >= NRows-1) row = NRows-2;

	double GridX = col * GeoSpacingX + GeoOffsetX;
	double GridY = row * GeoSpacingY + GeoOffsetY;

	if (GeoSpacingX == 0.0 || GeoSpacingY == 0.0) return 1;

	double fx = (x - GridX)/GeoSpacingX;
	double fy = (y - GridY)/GeoSpacingY;

	double xBL = GeoTable[row*NCols+col].x;
	double yBL = GeoTable[row*NCols+col].y;
	double zBL = GeoTable[row*NCols+col].z;
	double xBR = GeoTable[row*NCols+col+1].x;
	double yBR = GeoTable[row*NCols+col+1].y;
	double zBR = GeoTable[row*NCols+col+1].z;
	double xTL = GeoTable[(row+1)*NCols+col].x;
	double yTL = GeoTable[(row+1)*NCols+col].y;
	double zTL = GeoTable[(row+1)*NCols+col].z;
	double xTR = GeoTable[(row+1)*NCols+col+1].x;
	double yTR = GeoTable[(row+1)*NCols+col+1].y;
	double zTR = GeoTable[(row+1)*NCols+col+1].z;

	double xb = xBL + (xBR - xBL) * fx; 
	double yb = yBL + (yBR - yBL) * fx; 
	double zb = zBL + (zBR - zBL) * fx; 

	double xt = xTL + (xTR - xTL) * fx; 
	double yt = yTL + (yTR - yTL) * fx; 
	double zt = zTL + (zTR - zTL) * fx; 


	*cx = xb + (xt - xb) * fy;
	*cy = yb + (yt - yb) * fy;
	*cz = zb + (zt - zb) * fy + z;

	return 0;
}



//	Pass array to be inverted (A) and the size of the array (N)
//	Matrix size is really N x N+1


int CKinematics::Solve(double *A, int N)
{
	int i,j,l,m,k,N1=N+1;
	double y;
	
	for (i=0; i<N; i++)   // reduce all the rows
	{
		ASSERT(i*N1+i >=0 && i*N1+i <12);
		if (A[i*N1+i]==0.0)     // check if diagonal has a zero
		{
			l=i+1;            // it does, try and switch rows
			if (l>N-1) return 1; // return error if no more rows to switch
			while (A[l*N1+i]==0.0 && l<(N-1)) l++;
			for (m=0; m<=N; m++)  // swap the row
			{
				y=A[i*N1+m];
				ASSERT(i*N1+m >=0 && i*N1+m <12);
				A[i*N1+m]=A[l*N1+m];
				ASSERT(l*N1+m >=0 && l*N1+m <12);
				A[l*N1+m]=y;
			}
		}
		
		for (j=N; j>=i; j--)
		{
			ASSERT(i*N1+j >=0 && i*N1+j <12);
			A[i*N1+j]/=A[i*N1+i];    // divide row so that it starts with 1
		}

		for (j=0; j<N; j++)
			if (j!=i && A[j*N1+i]!=0.0)
				for (k=N; k>=i; k--)
				{
					ASSERT(j*N1+k >=0 && j*N1+k <12);
					A[j*N1+k]-=A[i*N1+k]*A[j*N1+i];
				}
	}
	return 0;
}
