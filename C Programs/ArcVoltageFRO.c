#include "KMotionDef.h"

#define CHANGE_TOL 0.02  // tolerancia de cambio
#define CHANGE_TIME 0.05  // tiempo de actualizacion
#define VCORT 3 // voltage on shortcircuit condition
#define VDES 7 // desired voltage
#define K 0.01 // filter coefficient

float LastFROTime=0;

main()
{
	float VARC_Filtered=0.0f,VARC,FRO=1.0f,T;
	int NewBit48,LastBit48=0;

	for (;;)
	{
		T = WaitNextTimeSlice();
		VARC=KANALOG_CONVERT_ADC_TO_VOLTS(ADC(5));
		VARC_Filtered = VARC_Filtered  * (1.0f - K) + K * VARC;   // perform filter
		
		NewBit48 = ReadBit(48);
		if (!LastBit48 && NewBit48)  //start with FRO=1 when Bit 48 turn on
			FRO=1.0f;
			
		LastBit48=NewBit48;
		
		if (ReadBit(48) && T > LastFROTime+CHANGE_TIME)
		{
			if (VARC < VCORT)
			{
				SetFRO(-3);
				LastFROTime=T;
			}
			else if (VARC > VDES+CHANGE_TOL)
			{
				FRO=FRO+0.01;
				if (FRO > 2.0f) FRO = 2.0f;
				SetFRO(FRO);
				LastFROTime=T;
			}
			else if (VARC < VDES-CHANGE_TOL)
			{
				FRO=FRO-0.01;
				if (FRO < 0.1f) FRO = 0.1f;
				SetFRO(FRO);
				LastFROTime=T;
			}
		}
	}
}
