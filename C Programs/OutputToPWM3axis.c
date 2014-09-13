#include "KMotionDef.h"

main()
{
	SetBitDirection(26,1);  		// Set bit 26 (PWM 0 as an output)
	SetBitDirection(27,1);  		// Set bit 27 (PWM 1 as an output)
	SetBitDirection(28,1);  		// Set bit 28 (PWM 2 as an output)
	FPGA(IO_PWMS_PRESCALE) = 1;  	// set pwm period to 30 KHz
	FPGA(IO_PWMS+1) = 1;			// enable the PWM0
	FPGA(IO_PWMS+3) = 1;			// enable the PWM1
	FPGA(IO_PWMS+5) = 1;			// enable the PWM2

	for (;;)  //loop forever
	{
		WaitNextTimeSlice();
		if (ch0->Enable)
			FPGA(IO_PWMS+0) = ch0->Output + 128;  // +128 converts to anti-phase
		else
			FPGA(IO_PWMS+0) = 128;  // whenever not enabled put 50% duty cycle

		if (ch1->Enable)
			FPGA(IO_PWMS+2) = ch1->Output + 128;  // +128 converts to anti-phase
		else
			FPGA(IO_PWMS+2) = 128;  // whenever not enabled put 50% duty cycle

		if (ch2->Enable)
			FPGA(IO_PWMS+4) = ch2->Output + 128;  // +128 converts to anti-phase
		else
			FPGA(IO_PWMS+4) = 128;  // whenever not enabled put 50% duty cycle
	}
}
