#include "KMotionDef.h"

// Commutate a 3PH Brushless motor from Hall Sensors

#define PWM_CHAN 8			// which pair of PWM channels used 
#define ENCODER_CHAN 4		// which encoder we are connected to
#define ENCODER_GAIN 1 		// Set to -1 if desired to reverse axis direction
#define AMPLITUDE 20   		// Set how hard to drive the coils pwm counts
#define CLAMP_VOLTAGE 85  	// volts - set a few volts higher than your supply
#define AXIS_CHAN 4       	// Axis channel to be used and configured


// Commutation offset - since we measured the Hall sensors
// effectively where the torque constant was zero offset by
// 1/4 of a cycle to get max torque.  Also add shift by 1/2
// of a state (1/2 of 1/6 = 1/12) so that the angle is at the
// center of the state.
#define COMM_OFF (1.0/12.0 + 0.25)

// 6 state Hall sequence is 
//     5,1,3,2,6,4 (0 and 7) invalid
//     0,1,2,3,4,5 (phase position)
//
float CommTable[8] =
{
	0.0,                // Hall=0 invalid
	1.0/6.0 + COMM_OFF,	// Hall=1 maps to phase pos 1
	3.0/6.0 + COMM_OFF,	// Hall=2 maps to phase pos 3
	2.0/6.0 + COMM_OFF,	// Hall=3 maps to phase pos 2
	5.0/6.0 + COMM_OFF,	// Hall=4 maps to phase pos 5
	0.0/6.0 + COMM_OFF,	// Hall=5 maps to phase pos 0
	4.0/6.0 + COMM_OFF,	// Hall=6 maps to phase pos 4
	0.0					// Hall=7 invalid
};

void main() 
{
    float A=AMPLITUDE;   // set coil current amplitude
	int WhichSnap,WhichClamp,WhichClampEnable;
	int index,h1,h2,h3;	
	
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
   	
	ch->Enable=FALSE;
	ch->OutputChan0=PWM_CHAN;
	ch->InputMode=ENCODER_MODE;
	ch->InputChan0=ENCODER_CHAN;
	ch->OutputMode=BRUSHLESS_3PH_MODE;
	ch->InputGain0=ENCODER_GAIN;

	// Main loop to commutate the motor so that
	// the Amplitude A is applied at the optimal
	// phase angle to generate maximum torque
	
	for (;;)
    {
    	WaitNextTimeSlice();

		h1=ReadBit(80);  // read the halls
		h3=ReadBit(82); 
		h2=ReadBit(83); 
		index=((h1<<2)|(h2<<1)|h3);  // combine halls into state

		Write3PH(ch, A, CommTable[index]);  // Drive motor at phase angle 
    }
}


