#include "KMotionDef.h"

// Defines axis 0 and 1 as simple steppers
// enables them
// sets them as an xy coordinate system for GCode

int main() 
{
	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=MICROSTEP_MODE;
	ch0->Vel=200000.000000;
	ch0->Accel=2500000.000000;
	ch0->Jerk=5e6;
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
	ch0->InputChan1=0;
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
	ch1->OutputMode=MICROSTEP_MODE;
	ch1->Vel=180000.000000;
	ch1->Accel=500000.000000;
	ch1->Jerk=5e6;
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
	ch1->InputChan1=1;
	ch1->OutputChan0=2;
	ch1->OutputChan1=3;
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
	ch2->OutputMode=MICROSTEP_MODE;
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
	ch2->InputChan1=2;
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
	ch3->OutputMode=MICROSTEP_MODE;
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
	ch3->InputChan1=3;
	ch3->OutputChan0=6;
	ch3->OutputChan1=7;
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


	ch4->InputMode=ENCODER_MODE;
	ch4->OutputMode=MICROSTEP_MODE;
	ch4->Vel=100.000000;
	ch4->Accel=1000.000000;
	ch4->Jerk=10000.000000;
	ch4->P=1.000000;
	ch4->I=0.000000;
	ch4->D=0.000000;
	ch4->FFAccel=0.000000;
	ch4->FFVel=0.000000;
	ch4->MaxI=200.000000;
	ch4->MaxErr=200.000000;
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
	ch4->StepperAmplitude=250.000000;

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
	ch5->Vel=100.000000;
	ch5->Accel=1000.000000;
	ch5->Jerk=10000.000000;
	ch5->P=1.000000;
	ch5->I=0.000000;
	ch5->D=0.000000;
	ch5->FFAccel=0.000000;
	ch5->FFVel=0.000000;
	ch5->MaxI=200.000000;
	ch5->MaxErr=200.000000;
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
	ch5->StepperAmplitude=250.000000;

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


	
	EnableAxisDest(0,0);
	EnableAxisDest(1,0);
	EnableAxisDest(2,0);
	EnableAxisDest(3,0);
	EnableAxisDest(4,0);
	EnableAxisDest(5,0);

	DefineCoordSystem6(0,1,2,3,4,5);

    return 0;
}
