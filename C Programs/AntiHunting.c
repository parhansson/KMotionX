#include "KMotionDef.h"

main()
{
	float SaveKI=0.01;
	
	for (;;)
	{
		if (fast_fabs(ch0->last_vel) < 100.0 * TIMEBASE)
			ch0->I = 0;
		else
			ch0->I = SaveKI;
	}
}
