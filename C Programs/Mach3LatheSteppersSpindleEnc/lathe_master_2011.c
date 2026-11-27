

#include "KMotionDef.h"

// Defines axis x and z as simple steppers
// enables them
// sets them as an xz coordinate system for GCode
// Defines axis 3 as spindle and enables it

int main() 
{

	SetBitDirection(0,1); // set spindle enable

	// setup ch1 as Z axis

	ch1->InputMode=NO_INPUT_MODE;
	ch1->OutputMode=STEP_DIR_MODE;
	ch1->Vel=30000;
	ch1->Accel=600000;
	ch1->Jerk=500000000;
	ch1->P=1;
	ch1->I=0;
	ch1->D=0;
	ch1->FFAccel=0;
	ch1->FFVel=0;
	ch1->MaxI=200;
	ch1->MaxErr=1000000;
	ch1->MaxOutput=200;
	ch1->DeadBandGain=1;
	ch1->DeadBandRange=0;
	ch1->InputChan0=1;
	ch1->InputChan1=0;
	ch1->OutputChan0=1;
	ch1->OutputChan1=1;
	ch1->MasterAxis=-1;
	ch1->LimitSwitchOptions=0x2c2c0000;
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

	// setup ch2 as X axis

	ch2->InputMode=NO_INPUT_MODE;
	ch2->OutputMode=STEP_DIR_MODE;
	ch2->Vel=30000;
	ch2->Accel=600000;
	ch2->Jerk=500000000;
	ch2->P=1;
	ch2->I=0;
	ch2->D=0;
	ch2->FFAccel=0;
	ch2->FFVel=0;
	ch2->MaxI=200;
	ch2->MaxErr=1000000;
	ch2->MaxOutput=200;
	ch2->DeadBandGain=1;
	ch2->DeadBandRange=0;
	ch2->InputChan0=2;
	ch2->InputChan1=0;
	ch2->OutputChan0=2;
	ch2->OutputChan1=2;
	ch2->MasterAxis=-1;
	ch2->LimitSwitchOptions=0x2c2c0000;
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
	ch2->MaxFollowingError=1000000000;
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


	// set up ch3 as spindle

	ch3->InputMode=ENCODER_MODE;
	ch3->OutputMode=STEP_DIR_MODE;
	ch3->Vel=10000;
	ch3->Accel=8000000;
	ch3->Jerk=180000000;
	ch3->P=1;
	ch3->I=0;
	ch3->D=0;
	ch3->FFAccel=0;
	ch3->FFVel=0;
	ch3->MaxI=200;
	ch3->MaxErr=200;
	ch3->MaxOutput=200;
	ch3->DeadBandGain=1;
	ch3->DeadBandRange=0;
	ch3->InputChan0=3;
	ch3->InputChan1=0;
	ch3->OutputChan0=11;
	ch3->OutputChan1=11;
	ch3->MasterAxis=-1;
	ch3->LimitSwitchOptions=0x4040000;
	ch3->InputGain0=1;
	ch3->InputGain1=1;
	ch3->InputOffset0=0;
	ch3->InputOffset1=0;
	ch3->OutputGain=1;
	ch3->OutputOffset=0.3000000119209;
	ch3->SlaveGain=1;
	ch3->BacklashMode=BACKLASH_OFF;
	ch3->BacklashAmount=0;
	ch3->BacklashRate=0;
	ch3->invDistPerCycle=1;
	ch3->Lead=0;
	ch3->MaxFollowingError=1000000000;
	ch3->StepperAmplitude=250;

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

	ch3->iir[2].B0=1;
	ch3->iir[2].B1=0;
	ch3->iir[2].B2=0;
	ch3->iir[2].A1=0;
	ch3->iir[2].A2=0;


	// enable axis and spindle

	EnableAxisDest(3,0);
    EnableAxisDest(2,0);
    EnableAxisDest(1,0);
    
    DefineCoordSystem(1,-1,2,-1);  
    return 0;
}
