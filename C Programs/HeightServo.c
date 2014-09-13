#include "KMotionDef.h"

main()
{
	while(1)  // loop forever
	{
		WaitNextTimeSlice();  // execute loop once per time slice (2 servo ticks)
		
		// Change Encoder loop's commanded destination based on error output of LVDT Height loop
		// User Variable 0 enables/disables the mode
		
		if (persist.UserData[0])
		{
			ch3->Dest += ch2->Output * TIMEBASE * 2.0;  
		}
	}
}
