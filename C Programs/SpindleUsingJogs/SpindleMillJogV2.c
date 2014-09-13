#include "KMotionDef.h"

#define SPINDLEAXIS 6
#define FACTOR (1000/60.0) //1000 counts/sec / 1000 counts/rev = 1 RPS = 60 RPM
#define SPINDLECW_BIT 154
#define SPINDLECCW_BIT 155
#define SPEEDVAR 99
#define STATEVAR 98
#define KMVAR 1


// desired speed is passed from KMotionCNC in variable KMVAR
// save in user variable STATEVAR whether it was off, CW, or CCW (0,1,-1)
// save in user variable SPEEDVAR the last desired speed

main()
{
	float speed = *(float *)&persist.UserData[KMVAR];  // value stored is actually a float 
	float LastState = persist.UserData[STATEVAR];  // get last state 
	
	persist.UserData[SPEEDVAR] = persist.UserData[KMVAR];  // Always save the last desired speed 
	
	if (LastState==0)  
	{
		// if spindle is off and User Changes the speed 
		// just save the desired speed
		
		return 0;
	}
	
	// spindle is already on, so ramp to new speed
	Jog(SPINDLEAXIS,speed * FACTOR);
	printf("Jogging Spindle %f counts/sec\n",speed * FACTOR);
}
