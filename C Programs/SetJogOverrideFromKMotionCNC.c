#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

main()
{
	DoPCFloat(PC_COMM_SET_JOG_OVERRIDE_X, 0.25);
	DoPCFloat(PC_COMM_SET_JOG_OVERRIDE_Y, 0.50);
}

