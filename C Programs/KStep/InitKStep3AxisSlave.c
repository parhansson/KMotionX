#include "KMotionDef.h"

// Defines axis 0, 1, 2 as simple step dir TTL outputs for KSTEP
// enables them
// sets them as an xyz coordinate system for GCode

int main() 
{
	double T0, LastX=0, LastY=0, LastZ=0, Tau;
	
	KStepPresent=TRUE;      // enable KSTEP input multiplexing
	FPGA(KAN_TRIG_REG)=4;  	// Mux PWM0 to JP7 Pin5 IO 44 for KSTEP 

	FPGA(STEP_PULSE_LENGTH_ADD) = 63 + 0x80;  // set polarity and pulse length to 4us
	
	ClearStopImmediately();
	
	ch0->InputMode=NO_INPUT_MODE;
	ch0->OutputMode=STEP_DIR_MODE;
	ch0->Vel=40000;
	ch0->Accel=200000;
	ch0->Jerk=4e+006;
	ch0->P=0;
	ch0->I=0.01;
	ch0->D=0;
	ch0->FFAccel=0;
	ch0->FFVel=0;
	ch0->MaxI=200;
	ch0->MaxErr=1e+006;
	ch0->MaxOutput=200;
	ch0->DeadBandGain=1;
	ch0->DeadBandRange=0;
	ch0->InputChan0=0;
	ch0->InputChan1=0;
	ch0->OutputChan0=8;
	ch0->OutputChan1=0;
	ch0->MasterAxis=-1;
	ch0->LimitSwitchOptions=0x11a;
	ch0->LimitSwitchNegBit=46;
	ch0->LimitSwitchPosBit=46;
	ch0->SoftLimitPos=1e+030;
	ch0->SoftLimitNeg=-1e+030;
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
	ch0->StepperAmplitude=20;

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

	ch0->iir[2].B0=0.000769;
	ch0->iir[2].B1=0.001538;
	ch0->iir[2].B2=0.000769;
	ch0->iir[2].A1=1.92076;
	ch0->iir[2].A2=-0.923833;
    EnableAxisDest(0,0);

	ch1->InputMode=NO_INPUT_MODE;
	ch1->OutputMode=STEP_DIR_MODE;
	ch1->Vel=40000;
	ch1->Accel=200000;
	ch1->Jerk=4e+006;
	ch1->P=0;
	ch1->I=0.01;
	ch1->D=0;
	ch1->FFAccel=0;
	ch1->FFVel=0;
	ch1->MaxI=200;
	ch1->MaxErr=1e+006;
	ch1->MaxOutput=200;
	ch1->DeadBandGain=1;
	ch1->DeadBandRange=0;
	ch1->InputChan0=0;
	ch1->InputChan1=0;
	ch1->OutputChan0=9;
	ch1->OutputChan1=0;
	ch1->MasterAxis=0;
	ch1->LimitSwitchOptions=0x115;
	ch1->LimitSwitchNegBit=47;
	ch1->LimitSwitchPosBit=47;
	ch1->SoftLimitPos=1e+030;
	ch1->SoftLimitNeg=-1e+030;
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
	ch1->StepperAmplitude=20;

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

	ch1->iir[2].B0=0.000769;
	ch1->iir[2].B1=0.001538;
	ch1->iir[2].B2=0.000769;
	ch1->iir[2].A1=1.92076;
	ch1->iir[2].A2=-0.923833;
    EnableAxisDest(1,0);

	ch2->InputMode=NO_INPUT_MODE;
	ch2->OutputMode=STEP_DIR_MODE;
	ch2->Vel=40000;
	ch2->Accel=200000;
	ch2->Jerk=4e+006;
	ch2->P=0;
	ch2->I=0.01;
	ch2->D=0;
	ch2->FFAccel=0;
	ch2->FFVel=0;
	ch2->MaxI=200;
	ch2->MaxErr=1e+006;
	ch2->MaxOutput=200;
	ch2->DeadBandGain=1;
	ch2->DeadBandRange=0;
	ch2->InputChan0=0;
	ch2->InputChan1=0;
	ch2->OutputChan0=10;
	ch2->OutputChan1=0;
	ch2->MasterAxis=-1;
	ch2->LimitSwitchOptions=0x100;
	ch2->LimitSwitchNegBit=0;
	ch2->LimitSwitchPosBit=0;
	ch2->SoftLimitPos=1e+009;
	ch2->SoftLimitNeg=-1e+009;
	ch2->InputGain0=1;
	ch2->InputGain1=1;
	ch2->InputOffset0=0;
	ch2->InputOffset1=0;
	ch2->OutputGain=-1;
	ch2->OutputOffset=0;
	ch2->SlaveGain=1;
	ch2->BacklashMode=BACKLASH_OFF;
	ch2->BacklashAmount=0;
	ch2->BacklashRate=0;
	ch2->invDistPerCycle=1;
	ch2->Lead=0;
	ch2->MaxFollowingError=1000000000;
	ch2->StepperAmplitude=20;

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
	EnableAxisDest(2,0);

	DefineCoordSystem(0,2,-1,-1);
	
	SetBitDirection(45,1);  // set Enable Signal as Output
	SetBit(45);				// Enable the amplifiers
	
//  Add a small amount of Coordinated Motion Path smoothing if desired
//	Tau = 0.001;  // seconds for Low Pass Filter Time Constant
//	KLP = exp(-TIMEBASE/Tau);
	KLP=0; // force to 0 to disable
//	printf("Tau=%f KLP=%f\n",Tau,KLP);



	for (;;)
	{
		WaitNextTimeSlice();
		SetStateBit(46, ch0->Dest <=  1000.0);
		SetStateBit(47, ch0->Dest >= -1000.0);
		if (ch0->Enable && !ch1->Enable) DisableAxis(0);
		if (ch1->Enable && !ch0->Enable) DisableAxis(1);
	}


	
#if 0	
	for (;;) // loop forever
	{
		WaitNextTimeSlice();
		
		// Service Amplifier disable after no activity for a while
		if (ch0->Dest != LastX || ch1->Dest != LastY || ch2->Dest != LastZ)
		{
			// we moved - enable KStep Amplifers
			SetBit(45);
			T0 = Time_sec();  // record the time and position of last motion
			LastX=ch0->Dest;
			LastY=ch1->Dest;
			LastZ=ch2->Dest; 
		}
		else
		{
			if (Time_sec() > T0 + 10.0) ClearBit(45);
		}
	}
#endif
    return 0;
}
