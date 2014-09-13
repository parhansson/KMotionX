#include "KMotionDef.h"

// Example of how to set/disable coordinated motion low pass filtering

main()
{
	double Tau = 0.001;  // seconds for Low Pass Filter Time Constant

	KLP = exp(-TIMEBASE/Tau);
	
//	KLP=0; // force to 0 to disable
	
	printf("Tau=%f KLP=%f\n",Tau,KLP);
}
