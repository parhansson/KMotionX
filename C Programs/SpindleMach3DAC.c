#include "KMotionDef.h"

//Plugin Notifications and defines..
enum { EX_DDA , EX_VMS, EX_COMMAND, EX_SPINON, EX_SPINOFF, EX_SPINSPEED, EX_MOTORTUNED 
       , EX_SETUP, EX_FEEDHOLD, EX_RUN, EX_ESTOP , EX_CONFIG };

main()
{
	int message = persist.UserData[0];  // Mach3 message ID 
	int Direction = persist.UserData[1];  // Mach3 Spindle Direction 
	float speed = *(float *)&persist.UserData[2];  // value stored is actually a float 

	printf("Mach3 Notify Message=%d, Direction=%2d, Spindle Set to %f\n",message,Direction,speed); 

	switch (message)
	{
		case EX_SPINSPEED:
			printf("Spindle Speed Set to %f\n",speed); 
			
			DAC(3,(int)(speed*2047.0));
			
			break;

		case EX_SPINON:
			if (Direction==0)
			{
				printf("Spindle CW ON\n"); 
				SetBit(144);
				ClearBit(145);
			}
			else if (Direction==1)
			{
				printf("Spindle CCW ON\n"); 
				ClearBit(144);
				SetBit(145);
			}
			break;

		case EX_SPINOFF:
			{
				printf("Spindle Stop\n"); 
				ClearBit(144);
				ClearBit(145);
			}
			break;
	}
}
