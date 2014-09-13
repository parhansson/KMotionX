#include "KMotionDef.h"

#define TOL 10		  // +/- Tolerance in counts for being in position
#define DURATION 0.15 // duration that it must be in position

main()
{
	double T0,CurTime;
	
	T0=Time_sec();
	for (;;)  //loop forever
	{
		CurTime=WaitNextTimeSlice();

		//reset timer if out of position
		if (fast_fabs(ch0->Position - ch0->Dest) > TOL ||
			fast_fabs(ch1->Position - ch1->Dest) > TOL ||
			fast_fabs(ch2->Position - ch2->Dest) > TOL)
			T0=CurTime;
			
		if 	(CurTime > T0 + DURATION)
			SetBit(46);
		else
			ClearBit(46);
	}
}
