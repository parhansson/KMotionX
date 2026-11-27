#include "KMotionDef.h"

int main() 
{
   	int j;
    float dist_to_move=100.0f;

	// setup an axis for a Stepper Motor 

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
	ch0->MaxFollowingError=1000000000.000000;
	ch0->StepperAmplitude=250.000000;
    EnableAxisDest(0,0.0);
    
    // Move Back and Forth

    for (j=0; j<4; j++)
    {
        MoveRel(0,dist_to_move);
        while (!CheckDone(0)) ;
        MoveRel(0,-dist_to_move);
        while (!CheckDone(0)) ;
    }
    return 0;
}
