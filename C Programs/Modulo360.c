#include "KMotionDef.h"

#define CNTS_PER_DEG 1000.0
#define AXISA 4

double modf(double, double*); // returns integer part with more than 32-bit precision

double floor(double x) // truncate down toward -infinity
{
   double y; 
   return (modf(x, &y) < 0 ? y - 1 : y);
}

main()
{
	double dest = chan[AXISA].Dest;
	double cnts = CNTS_PER_DEG * 360.0;
	EnableAxisDest(AXISA, dest - (floor(dest/cnts)) * cnts);
}
