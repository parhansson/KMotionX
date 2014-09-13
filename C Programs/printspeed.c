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
        speed = sqrtf(vx*vx+vy*vy);
		printf("speed = %f steps/sec\n",speed);
    }
}
