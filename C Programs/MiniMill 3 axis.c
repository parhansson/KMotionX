#include "KMotionDef.h"

// Configure axes for Mini Mill

void main() 
{
    float k=0,A=150.0f;   // set coil current amplitude to 30 PWM units

	SetBit(29); //Fan on
   	// rotate until we find the index mark

	ch0->Enable=FALSE;
    SetBitDirection(8,0);  // define bit 8 (index) as input
	for (;;)
    {
		Delay_sec(0.001);  // wait a millisecond 
		
		Write3PH(ch0,A, --k/1000.0);  // move the pole 

		if (!ReadBit(8))  // check for index mark
		{
			ch0->Position=0;  // set current position to Zero
			
			// set commutation offset to 1/6th of a cycle
			// encoder has 4000 counts/rev 
			// motor has 3 cycles per rev
			
			ch0->CommutationOffset = 4000/3.0/6.0;
			break;
		}
    }

	Write3PH(ch0,0,0);    // turn off the coil    

	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=BRUSHLESS_3PH_MODE;
	ch0->Vel=50000.000000;
	ch0->Accel=100000.000000;
	ch0->Jerk=300000.000000;
	ch0->P=0.600000;
	ch0->I=0.001000;
	ch0->D=0.000000;
	ch0->FFAccel=0.000000;
	ch0->FFVel=0.000000;
	ch0->MaxI=200.000000;
	ch0->MaxErr=200.000000;
	ch0->MaxOutput=230.000000;
	ch0->DeadBandGain=0.100000;
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
	ch0->invDistPerCycle=-0.000750;
	ch0->Lead=0.000000;
	ch0->MaxFollowingError=4000.000000;
	ch0->StepperAmplitude=100.000000;

	ch0->iir[0].B0=3.890210;
	ch0->iir[0].B1=-7.562370;
	ch0->iir[0].B2=3.675210;
	ch0->iir[0].A1=1.889450;
	ch0->iir[0].A2=-0.892502;

	ch0->iir[1].B0=1.000000;
	ch0->iir[1].B1=0.000000;
	ch0->iir[1].B2=0.000000;
	ch0->iir[1].A1=0.000000;
	ch0->iir[1].A2=0.000000;

	ch0->iir[2].B0=0.002960;
	ch0->iir[2].B1=0.005921;
	ch0->iir[2].B2=0.002960;
	ch0->iir[2].A1=1.841730;
	ch0->iir[2].A2=-0.853569;

	ch1->InputMode=NO_INPUT_MODE;
	ch1->OutputMode=MICROSTEP_MODE;
	ch1->Vel=250.000000;
	ch1->Accel=300.000000;
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
	ch1->OutputChan0=2;
	ch1->OutputChan1=3;
	ch1->LimitSwitchOptions=0x0;
	ch1->InputGain0=1.000000;
	ch1->InputGain1=1.000000;
	ch1->InputOffset0=0.000000;
	ch1->InputOffset1=0.000000;
	ch1->invDistPerCycle=1.000000;
	ch1->Lead=100.000000;
	ch1->MaxFollowingError=1000000000.000000;
	ch1->StepperAmplitude=50.000000;

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
	
	ch2->InputMode=NO_INPUT_MODE;
	ch2->OutputMode=MICROSTEP_MODE;
	ch2->Vel=250.000000;
	ch2->Accel=500.000000;
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
	ch2->Lead=100.000000;
	ch2->MaxFollowingError=1000000000.000000;
	ch2->StepperAmplitude=50.000000;

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
   	
	DefineCoordSystem(1,2,0,-1);
	
	EnableAxis(0);
	EnableAxis(1);
	EnableAxis(2);
}

