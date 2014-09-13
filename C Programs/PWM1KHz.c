#include "KMotionDef.h"

main()
{
	SetBitDirection(26,1);  // define bit as an output
	FPGA(IO_PWMS_PRESCALE) = 65;  	// divide clock by 65 (1 KHz)
	FPGA(IO_PWMS) = 128;  			// square wave
	FPGA(IO_PWMS+1) = 1;  			// Enable
}
