#include "KMotionDef.h"

#define N 2000

extern double CS0_TimeExecuted;

main()
{
	int i,k;
	double X0,T0,*p=gather_buffer;
	
	CS0_TimeExecuted=0;

	*p++ = 0.0;
	*p++ = ch2->Dest;
	X0 = *p++ = ch2->Dest;
	
	while (X0 == ch2->Dest) ;	// wait till we Start
	
	T0 = Time_sec();

	// Capture Data
	
	for (i=0; i<N-1; i++)
	{
		for (k=0; k<1; k++)	WaitNextTimeSlice();
		
		*p++ = Time_sec() - T0;
		*p++ = ch2->Dest;
		*p++ = ch2->Position;
	}
	
	p=gather_buffer;

	for (i=0; i<N; i++)
	{
		printf("%16.9f,%16.3f,%16.3f\n",p[0],p[1],p[2]);
		p += 3;
	}
}
