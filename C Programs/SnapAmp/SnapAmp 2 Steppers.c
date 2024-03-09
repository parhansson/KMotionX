#include "KMotionDef.h"

// Configure Snap Amp for 2 stepper motors

void main() 
{
	// peak current limits
	
	WriteSnapAmp(SNAP0+SNAP_PEAK_CUR_LIMIT0,10); 
	WriteSnapAmp(SNAP0+SNAP_PEAK_CUR_LIMIT1,10); 

	// voltage clamps
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP0 ,SNAP_CONVERT_VOLTS_TO_ADC(80.0));
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP_ENA0,1);
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP1 ,SNAP_CONVERT_VOLTS_TO_ADC(80.0));
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP_ENA1,1);

	Delay_sec(0.1);  // wait for any fault to clear
   	
   	
	ch0->InputMode=NO_INPUT_MODE;
	ch0->OutputMode=MICROSTEP_MODE;
	ch0->Vel=400;
	ch0->Accel=5000;
	ch0->Jerk=50000;
	ch0->P=1;
	ch0->I=0;
	ch0->D=0;
	ch0->FFAccel=0;
	ch0->FFVel=0;
	ch0->MaxI=200;
	ch0->MaxErr=1e+06;
	ch0->MaxOutput=200;
	ch0->DeadBandGain=1;
	ch0->DeadBandRange=0;
	ch0->InputChan0=0;
	ch0->InputChan1=0;
	ch0->OutputChan0=8;
	ch0->OutputChan1=9;
	ch0->MasterAxis=-1;
	ch0->LimitSwitchOptions=0x100;
	ch0->LimitSwitchNegBit=0;
	ch0->LimitSwitchPosBit=0;
	ch0->SoftLimitPos=1e+09;
	ch0->SoftLimitNeg=-1e+09;
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
	ch0->MaxFollowingError=1000000000;
	ch0->StepperAmplitude=60;

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
	ch1->OutputMode=MICROSTEP_MODE;
	ch1->Vel=400;
	ch1->Accel=5000;
	ch1->Jerk=50000;
	ch1->P=1;
	ch1->I=0;
	ch1->D=0;
	ch1->FFAccel=0;
	ch1->FFVel=0;
	ch1->MaxI=200;
	ch1->MaxErr=1e+06;
	ch1->MaxOutput=200;
	ch1->DeadBandGain=1;
	ch1->DeadBandRange=0;
	ch1->InputChan0=1;
	ch1->InputChan1=1;
	ch1->OutputChan0=10;
	ch1->OutputChan1=11;
	ch1->MasterAxis=-1;
	ch1->LimitSwitchOptions=0x100;
	ch1->LimitSwitchNegBit=0;
	ch1->LimitSwitchPosBit=0;
	ch1->SoftLimitPos=1e+09;
	ch1->SoftLimitNeg=-1e+09;
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
	ch1->MaxFollowingError=1000000000;
	ch1->StepperAmplitude=60;

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
	
	
	DefineCoordSystem(0,1,-1,-1);
	
	EnableAxis(0);
	EnableAxis(1);
}

