#include "KMotionDef.h"

#define N 1000

main()
{
	int i,k;
	double T0,*p=gather_buffer;
	
	while (CS0_TimeExecuted==0.0) ;	// wait till we Start
	
	t0 = Time_sec();

	// Capture Data
	
	for (i=0; i<N; i++)
	{
		for (k=0; k<4; k++)	WaitNextTimeSlice();
		
		*p++ = Time_sec() - T0;
		*p++ = ch0->Dest;
		*p++ = ch0->Position;
		*p++ = ch1->Dest;
		*p++ = ch1->Position;
	}

	for (i=0; i<N; i++)
	{
		for (k=0; k<4; k++)	WaitNextTimeSlice();
		
		*p++ = Time_sec() - T0;
		*p++ = ch0->Dest;
		*p++ = ch0->Position;
		*p++ = ch1->Dest;
		*p++ = ch1->Position;
	}


}
