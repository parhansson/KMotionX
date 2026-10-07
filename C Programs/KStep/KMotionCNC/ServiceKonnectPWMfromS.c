#include "KMotionDef.h"

void ServiceKonnectPWM(void);


main()
{
	for(;;)
	{
		WaitNextTimeSlice();
		ServiceKonnectPWM();
	}
}

#define KMVAR PC_COMM_CSS_S // variable KMotionCNC will pass speed parameter (113)
#define C 0.00029f // 1000uF
#define R 100.0f // 100 ohms
#define Vcc 11.210f // supply voltage
#define HIGH_BIT 60 // This output drives Cap high
#define LOW_BIT  61 // This output drives Cap low


void ServiceKonnectPWM(void)
{
	static int FirstTime=TRUE;
	static float Vc=0.0f;
	static double T0;
	static int State;
	double T=Time_sec(); 


	if (FirstTime)
	{
		FirstTime=FALSE;
		T0=T;
		State=0;
	}
	else
	{
		float V,I;
		float RPM=*(float *)&persist.UserData[KMVAR];
		float Vout = RPM*(5.0 / 1000.0);	     

		// Compute Voltage applied to Cap
		V=Vcc*State;

		// Compute current
		I=(V-Vc)/R;

		// Compute new Cap Voltage
		Vc += I/C*(T-T0);

		// determine next state

		if (Vc > Vout)
		{	
			ClearBit(HIGH_BIT);
			SetBit(LOW_BIT);
			State=0;
		}
		else
		{
			ClearBit(LOW_BIT);
			SetBit(HIGH_BIT);
			State=1;
		}

		T0=T; // save time when applied
	}
}





