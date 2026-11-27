#include "KMotionDef.h"

// Defines axis 0 as a simple stepper
//
// Drives motor destination to match encoder position
// (electronic gearing)


int main() 
{
	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=MICROSTEP_MODE;
	ch0->Vel=100.000000;
	ch0->Accel=1000.000000;
	ch0->Jerk=10000.000000;
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
	ch0->StepperAmplitude=250.000000;
    EnableAxisDest(0,0);

    for(;;)
    {
		ch0->Dest = ch0->Position * 0.01;
    }
}
