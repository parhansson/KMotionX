#include "KMotionDef.h"

// Phase find and servo Axis 0 as a 3 phase brushless without using Z index

void main() 
{
    float A=50.0f;   // set coil current amplitude to A PWM units

	SetBit(29); //Fan on

	ch0->Enable=FALSE;

	Write3PH(ch0,A, 0.0);  // energize a pole at angle zero 
	
	Delay_sec(3.0);
	
	
	// set commutation offset to 1/6th of a cycle
	// encoder has 4000 counts/rev 
	// motor has 3 cycles per rev
	
	ch0->CommutationOffset = 4000/3.0/6.0;

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
	ch0->invDistPerCycle=-0.000250;
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

	EnableAxis(0);
}

