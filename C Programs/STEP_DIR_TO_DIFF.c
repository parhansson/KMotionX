#include "KMotionDef.h"

int main()
{
	FPGA(STEP_DIR_TO_DIFF) = 0xFF;
	printf("STEP_DIR_TO_DIFF set to %X\n", FPGA(STEP_DIR_TO_DIFF));
}
