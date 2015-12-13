#include "KMotionDef.h"

main()
{
	for(;;)
	{
		WaitNextTimeSlice();
		ServiceSpindleCount();
	}
}

#define SPINDLE_PULSE_BIT 46
#define SPINDLE_AXIS 5
#define COUNTS_PER_REV2 1000
void ServiceSpindleCount(void)
{
	static int LastState=0;
	int NewState;
	
	NewState=ReadBit(SPINDLE_PULSE_BIT);
	
	if (NewState != LastState)
	{
		chan[SPINDLE_AXIS].Position+=COUNTS_PER_REV2;
		LastState=NewState;
	}
}
