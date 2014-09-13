#include "KMotionDef.h"

// Service Tangential Knife
//
// LED Bit 46 turns Tracking on and off

#define X 0
#define Y 1
#define A 2

#define DELTA 100 // number of counts to move before calculating angle

#define AFACTOR (180.0f/PI_F) // converts radians to axis A counts
#define TAU 0.01 // Low pass filter smoothing factor for A axis

float FindAngle(float x, float y);

int wraps=0;
float LastAngle=0;

main()
{
	double t0,t1,dx,dy,d,Lastx, Lasty;
	int LastReadBit=0;
	
	Lastx=chan[X].Dest;
	Lasty=chan[Y].Dest;
	
	for (;;) // loop forever
	{
		WaitNextTimeSlice();
		
		if (ReadBit(46))  // Tangential control on?
		{
			if (!LastReadBit)
			{
				DefineCoordSystem(X,Y,-1,-1);  // remove A axis from coordinate system
				LastAngle = chan[A].Dest * (1.0f/AFACTOR);
			}
		
			LastReadBit=1;

			// check if we moved far enough to determine an angle
			
			dx = (chan[X].Dest - Lastx);
			dy = (chan[Y].Dest - Lasty);
			
			d = dx*dx+dy*dy;
			
			if (d > DELTA*DELTA)
			{
				// yes we moved enough, compute new angle

				t0=Time_sec();
				LastAngle = FindAngle(dx,dy);
				t1=Time_sec();
				
//				printf("dx=%f dy=%f theta=%f time=%f\n",dx,dy,LastAngle,(t1-t0)*1e6);
				
				MoveExp(A,LastAngle*AFACTOR,TAU);
			
				Lastx=chan[X].Dest;  // save new position
				Lasty=chan[Y].Dest;
			}
		}
		else
		{
			if (LastReadBit)  // just switched off?
			{
				DefineCoordSystem(X,Y,-1,2);  // put A axis back in coordinate system
				Move(A,LastAngle*AFACTOR);  // finish any Move completely
			}
			
			LastReadBit=0;

			// not on, just track current position
			Lastx=chan[X].Dest;  // save new position
			Lasty=chan[Y].Dest;
		}
	}
}

// compute angle in most accurate manner that handles 360 degrees using 32-bit floats
// compute the number of wraps that would make us closeset to the last value


float FindAngle(float x, float y)
{
	float theta;
	
	if (fast_fabsf(x) < fast_fabsf(y))
		theta = atan2f(y,x);
	else
		theta = PI_2F-atan2f(x,y);
		
	// Diff between expected position and position with no wraps
	// Diff nearest zero, offset by 1024 wraps to avoid requiring floor()
	
	wraps = (int)(((LastAngle-theta)+(1024.0f*TWO_PI_F+PI_F))*(1.0f/TWO_PI_F))-1024;
	
	// factor in wraparounds
	return theta + wraps*TWO_PI_F;
}
