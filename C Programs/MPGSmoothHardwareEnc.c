#include "KMotionDef.h"

// Example Init program that includes "smooth" MPG motion example
// which makes use of the exponential motion command.

#define MPG_INPUT_AXIS 6

#define TAU 0.08 // smoothness factor (Low Pass Time constant seconds)
#define FINAL_TIME 1.0 // Set final dest after this amount of time with no change

#define ENABLE_MPG 14

#define SELECTX 16
#define SELECTY 17
#define SELECTZ 18

#define FACTOR1 19
#define FACTOR10 20
#define FACTOR100 21

main()
{
	int Change1, NewPos, Pos;
	int InMotion=FALSE,Axis,LastAxis=-1;
	double LastChangeTime=0,Target,Factor=0;
	
	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=CL_STEP_DIR_MODE;
	ch0->Vel=10000.000000;
	ch0->Accel=60000.000000;
	ch0->Jerk=10000000.000000;
	ch0->P=0.000000;
	ch0->I=0.015000;
	ch0->D=0.000000;
	ch0->FFAccel=0.000000;
	ch0->FFVel=0.000000;
	ch0->MaxI=200.000000;
	ch0->MaxErr=200.000000;
	ch0->MaxOutput=2000.000000;
	ch0->DeadBandGain=1.000000;
	ch0->DeadBandRange=0.000000;
	ch0->InputChan0=0;
	ch0->InputChan1=0;
	ch0->OutputChan0=12;
	ch0->OutputChan1=0;
	ch0->LimitSwitchOptions=0x0;
	ch0->InputGain0=-2.500000;
	ch0->InputOffset0=0.000000;
	ch0->OutputGain=1.000000;
	ch0->OutputOffset=0.000000;
	ch0->SlaveGain=1.00000;
	ch0->BacklashMode=BACKLASH_OFF;
	ch0->BacklashAmount=0.00000;
	ch0->BacklashRate=0.00000;
	ch0->invDistPerCycle=1.000000;
	ch0->Lead=0.000000;
	ch0->MaxFollowingError=100000.000000;
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
	ch0->iir[2].A2=0.00000;
    
	
	EnableAxisDest(0,0);		

	EnableAxisDest(0,ch0->Position);
	DefineCoordSystem(0,-1,-1,-1);
	
	Pos = chan[MPG_INPUT_AXIS].Position;

	for (;;)
	{
		NewPos = chan[MPG_INPUT_AXIS].Position;
		Change1 = NewPos - Pos;
		Pos = NewPos;

		if (ReadBit(ENABLE_MPG)) // if button pressed ignore the encoder.
			Change1 = 0;
		else if (ReadBit(FACTOR1))  // is X1 selected?
			Factor = 2;
		else if (ReadBit(FACTOR10))  // is X10 selected?
			Factor = 20;
		else if (ReadBit(FACTOR100))  // is X100 selected?
			Factor = 200;
		else                   
			Factor = 0.0;

		if (ReadBit(SELECTX))  // is x selected?
			Axis=0;
		else if (ReadBit(SELECTY))  // is y selected?
			Axis=1;
		else if (ReadBit(SELECTZ))  // is z selected?
			Axis=2;

		// check if the Axis just changed or we have been 
		// converging to the target for a long time
		if (Axis != LastAxis || 
			(InMotion && Time_sec() > LastChangeTime+FINAL_TIME))
		{
			if (InMotion)
				Move(LastAxis,Target);  //finalize any motion
	
			LastAxis = Axis;
			InMotion = FALSE;
		}
		
		if (Change1) // did we move?
		{
			if (!InMotion) Target = chan[Axis].Dest;
			Target += Change1 * Factor;
			MoveExp(Axis,Target,TAU);  // note: contains a WaitNextTimeSlice
			LastChangeTime = Time_sec();
			InMotion=TRUE;
		}
		else
		{
			WaitNextTimeSlice();
		}		
	}	

    return 0;
}
