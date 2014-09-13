#include "KMotionDef.h"

// Defines axis 0, 1, 2 as simple step dir TTL outputs for KSTEP
// enables them
// sets them as an xyz coordinate system for GCode

int main() 
{
	double T0, LastX=0, LastY=0, LastZ=0, LastA=0, Tau;
	
	KStepPresent=TRUE;      // enable KSTEP input multiplexing
	FPGA(KAN_TRIG_REG)=4;  	// Mux PWM0 to JP7 Pin5 IO 44 for KSTEP 

	FPGA(STEP_PULSE_LENGTH_ADD) = 63 + 0x80;  // set polarity and pulse length to 4us
	
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
	ch0->LimitSwitchOptions=0x0;
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
	ch1->MasterAxis=-1;
	ch1->LimitSwitchOptions=0x0;
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
	ch2->LimitSwitchOptions=0x0;
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
	
	ch3->InputMode=NO_INPUT_MODE;
	ch3->OutputMode=STEP_DIR_MODE;
	ch3->Vel=4000;
	ch3->Accel=400000;
	ch3->Jerk=4e+006;
	ch3->P=0;
	ch3->I=0.01;
	ch3->D=0;
	ch3->FFAccel=0;
	ch3->FFVel=0;
	ch3->MaxI=200;
	ch3->MaxErr=1e+006;
	ch3->MaxOutput=200;
	ch3->DeadBandGain=1;
	ch3->DeadBandRange=0;
	ch3->InputChan0=0;
	ch3->InputChan1=0;
	ch3->OutputChan0=11;
	ch3->OutputChan1=0;
	ch3->MasterAxis=-1;
	ch3->LimitSwitchOptions=0x0;
	ch3->InputGain0=1;
	ch3->InputGain1=1;
	ch3->InputOffset0=0;
	ch3->InputOffset1=0;
	ch3->OutputGain=1;
	ch3->OutputOffset=0;
	ch3->SlaveGain=1;
	ch3->BacklashMode=BACKLASH_OFF;
	ch3->BacklashAmount=0;
	ch3->BacklashRate=0;
	ch3->invDistPerCycle=1;
	ch3->Lead=0;
	ch3->MaxFollowingError=1000000000;
	ch3->StepperAmplitude=20;

	ch3->iir[0].B0=1;
	ch3->iir[0].B1=0;
	ch3->iir[0].B2=0;
	ch3->iir[0].A1=0;
	ch3->iir[0].A2=0;

	ch3->iir[1].B0=1;
	ch3->iir[1].B1=0;
	ch3->iir[1].B2=0;
	ch3->iir[1].A1=0;
	ch3->iir[1].A2=0;

	ch3->iir[2].B0=0.000769;
	ch3->iir[2].B1=0.001538;
	ch3->iir[2].B2=0.000769;
	ch3->iir[2].A1=1.92076;
	ch3->iir[2].A2=-0.923833;
	EnableAxisDest(3,0);

	DefineCoordSystem(0,1,2,3);
	
	SetBitDirection(45,1);  // set Enable Signal as Output
	SetBit(45);				// Enable the amplifiers
	
//  Add a small amount of Coordinated Motion Path smoothing if desired
//	Tau = 0.001;  // seconds for Low Pass Filter Time Constant
//	KLP = exp(-TIMEBASE/Tau);
	KLP=0; // force to 0 to disable
//	printf("Tau=%f KLP=%f\n",Tau,KLP);
	
	
	for (;;) // loop forever
	{
		WaitNextTimeSlice();
		
		// Service Amplifier disable after no activity for a while
		if (ch0->Dest != LastX || ch1->Dest != LastY || ch2->Dest != LastZ || ch3->Dest != LastA)
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

    return 0;
}
