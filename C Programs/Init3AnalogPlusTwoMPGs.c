#include "KMotionDef.h"

#define QA 26	// define to which IO bits the AB signals are connected 
#define QB 27	
#define QA_2 28	// define to which IO bits the AB signals are connected 
#define QB_2 29	

main()
{
	int Change1=0,  Change2=0,   Pos=0;
	int Change1_2=0,Change2_2=0, Pos_2=0;

	int BitA, DiffX2, PosNoWrap, NewPos, wraps;

	double Factor=0;
	
	SetBit(152);
	SetBit(157);
	SetBit(158);
	
	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=DAC_SERVO_MODE;
	ch0->Vel=100000.000000;
	ch0->Accel=1000000.000000;
	ch0->Jerk=10000000.000000;
	ch0->P=1.000000;
	ch0->I=0.000000;
	ch0->D=0.000100;
	ch0->FFAccel=0.000000;
	ch0->FFVel=0.000000;
	ch0->MaxI=200.000000;
	ch0->MaxErr=200.000000;
	ch0->MaxOutput=200.000000;
	ch0->DeadBandGain=1.000000;
	ch0->DeadBandRange=0.000000;
	ch0->InputChan0=0;
	ch0->InputChan1=1;
	ch0->OutputChan0=0;
	ch0->OutputChan1=1;
	ch0->LimitSwitchOptions=0x8989000f;
	ch0->InputGain0=1.000000;
	ch0->InputGain1=1.000000;
	ch0->InputOffset0=0.000000;
	ch0->InputOffset1=0.000000;
	ch0->invDistPerCycle=1.000000;
	ch0->Lead=0.000000;
	ch0->MaxFollowingError=1000000000.000000;
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
	ch0->iir[2].A2=0.000000;
    EnableAxis(0);
    
		ch1->InputMode=ENCODER_MODE;
	ch1->OutputMode=DAC_SERVO_MODE;
	ch1->Vel=10000.000000;
	ch1->Accel=100000.000000;
	ch1->Jerk=1000000.000000;
	ch1->P=-0.900000;
	ch1->I=0.000000;
	ch1->D=0.000010;
	ch1->FFAccel=0.000000;
	ch1->FFVel=0.000000;
	ch1->MaxI=200.000000;
	ch1->MaxErr=200.000000;
	ch1->MaxOutput=200.000000;
	ch1->DeadBandGain=1.000000;
	ch1->DeadBandRange=0.000000;
	ch1->InputChan0=1;
	ch1->InputChan1=1;
	ch1->OutputChan0=1;
	ch1->OutputChan1=1;
	ch1->LimitSwitchOptions=0x8a8a000f;
	ch1->InputGain0=-1.000000;
	ch1->InputGain1=1.000000;
	ch1->InputOffset0=0.000000;
	ch1->InputOffset1=0.000000;
	ch1->invDistPerCycle=1.000000;
	ch1->Lead=0.000000;
	ch1->MaxFollowingError=1000000000.000000;
	ch1->StepperAmplitude=250.000000;

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

	EnableAxis(1);
	
    DefineCoordSystem(0,1,2,-1);
    
	for(;;)
	{
		WaitNextTimeSlice();


		// DO 1ST HANDWHEEL
		
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

		// DO 2ND HANDWHEEL

		// convert quadrature to 2 bit binary
		BitA = ReadBit(QA_2);   
		PosNoWrap = (ReadBit(QB_2) ^ BitA) | (BitA<<1);

		// Diff between expected position based on average of two prev deltas
		// and position with no wraps.  (Keep as X2 to avoid division by 2)
		
		DiffX2 = 2*(Pos_2-PosNoWrap) + (Change2_2+Change1_2);
		
		// Calc quadrature wraparounds to bring Diff nearest zero
		// offset by 128 wraps to avoid requiring floor()
		wraps = ((DiffX2+1028)>>3)-128;
		
		// factor in the quadrature wraparounds
		NewPos = PosNoWrap + (wraps<<2);

		Change2_2 = Change1_2;
		Change1_2 = NewPos - Pos_2;
		Pos_2 = NewPos;

		// Read Speed Selector

		if (ReadBit(31))  // is X1 selected?
			Factor = 1.0;
		else if (ReadBit(32))  // is X100 selected?
			Factor = 75.0;
		else                   // must be X10 then
			Factor = 10.0;

		if (ReadBit(30))
		{
			// this is only executed if handwheels are enabled
			// bit #30 is 1

			ch0->Dest += Change1   * Factor;
			ch1->Dest += Change1_2 * Factor;
		}
	}
    return 0;
}

