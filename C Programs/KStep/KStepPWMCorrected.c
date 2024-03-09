#include "KMotionDef.h"
#include "CorrectAnalogFunction.c"

#define RefVoltage 13.02f

main()
{
	float V=7.0f;
	
	FPGA(KAN_TRIG_REG)=4;  // Mux PWM0 to JP7 Pin5 IO 44 for KSTEP 
	SetBitDirection(44,1);  // define bit as an output
	FPGA(IO_PWMS_PRESCALE) = 46;  	// divide clock by 46 (1.4 KHz)
	FPGA(IO_PWMS+1) = 1;  			// Enable
	
	FPGA(IO_PWMS) = CorrectAnalog(V/RefVoltage);  	// Set PWM
//	FPGA(IO_PWMS) = 0;  	// Set PWM to a specific count
}

