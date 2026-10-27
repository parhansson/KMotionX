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
	m_MotionParams.MaxAccelV = 1.0;
	m_MotionParams.MaxAccelU = 1.0;
	m_MotionParams.MaxAccelC = 1.0;
	m_MotionParams.MaxAccelB = 1.0;
	m_MotionParams.MaxAccelA = 1.0;
	m_MotionParams.MaxAccelX = 1.0;
	m_MotionParams.MaxAccelY = 1.0;
	m_MotionParams.MaxAccelZ = 1.0;
	m_MotionParams.MaxVelV = 1.0;
	m_MotionParams.MaxVelU = 1.0;
	m_MotionParams.MaxVelC = 1.0;
	m_MotionParams.MaxVelB = 1.0;
	m_MotionParams.MaxVelA = 1.0;
	m_MotionParams.MaxVelX = 1.0;
	m_MotionParams.MaxVelY = 1.0;
	m_MotionParams.MaxVelZ = 1.0;

	m_MotionParams.MaxRapidJerkV = 10.0;
	m_MotionParams.MaxRapidJerkU = 10.0;
	m_MotionParams.MaxRapidJerkC = 10.0;
	m_MotionParams.MaxRapidJerkB = 10.0;
	m_MotionParams.MaxRapidJerkA = 10.0;
	m_MotionParams.MaxRapidJerkX = 10.0;
	m_MotionParams.MaxRapidJerkY = 10.0;
	m_MotionParams.MaxRapidJerkZ = 10.0;
	m_MotionParams.MaxRapidAccelV = 1.0;
	m_MotionParams.MaxRapidAccelU = 1.0;
	m_MotionParams.MaxRapidAccelC = 1.0;
	m_MotionParams.MaxRapidAccelB = 1.0;
	m_MotionParams.MaxRapidAccelA = 1.0;
	m_MotionParams.MaxRapidAccelX = 1.0;
	m_MotionParams.MaxRapidAccelY = 1.0;
	m_MotionParams.MaxRapidAccelZ = 1.0;
	m_MotionParams.MaxRapidVelV = 1.0;
	m_MotionParams.MaxRapidVelU = 1.0;
	m_MotionParams.MaxRapidVelC = 1.0;
	m_MotionParams.MaxRapidVelB = 1.0;
	m_MotionParams.MaxRapidVelA = 1.0;
	m_MotionParams.MaxRapidVelX = 1.0;
	m_MotionParams.MaxRapidVelY = 1.0;
	m_MotionParams.MaxRapidVelZ = 1.0;

	m_MotionParams.CountsPerInchV = 100.0;
	m_MotionParams.CountsPerInchU = 100.0;
	m_MotionParams.CountsPerInchC = 100.0;
	m_MotionParams.CountsPerInchB = 100.0;
	m_MotionParams.CountsPerInchA = 100.0;
	m_MotionParams.CountsPerInchX = 100.0;
	m_MotionParams.CountsPerInchY = 100.0;
	m_MotionParams.CountsPerInchZ = 100.0;
	m_MotionParams.MaxLinearLength = 1e30;  //Infinity for default case
	m_MotionParams.MaxAngularChange = 1e30;  // limit the segment angle change for nonlinear systems
	m_MotionParams.MaxRapidFRO = 1;
	m_MotionParams.CollinearTol = 0.0002;
	m_MotionParams.CornerTol = 0.0002;
	m_MotionParams.LogSegments = false;
	m_MotionParams.FacetAngle = 0.5;
	m_MotionParams.UseOnlyLinearSegments=false;
	m_MotionParams.DoRapidsAsFeeds=false;
	m_MotionParams.DegreesA=false;
	m_MotionParams.DegreesB=false;
	m_MotionParams.DegreesC=false;

	m_MotionParams.SoftLimitNegX=
	m_MotionParams.SoftLimitNegY=
	m_MotionParams.SoftLimitNegZ=
	m_MotionParams.SoftLimitNegA=
	m_MotionParams.SoftLimitNegB =
	m_MotionParams.SoftLimitNegC = 
	m_MotionParams.SoftLimitNegU =
	m_MotionParams.SoftLimitNegV = -1e30;

	m_MotionParams.SoftLimitPosX=
	m_MotionParams.SoftLimitPosY=
	m_MotionParams.SoftLimitPosZ=
	m_MotionParams.SoftLimitPosA=
	m_MotionParams.SoftLimitPosB =
	m_MotionParams.SoftLimitPosC = 
	m_MotionParams.SoftLimitPosU =
	m_MotionParams.SoftLimitPosV = 1e30;

	m_MotionParams.TCP_Active = false;
	m_MotionParams.TCP_X = 0.0;
	m_MotionParams.TCP_Y = 0.0;
	m_MotionParams.TCP_Z = 0.0;

	m_MotionParams.RadiusA = 1.0;
	m_MotionParams.RadiusB = 1.0;
	m_MotionParams.RadiusC = 1.0;
	m_MotionParams.ArcsToSegs = false;

	// 3rd Order (jerk limited) Trajectory Planner options: default to the
	// LEGACY planner.  Applications that predate these fields (the simple
	// CoordMotion examples) initialize nothing here, so without explicit
	// defaults they inherit heap garbage - observed as the 3rd Order
	// Planner randomly activating and rejecting the (unset) Jerk limits.
	m_MotionParams.ThirdOrderTP = false;
	m_MotionParams.CubicKnots = false;
	m_MotionParams.MaxJerkV = 10.0;
	m_MotionParams.MaxJerkU = 10.0;
	m_MotionParams.MaxJerkC = 10.0;
	m_MotionParams.MaxJerkB = 10.0;
	m_MotionParams.MaxJerkA = 10.0;
	m_MotionParams.MaxJerkX = 10.0;
	m_MotionParams.MaxJerkY = 10.0;
	m_MotionParams.MaxJerkZ = 10.0;

	// per-actuator constraint table: off, no slots defined (Scale 0 marks
	// a slot as not part of the coordinate system)
	m_MotionParams.ActuatorLimits = false;
	for (int k = 0; k < MAX_TP_ACTUATORS; k++)
	{
		m_MotionParams.ActScale[k] = 0.0;
		m_MotionParams.MaxActVel[k] = 0.0;
		m_MotionParams.MaxActAccel[k] = 0.0;
		m_MotionParams.MaxActJerk[k] = 0.0;
		m_MotionParams.ActDegrees[k] = false;
	}

	MaxCorrPerIteration = 1.0; // initial trust radius in invert transform
	m_InvBudgetIters = 0;      // 0 = full solve (see Kinematics.h)
