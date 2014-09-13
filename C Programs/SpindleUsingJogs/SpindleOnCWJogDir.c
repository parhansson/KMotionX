#include "KMotionDef.h"

#define SPINDLEAXIS 4
#define FACTOR (2048/60.0) //1000 counts/sec / 100 counts/rev = 1 RPS = 60 RPM
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
	float speed = *(float *)&persist.UserData[SPEEDVAR];  // value stored is actually a float 
	float LastState = persist.UserData[STATEVAR];  // get last state 
	
	if (LastState==-1)  
	{
		// if spindle was CCW now we want CW 
		// spin down
		
		ClearBit(SPINDLECW_BIT);
		ClearBit(SPINDLECCW_BIT);
		Jog(SPINDLEAXIS,0);
		while (!CheckDone(SPINDLEAXIS)) ;
	}
	
	// turn spindle on CW and ramp to new speed
	SetBit(SPINDLECW_BIT);
	
	// spindle is already on, so ramp to new speed
	Jog(SPINDLEAXIS,speed * FACTOR);
	printf("Jogging Spindle %f counts/sec\n",speed * FACTOR);

	persist.UserData[STATEVAR] = 1;  // remember we are CW
}

