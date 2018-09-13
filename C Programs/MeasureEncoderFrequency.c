#include "KMotionDef.h"

#define FAXIS 6

main()
{
	double T0, T1, Pos,Pos0, Freq;
	
	T0=Time_sec();
	Pos0=chan[FAXIS].Position;
	
	for (;;)
	{
		T1=WaitNextTimeSlice();
		Pos = chan[FAXIS].Position;
		
		Freq = (Pos - Pos0)/(T1 - T0);
		T0=T1;
		Pos0=Pos;
		
		// as a diagnostic put the result into the 
		// Dest so it can be observed on the Axis Screen
		chan[FAXIS].Dest = Freq;
	}
}