#ifndef _KMOTIONX
	InitializeCriticalSection(&m_InvCS);
#endif

	for (int k = 0; k < 6; k++) m_InvGuess[k] = 0.0;
	m_InvWarmValid = false;

	GeoTableValid= AnyLinearTableValid=false;
	GeoTable = NULL;
	for (int i = 0; i < NGCODE_AXES; i++)
	{
		LinearTables[i] = NULL;
		LinearTableValid[i] = false;
	}
}



CKinematics::~CKinematics()
{
	if (GeoTable) delete [] GeoTable;
#ifndef _KMOTIONX
	DeleteCriticalSection(&m_InvCS);
#endif
}

int CKinematics::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double *Acts, bool NoGeo)
{
	GeoCorrect(x, y, z, &x, &y, &z);
	LinearCorrect(x, &x, 0); // perform Screw type linear Table corrections
	LinearCorrect(y, &y, 1);
	LinearCorrect(z, &z, 2);
	LinearCorrect(a, &a, 3);
	LinearCorrect(b, &b, 4);
	LinearCorrect(c, &c, 5);

	Acts[0] = x*m_MotionParams.CountsPerInchX;
	Acts[1] = y*m_MotionParams.CountsPerInchY;
	Acts[2] = z*m_MotionParams.CountsPerInchZ;
	Acts[3] = a*m_MotionParams.CountsPerInchA;
	Acts[4] = b*m_MotionParams.CountsPerInchB;
	Acts[5] = c*m_MotionParams.CountsPerInchC;

	return 0;
}

// by default the 8 axis functions call the 6 axis functions for the case where 6-axis overrides exist but no 8 axis ovverrides

int CKinematics::TransformCADtoActuators(double x, double y, double z, double a, double b, double c, double u, double v, double *Acts, bool NoGeo)
{
	Acts[6] = u*m_MotionParams.CountsPerInchU;
	Acts[7] = v*m_MotionParams.CountsPerInchV;

	return TransformCADtoActuators(x, y, z, a, b, c, Acts, NoGeo);
}

// as default do nothing
int CKinematics::RemapForNonStandardAxes(double *x, double *y, double *z, double *a, double *b, double *c)
{
	return 0;
}


// Iteratively invert TransformCADtoActuators with damped (trust region)
// Newton iteration.  ONE implementation shared by ALL Kinematics classes
// (each formerly carried its own copy with a fixed step clamp).
//
// - Warm starts from the last converged solution: callers invert dense
//   sequences of nearby points (DROs, G-view path drawing) which then
//   converge in 1-2 iterations.  Cold calls start from m_InvGuess,
//   which derived constructors set for machines whose origin is
//   unreachable or singular.
//
// - The correction step is limited by an ADAPTIVE trust radius instead
//   of a fixed clamp: it doubles on every accepted step and shrinks 4x
//   (undoing the step) whenever a step makes the actuator-space
//   residual worse.  Very nonlinear machines are damped exactly where
//   Newton overshoots, while the reachable range is UNBOUNDED - a fixed
//   clamp limited range to clamp*iterations (a 100m machine could never
//   invert under a 0.1 unit clamp).  MaxCorrPerIteration (settable in
//   Kinematics.txt) is the INITIAL trust radius.
//
// - Solves the full 6x6 system (any axis may affect any actuator).  An
//   axis no actuator responds to is held at its guess instead of making
//   the system singular.

