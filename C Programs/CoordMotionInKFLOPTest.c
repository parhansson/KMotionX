/* Performs basic Coordinated Motion within KFLOP
   Basic Trapazoidal Acceleration is calculated
   Copyright Dynomotion                        */

#include "KMotionDef.h"
#include "CoordMotionInKFLOPFunctions.c"

main()
{
	// Do an example xy coordinated Move
	
	DefineCoordSystem(0,1,-1,-1);  // just use x and y channels
	
	VM=10000; // Max allowed feed rate counts/sec
	A=4000; // Max acceleration counts/sec2
	
	
	OpenBuf();  // open the coordinated motion buffer
	
	x0=y0=0.0; // starting point
	x1=y1=10000.0; // ending point
	DoLinear();

	//now move back
	
	x0=y0=10000.0; // starting point
	x1=y1=0.0; // ending point
	DoLinear();

	// Do a half circle CCW
	axisx=0; axisy=1; axisz=-1;
	xc=-10000.0, yc=0.0;
	xradius=10000.0, yradius=10000.0;
	theta0=0.0, d_theta=PI;
	DoArc();

	// complete the circle CCW
	theta0=PI, d_theta=PI;
	DoArc();


	ExecBuf();  // launch the motion
	
	while (!CheckDoneBuf()) ;  // wait till finished
}

