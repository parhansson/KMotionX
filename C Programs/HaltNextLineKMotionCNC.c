#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

main()
{
	DoPC(PC_COMM_HALT_NEXT_LINE);
}
