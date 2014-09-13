#include "KMotionDef.h"

#define SPINDLE_AXIS 4
#define CNTS_PER_REV 1200

main()
{
    double T0,T1;
    double P0,P1;
    
    for (;;)
    {
    	T0=WaitNextTimeSlice();
    	P0=chan[SPINDLE_AXIS].Position;
        Delay_sec(1);
    	T1=WaitNextTimeSlice();
    	P1=chan[SPINDLE_AXIS].Position;
    	
        printf("T=%f RPM=%.2f\n",
            T1-T0, 60.0/CNTS_PER_REV*(P1-P0)/(T1-T0));
    }
}
