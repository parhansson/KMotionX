#include "KMotionDef.h"

//Plugin Notifications and defines..
enum { EX_DDA , EX_VMS, EX_COMMAND, EX_SPINON, EX_SPINOFF, EX_SPINSPEED, EX_MOTORTUNED 
       , EX_SETUP, EX_FEEDHOLD, EX_RUN, EX_ESTOP , EX_CONFIG };
       
#define SPINDLE_AXIS 4  // axis set up as Spindle, possibly Step/Dir or Servo
#define FACTOR 10000.0 		// Converts fractional pulley speed to counts/sec (may be negative)

main()
{
	int message = persist.UserData[0];  // Mach3 message ID 
	int Direction = persist.UserData[1];  // Mach3 Spindle Direction 
	float speed = *(float *)&persist.UserData[2];  // value stored is actually a float
	int DirFactor = 1;
	
	if (Direction==0) DirFactor=-1; // change Direcion 0 or 1 to DirFactor -1 or +1
	

	printf("Mach3 Notify Message=%d, Direction=%2d, Spindle Set to %f\n",message,Direction,speed); 

	switch (message)
	{
		case EX_SPINSPEED:
			if (Direction != -1)
			{
				printf("Spindle Speed Set to %f\n",speed); 
				Jog(SPINDLE_AXIS,speed*FACTOR*DirFactor);
			}
			break;

		case EX_SPINON:
			if (Direction==0)
			{
				printf("Spindle CW ON\n"); 
			}
			else if (Direction==1)
			{
				printf("Spindle CCW ON\n"); 
			}
			break;

		case EX_SPINOFF:
			printf("Spindle Stop\n"); 
			Jog(SPINDLE_AXIS,0.0);
			break;
	}
}
