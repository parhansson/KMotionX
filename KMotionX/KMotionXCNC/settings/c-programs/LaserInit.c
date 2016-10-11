#include "KMotionDef.h"
//extern PARAMETRIC_COEFF *LastCoordSystem0;
//extern PARAMETRIC_COEFF *ParametricCoeffs;
//extern double CS0_TimeExecuted;
//extern double CS0_t;
extern PARAMETRIC_COEFF *CoordSystem0;
extern int ParametricIndex;

//Step and Dir on JP7 pin 15-20, IO bits 8-13
// JP7 pin 7-14, IO bits 0-7 available for end limit switches 5v tolerant
#define STEPS_INCH 2540
#define STEPS_MM 100
#define STEPS_MM_Z 133.3333

#define CLOCK 16.6666e6
main()
{
  ch0->InputMode=NO_INPUT_MODE;
  ch0->OutputMode=STEP_DIR_MODE;
  ch0->Vel=STEPS_MM*400; // 400 mm/s;
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
  ch0->LimitSwitchOptions=0x100;
  ch0->LimitSwitchNegBit=0;
  ch0->LimitSwitchPosBit=0;
  ch0->SoftLimitPos=1e+009;
  ch0->SoftLimitNeg=-1e+009;
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
  ch0->invDistPerCycle=2.5;
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

  ch0->iir[2].B0=0.000768788;
  ch0->iir[2].B1=0.00153758;
  ch0->iir[2].B2=0.000768788;
  ch0->iir[2].A1=1.92076;
  ch0->iir[2].A2=-0.923833;


  ch1->InputMode=NO_INPUT_MODE;
  ch1->OutputMode=STEP_DIR_MODE;
  ch1->Vel=STEPS_MM*400; // 400 mm/s;
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
  ch1->InputChan0=1;
  ch1->InputChan1=0;
  ch1->OutputChan0=9;
  ch1->OutputChan1=0;
  ch1->MasterAxis=-1;
  ch1->LimitSwitchOptions=0x100;
  ch1->LimitSwitchNegBit=0;
  ch1->LimitSwitchPosBit=0;
  ch1->SoftLimitPos=1e+009;
  ch1->SoftLimitNeg=-1e+009;
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
  ch1->invDistPerCycle=2.5;
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
  ch1->iir[2].A1=1.92081;
  ch1->iir[2].A2=-0.923885;


  ch2->InputMode=NO_INPUT_MODE;
  ch2->OutputMode=STEP_DIR_MODE;
  ch2->Vel=STEPS_MM_Z*20; // 20 mm/s;
  ch2->Accel=80000;
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
  ch2->InputChan0=2;
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
  ch2->OutputGain=1;
  ch2->OutputOffset=0;
  ch2->SlaveGain=1;
  ch2->BacklashMode=BACKLASH_OFF;
  ch2->BacklashAmount=0;
  ch2->BacklashRate=0;
  ch2->invDistPerCycle=500;//1.25;
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

  ch2->iir[2].B0=0.000769;
  ch2->iir[2].B1=0.001538;
  ch2->iir[2].B2=0.000769;
  ch2->iir[2].A1=1.92081;
  ch2->iir[2].A2=-0.923885;

  EnableAxisDest(0,0);
  EnableAxisDest(1,0);
  EnableAxisDest(2,0);

  DefineCoordSystem(0,1,2,-1);
  //SetBitDirection(int bit, int dir);
  SetBitDirection(2, 1);
  SetBitDirection(4, 1);
  SetBitDirection(3, 1);
  
  printf("Machine initiated\n");
  
  return;

}
