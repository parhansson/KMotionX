#include "KMotionDef.h"
#include "AdjustSoftLimits.c"

// tests functionality of adjusting softlimits feedhold based on 
// Velocity and Acceleration using the CheckDistToStop Calculations

main()
{
	for (;;)
	{
		WaitNextTimeSlice();
		CheckDistToStop(ch0);
		CheckDistToStop(ch1);
		CheckDistToStop(ch2);
		CheckDistToStop(ch3);
	}
}
