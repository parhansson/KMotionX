#include "KMotionDef.h"

void main()
{
	// change pulsewidth polarity and mux for only generator #1
	FPGA(STEP_PULSE_LENGTH_ADD_EX+1)=0x80+0x40+4;
	printf("Pulse length = %x\n",FPGA(STEP_PULSE_LENGTH_ADD_EX+1));
}
