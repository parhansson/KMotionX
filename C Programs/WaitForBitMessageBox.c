#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

// Example to wait for a signal (clamps) before continuing GCode and
// prompts Operator after timeout waiting for clamps

#define WAITBIT 46    // bit to wait for
#define POLARITY 1    // state to wait for
#define TIME_OUT 5.0  // Timeout

main()
{
    int Answer;
    double T1,T0=Time_sec();;


    while (ReadBit(WAITBIT)!=POLARITY)
    {
        T1=WaitNextTimeSlice();
        if (T1 > T0+TIME_OUT)
        {
            Answer = MsgBox("Continue Waiting for Clamps?",MB_YESNO);
            if (Answer==IDNO)
            {
                DoPC(PC_COMM_HALT);
                break;
            }
            T0=T1;
        }
    }	
}
