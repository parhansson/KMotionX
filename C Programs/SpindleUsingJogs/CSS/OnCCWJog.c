#include "KMotionDef.h"

#include "MySpindleDefs.h"

int   *css_mode = &persist.UserData[PC_COMM_CSS_MODE];			// Mode 1=Normal RPM mode. 2=CSS

// desired speed is passed from KMotionCNC in variable KMVAR
// save in user variable STATEVAR whether it was off, CW, or CCW (0,1,-1)
// save in user variable SPEEDVAR the last desired speed

main()
{
	float speed = *(float *)&persist.UserData[SPEEDVAR];  // value stored is actually a float 
	float LastState = persist.UserData[STATEVAR];  // get last state 
	
	if (LastState==1)  
	{
		// if spindle was CW now we want CCW 
		// spin down
		
		ClearBit(SPINDLECW_BIT);
		ClearBit(SPINDLECCW_BIT);
		Jog(SPINDLEAXIS,0);
		while (!CheckDone(SPINDLEAXIS)) ;
	}
	
	// turn spindle on CCW and ramp to new speed
	SetBit(SPINDLECCW_BIT);
	
	if (*css_mode != 2)
	{
		// spindle is already on, so ramp to new speed
		Jog(SPINDLEAXIS,speed * FACTOR);
		printf("Jogging Spindle %f counts/sec\n",speed * FACTOR);
	}	
	persist.UserData[STATEVAR] = -1;  // remember we are CCW
}

