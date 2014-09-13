#include "KMotionDef.h"

main()
{
	for (;;)  //loop forever
	{
		WaitNextTimeSlice();

		// any axis disabled ??
		if (!ch0->Enable || !ch1->Enable || !ch2->Enable)
		{
			if (ch0->Enable) DisableAxis(0);  // axis still enabled?  - Disable it
			if (ch1->Enable) DisableAxis(1);  // axis still enabled?  - Disable it
			if (ch2->Enable) DisableAxis(2);  // axis still enabled?  - Disable it
		}
	}
}
