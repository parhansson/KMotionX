/* Performs basic Coordinated Motion within KFLOP
   Basic Trapazoidal Acceleration is calculated
   Copyright Dynomotion                        */

#include "KMotionDef.h"

extern PARAMETRIC_COEFF *LastCoordSystem0;
extern PARAMETRIC_COEFF *ParametricCoeffs;
extern double CS0_TimeExecuted;
extern double CS0_t;
extern PARAMETRIC_COEFF *CoordSystem0;
extern int ParametricIndex;

void InterpolateLinear(double a, double b, double c, double d, double t);
void InterpolateArc(double a, double b, double c, double d, double t);
void DoLinear(void);
void DoArc(void);
void tp_calc_seg_trip_states(int Arc);
double CalcLinearLength(void);
double CalcArcLength(void);
    
void ExecBuf(void);
void OpenBuf(void);
    
double x0=0, y0=0, z0=0, a0=0, b0=0, c0=0, u0=0, v0=0;
double x1=0, y1=0, z1=0, a1=0, b1=0, c1=0, u1=0, v1=0;

double V0=0; // initial velocity
double V1=0; // final velocity
double VM=10000; // Max allowed feed rate counts/sec
double A=100000; // Max acceleration counts/sec2

char axisx, axisy, axisz;
double xc, yc;
double xradius, yradius;
double theta0, d_theta;


// do calcs for a linear motion
void DoLinear(void)
{
	tp_calc_seg_trip_states(0);
}

// do calcs for an arc motion
void DoArc(void)
{
	tp_calc_seg_trip_states(1);
}

// calculate the trip states (three 2nd order polynomials)
// for a segment given the initial and ending velocities
// X = length of physical motion
void tp_calc_seg_trip_states(int Arc)
{
    double ta,td,da,dd,tc,cruise,bcoeff,dcoeff,X,iX,VMp;
    
    if (Arc)
		X=CalcArcLength();
	else 
		X=CalcLinearLength();
		
		
	if (X==0.0) return;
		
	iX=1.0/X; // reciprical

    // time to achieve max vel
    ta = (VM - V0)/A;

    // dist to achieve max vel
    da = (V0 + 0.5 * A * ta) * ta;

    // time to decel from max vel
    td = (VM - V1)/A;

    // dist to decel from max vel
    dd = (V1 + 0.5 * A * td) * td;

    if (X > da + dd)
    {
        // we need const vel phase
        tc = (X-da-dd)/VM;
        VMp = VM;
    }
    else
    {
        // must solve to determine time at highest vel
        VMp = sqrt((A*V1*V1+A*V0*V0+2.0f*A*A*X)/(A+A));

        ta = (VMp-V0)/A;
        td = (VMp-V1)/A;
        tc=0.0f;
    }
    
    cruise=0.5*A*ta*ta + V0*ta;  // position to begin cruise
    bcoeff = 0.5*A*iX;
    dcoeff = (cruise + VMp*tc)*iX;

	if (Arc)
	{
		InterpolateArc(0, bcoeff, V0 *iX,        0,   ta);	// phase 0 - Accelerate
		InterpolateArc(0,      0, VMp*iX, cruise*iX,  tc);	// phase 1 - Const vel
		InterpolateArc(0,-bcoeff, VMp*iX, dcoeff,     td); 	// phase 2 - Decel
	}
	else
	{
		InterpolateLinear(0, bcoeff, V0* iX,        0,   ta);	// phase 0 - Accelerate
		InterpolateLinear(0,      0, VMp*iX, cruise*iX,  tc);	// phase 1 - Const vel
		InterpolateLinear(0,-bcoeff, VMp*iX, dcoeff,     td); 	// phase 2 - Decel
	}
}

double CalcLinearLength(void)
{
	double dx=(x1-x0);
	double dy=(y1-y0);
	double dz=(z1-z0);
	double da=(a1-a0);
	double db=(b1-b0);
	double dc=(c1-c0);
	double du=(u1-u0);
	double dv=(v1-v0);
	return sqrt(dx*dx+dy*dy+dz*dz+da*da+db*db+dc*dc+du*du+dv*dv);
}

