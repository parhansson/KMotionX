#include "KMotionDef.h"
#define SMALL_ERR 5
#define LONG_TIME 0.5

void DoDither(CHAN *ch, float P_original, float P_low, float I_original, double *T0, double T1);

main()
{
	// save original gains and do anti-dither

	double T1,T0_0=0,T0_1=0,T0_2=0;  

	float P_original_0 = 0.4;//ch0->P;
	float P_low_0      = P_original_0 * 0.1;
	float I_original_0 = 0.05;//ch0->I;
	
	float P_original_1 = 0.4;//ch1->P;
	float P_low_1      = P_original_1 * 0.1;
	float I_original_1 = 0.05;//ch1->I;
	
	float P_original_2 = 0.4;//ch2->P;
	float P_low_2      = P_original_2 * 0.1;
	float I_original_2 = 0.05;//ch2->I;
	
	for (;;)  //loop forever
	{
		T1=WaitNextTimeSlice();  // waits and returns current time
   
		DoDither(ch0, P_original_0, P_low_0, I_original_0, &T0_0, T1);
		DoDither(ch1, P_original_1, P_low_1, I_original_1, &T0_1, T1);
		DoDither(ch2, P_original_2, P_low_2, I_original_2, &T0_2, T1);
	}    
}


void DoDither(CHAN *ch, float P_original, float P_low, float I_original, double *T0, double T1)
{
	if (fast_fabs(ch->LastFollowingError) > SMALL_ERR) *T0=T1;

	if (T1 - *T0 > LONG_TIME)// error for a long time
	{
		ch->P = P_low;   // reduced gain
		ch->I = 0.0;     // no integrator
	}
	else
	{
		ch->P = P_original;    // normal gain
		ch->I = I_original;    // normal integrator
	}
}

