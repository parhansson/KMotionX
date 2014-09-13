#include "KMotionDef.h"
#include "..\CorrectAnalogFunction.c"

#define RPM_FACTOR 500.0 // RPM for full duty cycle (max analog out)

// desired speed is passed in variable 1


main()
{
	float speed = *(float *)&persist.UserData[1];  // value stored is actually a float 
	
	FPGA(KAN_TRIG_REG)=4;  			// Mux PWM0 to JP7 Pin5 IO 44 for KSTEP 
	SetBitDirection(44,1);  		// define bit as an output
	FPGA(IO_PWMS_PRESCALE) = 46;  	// divide clock by 46 (1.4 KHz)
	FPGA(IO_PWMS+1) = 1;  			// Enable
	
	FPGA(IO_PWMS) = CorrectAnalog(speed/RPM_FACTOR);  	// Set PWM
}
