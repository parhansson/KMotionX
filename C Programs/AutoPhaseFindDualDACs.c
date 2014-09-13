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

#define DAC_CHAN 0      // which pair of PWM channels used
#define ENCODER_CHAN 0  // which encoder we are connected to
#define ENCODER_GAIN 1  // Set to -1 if desired to reverse axis direction
#define AMPLITUDE 200   // Set how hard to drive the coils pwm counts
#define Z_BIT_NUMBER 2  // What bit the Z index is connected to
#define AXIS_CHAN 0     // Axis channel to be used and configured
#define Ncycles 4       // don't change this

// x must be in the range 0->1 returns sin(x) and sin(x+120 degrees)
float Sine3PH (float x, float *sin2);

// put a voltage v on a 3 Phase motor at specified commutation angle
void Write3PH_DACs(CHAN *ch0, float v, double angle_in_cycles); 

// return fractional part of a huge number accurately
float fractionf(double v);


void main() 
{
	float mid,k=0,dk=0.2,A=AMPLITUDE;   // set coil current amplitude
	int i,ignore=300,kpos[Ncycles],zmark,m=0;
	double cnts_per_cycle,p0[Ncycles];
	CHAN *ch = &chan[AXIS_CHAN];

	// rotate until we find the index mark

	ch->Enable=FALSE;
	ch->InputMode=ENCODER_MODE;
	ch->InputChan0=ENCODER_CHAN;
	ch->OutputChan0=DAC_CHAN;
	ch->OutputChan1=DAC_CHAN+1;
	ch->OutputMode=NO_OUTPUT_MODE;
	ch->InputGain0=ENCODER_GAIN;

	for (;;)
	{
		WaitNextTimeSlice();

		k+= dk;

		Write3PH_DACs(ch,A, k/10000.0);  // move the pole 

		zmark = ReadBit(Z_BIT_NUMBER);

		if (!zmark && ignore>0) ignore--;

		if (ignore==0 && zmark)  // check for index mark
		{
			p0[m]=ch->Position;  // save position
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

	Write3PH_DACs(ch,0,0);    // turn off the coil    

	printf("\nREPORT\n------\n");
	for (i=0; i<Ncycles; i++)
		printf("%d Position = %6.0f PhaseAngle = %f\n",i,p0[i],kpos[i]/10000.0);

	printf("Counts per rev = %6.0f\n",p0[1]-p0[0]);
	cnts_per_cycle = (p0[1]-p0[0])/(kpos[1]-kpos[0])*10000.0;
	printf("Counts per cycle = %6.0f\n",cnts_per_cycle);

	// round to 16
	if (cnts_per_cycle>0)
		cnts_per_cycle = ((int)(cnts_per_cycle/16.0 + 0.5))*16.0;
	else
		cnts_per_cycle = ((int)(cnts_per_cycle/16.0 - 0.5))*16.0;

	printf("Counts per cycle (rounded to 16)= %6.0f\n",cnts_per_cycle);

	ch->invDistPerCycle = 1.0/cnts_per_cycle;
	printf("invDistPerCycle (rounded)= %15.12f\n",ch->invDistPerCycle);

	mid = (kpos[2]+kpos[1])/20000.0;
	mid = mid - (int)mid;
	ch->CommutationOffset = mid*cnts_per_cycle + 0.25*fast_fabs(cnts_per_cycle);
	printf("Commutation offset = %6.0f\n",ch->CommutationOffset);
	printf("Input Gain Specified = %6.3f\n",ch->InputGain0);
}


// put a voltage v on a 3 Phase motor at specified commutation angle
void Write3PH_DACs(CHAN *ch, float v, double angle_in_cycles)
{
	float theta,sin1f;

	if (angle_in_cycles<0)
		theta = 1.0f-fractionf(-angle_in_cycles);
	else
		theta = fractionf(angle_in_cycles);

	DAC(ch->OutputChan0, (int)(v * Sine3PH(theta,&sin1f)));
	DAC(ch->OutputChan1, (int)(v * sin1f));
}
