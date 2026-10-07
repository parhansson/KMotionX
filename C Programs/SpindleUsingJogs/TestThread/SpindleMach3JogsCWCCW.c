#include "KMotionDef.h"

// Handles Mach3 Messages to command an Axis to Jog 
// Assumes two bits used to enable CW or CCW Spindle directions

//Plugin Notifications and defines..
enum { EX_DDA , EX_VMS, EX_COMMAND, EX_SPINON, EX_SPINOFF, EX_SPINSPEED, EX_MOTORTUNED
, EX_SETUP, EX_FEEDHOLD, EX_RUN, EX_ESTOP , EX_CONFIG };


#define SPINDLE_AXIS 6  // axis set up as Spindle, possibly Step/Dir or Servo
#define FACTOR 2000000.0            // Converts fractional pulley speed to counts/sec (may be negative)
#define SPINDLECW_BIT 38
#define SPINDLECCW_BIT 39

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
			SetBit(SPINDLECW_BIT);
			ClearBit(SPINDLECCW_BIT);
			Jog(SPINDLE_AXIS,speed*FACTOR);
			printf("Spindle CW ON\n");
		}
		else if (Direction==1)
		{
			ClearBit(SPINDLECW_BIT);
			SetBit(SPINDLECCW_BIT);
			Jog(SPINDLE_AXIS,speed*FACTOR*-1);
			printf("Spindle CCW ON\n");
		}
		break;

	case EX_SPINOFF:
		*On = FALSE;
		printf("Spindle Stop\n");
		ClearBit(SPINDLECW_BIT);
		ClearBit(SPINDLECCW_BIT);
		Jog(SPINDLE_AXIS,0.0);
		break;
	}
}
