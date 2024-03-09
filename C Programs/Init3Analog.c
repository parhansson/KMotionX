#include "KMotionDef.h"

// Default Initial Configuration for 3 Axis Analog DAC Servo with encoder feedback
//
// Note Low Gains, Limited Max Limits, No Limit Options, No Soft Limits, 
// Large allowed Following Error, No Feed Forward, No Filters, etc

void main()
{
	SetBit(152);  // enable AMPS
	SetBit(157);
	SetBit(158);
	
	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=DAC_SERVO_MODE;
	ch0->Vel=40000;
	ch0->Accel=400000;
	ch0->Jerk=4e+06;
	ch0->P=0.2;
	ch0->I=0;
	ch0->D=0;
	ch0->FFAccel=0;
	ch0->FFVel=0;
	ch0->MaxI=200;
	ch0->MaxErr=200;
	ch0->MaxOutput=200;
	ch0->DeadBandGain=1;
	ch0->DeadBandRange=0;
	ch0->InputChan0=0;
	ch0->InputChan1=1;
	ch0->OutputChan0=0;
	ch0->OutputChan1=1;
	ch0->MasterAxis=-1;
	ch0->LimitSwitchOptions=0x100;
	ch0->LimitSwitchNegBit=0;
	ch0->LimitSwitchPosBit=0;
	ch0->SoftLimitPos=1e+30;
	ch0->SoftLimitNeg=-1e+30;
	ch0->InputGain0=1;
	ch0->InputGain1=1;
	ch0->InputOffset0=0;
	ch0->InputOffset1=0;
	ch0->OutputGain=1;
	ch0->OutputOffset=0;
	ch0->SlaveGain=1;
	ch0->BacklashMode=BACKLASH_OFF;
	ch0->BacklashAmount=0;
	ch0->BacklashRate=0;
	ch0->invDistPerCycle=1;
	ch0->Lead=0;
	ch0->MaxFollowingError=10000000;
	ch0->StepperAmplitude=250;

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
	ch1->OutputMode=DAC_SERVO_MODE;
	ch1->Vel=40000;
	ch1->Accel=400000;
	ch1->Jerk=4e+06;
	ch1->P=0.2;
	ch1->I=0;
	ch1->D=0;
	ch1->FFAccel=0;
	ch1->FFVel=0;
	ch1->MaxI=200;
	ch1->MaxErr=200;
	ch1->MaxOutput=200;
	ch1->DeadBandGain=1;
	ch1->DeadBandRange=0;
	ch1->InputChan0=1;
	ch1->InputChan1=1;
	ch1->OutputChan0=1;
	ch1->OutputChan1=1;
	ch1->MasterAxis=-1;
	ch1->LimitSwitchOptions=0x100;
	ch1->LimitSwitchNegBit=0;
	ch1->LimitSwitchPosBit=0;
	ch1->SoftLimitPos=1e+30;
	ch1->SoftLimitNeg=-1e+30;
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
	ch1->invDistPerCycle=1;
	ch1->Lead=0;
	ch1->MaxFollowingError=10000000;
	ch1->StepperAmplitude=250;

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
	
	ch2->InputMode=ENCODER_MODE;
	ch2->OutputMode=DAC_SERVO_MODE;
	ch2->Vel=40000;
	ch2->Accel=400000;
	ch2->Jerk=4e+06;
	ch2->P=0.2;
	ch2->I=0;
	ch2->D=0;
	ch2->FFAccel=0;
	ch2->FFVel=0;
	ch2->MaxI=200;
	ch2->MaxErr=200;
	ch2->MaxOutput=200;
	ch2->DeadBandGain=1;
	ch2->DeadBandRange=0;
	ch2->InputChan0=2;
	ch2->InputChan1=1;
	ch2->OutputChan0=2;
	ch2->OutputChan1=1;
	ch2->MasterAxis=-1;
	ch2->LimitSwitchOptions=0x100;
	ch2->LimitSwitchNegBit=0;
	ch2->LimitSwitchPosBit=0;
	ch2->SoftLimitPos=1e+30;
	ch2->SoftLimitNeg=-1e+30;
	ch2->InputGain0=1;
	ch2->InputGain1=1;
	ch2->InputOffset0=0;
	ch2->InputOffset1=0;
	ch2->OutputGain=1;
	ch2->OutputOffset=0;
	ch2->SlaveGain=1;
	ch2->BacklashMode=BACKLASH_OFF;
	ch2->BacklashAmount=0;
	ch2->BacklashRate=0;
	ch2->invDistPerCycle=1;
	ch2->Lead=0;
	ch2->MaxFollowingError=10000000;
	ch2->StepperAmplitude=250;

	ch2->iir[0].B0=1;
	ch2->iir[0].B1=0;
	ch2->iir[0].B2=0;
	ch2->iir[0].A1=0;
	ch2->iir[0].A2=0;

	ch2->iir[1].B0=1;
	ch2->iir[1].B1=0;
	ch2->iir[1].B2=0;
	ch2->iir[1].A1=0;
	ch2->iir[1].A2=0;

	ch2->iir[2].B0=1;
	ch2->iir[2].B1=0;
	ch2->iir[2].B2=0;
	ch2->iir[2].A1=0;
	ch2->iir[2].A2=0;

    EnableAxis(0);
	EnableAxis(1);
	EnableAxis(2);

    DefineCoordSystem(0,1,2,-1);
}

