#include "KMotionDef.h"

#define SPINDLEAXIS 4
#define FACTOR (-500.0/2007) // -500 counts = 2007 RPM

// desired speed is passed in variable 1
// save in user variable 99 the last speed setting
// save in user variable 98 whether it was off, CW, or CCW (0,1,-1)
// save in user variable 97 the last desired speed


main()
{
	float speed = *(float *)&persist.UserData[97];  // value stored is actually a float 
	float LastSpeed = *(float *)&persist.UserData[99];  // get last speed setting 
	float LastState = persist.UserData[98];  // get last state 
	
	{
		// if spindle was CW or CCW 
		// spin down
		
		ClearBit(154);
		ClearBit(155);
		Jog(4,0);
		while (!CheckDone(SPINDLEAXIS)) ;

		LastSpeed = 0.0;
		*(float *)&persist.UserData[99] = LastSpeed;  // save the last speed
		persist.UserData[98] = 0;  // remember we are Off
	}
}

