#include "KMotionDef.h"

#define N 10000

extern double CS0_TimeExecuted;

main()
{
	int i,k;
	double X0,Y0,Z0,T0,*p=gather_buffer;

	CS0_TimeExecuted=0.0;	
	while (CS0_TimeExecuted==0.0) ;	// wait till we Start

	*p++ = 0.0;
	X0 = *p++ = ch0->Dest;
	Y0 = *p++ = ch1->Dest;
	Z0 = *p++ = ch2->Dest;
	
	T0 = Time_sec();

	// Capture Data
	
	for (i=0; i<N-1; i++)
	{
		for (k=0; k<1; k++)	WaitNextTimeSlice();
		
		*p++ = Time_sec() - T0;
		*p++ = ch0->Dest;
		*p++ = ch1->Dest;
		*p++ = ch2->Dest;
	}
	
	p=gather_buffer;

	FILE *f=fopen("C:\\temp\\kflopdata.txt","wt");
	for (i=0; i<N; i++)
	{
		// round times to neaarest servo tick
		
		p[0] = ((int)(p[0]/TIMEBASE + 0.5))*TIMEBASE;
		fprintf(f,"%16.9f,%16.3f,%16.3f,%16.3f\n",p[0],p[1],p[2],p[3]);
		p += 4;
	}
	fclose(f);
}