// calculates path length of arc (helix)
// (doesn't handle ellipse case)
double CalcArcLength(void)
{
	double dxy=d_theta * xradius;
	double dz=(z1-z0);
	return sqrt(dxy*dxy+dz*dz);
}


// create a linear interpolated motion segment
//
// given initial (x,y,z) and final (x,y,z)
// 
// and 3rd order parametric equation where
//        
//        3      2
// p = a t  + b t  + c t + d
//
// p is the fraction along the path as a function of time.  
// p = 0 produces point 0, p=1 produces point 1 
// This piece of path normally only spans a portion of
// the entire path from the inital to final point.
//
// ie: t might vary from 0.0->0.1 where p might vary from 
// 0.5->0.6
//
// t is the length of time for the segment 
// (zero is defined as beg of segment)

void InterpolateLinear(double a, double b, double c, double d, double t)
{                                         
    PARAMETRIC_COEFF *pCoordSys = &ParametricCoeffs[ParametricIndex];
	
	double dx = x1-x0;
	double dy = y1-y0;
	double dz = z1-z0;
	double da = a1-a0;
	double db = b1-b0;
	double dc = c1-c0;
	double du = u1-u0;
	double dv = v1-v0;

	pCoordSys->a = a; // define coord system as parametric from 0->1
	pCoordSys->b = b;	
	pCoordSys->c = c;	
	pCoordSys->d = d;	
	pCoordSys->t = t;	  
	
   	// linear interpolated from coord system 0    (x = c*p+d)
	
	pCoordSys->trajectory_mode = TRAJECTORY_LINEAR;

	pCoordSys->x_axis = (unsigned char)CS0_axis_x;
	pCoordSys->y_axis = (unsigned char)CS0_axis_y;
	pCoordSys->z_axis = (unsigned char)CS0_axis_z;
	pCoordSys->a_axis = (unsigned char)CS0_axis_a;
	pCoordSys->b_axis = (unsigned char)CS0_axis_b;
	pCoordSys->c_axis = (unsigned char)CS0_axis_c;
	pCoordSys->u_axis = (unsigned char)CS0_axis_u;
	pCoordSys->v_axis = (unsigned char)CS0_axis_v;

	if (CS0_axis_x >= 0)
	{
       	// linear interpolated from coord system 0    (x = c*p+d)
		pCoordSys->X.c = dx;
		pCoordSys->X.d = x0;
	}
	
	if (CS0_axis_y >= 0)
	{
       	// linear interpolated from coord system 0    (y = c*p+d)
		pCoordSys->Y.c = dy;
		pCoordSys->Y.d = y0;
	}
	
	if (CS0_axis_z >= 0)
	{
       	// linear interpolated from coord system 0    (z = c*p+d)
		pCoordSys->Z.c = dz;
		pCoordSys->Z.d = z0;
	}

	if (CS0_axis_a >= 0)
	{
       	// linear interpolated from coord system 0    (a = c*p+d)
		pCoordSys->A.c = da;
		pCoordSys->A.d = a0;
	}

	if (CS0_axis_b >= 0)
	{
       	// linear interpolated from coord system 0    (b = c*p+d)
		pCoordSys->B.c = db;
		pCoordSys->B.d = b0;
	}

	if (CS0_axis_c >= 0)
	{
       	// linear interpolated from coord system 0    (c = c*p+d)
		pCoordSys->C.c = dc;
		pCoordSys->C.d = c0;
	}

	if (CS0_axis_u >= 0)
	{
       	// linear interpolated from coord system 0    (u = c*p+d)
		pCoordSys->U.c = du;
		pCoordSys->U.d = u0;
	}

	if (CS0_axis_v >= 0)
	{
       	// linear interpolated from coord system 0    (v = c*p+d)
		pCoordSys->V.c = dv;
		pCoordSys->V.d = v0;
	}

	if (++ParametricIndex == MAX_SEGMENTS) ParametricIndex=0;   // wrap the buffer              
	ParametricCoeffs[ParametricIndex].trajectory_mode = TRAJECTORY_OFF; // keep terminated
}

