#include "KMotionDef.h"

// Measure and display IRQ Time
//
// Assume no other user threads are executing
// In such a case only, the system thread and this thread execute
// thus this thread executes every other servo interrupt
//
// watch clock until we see a gap in time > 10us 
// last time we saw is close to when our time slice ended
//
// knowing time from slice to slice and our execution Time_sec
// remainder must be the interrupt time

main()
{
	int i=0;
	double t0,t1,t2,t3;

	for (;;)
	{
		t0=WaitNextTimeSlice();
		t1=WaitNextTimeSlice();
	
		t3=Time_sec();
	
		do
		{
			t2=t3;
			t3=Time_sec();
		}
		while (t3-t2 < 10e-6) ;
		
		if (i==0)
		{
			printf("Time Slice = %8.2f us\n",(t1-t0)*0.5e6);
			printf("Thread Execution time = %8.2f us\n",(t2-t1)*1e6);
			i++;
		}
		printf("IRQ time = %8.2f us\n",((t1-t0)*0.5-(t2-t1))*1e6);
		Delay_sec(1);
	}
}
