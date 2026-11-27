//*************************** KFlop Initialize Program *************************************

#include "KMotionDef.h"

int Blink(void)
{
int i;

	for (i=0; i<4; i++)
	{
		WaitNextTimeSlice();
		SetBit(46); //Kflop LED
		Delay_sec(0.1);
		ClearBit(46);
		Delay_sec(0.1);
   	}	
  
   	printf("@Initialization Complete\n");
    
	return 0;

}

int main() 
{
	
    SetBit(46); //Kflop LED
    
   	KStepPresent=TRUE;    //which tells KFLOP that KSTEP is connected 
 	FPGA(KAN_TRIG_REG)=4; //Mux PWM0 to JP7 Pin5 IO 44 for KSTEP 
	FPGA(STEP_PULSE_LENGTH_ADD) = 63 + 0x80; //which sets polarity and pulse length to 4us

    // X Axis Master
	ch0->InputMode=NO_INPUT_MODE;
	ch0->OutputMode=STEP_DIR_MODE;
	ch0->Vel=20000;
	ch0->Accel=50000;
	ch0->Jerk=2e+06;
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
	ch0->OutputChan1=0;
	ch0->MasterAxis=-1;
	ch0->LimitSwitchOptions=0x12c;
	ch0->LimitSwitchNegBit=1024;
	ch0->LimitSwitchPosBit=1025;
	ch0->SoftLimitPos=1e+09;
	ch0->SoftLimitNeg=-1e+09;
	ch0->InputGain0=1;
	ch0->InputGain1=1;
	ch0->InputOffset0=0;
	ch0->InputOffset1=0;
	ch0->OutputGain=-1;
	ch0->OutputOffset=0;
	ch0->SlaveGain=1;
	ch0->BacklashMode=BACKLASH_OFF;
	ch0->BacklashAmount=0;
	ch0->BacklashRate=0;
	ch0->invDistPerCycle=250;
	ch0->Lead=0;
	ch0->MaxFollowingError=1000000000;
	ch0->StepperAmplitude=100;

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

    // Y Axis Master
	ch1->InputMode=NO_INPUT_MODE;
	ch1->OutputMode=STEP_DIR_MODE;
	ch1->Vel=20000;
	ch1->Accel=50000;
	ch1->Jerk=2e+06;
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
	ch1->InputChan0=0;
	ch1->InputChan1=0;
	ch1->OutputChan0=9;
	ch1->OutputChan1=0;
	ch1->MasterAxis=-1;
	ch1->LimitSwitchOptions=0x12c;
	ch1->LimitSwitchNegBit=1026;
	ch1->LimitSwitchPosBit=1027;
	ch1->SoftLimitPos=1e+09;
	ch1->SoftLimitNeg=-1e+09;
	ch1->InputGain0=1;
	ch1->InputGain1=1;
	ch1->InputOffset0=0;
	ch1->InputOffset1=0;
	ch1->OutputGain=-1;
	ch1->OutputOffset=0;
	ch1->SlaveGain=1;
	ch1->BacklashMode=BACKLASH_OFF;
	ch1->BacklashAmount=0;
	ch1->BacklashRate=0;
	ch1->invDistPerCycle=250;
	ch1->Lead=0;
	ch1->MaxFollowingError=1000000000;
	ch1->StepperAmplitude=100;

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

	// Z Axis
	ch2->InputMode=NO_INPUT_MODE;
	ch2->OutputMode=STEP_DIR_MODE;
	ch2->Vel=20000;
	ch2->Accel=50000;
	ch2->Jerk=2e+06;
	ch2->P=1;
	ch2->I=0;
	ch2->D=0;
	ch2->FFAccel=0;
	ch2->FFVel=0;
	ch2->MaxI=200;
	ch2->MaxErr=1e+06;
	ch2->MaxOutput=200;
	ch2->DeadBandGain=1;
	ch2->DeadBandRange=0;
	ch2->InputChan0=0;
	ch2->InputChan1=0;
	ch2->OutputChan0=10;
	ch2->OutputChan1=0;
	ch2->MasterAxis=-1;
	ch2->LimitSwitchOptions=0x128;
	ch2->LimitSwitchNegBit=0;
	ch2->LimitSwitchPosBit=1028;
	ch2->SoftLimitPos=1e+09;
	ch2->SoftLimitNeg=-1e+09;
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
	ch2->invDistPerCycle=254;
	ch2->Lead=0;
	ch2->MaxFollowingError=1000000000;
	ch2->StepperAmplitude=100;

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

	ch2->iir[2].B0=0.000769;
	ch2->iir[2].B1=0.001538;
	ch2->iir[2].B2=0;
	ch2->iir[2].A1=1.92076;
	ch2->iir[2].A2=-0.923833;
	EnableAxisDest(2,0);

	// X Axis Slave
	ch3->InputMode=NO_INPUT_MODE;
	ch3->OutputMode=STEP_DIR_MODE;
	ch3->Vel=20000;
	ch3->Accel=50000;
	ch3->Jerk=2e+06;
	ch3->P=1;
	ch3->I=0;
	ch3->D=0;
	ch3->FFAccel=0;
	ch3->FFVel=0;
	ch3->MaxI=200;
	ch3->MaxErr=1e+06;
	ch3->MaxOutput=200;
	ch3->DeadBandGain=1;
	ch3->DeadBandRange=0;
	ch3->InputChan0=0;
	ch3->InputChan1=0;
	ch3->OutputChan0=12;
	ch3->OutputChan1=0;
	ch3->MasterAxis=0;
	ch3->LimitSwitchOptions=0x12c;
	ch3->LimitSwitchNegBit=1024;
	ch3->LimitSwitchPosBit=1025;
	ch3->SoftLimitPos=1e+09;
	ch3->SoftLimitNeg=-1e+09;
	ch3->InputGain0=1;
	ch3->InputGain1=1;
	ch3->InputOffset0=0;
	ch3->InputOffset1=0;
	ch3->OutputGain=-1;
	ch3->OutputOffset=0;
	ch3->SlaveGain=1;
	ch3->BacklashMode=BACKLASH_OFF;
	ch3->BacklashAmount=0;
	ch3->BacklashRate=0;
	ch3->invDistPerCycle=250;
	ch3->Lead=0;
	ch3->MaxFollowingError=1000000000;
	ch3->StepperAmplitude=100;

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

	// Y Axis Slave
	ch4->InputMode=NO_INPUT_MODE;
	ch4->OutputMode=STEP_DIR_MODE;
	ch4->Vel=20000;
	ch4->Accel=50000;
	ch4->Jerk=2e+06;
	ch4->P=1;
	ch4->I=0;
	ch4->D=0;
	ch4->FFAccel=0;
	ch4->FFVel=0;
	ch4->MaxI=200;
	ch4->MaxErr=1e+06;
	ch4->MaxOutput=200;
	ch4->DeadBandGain=1;
	ch4->DeadBandRange=0;
	ch4->InputChan0=0;
	ch4->InputChan1=0;
	ch4->OutputChan0=13;
	ch4->OutputChan1=0;
	ch4->MasterAxis=1;
	ch4->LimitSwitchOptions=0x12c;
	ch4->LimitSwitchNegBit=1026;
	ch4->LimitSwitchPosBit=1027;
	ch4->SoftLimitPos=1e+09;
	ch4->SoftLimitNeg=-1e+09;
	ch4->InputGain0=1;
	ch4->InputGain1=1;
	ch4->InputOffset0=0;
	ch4->InputOffset1=0;
	ch4->OutputGain=-1;
	ch4->OutputOffset=0;
	ch4->SlaveGain=1;
	ch4->BacklashMode=BACKLASH_OFF;
	ch4->BacklashAmount=0;
	ch4->BacklashRate=0;
	ch4->invDistPerCycle=250;
	ch4->Lead=0;
	ch4->MaxFollowingError=1000000000;
	ch4->StepperAmplitude=100;

	ch4->iir[0].B0=1;
	ch4->iir[0].B1=0;
	ch4->iir[0].B2=0;
	ch4->iir[0].A1=0;
	ch4->iir[0].A2=0;

	ch4->iir[1].B0=1;
	ch4->iir[1].B1=0;
	ch4->iir[1].B2=0;
	ch4->iir[1].A1=0;
	ch4->iir[1].A2=0;

	ch4->iir[2].B0=0.000769;
	ch4->iir[2].B1=0.001538;
	ch4->iir[2].B2=0.000769;
	ch4->iir[2].A1=1.92076;
	ch4->iir[2].A2=-0.923833;
	EnableAxisDest(4,0);


	DefineCoordSystem(0,1,2,-1);

	SetBitDirection(45,1); //This sets the Enable Signal as Output
	SetBit(45); //This Enables the amplifiers	

	SetFRO(1.0);

	Blink();


    return 0;
    
}

