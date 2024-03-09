#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

main()
{
	double DROx, DROy, DROz, DROa, DROb, DROc;
	GetDROs(&DROx, &DROy, &DROz, &DROa, &DROb, &DROc);
	DoPCFloat(PC_COMM_SET_X, DROx/2);
}
