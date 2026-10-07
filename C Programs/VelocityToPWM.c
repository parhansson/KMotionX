#include "KMotionDef.h"

// Drives PWM output proportional to current Speed

#define FACTOR 0.01f  // factor to convert speed to PWM counts

main()
{
	float Speed;
	int PWM;
	
	SetBitDirection(26,1);  		// define bit as an output
	FPGA(IO_PWMS_PRESCALE) = 13;  	// divide clock by 13 (5 KHz)
	FPGA(IO_PWMS) = 0; 				// PWM off 
	FPGA(IO_PWMS+1) = 1;  			// Enable

	for(;;)
	{
		WaitNextTimeSlice();
		// compute xy speed in counts/sec
		Speed = sqrtf(ch0->last_vel*ch0->last_vel + ch1->last_vel*ch1->last_vel);
		PWM = (int)(Speed * FACTOR); 	// scale PWM 
		if (PWM>255) PWM=255;			// limit to max PWM
		FPGA(IO_PWMS) = PWM; 			// output PWM 
	}
}