int CKinematics::InvertTransformCADtoActuators(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	double sol[6];
	int i, ret;

	InvLock();

	// fast attempt (warm started when available, adaptive trust radius),
	// then the careful fallback: from the cold guess with the step
	// always limited to MaxCorrPerIteration, so the iteration walks to
	// the nearby solution and cannot jump across the workspace onto a
	// second mathematically valid pose (e.g. a lift link folded up over
	// its arm).  The fallback reproduces the original per-class inverter
	// behavior that was slow but reliable.  In probe mode
	// (m_InvBudgetIters > 0) the fast pass gets the caller's iteration
	// budget and the fallback is skipped.
	if (InvertWorker(Acts, NoGeo, true, true,
			(m_InvBudgetIters > 0) ? m_InvBudgetIters : 200, sol) == 0 ||
		(m_InvBudgetIters == 0 &&
		InvertWorker(Acts, NoGeo, false, false, 1000, sol) == 0))
	{
		m_InvWarmValid = true;
		for (i=0;i<6;i++) m_InvWarm[i]=sol[i];
		ret = 0;
	}
	else
	{
		// truly failed, return the last iterate (and don't warm start
		// the next call from it)
		m_InvWarmValid = false;
		ret = 1;
	}

	InvUnlock();

	*xr = sol[0]; *yr = sol[1]; *zr = sol[2];
	*ar = sol[3]; *br = sol[4]; *cr = sol[5];
	return ret;
}

// careful-walk-only entry: derived classes that VERIFY solutions (e.g.
// a reach/branch check) call this when the fast pass converged onto an
// unintended solution branch
int CKinematics::InvertTransformCADtoActuatorsSlow(double *Acts, double *xr, double *yr, double *zr, double *ar, double *br, double *cr, bool NoGeo)
{
	double sol[6];
	int i, ret;

	InvLock();

	if (InvertWorker(Acts, NoGeo, false, false, 1000, sol) == 0)
	{
		m_InvWarmValid = true;
		for (i=0;i<6;i++) m_InvWarm[i]=sol[i];
		ret = 0;
	}
	else
	{
		m_InvWarmValid = false;
		ret = 1;
	}

	InvUnlock();

	*xr = sol[0]; *yr = sol[1]; *zr = sol[2];
	*ar = sol[3]; *br = sol[4]; *cr = sol[5];
	return ret;
}

