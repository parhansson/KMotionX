#include "KMotionDef.h"

// drive PWM 0 as spindle off
//

// PWM 0 is KFLOP IO bit #26
// JP6 Aux#1 Pin 5

main()
{
	FPGA(IO_PWMS+1) = 1;    // enable the PWM
	FPGA(IO_PWMS)   = 0; 	// set the PWM off
}
