#include "KMotionDef.h"

// Simple test to full step small 5-wire unipolar stepper 
// motor using 4 Konnect Outputs.  Coils currents must be 0.25A or less
//
// Configure Axis0 as 
// No Input, No Output, and enable the Axis
// 
// Console command below should move motor
// Jerk0=40000;Accel0=400;Vel0=400;MoveRel0=1000
//
// Note: this is just a fun demo.  Full stepping, 
// no current regulation or chopping.
// Performance will be poor

main()
{
	int k;
	
	InitAux();
	AddKonnect(0,&VirtualBits,VirtualBitsEx);
	
	for (;;)
	{
		WaitNextTimeSlice();
		k=((int)ch0->Dest)&3;

		if (k==0)
			VirtualBits=0x6000;
		else if (k==1)
			VirtualBits=0xa000;
		else if (k==2)
			VirtualBits=0x9000;
		else
			VirtualBits=0x5000;
	}
}

