#include "KMotionDef.h"

// Defines axis as Servos with output as PWM Sign and Magnitude
// enables them
// sets them as an xy coordinate system for GCode

#define MAXPWM 242
#define DIR_BIT0 30
#define DIR_BIT1 31
#define DIR_BIT2 32
#define DIR_BIT3 33

void OutputSignMag(int ch, int pwm, int dir_bit);

int main() 
{
	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=NO_OUTPUT_MODE;
	ch0->Vel=100.000000;
	ch0->Accel=1000.000000;
	ch0->Jerk=10000.000000;
	ch0->P=1.000000;
	ch0->I=0.000000;
	ch0->D=0.000000;
	ch0->FFAccel=0.000000;
	ch0->FFVel=0.000000;
	ch0->MaxI=200.000000;
	ch0->MaxErr=200.000000;
	ch0->MaxOutput=200.000000;
	ch0->DeadBandGain=1.000000;
	ch0->DeadBandRange=0.000000;
	ch0->InputChan0=0;
	ch0->InputChan1=1;
	ch0->OutputChan0=0;
	ch0->OutputChan1=1;
	ch0->LimitSwitchOptions=0x0;
	ch0->InputGain0=1.000000;
	ch0->InputGain1=1.000000;
	ch0->InputOffset0=0.000000;
	ch0->InputOffset1=0.000000;
	ch0->invDistPerCycle=1.000000;
	ch0->Lead=0.000000;
	ch0->MaxFollowingError=1000000000.000000;
	ch0->StepperAmplitude=250.000000;

	ch0->iir[0].B0=1.000000;
	ch0->iir[0].B1=0.000000;
	ch0->iir[0].B2=0.000000;
	ch0->iir[0].A1=0.000000;
	ch0->iir[0].A2=0.000000;

	ch0->iir[1].B0=1.000000;
	ch0->iir[1].B1=0.000000;
	ch0->iir[1].B2=0.000000;
	ch0->iir[1].A1=0.000000;
	ch0->iir[1].A2=0.000000;

	ch0->iir[2].B0=1.000000;
	ch0->iir[2].B1=0.000000;
	ch0->iir[2].B2=0.000000;
	ch0->iir[2].A1=0.000000;
	ch0->iir[2].A2=0.000000;

	ch1->InputMode=ENCODER_MODE;
	ch1->OutputMode=NO_OUTPUT_MODE;
	ch1->Vel=100.000000;
	ch1->Accel=1000.000000;
	ch1->Jerk=10000.000000;
	ch1->P=1.000000;
	ch1->I=0.000000;
	ch1->D=0.000000;
	ch1->FFAccel=0.000000;
	ch1->FFVel=0.000000;
	ch1->MaxI=200.000000;
	ch1->MaxErr=200.000000;
	ch1->MaxOutput=200.000000;
	ch1->DeadBandGain=1.000000;
	ch1->DeadBandRange=0.000000;
	ch1->InputChan0=1;
	ch1->InputChan1=2;
	ch1->OutputChan0=3;
	ch1->OutputChan1=2;
	ch1->LimitSwitchOptions=0x0;
	ch1->InputGain0=1.000000;
	ch1->InputGain1=1.000000;
	ch1->InputOffset0=0.000000;
	ch1->InputOffset1=0.000000;
	ch1->invDistPerCycle=1.000000;
	ch1->Lead=0.000000;
	ch1->MaxFollowingError=1000000000.000000;
	ch1->StepperAmplitude=250.000000;

	ch1->iir[0].B0=1.000000;
	ch1->iir[0].B1=0.000000;
	ch1->iir[0].B2=0.000000;
	ch1->iir[0].A1=0.000000;
	ch1->iir[0].A2=0.000000;

	ch1->iir[1].B0=1.000000;
	ch1->iir[1].B1=0.000000;
	ch1->iir[1].B2=0.000000;
	ch1->iir[1].A1=0.000000;
	ch1->iir[1].A2=0.000000;

	ch1->iir[2].B0=1.000000;
	ch1->iir[2].B1=0.000000;
	ch1->iir[2].B2=0.000000;
	ch1->iir[2].A1=0.000000;
	ch1->iir[2].A2=0.000000;

	ch2->InputMode=ENCODER_MODE;
	ch2->OutputMode=NO_OUTPUT_MODE;
	ch2->Vel=100.000000;
	ch2->Accel=1000.000000;
	ch2->Jerk=10000.000000;
	ch2->P=1.000000;
	ch2->I=0.000000;
	ch2->D=0.000000;
	ch2->FFAccel=0.000000;
	ch2->FFVel=0.000000;
	ch2->MaxI=200.000000;
	ch2->MaxErr=200.000000;
	ch2->MaxOutput=200.000000;
	ch2->DeadBandGain=1.000000;
	ch2->DeadBandRange=0.000000;
	ch2->InputChan0=2;
	ch2->InputChan1=3;
	ch2->OutputChan0=4;
	ch2->OutputChan1=5;
	ch2->LimitSwitchOptions=0x0;
	ch2->InputGain0=1.000000;
	ch2->InputGain1=1.000000;
	ch2->InputOffset0=0.000000;
	ch2->InputOffset1=0.000000;
	ch2->invDistPerCycle=1.000000;
	ch2->Lead=0.000000;
	ch2->MaxFollowingError=1000000000.000000;
	ch2->StepperAmplitude=250.000000;

	ch2->iir[0].B0=1.000000;
	ch2->iir[0].B1=0.000000;
	ch2->iir[0].B2=0.000000;
	ch2->iir[0].A1=0.000000;
	ch2->iir[0].A2=0.000000;

	ch2->iir[1].B0=1.000000;
	ch2->iir[1].B1=0.000000;
	ch2->iir[1].B2=0.000000;
	ch2->iir[1].A1=0.000000;
	ch2->iir[1].A2=0.000000;

	ch2->iir[2].B0=1.000000;
	ch2->iir[2].B1=0.000000;
	ch2->iir[2].B2=0.000000;
	ch2->iir[2].A1=0.000000;
	ch2->iir[2].A2=0.000000;

	ch3->InputMode=ENCODER_MODE;
	ch3->OutputMode=NO_OUTPUT_MODE;
	ch3->Vel=100.000000;
	ch3->Accel=1000.000000;
	ch3->Jerk=10000.000000;
	ch3->P=1.000000;
	ch3->I=0.000000;
	ch3->D=0.000000;
	ch3->FFAccel=0.000000;
	ch3->FFVel=0.000000;
	ch3->MaxI=200.000000;
	ch3->MaxErr=200.000000;
	ch3->MaxOutput=200.000000;
	ch3->DeadBandGain=1.000000;
	ch3->DeadBandRange=0.000000;
	ch3->InputChan0=3;
	ch3->InputChan1=4;
	ch3->OutputChan0=7;
	ch3->OutputChan1=6;
	ch3->LimitSwitchOptions=0x0;
	ch3->InputGain0=1.000000;
	ch3->InputGain1=1.000000;
	ch3->InputOffset0=0.000000;
	ch3->InputOffset1=0.000000;
	ch3->invDistPerCycle=1.000000;
	ch3->Lead=0.000000;
	ch3->MaxFollowingError=1000000000.000000;
	ch3->StepperAmplitude=250.000000;

	ch3->iir[0].B0=1.000000;
	ch3->iir[0].B1=0.000000;
	ch3->iir[0].B2=0.000000;
	ch3->iir[0].A1=0.000000;
	ch3->iir[0].A2=0.000000;

	ch3->iir[1].B0=1.000000;
	ch3->iir[1].B1=0.000000;
	ch3->iir[1].B2=0.000000;
	ch3->iir[1].A1=0.000000;
	ch3->iir[1].A2=0.000000;

	ch3->iir[2].B0=1.000000;
	ch3->iir[2].B1=0.000000;
	ch3->iir[2].B2=0.000000;
	ch3->iir[2].A1=0.000000;
	ch3->iir[2].A2=0.000000;
	
	EnableAxisDest(0,0);
	EnableAxisDest(1,0);
	EnableAxisDest(2,0);
	EnableAxisDest(3,0);

	DefineCoordSystem(0,1,2,3);
	SetBitDirection(26,1);  		// Set bit 26 (PWM 0 as an output)
	SetBitDirection(27,1);  		// Set bit 27 (PWM 1 as an output)
	SetBitDirection(28,1);  		// Set bit 28 (PWM 2 as an output)
	SetBitDirection(29,1);  		// Set bit 29 (PWM 3 as an output)
	
	SetBitDirection(DIR_BIT0,1);	// Set bit 30 (Direction an output)
	SetBitDirection(DIR_BIT1,1);	// Set bit 31 (Direction an output)
	SetBitDirection(DIR_BIT2,1);	// Set bit 32 (Direction an output)
	SetBitDirection(DIR_BIT3,1);	// Set bit 33 (Direction an output)
	
	FPGA(IO_PWMS_PRESCALE) = 1;  	// set pwm period to 30 KHz
	FPGA(IO_PWMS+1) = 1;			// enable the PWM
	FPGA(IO_PWMS+3) = 1;			// enable the PWM
	FPGA(IO_PWMS+5) = 1;			// enable the PWM
	FPGA(IO_PWMS+7) = 1;			// enable the PWM

	for (;;)  //loop forever
	{
		WaitNextTimeSlice();
		OutputSignMag(0,0,DIR_BIT0);
		OutputSignMag(1,1,DIR_BIT1);
		OutputSignMag(2,2,DIR_BIT2);
		OutputSignMag(3,3,DIR_BIT3);
	}
}

// put the servo Output to a specified PWM channel 
// and direction bit in signed and magnitude mode

void OutputSignMag(int ch, int pwm, int dir_bit)
{
	if (chan[ch].Enable)
	{
		if (chan[ch].Output >= 0)
		{
			if (chan[ch].Output > MAXPWM) // don't go to 100%
				FPGA(IO_PWMS+pwm*2) = MAXPWM;  
			else
				FPGA(IO_PWMS+pwm*2) = chan[ch].Output;  
			SetBit(dir_bit);
		}
		else
		{
			if (chan[ch].Output < -MAXPWM) // don't go to 100%
				FPGA(IO_PWMS+pwm*2) = MAXPWM;  
			else
				FPGA(IO_PWMS+pwm*2) = -chan[ch].Output;  
			ClearBit(dir_bit);
		}
	}
	else
	{
		FPGA(IO_PWMS+pwm*2) = 0;  // whenever not enabled put 0% duty cycle
	}
}
