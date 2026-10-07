#include "KMotionDef.h"

// Watches for EStop Input and disables Axes when detected

main()
{
	for (;;)  //loop forever
	{
		WaitNextTimeSlice();

		// if ESTOP present disable any enabled Axis ??
#define ESTOP_BIT 180
		if (ReadBit(ESTOP_BIT))
		{
			if (ch0->Enable) DisableAxis(0);  // axis still enabled?  - Disable it
			if (ch1->Enable) DisableAxis(1);  // axis still enabled?  - Disable it
			if (ch2->Enable) DisableAxis(2);  // axis still enabled?  - Disable it
		}
	}
}
