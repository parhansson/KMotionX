#include "KMotionDef.h"

#define QA 26	// define to which IO bits the AB signals are connected 
#define QB 27	

main()
{
	int BitA,Change1=0,Change2=0, DiffX2;
	int PosNoWrap, NewPos, Pos=0, wraps;
	double Factor=0;
	
	SetBit(152);
	SetBit(157);
	SetBit(158);
	
	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=DAC_SERVO_MODE;
	ch0->Vel=100000.000000;
	ch0->Accel=1000000.000000;
	ch0->Jerk=10000000.000000;
	ch0->P=0.100000;
	ch0->I=0.000000;
	ch0->D=0.000000;
	ch0->FFAccel=0.000000;
	ch0->FFVel=0.000000;
	ch0->MaxI=200.000000;
	ch0->MaxErr=20000.000000;
	ch0->MaxOutput=2000.000000;
	ch0->DeadBandGain=1.000000;
	ch0->DeadBandRange=0.000000;
	ch0->InputChan0=0;
	ch0->InputChan1=0;
	ch0->OutputChan0=0;
	ch0->OutputChan1=0;
	ch0->LimitSwitchOptions=0x0;
	ch0->InputGain0=1.000000;
	ch0->InputGain1=1.000000;
	ch0->InputOffset0=0.000000;
	ch0->InputOffset1=0.000000;
	ch0->invDistPerCycle=0.000250;
	ch0->Lead=0.000000;
	ch0->MaxFollowingError=100000.000000;
	ch0->StepperAmplitude=100.000000;

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
	ch1->OutputMode=DAC_SERVO_MODE;
	ch1->Vel=100000.000000;
	ch1->Accel=1000000.000000;
	ch1->Jerk=10000000.000000;
	ch1->P=0.700000;
	ch1->I=0.003000;
	ch1->D=0.000000;
	ch1->FFAccel=0.000000;
	ch1->FFVel=0.000000;
	ch1->MaxI=2000.000000;
	ch1->MaxErr=20000.000000;
	ch1->MaxOutput=2000.000000;
	ch1->DeadBandGain=1.000000;
	ch1->DeadBandRange=0.000000;
	ch1->InputChan0=1;
	ch1->InputChan1=1;
	ch1->OutputChan0=1;
	ch1->OutputChan1=1;
	ch1->LimitSwitchOptions=0x0;
	ch1->InputGain0=1.000000;
	ch1->InputGain1=1.000000;
	ch1->InputOffset0=0.000000;
	ch1->InputOffset1=0.000000;
	ch1->invDistPerCycle=0.000250;
	ch1->Lead=0.000000;
	ch1->MaxFollowingError=1000.000000;
	ch1->StepperAmplitude=100.000000;

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
	ch2->OutputMode=DAC_SERVO_MODE;
	ch2->Vel=100000.000000;
	ch2->Accel=1000000.000000;
	ch2->Jerk=10000000.000000;
	ch2->P=0.300000;
	ch2->I=0.003000;
	ch2->D=0.000000;
	ch2->FFAccel=0.000000;
	ch2->FFVel=0.000000;
	ch2->MaxI=2000.000000;
	ch2->MaxErr=20000.000000;
	ch2->MaxOutput=2000.000000;
	ch2->DeadBandGain=1.000000;
	ch2->DeadBandRange=0.000000;
	ch2->InputChan0=2;
	ch2->InputChan1=2;
	ch2->OutputChan0=2;
	ch2->OutputChan1=2;
	ch2->LimitSwitchOptions=0x0;
	ch2->InputGain0=1.000000;
	ch2->InputGain1=1.000000;
	ch2->InputOffset0=0.000000;
	ch2->InputOffset1=0.000000;
	ch2->invDistPerCycle=0.000250;
	ch2->Lead=0.000000;
	ch2->MaxFollowingError=1000.000000;
	ch2->StepperAmplitude=100.000000;

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

    EnableAxis(0);
	EnableAxis(1);
	EnableAxis(2);

    DefineCoordSystem(0,1,2,-1);
}

