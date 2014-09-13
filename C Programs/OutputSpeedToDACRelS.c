#include "KMotionDef.h"

#define CntsPerMM 1000.0

main()
{
	double T1,T0=Time_sec();
	double LastX = ch0->Dest;
	double LastY = ch1->Dest;
	float Speed,SpeedX, SpeedY;
	float MaxSpeed;
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
		
		// get S 100% power speed that was set int Var2 by the 
		// Spindle Program and convert to cnts/sec 
		MaxSpeed = *(float *)&persist.UserData[2] * CntsPerMM * (1.0f/60.0f);   
			
		dac = (int)(Speed * 2047.0 / MaxSpeed);
		
		if (dac > 2047) dac=2047;  // limit DAC to DAC range
		
		DAC(7,dac); // output vector speed to DAC
	}
}