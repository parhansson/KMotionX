#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

main()
{
	// Change Tool #3 Length to 12.34 passed up via persist double #10
	SetToolLength(3-1,12.34);
}
