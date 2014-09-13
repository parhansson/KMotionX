#include "KMotionDef.h"

#define MAX_RPM 3000.0f

// drive PWM 0 as spindle
//

// PWM 0 is KFLOP IO bit #26
// JP6 Aux#1 Pin 5

main()
{
	int pwm;
	float speed = *(float *)&persist.UserData[0];  // value stored is actually a float 
	
	pwm = speed/MAX_RPM * 255.0f;
	
	if (pwm > 255) pwm=255;  // limit to max pwm value

	printf("Spindle Set to %f pwm %d\n",speed, pwm);  // print the desired speed
	
	FPGA(IO_PWMS+1) = 1;    // enable the PWM
	FPGA(IO_PWMS)   = pwm; 	// set the PWM
}
