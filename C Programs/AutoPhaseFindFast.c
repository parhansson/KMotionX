#include "KMotionDef.h"

// Drive a 3 Phase motor that has a Z index by
// 3 phase driving the coils (like a stepping motor)
// two revs each direction (two z pulses)
// monitor how many counts/cycle and counts/rev 
// (including direction)and also determine the commutation
// offset by recording the phase angle at the index 
// (which will be where zero is set) and offsetting by 1/4 of a cycle 
//
// Set the following parameters according to your situation
// See the report generated on the Console Screen

#define PWM_CHAN 8   // which pair of PWM channels used
#define ENCODER_CHAN 8  // which encoder we are connected to
#define ENCODER_GAIN 1   // Set to -1 if desired to reverse axis direction
#define AMPLITUDE 10     // Set how hard to drive the coils pwm counts
#define Z_BIT_NUMBER 66  // What bit the Z index is connected to
#define CLAMP_VOLTAGE 85   // volts - set a few volts higher than your supply
#define AXIS_CHAN 0        // Axis channel to be used and configured
#define Ncycles 4   // don't change this

void main() 
{
    float mid,A=AMPLITUDE;   // set coil current amplitude
	int k=0,i,dk=1,WhichSnap,WhichClamp,WhichClampEnable,ignore=300,kpos[Ncycles],zmark,m=0;
	double cnts_per_cycle,p0[Ncycles];
	CHAN *ch = &chan[AXIS_CHAN];

	if (PWM_CHAN < 12)
		WhichSnap=SNAP0;
	else
		WhichSnap=SNAP1;
		
	if (PWM_CHAN==8 || PWM_CHAN==12)
	{
		WhichClamp=SNAP_SUPPLY_CLAMP0;
		WhichClampEnable=SNAP_SUPPLY_CLAMP_ENA0;
	}
	else
	{
		WhichClamp=SNAP_SUPPLY_CLAMP1;
		WhichClampEnable=SNAP_SUPPLY_CLAMP_ENA1;
	}
	
	
	WriteSnapAmp(WhichSnap+WhichClamp ,SNAP_CONVERT_VOLTS_TO_ADC(CLAMP_VOLTAGE));
	WriteSnapAmp(WhichSnap+WhichClampEnable,1);
	WriteSnapAmp(WhichSnap+SNAP_PEAK_CUR_LIMIT0,10);  // current limit
	WriteSnapAmp(WhichSnap+SNAP_PEAK_CUR_LIMIT1,10);  // current limit
	
	Delay_sec(1);  // wait for any fault to clear
   	
	// rotate until we find the index mark

	ch->Enable=FALSE;
	ch->OutputChan0=PWM_CHAN;
	ch->InputMode=ENCODER_MODE;
	ch->InputChan0=ENCODER_CHAN;
	ch->OutputMode=BRUSHLESS_3PH_MODE;
	ch->InputGain0=ENCODER_GAIN;

	for (;;)
    {
//		Delay_sec(0.001);  // wait a millisecond 
		WaitNextTimeSlice();

		k+= dk;
		
		Write3PH(ch,A, k/10000.0);  // move the pole 

		zmark = ReadBit(Z_BIT_NUMBER);
		
		if (!zmark && ignore>0) ignore--;
		
		if (ignore==0 && zmark)  // check for index mark
		{
			p0[m]=ch->Position; // save position
			kpos[m]=k;           // save phase angle
			
			if (++m == Ncycles) 
			{
				ch->Position=0;  // set current position to Zero
				break;
			}
			
			if (m==2) dk = -dk;
			ignore=300;
		}
    }

	Write3PH(ch,0,0);    // turn off the coil    

			
//			ch4->CommutationOffset = 3*8000/2.0/12.0 * 1.02;

	printf("\nREPORT\n------\n");
	for (i=0; i<Ncycles; i++)
			printf("%d Position = %6.0f PhaseAngle = %f\n",i,p0[i],kpos[i]/10000.0);
			
	printf("Counts per rev = %6.0f\n",p0[1]-p0[0]);
	cnts_per_cycle = (p0[1]-p0[0])/(kpos[1]-kpos[0])*10000.0;
	printf("Counts per cycle = %6.0f\n",cnts_per_cycle);
	
	// round to 10
	if (cnts_per_cycle>0)
		cnts_per_cycle = ((int)(cnts_per_cycle/10.0 + 0.5))*10.0;
	else
		cnts_per_cycle = ((int)(cnts_per_cycle/10.0 - 0.5))*10.0;
	
	printf("Counts per cycle (rounded)= %6.0f\n",cnts_per_cycle);
	
	ch->invDistPerCycle = 1.0/cnts_per_cycle;
	printf("invDistPerCycle (rounded)= %15.12f\n",ch->invDistPerCycle);
	
	mid = (kpos[2]+kpos[1])/20000.0;
	mid = mid - (int)mid;
	ch->CommutationOffset = mid*cnts_per_cycle + 0.25*fast_fabs(cnts_per_cycle);
	printf("Commutation offset = %6.0f\n",ch->CommutationOffset);
	printf("Input Gain Specified = %6.3f\n",ch->InputGain0);
}

