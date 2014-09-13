#include "KMotionDef.h"

// Configure dual Snap Amps for 3 stepper motors

void main() 
{
	
	WriteSnapAmp(SNAP0+SNAP_PEAK_CUR_LIMIT0,9);  // current limit
	WriteSnapAmp(SNAP0+SNAP_PEAK_CUR_LIMIT1,9);  // current limit
	WriteSnapAmp(SNAP1+SNAP_PEAK_CUR_LIMIT0,9);  // current limit
	WriteSnapAmp(SNAP1+SNAP_PEAK_CUR_LIMIT1,9);  // current limit
	Delay_sec(0.1);  // wait for any fault to clear
   	
   	
	ch4->InputMode=NO_INPUT_MODE;
	ch4->OutputMode=MICROSTEP_MODE;
	ch4->Vel=400.000000;
	ch4->Accel=5000.000000;
	ch4->Jerk=50000.000000;
	ch4->P=1.000000;
	ch4->I=0.000000;
	ch4->D=0.000000;
	ch4->FFAccel=0.000000;
	ch4->FFVel=0.000000;
	ch4->MaxI=200.000000;
	ch4->MaxErr=1000000.000000;
	ch4->MaxOutput=200.000000;
	ch4->DeadBandGain=1.000000;
	ch4->DeadBandRange=0.000000;
	ch4->InputChan0=4;
	ch4->InputChan1=4;
	ch4->OutputChan0=8;
	ch4->OutputChan1=9;
	ch4->LimitSwitchOptions=0x0;
	ch4->InputGain0=1.000000;
	ch4->InputGain1=1.000000;
	ch4->InputOffset0=0.000000;
	ch4->InputOffset1=0.000000;
	ch4->invDistPerCycle=1.000000;
	ch4->Lead=0.000000;
	ch4->MaxFollowingError=1000000000.000000;
	ch4->StepperAmplitude=60.000000;

	ch4->iir[0].B0=1.000000;
	ch4->iir[0].B1=0.000000;
	ch4->iir[0].B2=0.000000;
	ch4->iir[0].A1=0.000000;
	ch4->iir[0].A2=0.000000;

	ch4->iir[1].B0=1.000000;
	ch4->iir[1].B1=0.000000;
	ch4->iir[1].B2=0.000000;
	ch4->iir[1].A1=0.000000;
	ch4->iir[1].A2=0.000000;

	ch4->iir[2].B0=1.000000;
	ch4->iir[2].B1=0.000000;
	ch4->iir[2].B2=0.000000;
	ch4->iir[2].A1=0.000000;
	ch4->iir[2].A2=0.000000;
	
	
	ch5->InputMode=ENCODER_MODE;
	ch5->OutputMode=MICROSTEP_MODE;
	ch5->Vel=400.000000;
	ch5->Accel=5000.000000;
	ch5->Jerk=50000.000000;
	ch5->P=1.000000;
	ch5->I=0.000000;
	ch5->D=0.000000;
	ch5->FFAccel=0.000000;
	ch5->FFVel=0.000000;
	ch5->MaxI=200.000000;
	ch5->MaxErr=1000000.000000;
	ch5->MaxOutput=200.000000;
	ch5->DeadBandGain=1.000000;
	ch5->DeadBandRange=0.000000;
	ch5->InputChan0=5;
	ch5->InputChan1=5;
	ch5->OutputChan0=10;
	ch5->OutputChan1=11;
	ch5->LimitSwitchOptions=0x0;
	ch5->InputGain0=1.000000;
	ch5->InputGain1=1.000000;
	ch5->InputOffset0=0.000000;
	ch5->InputOffset1=0.000000;
	ch5->invDistPerCycle=1.000000;
	ch5->Lead=0.000000;
	ch5->MaxFollowingError=1000000000.000000;
	ch5->StepperAmplitude=60.000000;

	ch5->iir[0].B0=1.000000;
	ch5->iir[0].B1=0.000000;
	ch5->iir[0].B2=0.000000;
	ch5->iir[0].A1=0.000000;
	ch5->iir[0].A2=0.000000;

	ch5->iir[1].B0=1.000000;
	ch5->iir[1].B1=0.000000;
	ch5->iir[1].B2=0.000000;
	ch5->iir[1].A1=0.000000;
	ch5->iir[1].A2=0.000000;

	ch5->iir[2].B0=1.000000;
	ch5->iir[2].B1=0.000000;
	ch5->iir[2].B2=0.000000;
	ch5->iir[2].A1=0.000000;
	ch5->iir[2].A2=0.000000;
	
	
	DefineCoordSystem(4,5,-1,-1);
	
	EnableAxis(4);
	EnableAxis(5);
}

