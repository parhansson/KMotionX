#include "KMotionDef.h"

main()
{
	double x,y,x1,y1,dx1,dy1,t1,t,dx,dy,dt,Ax,Ay,S;
	
	x1 = ch0->Dest / 9551.098;
	y1 = ch2->Dest / 13333.2;
	dx1=dy1=0.0;
	
	for(;;)
	{
		Delay_sec(0.1);
		t=WaitNextTimeSlice();
		x = ch0->Dest / 9551.098;
		y = ch1->Dest / 13333.2;
		dt = (t1-t);
		dx = (x-x1)/dt;
		dy = (y-y1)/dt;
		Ax = (dx-dx1)/dt;
		Ay = (dy-dy1)/dt;
		
		S = sqrt(dx*dx+dy*dy);
		if (S !=0.0)
		{
			printf("VelX=%7.2f Y=%7.2f S=%7.2fips Ax=%7.2f Ay=%7.2f\n",
				dx,dy,sqrt(dx*dx+dy*dy),Ax,Ay);
		
		}
		dx1 = dx;
		dy1 = dy;
		x1 = x;
		y1 = y;
		t1=t;
	}

}
