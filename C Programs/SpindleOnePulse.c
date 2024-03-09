#include "KMotionDef.h"

// counts pulses on single pulse encoder for Spindle

#define SPINDLE_PULSE_BIT 36
#define SPINDLE_AXIS 7


main()
{
    int NewState,LastState=ReadBit(SPINDLE_PULSE_BIT);
    for (;;)
    {
        NewState = ReadBit(SPINDLE_PULSE_BIT);

        if (NewState != LastState)
        {
            LastState=NewState;
            if (NewState) chan[SPINDLE_AXIS].Position++;
        }
    }
}