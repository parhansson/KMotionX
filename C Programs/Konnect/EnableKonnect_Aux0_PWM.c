#include "KMotionDef.h"

// Enables a Konnect on KFLOP JP4 Aux Port then
// PWM's two outputs as push-pull drivers such that
// when low passed filtered with an RC circuit becomes
// a variable analog source.
//
// Configure KFLOP to service Konnect 32 Input 16 output IO board
// Board address is 0, 
// 16 Outputs are mapped to Virtual IO 48-63 (VirtualBits)
// 32 Inputs are mapped to Virtual IO 1024-1055 (VirtualBits[0])
//
// Attach Service to Aux0 Port (KFLOP JP4) instead of standard Aux1 Port (KFLOP JP6)
//

void ServiceKonnectPWM(void);

double T,T0=0;
float Vout=0.0;  // desired voltage

main()
{
	InitAux();
	AddKonnect_Aux0(0,&VirtualBits,VirtualBitsEx);
	
	for(;;)
	{
		T=WaitNextTimeSlice();
		ServiceKonnectPWM();
		
		// Fixed
//		Vout = 0.1;
		
			
		//Generate a 5 Hz 3V Sine Wave
		Vout = 3.0f*sin(T * TWO_PI * 5.0) + 5.0;


		//Generate a Saw Tooth wave
//		Vout = 2 + 6.0* (5.0*T - ((int)(5.0*T)));
		
		//Generate a 5 Hz Square wave
//		Vout = (5.0*T - ((int)(5.0*T))) > 0.5 ? 8 : 2; 
	}
}

#define C 0.00029f // 1000uF
#define R 100.0f // 100 ohms
#define Vcc 11.230f // supply voltage
#define HIGH_BIT 62 // This output drives Cap high
#define LOW_BIT  63 // This output drives Cap low


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