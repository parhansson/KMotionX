#pragma TI_COMPILER(3)
#pragma CODE_SECTION(main,".IRAM")
#include "KMotionDef.h"

// Benchmark 4 million loops with double precision math

main()
{
	int i,l;
	double T0,T1,k=0;
	volatile double n;
	
	T0=Time_sec();
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
	T1=Time_sec();
	printf("Time = %f\n",T1-T0);
}
