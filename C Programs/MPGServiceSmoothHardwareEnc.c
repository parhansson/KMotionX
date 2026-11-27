#include "KMotionDef.h"

// Example Init program that includes "smooth" MPG motion example
// which makes use of the exponential motion command.

#define MPG_INPUT_AXIS 7

#define TAU 0.08 // smoothness factor (Low Pass Time constant seconds)
#define FINAL_TIME 1.0 // Set final dest after this amount of time with no change

#define ENABLE_MPG 29

#define SELECTX 20
#define SELECTY 17
#define SELECTZ 18
#define SELECTA 19

#define FACTOR1 16
#define FACTOR10 21
#define FACTOR100 22
#define FACTOR200 23

#define HANDWHEEL_EN	29

void ServiceMPG(void)
{
	static int Pos, FirstTime=TRUE;
	static int InMotion=FALSE,Axis,LastAxis=-1;
	static double LastChangeTime=0,Target,Factor=0;
	int Change1, NewPos;
	
	if (FirstTime)
	{
		Pos = chan[MPG_INPUT_AXIS].Position;
		FirstTime=FALSE;
	}

	NewPos = chan[MPG_INPUT_AXIS].Position;
	Change1 = NewPos - Pos;
	Pos = NewPos;

	if (!ReadBit(HANDWHEEL_EN) || JOB_ACTIVE) // if not button pressed or Job Active ignore the encoder.
		Change1 = 0;
	else if (ReadBit(FACTOR1))  // is X1 selected?
		Factor = 4;
	else if (ReadBit(FACTOR10))  // is X10 selected?
		Factor = 20;
	else if (ReadBit(FACTOR100))  // is X100 selected?
		Factor = 40;
	else if (ReadBit(FACTOR200))  // is X200 selected?
		Factor = 400;
	else                   
		Factor = 0.0;

	if (ReadBit(SELECTX))  // is x selected?
		Axis=0;
	else if (ReadBit(SELECTY))  // is y selected?
		Axis=1;
	else if (ReadBit(SELECTZ))  // is z selected?
		Axis=2;
	else if (ReadBit(SELECTA))  // is A selected?
		Axis=3;

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
