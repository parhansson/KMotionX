#include "KMotionDef.h"

main()
{
	for (;;)
	{
		printf("Temp A side = %6.1f B side = %6.1f degrees F\n",
			ReadSnapAmp(SNAP0+SNAP_TEMP0)/8.0,
			ReadSnapAmp(SNAP0+SNAP_TEMP1)/8.0);
		Delay_sec(1);
	}
}
