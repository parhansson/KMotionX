/* Performs basic Coordinated Motion within KFLOP
   Basic Trapazoidal Acceleration is calculated
   Copyright Dynomotion                        */

#include "KMotionDef.h"
#include "CoordMotionInKFLOPFunctions.c"

#define number_sides 8
#define radius 1000.0
#define radius_circle (radius / cos(PI / number_sides))

#define SMALL 0.001

main()
{
	int i;
	double angle;

	// Do example xy coordinated Moves
	
	DefineCoordSystem(0,1,-1,-1);  // just use x and y channels
	
	VM=1000; // Max allowed feed rate counts/sec
	A=5000; // Max acceleration counts/sec2
	
	OpenBuf();  // open the coordinated motion buffer


	// do a ploygon
	x0=ch0->Dest; // starting point wherever we are 
	y0=ch1->Dest; 
	
	for (i=0; i<number_sides+1; i++)
	{
		angle=(i + 0.5) * 2.0 * PI / number_sides;
		x1=cos(angle) * radius_circle + radius;
		y1=sin(angle) * radius_circle;
		
		if (fast_fabs(x0-x1) > SMALL || fast_fabs(y0-y1) > SMALL)  // avoid near zero length segments
			DoLinear();
		x0=x1;
		y0=y1;
	}

	CS0_Flushed=TRUE;	// Allow Motion to run to end
	ExecBuf();  // launch the motion
	while (!CheckDoneBuf()) ;  // wait till finished
}

