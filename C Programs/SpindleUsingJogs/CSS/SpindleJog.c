#include "KMotionDef.h"

#include "MySpindleDefs.h"

int   *css_mode = &persist.UserData[PC_COMM_CSS_MODE];			// Mode 1=Normal RPM mode. 2=CSS

// desired speed is passed from KMotionCNC in variable KMVAR
// save in user variable STATEVAR whether it was off, CW, or CCW (0,1,-1)
// save in user variable SPEEDVAR the last desired speed

main()
{
	float speed = *(float *)&persist.UserData[KMVAR];  // value stored is actually a float 
	float LastState = persist.UserData[STATEVAR];  // get last state 
	
	persist.UserData[SPEEDVAR] = persist.UserData[KMVAR];  // Always save the last desired speed 
	
	if (LastState==0 || *css_mode == 2)  
	{
		// if spindle is off (or CSS mode) and User Changes the speed 
		// just save the desired speed
		
		return 0;
	}
	
	// spindle is already on, so ramp to new speed
	Jog(SPINDLEAXIS,speed * FACTOR);
	printf("Jogging Spindle %f counts/sec\n",speed * FACTOR);
}
