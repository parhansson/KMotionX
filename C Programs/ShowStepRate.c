#include "KMotionDef.h"

main()
{
	double t0,t1,new,old;
	for (;;)
	{
		WaitNextTimeSlice();
		old = ch4->Dest;
		t0=Time_sec();
		Delay_sec(1.0);
		WaitNextTimeSlice();
		new = ch4->Dest;
		t1=Time_sec();
		printf("Rate = %13.2f steps/sec\n",(new-old)/(t1-t0));
	}
}
