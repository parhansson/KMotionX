#include "KMotionDef.h"

main()
{
	int i;

	for (i=0; i<4; i++)
		HRPWM_SetMode(i,0);  // switch from GPIO to HRPWM Mode
	
	HRPWM_01_PRESCALE_POWER(0);
	HRPWMPERIOD01 = 11400;   // 20KHz 13+ bits resolution
	HRPWM0= 10000;  		 // Set HRPWM 0 Period
	HRPWM1= 20000;  		 // Set HRPWM 1 Period
	
	HRPWMPERIOD2 = 65535;    // 3.5KHz 16 bits resolution
	HRPWM2 = 30000;			 // Set HRPWM 2 Period

	HRPWMPERIOD3 = 65535;	 // 3.5KHz 16 bits resolution
	HRPWM3 = 40000;			 // Set HRPWM 3 Period
}