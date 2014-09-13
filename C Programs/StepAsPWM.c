#include "KMotionDef.h"

// Example showing how any of the 8 Step and Direction 
// frequency generators may be used as a PWM

//STEP_RATE_ADD 0x3c - write a 32 bit word - Bit31=enable, Bit27=Drive, Bits24-26=chan, 0-23= signed fraction of 16.666MHz
//STEP_PULSE_LENGTH_ADD 0x06 - write a 6 bit word - 0-63 = # of 16.666MHz clocks to this address

#define CLOCK 16.6666e6

main()
{
	double DutyCycle, Period;
	int Frequency;
	int Pulse=50;      // Fixed pulse length of 50/16.66MHz = 2us 
	int StepChan = 1;  // KFLOP Step Gen #1 IO10 Pin 17
	SetBitDirection(10,1);

	FPGA(STEP_PULSE_LENGTH_ADD) = Pulse;

	DutyCycle= 0.33; // Note Duty Cycle can't be zero (useful range 0.02  - 0.98)
	
	Period = Pulse/DutyCycle;
	
	Frequency = (int)(0x7FFFFF/Period);
	
	printf("DutyCycle=%f, Pulse=%d clocks, Period=%f clocks\n",DutyCycle,Pulse,Period);
	
	FPGA(STEP_RATE_ADD+0) = Frequency;    // put 23 bit frequency
	FPGA(STEP_RATE_ADD+1) = Frequency>>8;
	FPGA(STEP_RATE_ADD+2) = Frequency>>16;
	FPGA(STEP_RATE_ADD+3) = 0x88 + StepChan;  // combine enable, Drive High and low mode, channel
}
