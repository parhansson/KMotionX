#include "KMotionDef.h"

// Hi guys, I was a little disappointed with the performance from the MPGsmooth.c program. It worked, but depending 
// on how i set the smoothing factor i would either have to live with fairly rough motion when on 0.010" increments, 
// or movement that dragged on forever until eventually the time limit finished the movement with a jolt at the end.

// I modified the program to start with a high smoothing factor (TAU) and then decrease the value based on the "no change" time,
// so that its very smooth at first and still finishes up motion in a reasonable amount of time. The starting value, minimum 
// value and rate of change are all easily adjustable in the program. Seems to be working very well for me so far. 
// I also took it a step further and set it up to modify axis acceleration during mpg movement and set it back to normal 
// when finished. These lines are ignored with comments in the file in case it might get confusing, but they are there 
// if you feel like setting up acceleration numbers too. A value of about half seemed to work well for me, if you go too 
// low it can do some odd motions when reversing.

// Anyway i thought I would share my program with you guys. Takes a little while to tune it, but it can provide 
// much better performance then all the mpg programs included with kmotioncnc. One last thing, im not a C programmer, 
// didnt know a thing a few months ago, so it may not be perfect, but it does work well. Hope it helps someone.

// Mark


#define QA 1030		// define to which IO bits the AB signals are connected 
#define QB 1031		// define to which IO bits the AB signals are connected 
#define TAU 0.35 		// Max smoothing factor
#define MINTAU 0.12 	// min smoothing factor
#define RATE 1.1 		// Smoothing factor rate of change per second after no mpg movement
#define FINAL_TIME 1.0 	// Set final dest after this amount of time with no change


main()
{	
	int BitA,Change1=0,Change2=0, DiffX2;
	int PosNoWrap, NewPos, Pos=0, wraps;
	int InMotion=FALSE,Axis,LastAxis=-1;
	double LastChangeTime=0,Target,Factor=0,Factor1=0;
	float NEWTAU;

	for(;;)
	{	
		// convert quadrature to 2 bit binary
		BitA = ReadBit(QA);   
		PosNoWrap = (ReadBit(QB) ^ BitA) | (BitA<<1);
		// Diff between expected position based on average of two prev deltas
		// and position with no wraps.  (Keep as X2 to avoid division by 2)		
		
		// DiffX2 = 2*(Pos-PosNoWrap) + (Change2+Change1); --------origonal line--------	
		DiffX2 = 2*(Pos-PosNoWrap); // ----------modified to prevent runaway--------------
		
		// Calc quadrature wraparounds to bring Diff nearest zero
		// offset by 128 wraps to avoid requiring floor()
		wraps = ((DiffX2+1028)>>3)-128;		
		// factor in the quadrature wraparounds
		NewPos = PosNoWrap + (wraps<<2);
		Change2 = Change1;
		Change1 = NewPos - Pos;
		Pos = NewPos;

		if (!ReadBit(1029) || JOB_ACTIVE) // if enable button not pressed or Job Active ignore the encoder.
			Change1 = 0;

		if (ReadBit(1027) && !ReadBit(1028))  // is X0.01 selected?
			Factor = 0.01;
		else if (!ReadBit(1027) && ReadBit(1028))  // is X0.1 selected?
			Factor = 0.1;
		else if (ReadBit(1027) && ReadBit(1028))  // is X1 selected?
			Factor = 1.0;
		else					// ignore encoder
			Change1 = 0;
		if (ReadBit(1024) && !ReadBit(1025))  // is x selected?
		{
			Axis=0;
			Factor = Factor * -1219.2; // multiply factor by counts per inch divided by 100
		}		
		else if (!ReadBit(1024) && ReadBit(1025))  // is y selected?
		{
			Axis=1;
			Factor = Factor * -1219.2; // multiply factor by counts per inch divided by 100
		}		
		else if (ReadBit(1024) && ReadBit(1025))  // is z selected?
		{
			Axis=2;
			Factor = Factor * -1524.0; // multiply factor by counts per inch divided by 100
		}		
		// check if the Axis just changed or we have been 
		// converging to the target for a long time
		if (Axis != LastAxis || 
			(InMotion && Time_sec() > LastChangeTime+FINAL_TIME))
		{
			// Optional Acceleration change during mpg movement
			// These next three lines set accelleration back to normal and finishes mpg movement
			//ch0->Accel=4000000; 
			//ch1->Accel=4000000;
			//ch2->Accel=6000000;

			if (InMotion)
				Move(LastAxis,Target);  //finalize any motion	
			LastAxis = Axis;
			InMotion = FALSE;
		}		

		
		
		if (Change1) // did we move?
		{
			// Optional Acceleration change during mpg movement
			// These next three lines set acceleration to a lower value for mpg movement
			//ch0->Accel=2000000;
			//ch1->Accel=2000000;
			//ch2->Accel=2500000;
			
			if (!InMotion) Target = chan[Axis].Dest;
			Target += Change1 * Factor;
			MoveExp(Axis,Target,TAU);  // note: contains a WaitNextTimeSlice
			LastChangeTime = Time_sec();
			InMotion=TRUE;
		}
		

		if (InMotion) 							// If still moving
		{		
			NEWTAU = TAU - (RATE * (Time_sec() - LastChangeTime));	 		// Adjust NEWTAU
			if (NEWTAU < MINTAU) NEWTAU = MINTAU; 				// Limit NEWTAU Mininum
			MoveExp(Axis,Target,NEWTAU);  					// Move with updated smoothing factor
		}
		else
		{
			WaitNextTimeSlice();
		}
	}  
}