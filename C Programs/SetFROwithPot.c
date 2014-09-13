#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define CHANGE_TOL 0.02  // only update if change is more than this
#define CHANGE_TIME 0.05  // don't update more often then this time
double LastFRO=-1;
double LastFROTime=0;

main()
{
	double Pot,FRO,T;

	for (;;)
	{
		T = WaitNextTimeSlice();
		
		// assume 5V range where 2.5V is nominal FRO
		Pot=KANALOG_CONVERT_ADC_TO_VOLTS(ADC(0)) - 2.5;
		
		FRO = Pot*0.5+1.0;

		// send message to KMotionCNC if the pot changed significantly
		// and it has been a while since the last message
		if ((FRO > LastFRO+CHANGE_TOL || FRO < LastFRO-CHANGE_TOL) && 
			T > LastFROTime+CHANGE_TIME)
		{
//			printf("FRO = %f\n",FRO);
			DoPCFloat(PC_COMM_SET_FRO,FRO);
			LastFRO=FRO;
			LastFROTime=T;
		}
	}
}
