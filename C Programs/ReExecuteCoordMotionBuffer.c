#include "KMotionDef.h"
#include "CoordMotionInKFLOPFunctions.c"

main()
{
	printf("Number of Segments in Motion Buffer = %d\n",ParametricIndex);
	LastCoordSystem0=&ParametricCoeffs[0];  // reset to start at beginning
	ExecBuf();
}
