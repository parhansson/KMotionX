#include "KMotionDef.h"

#define N 1000

extern double CS0_TimeExecuted;

main()
{
	int i=0,k,Trig=0;
	double X0,Y0,Z0,T0,*p=gather_buffer;

	
	T0 = Time_sec();

	// Capture Data
	
	for (;;)
	{
		for (k=0; k<1; k++)	WaitNextTimeSlice();
		
		*p++ = Time_sec() - T0;
		*p++ = ch0->Dest;
		*p++ = ch2->Position;
		*p++ = ch2->Dest;
		
		if (++i == N)
		{
			i=0;
			p=gather_buffer;
		}
		
		// Trigger
		if (!Trig && fast_fabs((float)(ch2->Position - ch2->Dest)) > 20) Trig=1;
		
		if ((Trig) && (++Trig==20)) break;
	}
	

	FILE *f=fopen("C:\\temp\\kflopdata.txt","wt");
	for (i=0; i<N; i++)
	{
		// round times to neaarest servo tick
		
		p[0] = ((int)(p[0]/TIMEBASE + 0.5))*TIMEBASE;
		fprintf(f,"%16.9f,%16.3f,%16.3f,%16.3f\n",p[0],p[1],p[2],p[3]);
		p += 4;

		if (p == gather_buffer + N * 4)
		{
			p=gather_buffer;
		}
	}
	fclose(f);
}