// one damped-Newton attempt.  adaptive=true: the trust radius doubles
// on every accepted step (range unbounded - a 100m machine inverts
// fine).  adaptive=false: the step stays clamped to MaxCorrPerIteration
// on every iteration (the careful walk - cannot leave the basin of the
// solution nearest the start).  useWarm selects the starting point.
int CKinematics::InvertWorker(double *Acts, bool NoGeo, bool adaptive, bool useWarm, int maxIter, double *vout)
{
	double Tol=1e-6;
	double d=0.1;					// should be linear over this range
	double v[6], vprev[6], p[6], e[6];
	double Acts0[MAX_ACTUATORS], ActsP[MAX_ACTUATORS];
	double A[6*7];
	int i,j;

	if (useWarm && m_InvWarmValid)
		for (i=0;i<6;i++) v[i]=m_InvWarm[i];
	else
		for (i=0;i<6;i++) v[i]=m_InvGuess[i];
	for (i=0;i<6;i++) vprev[i]=v[i];

	double hmax = adaptive ? 1e12 : ((MaxCorrPerIteration > 0) ? MaxCorrPerIteration : 1.0);
	double h = (MaxCorrPerIteration > 0) ? MaxCorrPerIteration : 1.0;  // trust radius
	double Rprev = -1.0;            // residual at the last accepted point

	for (int iter=0; iter<maxIter; iter++)
	{
		// cold restart if a (possibly stale) warm start is not converging
		if (adaptive && iter==100 && useWarm && m_InvWarmValid)
		{
			for (i=0;i<6;i++) v[i]=vprev[i]=m_InvGuess[i];
			h = (MaxCorrPerIteration > 0) ? MaxCorrPerIteration : 1.0;
			Rprev = -1.0;
		}

		TransformCADtoActuators(v[0],v[1],v[2],v[3],v[4],v[5],Acts0,NoGeo);

		// actuator-space residual: if the last step made it worse, undo
		// the step and retry smaller; otherwise accept and grow the radius
		double R = 0;
		for (i=0;i<6;i++)
		{
			double r = fabs(Acts[i]-Acts0[i]);
			if (r > R) R = r;
		}
		if (Rprev >= 0.0 && R > Rprev)
		{
			for (i=0;i<6;i++) v[i]=vprev[i];
			h *= 0.25;
			if (h < Tol) h = Tol;
			TransformCADtoActuators(v[0],v[1],v[2],v[3],v[4],v[5],Acts0,NoGeo);
		}
		else
		{
			Rprev = R;
			h *= 2.0;
			if (h > hmax) h = hmax;
		}

		// measure sensitivity of every actuator to every axis

		for (j=0;j<6;j++)
		{
			for (i=0;i<6;i++) p[i]=v[i];
			p[j] += d;
			TransformCADtoActuators(p[0],p[1],p[2],p[3],p[4],p[5],ActsP,NoGeo);
			for (i=0;i<6;i++) A[i*7+j] = (ActsP[i]-Acts0[i])/d;
		}
		for (i=0;i<6;i++) A[i*7+6] = Acts[i]-Acts0[i];   // desired changes

		// an axis no actuator responds to would make the system singular:
		// hold it at its current value instead
		for (j=0;j<6;j++)
		{
			double m=0;
			for (i=0;i<6;i++) if (fabs(A[i*7+j])>m) m=fabs(A[i*7+j]);
			if (m < 1e-12)
			{
				for (i=0;i<6;i++) { A[j*7+i]=0; A[i*7+j]=0; }
				A[j*7+j]=1; A[j*7+6]=0;
			}
		}

		Solve(A,6);  // solve simultaneous eqs

		for (i=0;i<6;i++) e[i]=A[i*7+6];   // corrections in CAD space

		// a singular pose can produce NaN/Inf corrections: treat like a
		// diverging step - shrink the radius and re-derive
		bool bad=false;
		for (i=0;i<6;i++) if (!(e[i]==e[i]) || fabs(e[i])>1e30) bad=true;
		if (bad)
		{
			h *= 0.25;
			if (h < Tol) h = Tol;
			continue;
		}

		// Done if all within Tolerance

		bool done=true;
		for (i=0;i<6;i++) if (fabs(e[i]) >= Tol) done=false;
		if (done)
		{
			for (i=0;i<6;i++) vout[i]=v[i];
			return 0;
		}

		// take the step, limited to the trust radius (direction preserved)

		double m=0;
		for (i=0;i<6;i++) if (fabs(e[i])>m) m=fabs(e[i]);
		if (m > h)
			for (i=0;i<6;i++) e[i] *= h/m;

		for (i=0;i<6;i++) { vprev[i]=v[i]; v[i]+=e[i]; }
	}

	// did not converge within maxIter
	for (i=0;i<6;i++) vout[i]=v[i];
	return 1;
}

// by default the 8 axis functions call the 6 axis functions for the case where 6-axis overrides exist but no 8 axis ovverrides

int CKinematics::TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, double *u, double *v, bool NoGeo)
{
	*u = Acts[6] / m_MotionParams.CountsPerInchU;
	*v = Acts[7] / m_MotionParams.CountsPerInchV;
	
	return TransformActuatorstoCAD(Acts, x, y, z, a, b, c, NoGeo);
}


int CKinematics::TransformActuatorstoCAD(double *Acts, double *x, double *y, double *z, double *a, double *b, double *c, bool NoGeo)
{
	if (GeoTableValid || AnyLinearTableValid)
	{
		// with GeoTable we must perform iterative inversion
		return InvertTransformCADtoActuators(Acts, x, y, z, a, b, c, NoGeo);
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

int CKinematics::ComputeAnglesOption(int is)
{
	return 0;
}

int CKinematics::MaxRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate)
{
	double Max,FeedRateToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);
	double fdu = fabs(du);
	double fdv = fabs(dv);

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &m_MotionParams, &pure_angle);

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
		if (fdy>0 && m_MotionParams.MaxVelY < FeedRateToUse * fdy / d) FeedRateToUse = m_MotionParams.MaxVelY * d / fdy;
		if (fdz>0 && m_MotionParams.MaxVelZ < FeedRateToUse * fdz / d) FeedRateToUse = m_MotionParams.MaxVelZ * d / fdz;
		if (fdu>0 && m_MotionParams.MaxVelU < FeedRateToUse * fdu / d) FeedRateToUse = m_MotionParams.MaxVelU * d / fdu;
		if (fdv>0 && m_MotionParams.MaxVelV < FeedRateToUse * fdv / d) FeedRateToUse = m_MotionParams.MaxVelV * d / fdv;

		if (fda>0)
		{
			Max = m_MotionParams.MaxVelA;
			if (Max < FeedRateToUse * fda/d) FeedRateToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxVelB;
			if (Max < FeedRateToUse * fdb/d) FeedRateToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxVelC;
			if (Max < FeedRateToUse * fdc/d) FeedRateToUse = Max * d/fdc;
		}
	}

	*rate = FeedRateToUse;

	return 0;
}

