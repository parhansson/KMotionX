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
	// spin down
	
	ClearBit(SPINDLECW_BIT);
	ClearBit(SPINDLECCW_BIT);
	Jog(SPINDLEAXIS,0);
	printf("Jogging Spindle Stop\n");
	persist.UserData[STATEVAR] = 0;  // remember we are Off
	while (!CheckDone(SPINDLEAXIS)) ;
}

