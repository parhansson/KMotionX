#include "KMotionDef.h"

main()
{
	// address of 6 bit pulse length 0-63= # 16.666MHz clocks, 
	// bit6 muxes generators 4-7 from JP7 to JP4 and JP6, 
	// bit7 reverses polarity 
	
	// Geckos work well with this and "open collector" outputs 

	FPGA(STEP_PULSE_LENGTH_ADD) = 0x80 + 63; // set inverted and to max 4us
}
