#include "KMotionDef.h"

// Example Init program that includes "smooth" MPG motion example
// which makes use of the exponential motion command.
//
// "Accelerates" by increasing rate depending on speed of MPG Rotation

#define QA 26	// define to which IO bits the AB signals are connected 
#define QB 27	

#define TAU 0.08 // smoothness factor (Low Pass Time constant seconds)
#define FINAL_TIME 1.0 // Set final dest after this amount of time with no change

#define MPG_LOW_PASS_K 0.001f  // smaller gives more smoothing
#define MPG_ACCEL 0.01f        // bigger gives more effect

main()
{
	int BitA,Change1=0,Change2=0, DiffX2;
	int PosNoWrap, NewPos, Pos=0, wraps;
	int InMotion=FALSE,Axis,LastAxis=-1;
	double LastChangeTime=0,Target,Factor=0;
	float MPG_Vel_Filtered=0.0;
	
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
	

	for (;;)
	{
		// convert quadrature to 2 bit binary
		BitA = ReadBit(QA);   
		PosNoWrap = (ReadBit(QB) ^ BitA) | (BitA<<1);

		// Diff between expected position based on average of two prev deltas
		// and position with no wraps.  (Keep as X2 to avoid division by 2)
		
		DiffX2 = 2*(Pos-PosNoWrap) + (Change2+Change1);
		
		// Calc quadrature wraparounds to bring Diff nearest zero
		// offset by 128 wraps to avoid requiring floor()
		wraps = ((DiffX2+1028)>>3)-128;
		
		// factor in the quadrature wraparounds
		NewPos = PosNoWrap + (wraps<<2);

		Change2 = Change1;
		Change1 = NewPos - Pos;
		Pos = NewPos;
		
		MPG_Vel_Filtered = Change1 * MPG_LOW_PASS_K + MPG_Vel_Filtered * (1.0f-MPG_LOW_PASS_K);

		if (ReadBit(33))  // is X0.01 selected?
			Factor = 0.01;
		else if (ReadBit(34))  // is X0.1 selected?
			Factor = 0.1;
		else if (ReadBit(31))  // is X1 selected?
			Factor = 1.0;
		else if (ReadBit(32))  // is X100 selected?
			Factor = 75.0;
		else                   // must be X10 then
			Factor = 10.0;

		if (ReadBit(28))  // is x selected?
			Axis=0;
		else if (ReadBit(29))  // is y selected?
			Axis=1;
		else if (ReadBit(30))  // is z selected?
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
			Target += Change1 * Factor * (1.0+MPG_ACCEL*fast_fabs(MPG_Vel_Filtered));
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
