#include "ExtendedLogging.h"
main()
{
	Delay_sec(1.5);	
	int i = 0;
	double T0 = Time_sec();
	double Tend;
	printf("Running Simple Thread...\n");

	for(i=0;i<10000;i++)
	{		
        int k;
		for (k=0; k<4; k++)	WaitNextTimeSlice();
		double tcurr = Time_sec() - T0;
		double y1 = sin(tcurr);
		double y2 = sin(2*tcurr);
		double y3 = sin(0.5*tcurr);
		printf("@0,%f,%f,%f,%f\n",tcurr,y1,y2,y3);
	}
	
	Tend = Time_sec() - T0;
	printf("Exiting Simple Thread, total time %g.\n", Tend);
}