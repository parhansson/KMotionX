#include "KMotionDef.h"
#include "CoordMotionInKFLOPFunctions.c"

// Creates a Trajectory Planned Accelerated, then Constant Feedrate path around an ellipse.
//
// This turns out to be a relatively complex problem involving elliptic integrals without
// a solution involving elementary functions.  So we use a numerical method involving small
// straight line segments instead.
//
// The ellipse is broken down into MAXN segments (MAXN+1 points - 1000 is probably a good number),

// the positions and distances between points is then computed and summed to get the total path length.
// How many points are needed for acceleration and deceleration are then
// searched for to determine how many segments are needed for acceleration, deceleration,
// and constant velocity.  If the acceleration or deceleration distance is not an exact
// number of segments long, the next number of segments is used and the acceleration is
// adjusted (slightly) so that the acceleration occurs over an exact number of segments.
// If the total length is not sufficient to allow for full acceleration then the maximum
// possible velocity is determined based on the beginning distance or ending distance
// whichever is smaller. 
//
// A table of points along the elliptic arc is first computed.  The points are created by dividing
// theta into equal deltas.  Along with the points the distance between points and total distance
// is tabulated.
//
// The program creates a coordinated motion buffer of the trajectory.  A motion to the starting point
// on the ellipse is first performed.  In order to facilitate a tangential knife system the direction
// of each segment is computed and included as part of the motion as the A axis.  After moving to the
// xy starting point of the ellipse an additional move to rotate the A to be tangent to the first segment
// is performed.
//
// It is assumed that the acceleration along the path is the major constraint of the system.  The centripetal 
// acceleration of going around tight curves is not considered.  it is also assumed that the A Axis
// (tangential  knife) is not a limiting factor.

 
 

#define MAXN 1000  // define number of segments along elliptic arc

#define ThetaStart 0.0  // setup to do a full ellipse
#define ThetaEnd TWO_PI 

#define RESX 300.0  // counts/inch
#define RESY 300.0  // counts/inch
#define RESZ 300.0  // counts/inch
#define RESA (3200.0/TWO_PI)  // counts/inch

double pointx[MAXN+1];  // tables to save points and info for points around the arc
double pointy[MAXN+1];
double dist[MAXN+1];
double sum[MAXN+1];

void Ellipse(double theta, double *x, double *y);
double ArcLength(double T0, double T1, int n);
void DoSegment(int i,double *V0, double A);
float FindAngle(float x, float y);
void DoEllipse(void);

float LastAngle;

double Rx=24.0;
double Ry=48.0;
double Cx=50.0;
double Cy=40.0;

double MaxA=80.0;  // max allowed acceleration along path in inches/sec
double MaxV=3200/60.0;  // maximum allowed speed along path (feed rate) in inches/sec

FILE *f;

main()
{
	VM=MaxV*RESX; // Max allowed feed rate counts/sec
	A=MaxA*RESX; // Max acceleration counts/sec2
	
	LastAngle=ch3->Dest/RESA;  // initialize last angle to where we currently are

	OpenBuf();  // open the coordinated motion buffer

	DoEllipse();  // Trajectory Plan the ellipse

	CS0_Flushed=TRUE;	// Allow Motion to run to end
	ExecBuf();  // launch the motion
	
	while (!CheckDoneBuf()) ;  // wait till finished
}

