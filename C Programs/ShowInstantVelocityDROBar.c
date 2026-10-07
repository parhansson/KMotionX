#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define CNTS_PER_INCH_X 20000.0
#define CNTS_PER_INCH_Y 20000.0
#define CNTS_PER_INCH_Z 15000.0

#define FULL_BAR_SCALE 50.0


main()
{
	double Rate,d,x0,dx,y0,dy,z0,dz,t0,t1;
	char s[80];
	
	for(;;)
	{
		t0=WaitNextTimeSlice();
		x0 = ch0->Dest / CNTS_PER_INCH_X;
		y0 = ch1->Dest / CNTS_PER_INCH_Y;
		z0 = ch2->Dest / CNTS_PER_INCH_Z;
		t1=WaitNextTimeSlice();
		dx = ch0->Dest / CNTS_PER_INCH_X - x0;
		dy = ch1->Dest / CNTS_PER_INCH_Y - y0;
		dz = ch2->Dest / CNTS_PER_INCH_Z - z0;
		
		d = sqrt(dx*dx + dy*dy + dz*dz);

		Rate = d/(t1-t0)*60.0;
		sprintf(s,"Rate %8.2f ipm\n",Rate);
		// Put it onto the Screen
		DROLabel(1000, 162, s);
		WriteVarFloat(163, Rate/FULL_BAR_SCALE);
		Delay_sec(0.5);
	}
}

