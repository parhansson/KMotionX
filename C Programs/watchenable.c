#include "KMotionDef.h"

main()
{
	for (;;)  //loop forever
	{
		WaitNextTimeSlice();

		if (ch0->Enable)
			SetBit(152);
		else
			ClearBit(152);
	}
}