void DoEllipse(void)
{
	int i,ibeg,iend;
	
	double dist_beg,dist_end,V0,ta,da,length,Time0,Time1,MaxAp,MaxVp,angle;

	Time0=Time_sec();
	
	// first compute total length of path and build tables of info
	length = ArcLength(ThetaStart,ThetaEnd,MAXN);
	
	Time1=Time_sec();

	printf("Length = %.9f Compute Time %.0f us\n",length, (Time1-Time0)*1e6);
	
    // time to achieve max vel
    ta = MaxV/MaxA;

    // dist to achieve max vel
    da = (0.5 * MaxA * ta) * ta;

   	printf("Accel Time %f Accel dist %f\n",ta,da);


	// first move from where we are to starting point on ellipse
	
	x0=ch0->Dest; // starting point is where we currently are
	y0=ch1->Dest; 
	z0=ch2->Dest; 
	a0=ch3->Dest; 
	x1 = pointx[0]*RESX;   
	y1 = pointy[0]*RESY;
	z1=z0;  // keep z and a the same
	a1=a0;  
	DoLinear();

	// now rotate knife to be at the right angle for the first segment
	angle = FindAngle(pointx[1]-pointx[0],pointy[1]-pointy[0]);
	
	printf("angle=%f %f %f\n",angle,pointx[1]-pointx[0],pointy[1]-pointy[0]);
	x0=x1; // xyz doesn't move
	y0=y1;
	z0=z1;
	a0=a1;
	a1=angle*RESA;  // rotate knife to proper angle
	DoLinear();
	
    // search to find segments needed to accellerate/decelerate
    // searches forward and backward throug the segments simultaneously
    // until either the begiining and ending are sufficient to accel/deccel
    // or we meet in the middle and run out of distance
    ibeg=0;
	iend=MAXN;
   	dist_beg = dist_end = 0.0;
    do
    {
    	if (dist_beg < dist_end)
			ibeg++;
		else
			iend--;

    	dist_beg = sum[ibeg];
		dist_end = sum[MAXN]-sum[iend];

//		printf("ibeg=%d iend=%d dist beg %.3f dist_end %.3f\n",ibeg,iend,dist_beg,dist_end);
    }
    while ((dist_beg<da || dist_end<da) && ibeg<iend);
    
    // if distance available is less than required accel distance
    // then reduce the max velocity to what is achievable
    
    if (ibeg>=iend)
    {
    	if (dist_beg<dist_end)
    		MaxVp=sqrt(2.0*MaxA*dist_beg);
    	else
    		MaxVp=sqrt(2.0*MaxA*dist_end);
    }
    else
    {
    	MaxVp=MaxV;
    }
    
    // adjust the acceleration to accelerate in exactly the beginning distance
    MaxAp = 0.5*MaxVp*MaxVp/dist_beg;

    // create all the segments

	f=fopen("C:\\temp\\kflopdata.txt","wt");

    V0=0.0;  // start from stop
	for (i=1;i<=ibeg;i++)
		DoSegment(i,&V0,MaxAp);  // accel segments

	for (;i<=iend;i++)
		DoSegment(i,&V0,0.0);  // constant velocity

    // adjust the acceleration to accelerate in exactly the ending distance
    MaxAp = 0.5*MaxVp*MaxVp/dist_end;
	for (;i<=MAXN;i++)
		DoSegment(i,&V0,-MaxAp);  // decel segments

    fclose(f);
}

void DoSegment(int i,double *V0, double A)
{
	double d,t,angle;
	static double T0=0.0;
	
	d = dist[i];  // get distance for this segment
	
	// determine how much time will be required to go this distance
	// at a starting Velocity of V0 while accelerating at A
	
	if (A!=0)
	{
		// must solve quadratic to determine time at end
		t = (-(*V0) + sqrt((*V0)*(*V0) + 2.0f*A*d))/A;
	}
	else
	{
		// constant velocity case is simple
		t = d/(*V0);
	}

	// determine the direction of the segment so we can control the tagential knife
	angle = FindAngle(pointx[i]-pointx[i-1],pointy[i]-pointy[i-1]);
	
	x0=x1; // this segment starts at the beg of previous
	y0=y1;
	a0=a1;
	
	x1=pointx[i]*RESX;
	y1=pointy[i]*RESY;
	a1=angle*RESA;
	InterpolateLinear(0.0, 0.5*A/d, *V0/d, 0.0, t);

	*V0 += t * A;  // new final velocity

	T0 += t;

	fprintf(f,"Time = %.9f dt= %.9f Velocity %.9f xy= %13f %13f angle= %f\n",T0, t, *V0, pointx[i], pointy[i], angle);
}



// compute arc length from theta0 to theta1 by
// dividing arc into n linear segments and summing them

double ArcLength(double T0, double T1, int n)
{
	int i;
	double dt = (T1-T0)/n;
	double dx,dy;
	
	Ellipse(T0,&pointx[0],&pointy[0]);
	sum[0]=0.0;
	
	for (i=1;i<=n;i++)
	{
		Ellipse(T0+dt*i,&pointx[i],&pointy[i]);
		dx=pointx[i]-pointx[i-1];
		dy=pointy[i]-pointy[i-1];
		dist[i] = sqrt(dx*dx+dy*dy);
		sum[i] = sum[i-1] + dist[i-0];
	}
	return sum[n];
}

// compute a point on ellipse from theta
void Ellipse(double theta, double *x, double *y)
{
	*x = Rx * cos(theta) + Cx; 
	*y = Ry * sin(theta) + Cy; 
}


// compute angle in most accurate manner that handles 360 degrees using 32-bit floats
// compute the number of wraps that would make us closeset to the last value
float FindAngle(float x, float y)
{
	int wraps;
	float theta;
	
	if (fast_fabsf(x) < fast_fabsf(y))
		theta = atan2f(y,x);
	else
		theta = PI_2F-atan2f(x,y);
		
	// Diff between expected position and position with no wraps
	// Diff nearest zero, offset by 1024 wraps to avoid requiring floor()
	
	wraps = (int)(((LastAngle-theta)+(1024.0f*TWO_PI_F+PI_F))*(1.0f/TWO_PI_F))-1024;
	
	// factor in wraparounds
	LastAngle=theta + wraps*TWO_PI_F;

	printf("lastangle=%f theta=%f wraps=%d\n",LastAngle*180.0/PI,theta*180.0/PI,wraps);
	
	return LastAngle;
}
