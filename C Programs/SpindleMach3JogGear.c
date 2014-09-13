#include "KMotionDef.h"

//Plugin Notifications and defines..
enum { EX_DDA , EX_VMS, EX_COMMAND, EX_SPINON, EX_SPINOFF, EX_SPINSPEED, EX_MOTORTUNED 
       , EX_SETUP, EX_FEEDHOLD, EX_RUN, EX_ESTOP , EX_CONFIG };
       
#define SPINDLE_AXIS 4  // axis set up as Spindle, possibly Step/Dir or ServoTick
#define PULLY_SPEED 2500.0  // Mach3 Pully Speed
#define COUNTS_PER_REV 3844.0
#define VIRTUAL_GEAR_BIT 48

main()
{
	int message = persist.UserData[0];  // Mach3 message ID 
	int Direction = persist.UserData[1];  // Mach3 Spindle Direction 
	float speed = *(float *)&persist.UserData[2];  // value stored is actually a float
	float speedRPM;
	int DirFactor = 1;
	
	if (Direction==0) DirFactor=-1; // change Direcion 0 or 1 to DirFactor -1 or +1
	

	printf("Mach3 Notify Message=%d, Direction=%2d, Spindle Set to %f\n",message,Direction,speed); 

	switch (message)
	{
		case EX_SPINSPEED:
			printf("Spindle Speed Set to %f\n",speed); 
			speedRPM = speed * PULLY_SPEED;
			
			if (speedRPM > 700.0)  // set the speed range to HI if over 700 RPM
				SetBit(VIRTUAL_GEAR_BIT);
			else
				ClearBit(VIRTUAL_GEAR_BIT);
				
			Jog(SPINDLE_AXIS,speedRPM*COUNTS_PER_REV*DirFactor/60.0);
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
