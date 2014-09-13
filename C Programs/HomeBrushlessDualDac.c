#include "KMotionDef.h"

// Home and set the commutation for a 3 phase brushless motor
// that is driven using two DAC Outputs
//
// Phase rotates until index mark is located and Encoder Zeroed 


// Function definitions

// x must be in the range 0->1 returns sin(x) and sin(x+120 degrees)
float Sine3PH (float x, float *sin2);

// put a voltage v on a 3 Phase motor at specified commutation angle
void Write3PH_DACs(CHAN *ch0, float v, double angle_in_cycles); 

// return fractional part of a huge number accurately
float fractionf(double v);

void main() 
{
	float k=0,A=200;   // set coil current amplitude DAC units
	double p0;

	// define the axis as 3 phase BRUSHLESS_3PH_MODE
	// and set low PID gains

	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=NO_OUTPUT_MODE;
	ch0->Vel=100000;
	ch0->Accel=100000;
	ch0->Jerk=1e+008;
	ch0->P=0.1;
	ch0->I=0;
	ch0->D=0;
	ch0->FFAccel=0;
	ch0->FFVel=0;
	ch0->MaxI=200;
	ch0->MaxErr=1e+006;
	ch0->MaxOutput=200;
	ch0->DeadBandGain=1;
	ch0->DeadBandRange=0;
	ch0->InputChan0=0;
	ch0->InputChan1=0;
	ch0->OutputChan0=0;
	ch0->OutputChan1=1;
	ch0->MasterAxis=-1;
	ch0->LimitSwitchOptions=0x100;
	ch0->LimitSwitchNegBit=0;
	ch0->LimitSwitchPosBit=0;
	ch0->SoftLimitPos=1e+030;
	ch0->SoftLimitNeg=-1e+030;
	ch0->InputGain0=1;
	ch0->InputGain1=1;
	ch0->InputOffset0=0;
	ch0->InputOffset1=0;
	ch0->OutputGain=1;
	ch0->OutputOffset=0;
	ch0->SlaveGain=1;
	ch0->BacklashMode=BACKLASH_OFF;
	ch0->BacklashAmount=0;
	ch0->BacklashRate=0;
	ch0->invDistPerCycle=-0.0003662109375;
	ch0->Lead=0;
	ch0->MaxFollowingError=4000;
	ch0->StepperAmplitude=20;

	ch0->iir[0].B0=1;
	ch0->iir[0].B1=0;
	ch0->iir[0].B2=0;
	ch0->iir[0].A1=0;
	ch0->iir[0].A2=0;

	ch0->iir[1].B0=1;
	ch0->iir[1].B1=0;
	ch0->iir[1].B2=0;
	ch0->iir[1].A1=0;
	ch0->iir[1].A2=0;

	ch0->iir[2].B0=1;
	ch0->iir[2].B1=0;
	ch0->iir[2].B2=0;
	ch0->iir[2].A1=0;
	ch0->iir[2].A2=0;

	// rotate until we find the index mark

	ch0->Enable=FALSE;
	for (;;)
	{
		WaitNextTimeSlice();

		Write3PH_DACs(ch0,A, ++k/10000.0);  // move the pole 

		if (ReadBit(2))  // check for index mark
		{
			p0=ch0->Position; // save position
			ch0->Position=0;  // set current position to Zero

			ch0->CommutationOffset = -202;  // from auto phase find

			printf("Position = %f\n",p0);
			break;
		}
	}

	EnableAxisDest(0,0.0f);  // Enable servo at destination of 0
	DefineCoordSystem(0,-1,-1,-1); // define which axes are in use

	// Loop forever outputting Servo output to dual DAC_SERVO_MODE

	for (;;)
	{
		CHAN *ch=ch0;

		if (ch->Enable)  // if axis enabled commutate and output else command zero to DACs
			Write3PH_DACs(ch, ch->Output, (ch->Position + ch->CommutationOffset) * ch->invDistPerCycle);		
		else
			Write3PH_DACs(ch, 0.0, 0.0);		
	}
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


