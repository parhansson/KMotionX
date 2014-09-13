#include "KMotionDef.h"

main()
{
	for (;;)					// repeat forever
	{
		WaitNextTimeSlice();	// execute one loop per time slice
		
		// check if all axes are enabled
		if (ch0->Enable && ch1->Enable && ch2->Enable)
		{
			SetBit(xxx);  // yes they are, enable the amplifier
		}
		else
		{
			ClearBit(xxx);  // no at least one is disabled, disable the amplifier
		}
	}
}