int CKinematics::MaxRapidRateInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate)
{
	BOOL pure_angle;
	double Max,FeedRateToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);
	double fdu = fabs(du);
	double fdv = fabs(dv);

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &m_MotionParams, &pure_angle);

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
		if (fdy>0 && m_MotionParams.MaxRapidVelY < FeedRateToUse * fdy / d) FeedRateToUse = m_MotionParams.MaxRapidVelY * d / fdy;
		if (fdz>0 && m_MotionParams.MaxRapidVelZ < FeedRateToUse * fdz / d) FeedRateToUse = m_MotionParams.MaxRapidVelZ * d / fdz;
		if (fdu>0 && m_MotionParams.MaxRapidVelU < FeedRateToUse * fdu / d) FeedRateToUse = m_MotionParams.MaxRapidVelU * d / fdu;
		if (fdv>0 && m_MotionParams.MaxRapidVelV < FeedRateToUse * fdv / d) FeedRateToUse = m_MotionParams.MaxRapidVelV * d / fdv;

		if (fda>0)
		{
			Max = m_MotionParams.MaxRapidVelA;
			if (Max < FeedRateToUse * fda/d) FeedRateToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxRapidVelB;
			if (Max < FeedRateToUse * fdb/d) FeedRateToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxRapidVelC;
			if (Max < FeedRateToUse * fdc/d) FeedRateToUse = Max * d/fdc;
		}
	}
	*rate = FeedRateToUse;

	return 0;
}


int CKinematics::MaxAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel)
{
	double Max,AccelToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);
	double fdu = fabs(du);
	double fdv = fabs(dv);

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &m_MotionParams, &pure_angle);

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
		if (fdy>0 && m_MotionParams.MaxAccelY < AccelToUse * fdy / d) AccelToUse = m_MotionParams.MaxAccelY * d / fdy;
		if (fdz>0 && m_MotionParams.MaxAccelZ < AccelToUse * fdz / d) AccelToUse = m_MotionParams.MaxAccelZ * d / fdz;
		if (fdu>0 && m_MotionParams.MaxAccelU < AccelToUse * fdu / d) AccelToUse = m_MotionParams.MaxAccelU * d / fdu;
		if (fdv>0 && m_MotionParams.MaxAccelV < AccelToUse * fdv / d) AccelToUse = m_MotionParams.MaxAccelV * d / fdv;

		if (fda>0)
		{
			Max = m_MotionParams.MaxAccelA;
			if (Max < AccelToUse * fda/d) AccelToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxAccelB;
			if (Max < AccelToUse * fdb/d) AccelToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxAccelC;
			if (Max < AccelToUse * fdc/d) AccelToUse = Max * d/fdc;
		}
	}

	*accel = AccelToUse;

	return 0;
}


// CAD-space step (in units of the FeedRateDistance path metric) used to
// probe the CAD->Actuator transform's local slope.  Small enough to stay
// within one GeoCorrect grid cell; large enough that counts-level double
// roundoff is negligible.
#define ACTUATOR_PROBE_STEP 1e-4

// Per actuator slot: |actuator units moved| per unit of CAD path length
// for motion in direction (dx..dv) at CAD point (px..pv), by transforming
// the point and a differentially stepped point.  Slots with zero
// effective Scale are reported as 0 (unused).
int CKinematics::ActuatorRatiosInDirection(double px, double py, double pz, double pa, double pb, double pc, double pu, double pv,
	double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *ratio)
{
	BOOL pure_angle;
	double Acts0[MAX_ACTUATORS], Acts1[MAX_ACTUATORS];
	int i;

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &m_MotionParams, &pure_angle);
	if (d <= 0.0)
	{
		for (i = 0; i < MAX_ACTUATORS; i++) ratio[i] = 0.0;
		return 0;
	}
	double h = ACTUATOR_PROBE_STEP / d;

	if (TransformCADtoActuators(px, py, pz, pa, pb, pc, pu, pv, Acts0)) return 1;
	if (TransformCADtoActuators(px + dx*h, py + dy*h, pz + dz*h, pa + da*h,
		                        pb + db*h, pc + dc*h, pu + du*h, pv + dv*h, Acts1)) return 1;

	for (i = 0; i < MAX_ACTUATORS; i++)
	{
		double Scale, Vel, Accel, Jerk;
		GetEffectiveActuatorLimits(&m_MotionParams, i, &Scale, &Vel, &Accel, &Jerk);
		if (Scale == 0.0)
			ratio[i] = 0.0;
		else
			ratio[i] = fabs(Acts1[i] - Acts0[i]) / (fabs(Scale) * ACTUATOR_PROBE_STEP);
	}
	return 0;
}

