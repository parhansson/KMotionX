#include "ExtendedLogging.h"
#define CHANNELTOTEST ch0
main()
{	
	double T0 = Time_sec();
	printf("Starting Logging thread on KFlop...\n");
	int logbit = 0;
	for (;;)
	{
		int k;
		int loggingRestarted = 0;
		for (k=0; k<4; k++)	WaitNextTimeSlice();
		logbit = ReadBit(ENABLELOGGINGBIT);
		if (!logbit) {
			continue;
		}		
		double tcurr = Time_sec() - T0;		
		printf("@0,%g,%g,%g,%g\n",tcurr,CHANNELTOTEST->Dest,CHANNELTOTEST->Position,CHANNELTOTEST->Output);
	}
	printf("Logging thread exit.\n");
}