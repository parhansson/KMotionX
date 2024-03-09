#include "KMotionDef.h"

main()
{
	double T0=0;
	for (;;)  // loop forever
	{
		double TimeRemaining = CS0_TimeDownloaded - CS0_TimeExecuted - CS0_t;
		
		if (TimeRemaining > 0.0) // Print only with time remaining
			printf("%12.6f Time Remaining = %f\n",Time_sec() - T0, TimeRemaining);
		else
			T0 = Time_sec(); // Time stamp last time we weren't executing motion
		
		Delay_sec(0.1);  // print only so often to avoid flooding the Console Screen
	}
}
