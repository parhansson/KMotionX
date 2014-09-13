#include "KMotionDef.h"

//Plugin Notifications and defines..
enum { EX_DDA , EX_VMS, EX_COMMAND, EX_SPINON, EX_SPINOFF, EX_SPINSPEED, EX_MOTORTUNED 
       , EX_SETUP, EX_FEEDHOLD, EX_RUN, EX_ESTOP , EX_CONFIG };
       
#define PULLEYSPEED 197000.0
#define SPINDLEAXIS 3
#define SPINDLE_ENABLE 0

main()
{
	int message = persist.UserData[0];  // Mach3 message ID 
	int Direction = persist.UserData[1];  // Mach3 Spindle Direction 
	float speed = *(float *)&persist.UserData[2];  // value stored is actually a float 

	Direction = 1-Direction*2;  // change from 0 and 1 to +1 and -1
	
	printf("\nMach3 Notify Message=%d, Direction=%2d, Spindle Set to %f\n",message,Direction,speed); 
	

	switch (message)
	{
		case EX_SPINSPEED:
			printf("Spindle Speed Set to %f\n",speed); 
			if (ReadBit(SPINDLE_ENABLE))  // if we are running, change the speed
				Jog(SPINDLEAXIS,PULLEYSPEED * speed * Direction); 
			break;

		case EX_SPINON:
			printf("Spindle CW ON Speed=%f\n",speed); 
			// turn spindle on CW and ramp to new speed
			SetBit(SPINDLE_ENABLE);
			Delay_sec(0.3);
			Jog(SPINDLEAXIS,PULLEYSPEED * speed * Direction); 
			break;

		case EX_SPINOFF:
			printf("Spindle Stop\n"); 
			// spin down
			Jog(SPINDLEAXIS,0);
			while (!CheckDone(SPINDLEAXIS)) ;
			Delay_sec(2.3);
			ClearBit(SPINDLE_ENABLE);
			break;
	}
}
