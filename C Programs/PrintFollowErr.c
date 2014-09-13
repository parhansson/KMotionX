#include "KMotionDef.h"

main()
{
	double vx,vy,speed;
	for (;;)
    {
        Delay_sec(0.5);
        WaitNextTimeSlice();  // ensure we don't get interrupted
		vx = (ch0->Dest - ch0->last_dest) * (1.0f/TIMEBASE);
        vy = (ch1->Dest - ch1->last_dest) * (1.0f/TIMEBASE);
		printf("speed = %f %f cnts/sec Err = %12.0f %12.0f %12.0f cnts\n",
			vx,vy,
			ch0->LastFollowingError,
			ch1->LastFollowingError,
			ch2->LastFollowingError);
    }
}
