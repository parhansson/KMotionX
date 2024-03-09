#include "KMotionDef.h"

#define N 10000

main()
{
	int i,k;
	double T0,*p=gather_buffer;

	
	// I just have it go, launched by an M code.  It records roughly 4 seconds from launch and
	// doesn't appear to saturate the USB line.  This lets me really target where I want it to
	// log data such that I'm seeing the exact problem area.  It displays axis position and 
	// following error!
	
	T0 = Time_sec();

	// Capture Data
	
	for (i=0; i<N; i++)
	{
		for (k=0; k<2; k++)	WaitNextTimeSlice();
		
		*p++ = Time_sec() - T0;
		*p++ = ch0->Position;
		*p++ = ch1->Position;
		*p++ = ch2->Position;
		*p++ = ch0->Dest;
		*p++ = ch1->Dest;
		*p++ = ch2->Dest;
	}
	
	p=gather_buffer;
	FILE *f=fopen("C:\\Temp\\KFLOPdata.txt","wt");
	for (i=0; i<N; i++)
	{
		// round times to nearest servo tick
		p[0] = ((int)(p[0]/TIMEBASE + 0.5))*TIMEBASE;
		fprintf(f,"%12.6f,%12.3f,%12.3f,%12.3f,%12.3f,%12.3f,%12.3f\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6]);
		p += 7;
	}
	fclose(f);
}
