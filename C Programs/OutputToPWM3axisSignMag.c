#include "KMotionDef.h"

main()
{
	SetBitDirection(26,1);  		// Set bit 26 (PWM 0 as an output)
	SetBitDirection(29,1);  		// Set bit 29 (DIR 0 as an output)
	
	FPGA(IO_PWMS_PRESCALE) = 1;  	// set pwm period to 30 KHz
	FPGA(IO_PWMS+1) = 1;			// enable the PWM0

	for (;;)  //loop forever
	{
		WaitNextTimeSlice();
		if (ch0->Enable)
		{
			if (ch0->Output >= 0.0f)
			{			
				SetBit(29);						// Direction
				FPGA(IO_PWMS+0) = ch0->Output;  // Magnitude
			}
			else
			{
				ClearBit(29);					 // Direction
				FPGA(IO_PWMS+0) = -ch0->Output;  // Magnitude
			}
		}
		else
		{
			FPGA(IO_PWMS+0) = 0;  // whenever not enabled put 0% duty cycle
		}
	}
}