int CKinematics::MaxRateInDirectionAtPoint(double px, double py, double pz, double pa, double pb, double pc, double pu, double pv,
	double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *rate)
{
	if (!m_MotionParams.ActuatorLimits)
		return MaxRateInDirection(dx, dy, dz, da, db, dc, du, dv, rate);

	double ratio[MAX_ACTUATORS], FeedRateToUse = 1e99;
	if (ActuatorRatiosInDirection(px, py, pz, pa, pb, pc, pu, pv,
	                              dx, dy, dz, da, db, dc, du, dv, ratio)) return 1;
	for (int i = 0; i < MAX_ACTUATORS; i++)
	{
		double Scale, Vel, Accel, Jerk;
		if (ratio[i] <= 0.0) continue;
		GetEffectiveActuatorLimits(&m_MotionParams, i, &Scale, &Vel, &Accel, &Jerk);
		if (Vel / ratio[i] < FeedRateToUse) FeedRateToUse = Vel / ratio[i];
	}
	*rate = FeedRateToUse;
	return 0;
}

int CKinematics::MaxAccelInDirectionAtPoint(double px, double py, double pz, double pa, double pb, double pc, double pu, double pv,
	double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel)
{
	if (!m_MotionParams.ActuatorLimits)
		return MaxAccelInDirection(dx, dy, dz, da, db, dc, du, dv, accel);

	double ratio[MAX_ACTUATORS], AccelToUse = 1e99;
	if (ActuatorRatiosInDirection(px, py, pz, pa, pb, pc, pu, pv,
	                              dx, dy, dz, da, db, dc, du, dv, ratio)) return 1;
	for (int i = 0; i < MAX_ACTUATORS; i++)
	{
		double Scale, Vel, Accel, Jerk;
		if (ratio[i] <= 0.0) continue;
		GetEffectiveActuatorLimits(&m_MotionParams, i, &Scale, &Vel, &Accel, &Jerk);
		if (Accel / ratio[i] < AccelToUse) AccelToUse = Accel / ratio[i];
	}
	*accel = AccelToUse;
	return 0;
}


int CKinematics::MaxRapidAccelInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *accel)
{
	double Max,AccelToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);
	double fdu = fabs(du);
	double fdv = fabs(dv);

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &m_MotionParams, &pure_angle);

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
		if (fdy>0 && m_MotionParams.MaxRapidAccelY < AccelToUse * fdy / d) AccelToUse = m_MotionParams.MaxRapidAccelY * d / fdy;
		if (fdz>0 && m_MotionParams.MaxRapidAccelZ < AccelToUse * fdz / d) AccelToUse = m_MotionParams.MaxRapidAccelZ * d / fdz;
		if (fdu>0 && m_MotionParams.MaxRapidAccelU < AccelToUse * fdu / d) AccelToUse = m_MotionParams.MaxRapidAccelU * d / fdu;
		if (fdv>0 && m_MotionParams.MaxRapidAccelV < AccelToUse * fdv / d) AccelToUse = m_MotionParams.MaxRapidAccelV * d / fdv;

		if (fda>0)
		{
			Max = m_MotionParams.MaxRapidAccelA;
			if (Max < AccelToUse * fda/d) AccelToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxRapidAccelB;
			if (Max < AccelToUse * fdb/d) AccelToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxRapidAccelC;
			if (Max < AccelToUse * fdc/d) AccelToUse = Max * d/fdc;
		}
	}

	*accel = AccelToUse;

	return 0;
}

