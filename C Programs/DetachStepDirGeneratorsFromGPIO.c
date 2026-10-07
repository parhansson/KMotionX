#include "KMotionDef.h"

void DetachStepDirGenerator(int StepDirChan);

main()
{
	DetachStepDirGenerator(0);  //set pins back as GPIO
	DetachStepDirGenerator(1);  //set pins back as GPIO
}

void DetachStepDirGenerator(int StepDirChan)
{
	FPGA(STEP_RATE_ADD)   = 0;
	FPGA(STEP_RATE_ADD+1) = 0;
	FPGA(STEP_RATE_ADD+2) = 0;
	FPGA(STEP_RATE_ADD+3) = 0x88 + StepDirChan;  // enable and configure to TTL mode 

	FPGA(STEP_RATE_ADD)   = 0;
	FPGA(STEP_RATE_ADD+1) = 0;
	FPGA(STEP_RATE_ADD+2) = 0;
	FPGA(STEP_RATE_ADD+3) = StepDirChan; // disable it
}
