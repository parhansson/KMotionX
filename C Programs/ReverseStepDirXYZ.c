#include "KMotionDef.h"

// Reverses the direction of motion for Step/Dir Outputs
// or DAC Servo Outputs

main()
{
	ch0->OutputGain = -1;
	ch1->OutputGain = -1;
	ch2->OutputGain = -1;
}
