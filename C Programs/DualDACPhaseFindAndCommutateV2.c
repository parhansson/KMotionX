#include "KMotionDef.h"

// Home and set the commutation for a 3 phase brushless motor
// that is driven using two DAC Outputs
//
// Phase rotates until index mark is located and Encoder Zeroed 


// Function definitions

// x must be in the range 0->1 returns sin(x) and sin(x+120 degrees)
float Sine3PH (float x, float *sin2);

// put a voltage v on a 3 Phase motor at specified commutation angle
void Write3PH_DACs(CHAN *ch, float v, double angle_in_cycles); 

// return fractional part of a huge number accurately
float fractionf(double v);

void main() 
{
	float k=0,A=200;   // set coil current amplitude DAC units
	double p0;
	int found0=FALSE,found1=FALSE;

	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=NO_OUTPUT_MODE;
	ch0->Vel=400000;
	ch0->Accel=400000;
	ch0->Jerk=1e+008;
	ch0->P=3;
	ch0->I=0;
	ch0->D=30;
	ch0->FFAccel=0;
	ch0->FFVel=0;
	ch0->MaxI=200;
	ch0->MaxErr=1e+006;
	ch0->MaxOutput=1000;
	ch0->DeadBandGain=1;
	ch0->DeadBandRange=0;
	ch0->InputChan0=0;
	ch0->InputChan1=0;
	ch0->OutputChan0=0;
	ch0->OutputChan1=1;
	ch0->MasterAxis=-1;
	ch0->LimitSwitchOptions=0x0;
	ch0->SoftLimitPos=1e+030;
	ch0->SoftLimitNeg=-1e+030;
	ch0->InputGain0=-1;
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
	ch0->MaxFollowingError=400000000;
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


	ch1->InputMode=ENCODER_MODE;
	ch1->OutputMode=NO_OUTPUT_MODE;
	ch1->Vel=350000;
	ch1->Accel=250000;
	ch1->Jerk=1e+008;
	ch1->P=3;
	ch1->I=0;
	ch1->D=30;
	ch1->FFAccel=0;
	ch1->FFVel=0;
	ch1->MaxI=200;
	ch1->MaxErr=1e+006;
	ch1->MaxOutput=1000;
	ch1->DeadBandGain=1;
	ch1->DeadBandRange=0;
	ch1->InputChan0=2;
	ch1->InputChan1=2;
	ch1->OutputChan0=2;
	ch1->OutputChan1=3;
	ch1->MasterAxis=-1;
	ch1->LimitSwitchOptions=0x0;
	ch1->SoftLimitPos=1e+030;
	ch1->SoftLimitNeg=-1e+030;
	ch1->InputGain0=1;
	ch1->InputGain1=1;
	ch1->InputOffset0=0;
	ch1->InputOffset1=0;
	ch1->OutputGain=1;
	ch1->OutputOffset=0;
	ch1->SlaveGain=1;
	ch1->BacklashMode=BACKLASH_OFF;
	ch1->BacklashAmount=0;
	ch1->BacklashRate=0;
	ch1->invDistPerCycle=0.0003662109375;
	ch1->Lead=0;
	ch1->MaxFollowingError=40000;
	ch1->StepperAmplitude=20;

	ch1->iir[0].B0=1;
	ch1->iir[0].B1=0;
	ch1->iir[0].B2=0;
	ch1->iir[0].A1=0;
	ch1->iir[0].A2=0;

	ch1->iir[1].B0=1;
	ch1->iir[1].B1=0;
	ch1->iir[1].B2=0;
	ch1->iir[1].A1=0;
	ch1->iir[1].A2=0;

	ch1->iir[2].B0=1;
	ch1->iir[2].B1=0;
	ch1->iir[2].B2=0;
	ch1->iir[2].A1=0;
	ch1->iir[2].A2=0;

	// rotate until we find the index mark

	ch0->Enable=FALSE;
	ch1->Enable=FALSE;

	while (!found0 || !found1) // loop until both found
	{
		WaitNextTimeSlice();

		k++;  // advance angle

		if (!found0)
		{
			Write3PH_DACs(ch0,A, k/10000.0);  // move the pole 

			if (ReadBit(36))  // check for index mark
			{
				found0=TRUE;
				p0=ch0->Position; // save position
				ch0->Position=0;  // set current position to Zero
				ch0->CommutationOffset = 4245;  // from auto phase find
				printf("Position 0 = %f\n",p0);
			}
		}

		if (!found1)
		{
			Write3PH_DACs(ch1,A, k/10000.0);  // move the pole 

			if (ReadBit(38))  // check for index mark
			{
				found1=TRUE;
				p0=ch1->Position; // save position
				ch1->Position=0;  // set current position to Zero
				ch1->CommutationOffset = 3265;  // from auto phase find
				printf("Position 1 = %f\n",p0);
			}
		}
	}

	EnableAxisDest(0,0.0f);  // Enable servo at destination of 0
	EnableAxisDest(1,0.0f);  // Enable servo at destination of 0

	DefineCoordSystem(0,1,-1,-1); // define which axes are in use

	// Loop forever outputting Servo output to dual DAC_SERVO_MODE

	for (;;)
	{	
		if (ch0->Enable)  // if axis enabled commutate and output else command zero to DACs
			Write3PH_DACs(ch0, ch0->Output, (ch0->Position + ch0->CommutationOffset) * ch0->invDistPerCycle);		
		else
			Write3PH_DACs(ch0, 0.0, 0.0);

		if (ch1->Enable)  // if axis enabled commutate and output else command zero to DACs
			Write3PH_DACs(ch1, ch1->Output, (ch1->Position + ch1->CommutationOffset) * ch1->invDistPerCycle);		
		else
			Write3PH_DACs(ch1, 0.0, 0.0);		
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
