#include "KMotionDef.h"

#define TMP 10                    // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

void ServiceJobTimer(void);

#define JOBTIMEPERSIST 162

main()
{
    for (;;)
    {
        WaitNextTimeSlice();
        ServiceJobTimer();
    }
}

// Monitor and time Jobs starting and ending
void ServiceJobTimer(void)
{
    static BOOL WasActive = FALSE;
    static double StartTime;
    double dt, Seconds;
    int Hours, Minutes;
    char s[80];

    if (JOB_ACTIVE && !WasActive)    // Started ??
    {
        StartTime = Time_sec();
        WasActive = TRUE;
    }
    if (!JOB_ACTIVE && WasActive)    // Ended ??
    {
        WasActive = FALSE;

        dt = Time_sec() - StartTime;
        Hours = (int)(dt / 3600.0);
        Minutes = (int)((dt - Hours * 3600.0) / 60.0);
        Seconds = dt - Hours * 3600.0 - Minutes * 60.0;

        if (Hours > 0)
            sprintf(s, "Last Run Time %d hours %d min %5.1f sec", Hours, Minutes, Seconds);
        else if (Minutes > 0)
            sprintf(s, "Last Run Time %d min %5.1f sec", Minutes, Seconds);
        else
            sprintf(s, "Last Run Time %5.1f sec", Seconds);

        // Put it onto the Screen using Persist
        DROLabel(1000, JOBTIMEPERSIST, s);
    }
}
