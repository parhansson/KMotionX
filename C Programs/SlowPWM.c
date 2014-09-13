#include "KMotionDef.h"

#define PWM_FREQ 50.0
#define BIT 46  // bit for PWM (46 is an LED)

void SetSlowPWM(double duty);

int Period=1.0/(PWM_FREQ*TIMEBASE*2);

main()
{
	SetSlowPWM(0.1);  // test function at some ratio
}


// create a software loop to pulse a bit at a certain
// frequency and duty cycle

void SetSlowPWM(double duty)
{
	int Pulse,i=0;

	Pulse = (int) (duty*Period+0.5);

	printf("Note: Pulse is %d Period is %d time slices\n",Pulse,Period);

	while (1) // loop foreved
	{
		WaitNextTimeSlice();  // wait for 2 servo ticks
		
		SetStateBit(BIT,i<Pulse);  // set/clear the bit 

		if (i++ >= Period) i=0; // loop through the period
	}
}