int CKinematics::MaxRapidJerkInDirection(double dx, double dy, double dz, double da, double db, double dc, double du, double dv, double *jerk)
{
	double Max,JerkToUse = 1e99;

	double fdx = fabs(dx);
	double fdy = fabs(dy);
	double fdz = fabs(dz);
	double fda = fabs(da);
	double fdb = fabs(db);
	double fdc = fabs(dc);
	double fdu = fabs(du);
	double fdv = fabs(dv);

	BOOL pure_angle;

	// compute total distance tool will move by considering both linear and angular movements  

	double d = FeedRateDistance(dx, dy, dz, da, db, dc, du, dv, &m_MotionParams, &pure_angle);

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
		if (fdy>0 && m_MotionParams.MaxRapidJerkY < JerkToUse * fdy / d) JerkToUse = m_MotionParams.MaxRapidJerkY * d / fdy;
		if (fdz>0 && m_MotionParams.MaxRapidJerkZ < JerkToUse * fdz / d) JerkToUse = m_MotionParams.MaxRapidJerkZ * d / fdz;
		if (fdu>0 && m_MotionParams.MaxRapidJerkU < JerkToUse * fdu / d) JerkToUse = m_MotionParams.MaxRapidJerkU * d / fdu;
		if (fdv>0 && m_MotionParams.MaxRapidJerkV < JerkToUse * fdv / d) JerkToUse = m_MotionParams.MaxRapidJerkV * d / fdv;

		if (fda>0)
		{
			Max = m_MotionParams.MaxRapidJerkA;
			if (Max < JerkToUse * fda/d) JerkToUse = Max * d/fda;
		}
		if (fdb>0)
		{
			Max = m_MotionParams.MaxRapidJerkB;
			if (Max < JerkToUse * fdb/d) JerkToUse = Max * d/fdb;
		}
		if (fdc>0)
		{
			Max = m_MotionParams.MaxRapidJerkC;
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


int CKinematics::ReadGeoTable(const wchar_t *name)
{
	double X,Y,Z;
	int row,col;
	
	AnyLinearTableValid = false;
	GeoTableValid=false;
	for (int i = 0; i < NGCODE_AXES; i++)
	{
		 if (ReadLinearTable(i, name, &LinearTableValid[i])) return 1;
		 AnyLinearTableValid |= LinearTableValid[i];
	}

	if (name[0]==0) return 0; // passing in no file turns off geocorrection
	
	FILE* f;
	_tfopen_s(&f, name, _T("rt,ccs=UTF-8"));

	if (!f)
	{
		if (AnyLinearTableValid) return 0;  // only Linears in play
		MessageBoxW(NULL, Translate("Unable to open Geometric Correction File : ") +  name, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		return 1;
	}

	int result = fwscanf(f,L"%d,%d",&NRows,&NCols);
		
	if (result != 2 || NRows < 2 || NRows > 4000 || NCols < 2 || NCols > 4000)
	{
		fclose(f);
		MessageBoxW(NULL, Translate("Invalid Geometric Correction File (NRows and NCols) : ") +  name, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		return 1;
	}

	result = fwscanf(f,L"%lf,%lf",&GeoSpacingX,&GeoSpacingY);
		
	if (result != 2)
	{
		fclose(f);
		MessageBoxW(NULL, Translate("Invalid Geometric Correction File (GeoSpacingX and GeoSpacingY) : ") +  name, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		return 1;
	}

	result = fwscanf(f,L"%lf,%lf",&GeoOffsetX,&GeoOffsetY);
		
	if (result != 2)
	{
		fclose(f);
		MessageBoxW(NULL, Translate("Invalid Geometric Correction File (GeoOffsetX and GeoOffsetY) : ") +  name, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
		return 1;
	}

	if (GeoTable) delete [] GeoTable;
	GeoTable = new CPT3D[NRows*NCols];

	for (int i=0; i<NRows*NCols; i++)
	{
		result = fwscanf(f,L"%d,%d,%lf,%lf,%lf",&row,&col,&X,&Y,&Z);

		if (result != 5 || row < 0 || row >= NRows || col < 0 || col >= NCols)
		{
			fclose(f);
			MessageBoxW(NULL, Translate("Invalid Geometric Correction File (invalid data value) : ") +  name, L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
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

int CKinematics::ReadLinearTable(int ia, const wchar_t *name, bool *valid)
{
	int col;
	double X;
	char AxisName[] = { 'X','Y','Z','A','B','C','U','V' };

	// Find location of last '.' in name
	CString Name = name;

	int i = Name.ReverseFind('.');

	if (i < 0)  // none?
		Name = Name + "_" + AxisName[ia];  // add to end
	else
		Name.Insert(i, (CString)"_" + AxisName[ia]);  // insert before suffix


	*valid = false;

	FILE* f;
	_tfopen_s(&f, Name, _T("rt,ccs=UTF-8"));

	if (!f)
	{
		return 0;
	}

	int result = fwscanf(f, L"%d", &NLinear[ia]);

	if (result != 1 || NLinear[ia] < 2 || NLinear[ia] > 10000)
	{
		fclose(f);
		MessageBoxW(NULL, Translate("Invalid Linear Table Correction File (NLinear) : ") + Name, L"KMotion", 
			MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
		return 1;
	}

	result = fwscanf(f, L"%lf", &LinearSpacings[ia]);

	if (result != 1)
	{
		fclose(f);
		MessageBoxW(NULL, Translate("Invalid Linear Table Correction File (LinearSpacing) : ") + Name, L"KMotion",
			MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
		return 1;
	}

	result = fwscanf(f, L"%lf", &LinearOffset[ia]);

	if (result != 1)
	{
		fclose(f);
		MessageBoxW(NULL, Translate("Invalid Linear Table Correction File (LinearOffset) : ") + Name, L"KMotion", 
			MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
		return 1;
	}

	if (LinearTables[ia]) delete[] LinearTables[ia];
	LinearTables[ia] = new double[NLinear[ia]];

	for (int i = 0; i < NLinear[ia]; i++) LinearTables[ia][i] = 0.0;


	for (int i = 0; i < NLinear[ia]; i++)
	{
		result = fwscanf(f, L"%d,%lf", &col, &X);

		if (result != 2 || col < 0 || col >= NLinear[ia])
		{
			fclose(f);
			MessageBoxW(NULL, Translate("Invalid Linear Table Correction File (invalid data value) : ") + Name, L"KMotion", 
				MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);
			return 1;
		}

		LinearTables[ia][col] = X;
	}

	fclose(f);

	*valid = true;
	return 0;

}

// Perform Linear correction (Screw mapping type)
int CKinematics::LinearCorrect(double x, double *cx, int ia)
{
	if (!LinearTableValid[ia]) return 1;

	int row = (int)floor((x - LinearOffset[ia]) / LinearSpacings[ia]);

	// stay within table

	if (row < 0) row = 0;
	if (row >= NLinear[ia] - 1) row = NLinear[ia] - 2;

	double GridX = row * LinearSpacings[ia] + LinearOffset[ia];

	if (LinearSpacings[ia] == 0.0) return 1;

	double fx = (x - GridX) / LinearSpacings[ia];

	double xL = LinearTables[ia][row];
	double xR = LinearTables[ia][row + 1];

	*cx = xL + (xR - xL) * fx;

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
	int NN=N*N1;
	double y;
	
	for (i=0; i<N; i++)   // reduce all the rows
	{
		ASSERT(i*N1+i >=0 && i*N1+i <NN);
		if (A[i*N1+i]==0.0)     // check if diagonal has a zero
		{
			l=i+1;            // it does, try and switch rows
			if (l>N-1) return 1; // return error if no more rows to switch
			while (A[l*N1+i]==0.0 && l<(N-1)) l++;
			for (m=0; m<=N; m++)  // swap the row
			{
				y=A[i*N1+m];
				ASSERT(i*N1+m >=0 && i*N1+m <NN);
				A[i*N1+m]=A[l*N1+m];
				ASSERT(l*N1+m >=0 && l*N1+m <NN);
				A[l*N1+m]=y;
			}
		}
		
		for (j=N; j>=i; j--)
		{
			ASSERT(i*N1+j >=0 && i*N1+j <NN);
			A[i*N1+j]/=A[i*N1+i];    // divide row so that it starts with 1
		}

		for (j=0; j<N; j++)
			if (j!=i && A[j*N1+i]!=0.0)
				for (k=N; k>=i; k--)
				{
					ASSERT(j*N1+k >=0 && j*N1+k <NN);
					A[j*N1+k]-=A[i*N1+k]*A[j*N1+i];
				}
	}
	return 0;
}

int CKinematics::Initialize()
{
	// Check if parameters exist in Kinematics.txt, if so update them and ask for confirmation value window yes/no, no abort
	GetParameter(L"MaxCorrPerIteration", &MaxCorrPerIteration);
	return 0;
}

int CKinematics::GetParameter(const wchar_t* key, double *v)
{
	FILE* f;
#ifdef _KMOTIONX
	_tfopen_s(&f, (CString)MainPath + L"/data/Kinematics.txt", _T("rt,ccs=UTF-8"));
#else
	_tfopen_s(&f, (CString)MainPath + L"\\Data\\Kinematics.txt", _T("rt,ccs=UTF-8"));
#endif

	if (!f) return 1;

	while (!feof(f))
	{
		wchar_t s[81], *p;
		fgetws(s, 80, f);

		p = wcsstr(s, key);

		if (p != NULL)
		{
			fclose(f);
			p = p + (int)wcslen(key);
			removeChar(p, ' ');  // remove spaces, tabs, or equal signs
			removeChar(p, '=');
			removeChar(p, '\t');

			int result = swscanf(p, L"%lf", v);
			if (result != 1) return 2;
			return 0;
		}
	}

	fclose(f);
	return 3;
}


void CKinematics::removeChar(wchar_t *s, int c) {

	int j, n = (int)wcslen(s);
	for (int i = j = 0; i<n; i++)
		if (s[i] != c)
			s[j++] = s[i];

	s[j] = '\0';
}
