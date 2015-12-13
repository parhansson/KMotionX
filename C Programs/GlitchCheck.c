#include "KMotionDef.h"

// Simple check for Glitch on Input Pin

main()
{
	while (!ReadBit(46)) ;  // tight loop while input is low
	printf("Input was high!!\n");
}
