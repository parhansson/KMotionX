#include "KMotionDef.h"

int GetTripState(int ch);

main()
{
	int NewTripState,OldTripState=-1;
	for (;;)
	{
		NewTripState = GetTripState(0);
		if (NewTripState != OldTripState)
		{
			OldTripState = NewTripState;
			printf("TripState = %d\n",NewTripState);
		}
	}
}

int GetTripState(int ch)
{
	TRIP_COEFF *p = chan[ch].pcoeff;
	if (p)
		return p - &chan[ch].c[0];
	else
		return 8;
}