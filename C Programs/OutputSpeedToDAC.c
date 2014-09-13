#include "KMotionDef.h"

// factor to convert speed in counts/sec to DAC counts
#define FACTOR 0.01 

main()
{
	double T1,T0=Time_sec();
	double LastX = ch0->Dest;
	double LastY = ch1->Dest;
	double Speed,SpeedX, SpeedY;
	int dac;

	while(1)
	{
		T1=WaitNextTimeSlice();  
		SpeedX = ch0->Dest - LastX;
		SpeedY = ch1->Dest - LastY;
		Speed = sqrt(SpeedX*SpeedX+SpeedY*SpeedY)/(T1-T0);
		LastX = ch0->Dest;
		LastY = ch1->Dest;
		T0=T1;
		dac = (int)(Speed * FACTOR);
		
		if (dac > 2047) dac=2047;  // limit DAC to DAC range
		
		DAC(7,dac); // output vector speed to DAC
	}
}