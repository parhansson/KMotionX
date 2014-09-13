#include "KMotionDef.h"

#define NULL 0

extern PARAMETRIC_COEFF *LastCoordSystem0;
extern PARAMETRIC_COEFF *ParametricCoeffs;
extern double CS0_TimeExecuted;
extern double CS0_t;
extern PARAMETRIC_COEFF *CoordSystem0;
extern int ParametricIndex;

void InterpolateLinear(float a, float b, float c, float d, float t);
    
void ExecBuf(void);
void OpenBuf(void);
    


float x0, y0, z0, a0, b0, c0, u0, v0;
float x1, y1, z1, a1, b1, c1, u1, v1;


main()
{
	// Do an example xy coordinated Move
	
	DefineCoordSystem(0,1,-1,-1);  // just use x and y channels
	
	OpenBuf();  // open the coordinated motion buffer
	
	x0=y0=0.0; // starting point
	x1=y1=1.0; // ending point

	// put in the buffer constant speed for 10 seconds 
	InterpolateLinear(0,0,0.1,0,10);

	//now move back
	
	x0=y0=1.0; // starting point
	x1=y1=0.0; // ending point

	// put in the buffer constant speed for 10 seconds 
	InterpolateLinear(0,0,0.1,0,10);

	ExecBuf();  // launch the motion
	
	while (!CheckDoneBuf()) ;  // wait till finished
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

void InterpolateLinear(float a, float b, float c, float d, float t)
{                                         
    PARAMETRIC_COEFF *pCoordSys = &ParametricCoeffs[ParametricIndex];
	
	float dx = x1-x0;
	float dy = y1-y0;
	float dz = z1-z0;
	float da = a1-a0;
	float db = b1-b0;
	float dc = c1-c0;
	float du = u1-u0;
	float dv = v1-v0;

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

	pCoordSys->X.c = dx;
	pCoordSys->X.d = x0;

	if (CS0_axis_y >= 0)
	{
       	// linear interpolated from coord system 0    (x = c*p+d)
		
		pCoordSys->Y.c = dy;
		pCoordSys->Y.d = y0;
	}
	
	if (CS0_axis_z >= 0)
	{
       	// linear interpolated from coord system 0    (x = c*p+d)
		
		pCoordSys->Z.c = dz;
		pCoordSys->Z.d = z0;
	}

	if (CS0_axis_a >= 0)
	{
       	// linear interpolated from coord system 0    (x = c*p+d)
		
		pCoordSys->A.c = da;
		pCoordSys->A.d = a0;
	}

	if (CS0_axis_b >= 0)
	{
       	// linear interpolated from coord system 0    (x = c*p+d)
		
		pCoordSys->B.c = db;
		pCoordSys->B.d = b0;
	}

	if (CS0_axis_c >= 0)
	{
       	// linear interpolated from coord system 0    (x = c*p+d)
		
		pCoordSys->C.c = dc;
		pCoordSys->C.d = c0;
	}

	if (CS0_axis_u >= 0)
	{
       	// linear interpolated from coord system 0    (x = c*p+d)
		
		pCoordSys->U.c = du;
		pCoordSys->U.d = u0;
	}

	if (CS0_axis_v >= 0)
	{
       	// linear interpolated from coord system 0    (x = c*p+d)
		
		pCoordSys->V.c = dv;
		pCoordSys->V.d = v0;
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
	}
}

void OpenBuf(void)
{  
	CoordSystem0 = NULL;
	LastCoordSystem0=&ParametricCoeffs[0];
	ParametricIndex=0;
    ParametricCoeffs[ParametricIndex].trajectory_mode = TRAJECTORY_OFF; // keep terminated
}