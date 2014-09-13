#include "KMotionDef.h"

// Home and set the commutation for a 3 phase brushless motor
//
// Assume index mark  

void main() 
{
    float k=0,A=30.0f;   // set coil current amplitude to 30 PWM units

   	// rotate until we find the index mark

	ch0->Enable=FALSE;
    SetBitDirection(8,0);  // define bit 8 (index) as input
	for (;;)
    {
		Delay_sec(0.001);  // wait a millisecond 
		
		Write3PH(ch0,A, ++k/1000.0);  // move the pole 

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

	// define the axis as 3 phase BRUSHLESS_3PH_MODE
	// and set low PID gains
	
	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=BRUSHLESS_3PH_MODE;
	ch0->Vel=100000.000000;
	ch0->Accel=10000000.000000;
	ch0->Jerk=100000000.000000;
	ch0->P=0.100000;
	ch0->I=0.000000;
	ch0->D=0.000000;
	ch0->FFAccel=0.000000;
	ch0->FFVel=0.000000;
	ch0->MaxI=200.000000;
	ch0->MaxErr=1000000.000000;
	ch0->MaxOutput=60.000000;
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
	ch0->invDistPerCycle=-0.000750; // CW (3 cycles/rev) / (4000 encoder cnts/rev)
	ch0->Lead=0.000000;
	ch0->MaxFollowingError=1000000000.000000;
	ch0->StepperAmplitude=20.000000;

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

	EnableAxisDest(0,0.0f);  // Enable servo at destination of 0
}

