#include "KMotionDef.h"

#define SPINDLEAXIS 4
#define FACTOR (-500.0/2007) // -500 counts = 2007 RPM

// desired speed is passed in variable 1
// save in user variable 99 the last speed setting
// save in user variable 98 whether it was off, CW, or CCW (0,1,-1)
// save in user variable 97 the last desired speed


main()
{
	float speed = *(float *)&persist.UserData[1];  // value stored is actually a float 
	float LastSpeed = *(float *)&persist.UserData[99];  // get last speed setting 
	float LastState = persist.UserData[98];  // get last state 
	
	if (LastState==0)  
	{
		// if spindle is off and User Changes the speed 
		// just save the desired speed
		
		persist.UserData[97] = persist.UserData[1];
		return 0;
	}
	
	// spindle is already on, so ramp to new speed
	
	if (LastSpeed != speed)
	{
		LastSpeed = speed;
		Jog(4,LastSpeed * FACTOR);
	}
	
	*(float *)&persist.UserData[99] = LastSpeed;
}