// create a circular (or helix) interpolated motion segment
//
// given initial (x,y,z) and final (x,y,z)
// 
// and 3rd order parametric equation where
//        
//        3      2
// p = a t  + b t  + c t + d
//
// p is distance along the path as a function of time.  
// This piece of path normally only spans a portion of
// the entire path from the inital to final point.
//
// ie: t might vary from 0.0->0.1 where p might vary from 
// 1.3->1.6 and the entire distance from initial to final
// points might be 2 inches
//
// t is the length of time for the segment 
// (zero is defined as beg of segment)

void InterpolateArc(double a, double b, double c, double d, double t)
{      
    PARAMETRIC_COEFF *pCoordSys = &ParametricCoeffs[ParametricIndex];
	
	double dz = z1-z0;
	double da = a1-a0;
	double db = b1-b0;
	double dc = c1-c0;
	
	pCoordSys->a = a; // define coord system as parametric from 0->1
	pCoordSys->b = b;	
	pCoordSys->c = c;	
	pCoordSys->d = d;	
	pCoordSys->t = t;	  
	
	// circular interpolated from coord system 0  (x = c*sin(p*a+b)+d)
	
	pCoordSys->trajectory_mode = TRAJECTORY_CIRCULAR;
	
	pCoordSys->x_axis = (unsigned char)axisx;
	pCoordSys->y_axis = (unsigned char)axisy;
	pCoordSys->z_axis = (unsigned char)axisz;
	pCoordSys->a_axis = CS0_axis_a;
	pCoordSys->b_axis = CS0_axis_b;
	pCoordSys->c_axis = CS0_axis_c;
	pCoordSys->u_axis = 0xff;
	pCoordSys->v_axis = 0xff;

	pCoordSys->X.d = xc;
	pCoordSys->X.c = xradius;
	pCoordSys->X.b = theta0+PI_2F;  // make x -> cosine
	pCoordSys->X.a = d_theta;

	pCoordSys->Y.d = yc;
	pCoordSys->Y.c = yradius;
	pCoordSys->Y.b = theta0;
	pCoordSys->Y.a = d_theta;

	if (CS0_axis_z >= 0)
	{
       	// linear interpolated   (z = c*p+d)
		pCoordSys->Z.c = dz;
		pCoordSys->Z.d = z0;
	}
	
	if (CS0_axis_a >= 0)
	{
       	// linear interpolated   (a = c*p+d)
		pCoordSys->A.c = da;
		pCoordSys->A.d = a0;
	}
	
	if (CS0_axis_b >= 0)
	{
       	// linear interpolated   (b = c*p+d)
		pCoordSys->B.c = db;
		pCoordSys->B.d = b0;
	}
	
	if (CS0_axis_c >= 0)
	{
       	// linear interpolated   (c = c*p+d)
		pCoordSys->C.c = dc;
		pCoordSys->C.d = c0;
	}

	if (++ParametricIndex == MAX_SEGMENTS) ParametricIndex=0;   // wrap the buffer              
	ParametricCoeffs[ParametricIndex].trajectory_mode = TRAJECTORY_OFF; // keep terminated
}


void ExecBuf(void)
{
	if (LastCoordSystem0)  // verify where we should begin/resume execution is valid
	{
		if (LastCoordSystem0 == &ParametricCoeffs[0])  // if starting from the beginning reset executed time
    		CS0_TimeExecuted=0.0;

    	CS0_t=0.0;
		CS0_TimeBase=TIMEBASE;
	    CoordSystem0 = LastCoordSystem0;  
   		CS0_Flushed=TRUE;	// Allow Motion to run to end
	}
}

void OpenBuf(void)
{  
	CoordSystem0 = NULL;
	LastCoordSystem0=&ParametricCoeffs[0];
	ParametricIndex=0;
    ParametricCoeffs[ParametricIndex].trajectory_mode = TRAJECTORY_OFF; // keep terminated
}