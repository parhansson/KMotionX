#include "KMotionDef.h"

main()
{
	SetBitDirection(26,1);  		// Set bit 26 (PWM 0 as an output)
	FPGA(IO_PWMS_PRESCALE) = 1;  	// set pwm period to 30 KHz
	FPGA(IO_PWMS+1) = 1;			// enable the PWM

	for (;;)  //loop forever
	{
		WaitNextTimeSlice();
		if (ch0->Enable)
			FPGA(IO_PWMS+0) = ch0->Output + 128;  // +128 converts to anti-phase
		else
			FPGA(IO_PWMS+0) = 128;  // whenever not enabled put 50% duty cycle
	}
}
