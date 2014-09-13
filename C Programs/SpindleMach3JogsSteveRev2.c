#include "KMotionDef.h"

/*
NOTE: this program was modified by Steve Simpson on or about 04/30/2013 and is an example for educational purposes and is not presented as a tested and safe program to use on a working CNC system. Any use whatsoever of this program is at the users risk.
Rev2 remembers whether the spindle is On or Off in persist variable 3 - TK
*/


//Plugin Notifications and defines..
enum { EX_DDA , EX_VMS, EX_COMMAND, EX_SPINON, EX_SPINOFF, EX_SPINSPEED, EX_MOTORTUNED
, EX_SETUP, EX_FEEDHOLD, EX_RUN, EX_ESTOP , EX_CONFIG };


#define SPINDLE_AXIS 6  // axis set up as Spindle, possibly Step/Dir or Servo
#define FACTOR 2000000.0            // Converts fractional pulley speed to counts/sec (may be negative)

main()
{
	int message = persist.UserData[0];  // Mach3 message ID
	int Direction = persist.UserData[1];  // Mach3 Spindle Direction
	float speed = *(float *)&persist.UserData[2];  // value stored is actually a float
	int *On = &persist.UserData[3];  // Place to remember whether we are on or off
	int DirFactor = -1;

	if (Direction==0) DirFactor=1; // change Direcion 0 or 1 to DirFactor -1 or +1

	printf("Mach3 Notify Message=%d, Direction=%2d, Spindle Set to %f\n",message,Direction,speed);

	switch (message)
	{
	case EX_SPINSPEED:
		printf("Spindle Speed Set to %f\n",speed);
		if (*On)
		{
			printf("Spindle Is Running\n");
			Jog(SPINDLE_AXIS,speed*FACTOR*DirFactor);
		}
		break;

	case EX_SPINON:
		*On = TRUE;
		if (Direction==0)
		{
			Jog(SPINDLE_AXIS,speed*FACTOR);
			printf("Spindle CW ON\n");
		}
		else if (Direction==1)
		{
			Jog(SPINDLE_AXIS,speed*FACTOR*-1);
			printf("Spindle CCW ON\n");
		}
		break;

	case EX_SPINOFF:
		*On = FALSE;
		printf("Spindle Stop\n");
		Jog(SPINDLE_AXIS,0.0);
		break;
	}
}
