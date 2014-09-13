#include "KMotionDef.h"

#define ZAXIS 3
main()
{
	if (ReadBit(46))  // Use an LED as a toggle bit
	{
		ClearBit(46);
		chan[ZAXIS].OutputMode=NO_OUTPUT_MODE;  // stop the servo from commanding any output
		chan[ZAXIS].MaxFollowingError=10000000; // stop looking for following errors
		WritePWMC(chan[ZAXIS].OutputChan0, 0);  // command zero current to the motor
	}	
	else
	{
		SetBit(46);
		chan[ZAXIS].OutputMode=DC_SERVO_MODE;  // restore the output mode
		chan[ZAXIS].MaxFollowingError=100;     // resume looking for following errors
	}
}
