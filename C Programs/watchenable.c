#include "KMotionDef.h"

int main()
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
