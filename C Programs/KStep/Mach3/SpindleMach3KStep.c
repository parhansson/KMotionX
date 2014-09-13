#include "KMotionDef.h"
#include "..\CorrectAnalogFunction.c"

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
			FPGA(KAN_TRIG_REG)=4;  		// Mux PWM0 to JP7 Pin5 IO 44 for KSTEP 
			SetBitDirection(44,1);  	// define bit as an output
			FPGA(IO_PWMS_PRESCALE) = 46;// divide clock by 46 (1.4 KHz)
			FPGA(IO_PWMS+1) = 1;  		// Enable
			FPGA(IO_PWMS) = CorrectAnalog(speed);  	// Set PWM
			printf("Spindle Speed Set to %f\n",speed); 
			break;

		case EX_SPINON:
			if (Direction==0)
			{
				SetBit(1);
				ClearBit(0);
				printf("Spindle CW ON\n"); 
			}
			else if (Direction==1)
			{
				SetBit(0);
				ClearBit(1);
				printf("Spindle CCW ON\n"); 
			}
			break;

		case EX_SPINOFF:
			ClearBit(0);
			ClearBit(1);
			FPGA(KAN_TRIG_REG)=4;  		// Mux PWM0 to JP7 Pin5 IO 44 for KSTEP 
			SetBitDirection(44,1);  	// define bit as an output
			FPGA(IO_PWMS_PRESCALE) = 46;// divide clock by 46 (1.4 KHz)
			FPGA(IO_PWMS+1) = 1;  		// Enable
			FPGA(IO_PWMS) = 0;  		// Set PWM off
			printf("Spindle Stop\n"); 
			break;
	}
}
