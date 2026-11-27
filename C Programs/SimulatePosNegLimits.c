#include "KMotionDef.h"

void SimulateLimits(CHAN *ch, int bit, int limit_polarity);

int main()
{
	for (;;)  // loop forever
	{
		SimulateLimits(ch0, 48, 1);  // Simulate virtual Pos/Neg limits for an axis
		
		SetStateBit(48, ch0->Dest > 10000 || ch0->Dest < -1000);  // fake physical limit
	}
}

// Simulate pos and neg limits from a single input
// Axis must be configured to use Virtual bits for limits
// and to stop when high
void SimulateLimits(CHAN *ch, int bit, int limit_polarity)
{
	if (ReadBit(bit) == limit_polarity)  // limit switch hit?
	{
		if (!ReadBit(ch->LimitSwitchPosBit) && // only set one until back out of limit
			!ReadBit(ch->LimitSwitchNegBit))
		{
			if (ch->DirectionOfMotion == 1)  // going positive?
				SetBit(ch->LimitSwitchPosBit);

			if (ch->DirectionOfMotion == -1)  // going negative?
				SetBit(ch->LimitSwitchNegBit);
		}
	}
	else // out of limits?
	{
		ClearBit(ch->LimitSwitchPosBit);
		ClearBit(ch->LimitSwitchNegBit);
	}
}