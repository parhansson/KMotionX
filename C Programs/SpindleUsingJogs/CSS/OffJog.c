#include "KMotionDef.h"

#include "MySpindleDefs.h"


// desired speed is passed from KMotionCNC in variable KMVAR
// save in user variable STATEVAR whether it was off, CW, or CCW (0,1,-1)
// save in user variable SPEEDVAR the last desired speed

main()
{
	// spin down
	
	ClearBit(SPINDLECW_BIT);
	ClearBit(SPINDLECCW_BIT);
	Jog(SPINDLEAXIS,0);
	printf("Jogging Spindle Stop\n");
	persist.UserData[STATEVAR] = 0;  // remember we are Off
	while (!CheckDone(SPINDLEAXIS)) ;
}

