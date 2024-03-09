// Example Function as "smooth" MPG motion example
// which makes use of the exponential motion command.
// Additionally double Filtered to be very smoot with limited Jerk

#define MPG_INPUT_AXIS 5  // Axis used to read hardware encoder (not necessarily the encoder channel)

#define TAU  0.01			// smoothness factor (Low Pass Time constant seconds for MoveExp)
#define TAU1 0.015			// smoothness factor (Low Pass Time constant seconds for pre filter 1)
#define TAU2 0.015			// smoothness factor (Low Pass Time constant seconds for pre filter 2)
#define FINAL_TIME 1.0		// Set final dest after this amount of time with no change

#define ENABLE_MPG 29   // Switch/Button used to enable MPG Motion

#define FACTOR1 16
#define FACTOR10 21
#define FACTOR100 22
#define FACTOR200 23

#define SELECTX 20
#define SELECTY 17
#define SELECTZ 18
#define SELECTA 19


void ServiceMPG(void)
{
	static int Pos, FirstTime = TRUE;
	static int InMotion = FALSE, Axis, LastAxis = -1;
	static double LastChangeTime = 0, Target, Factor = 0;
	static double Target1, Target2, K1, K2, K1M, K2M;

	int Change1, NewPos;

	if (FirstTime)
	{
		Pos = chan[MPG_INPUT_AXIS].Position;
		K1 = exp(-2 * TIMEBASE / TAU1);	// filter coefficients
		K2 = exp(-2 * TIMEBASE / TAU2);
		K1M = 1.0 - K1;
		K2M = 1.0 - K2;
		FirstTime = FALSE;
	}

	NewPos = chan[MPG_INPUT_AXIS].Position;
	Change1 = NewPos - Pos;
	Pos = NewPos;

	if (!ReadBit(ENABLE_MPG) || JOB_ACTIVE)	// if not button pressed or Job Active ignore the encoder.
		Change1 = 0;
	
	if (ReadBit(FACTOR1))		// is X1 selected?
		Factor = 4;
	else if (ReadBit(FACTOR10))	// is X10 selected?
		Factor = 20;
	else if (ReadBit(FACTOR100))	// is X100 selected?
		Factor = 40;
	else if (ReadBit(FACTOR200))	// is X200 selected?
		Factor = 400;
	else
		Factor = 0.0;

	if (ReadBit(SELECTX))		// is x selected?
		Axis = 0;
	else if (ReadBit(SELECTY))	// is y selected?
		Axis = 1;
	else if (ReadBit(SELECTZ))	// is z selected?
		Axis = 2;
	else if (ReadBit(SELECTA))	// is A selected?
		Axis = 3;


	// Feedhold fully stopped ??
	if (CS0_StoppingState == 4 && InMotion)
	{
		Change1 = 0;			// ignore any MPG change
		Jog(LastAxis, 0);
		InMotion = FALSE;
	}

	// check if the Axis just changed or we have been 
	// converging to the target for a long time
	if (Axis != LastAxis || (InMotion && Time_sec() > LastChangeTime + FINAL_TIME))
	{
		if (InMotion)
			Move(LastAxis, Target1);	//finalize any motion

		LastAxis = Axis;
		InMotion = FALSE;
	}

	if (Change1)				// did we move?
	{
		if (!InMotion)
			Target = Target1 = Target2 = chan[Axis].Dest;
		Target1 += Change1 * Factor;
		LastChangeTime = Time_sec();
		InMotion = TRUE;
	}

	if (InMotion)				// If moving
	{
		if (Target1 > chan[LastAxis].SoftLimitPos)
			Target1 = chan[LastAxis].SoftLimitPos;
		if (Target1 < chan[LastAxis].SoftLimitNeg)
			Target1 = chan[LastAxis].SoftLimitNeg;
		Target2 = Target2 * K1 + Target1 * K1M;
		Target = Target * K2 + Target2 * K2M;
		MoveExp(Axis, Target, TAU);
	}
}
