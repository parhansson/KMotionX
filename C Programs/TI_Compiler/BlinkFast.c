#include "KMotionDef.h"

// Benchmark 4 million loops with double precision math

main()
{
	int i,l;
	double k=0;
	volatile double n;
	
	for(l=0;l<30;l++)
	{
		SetBit(47);
		SetBit(46);
		for (i=0;i<2000000;i++) k+=i;
		n+=k;
		ClearBit(47);
		ClearBit(46);
		for (i=0;i<2000000;i++) k+=i;;
		n+=k;
	}
}
