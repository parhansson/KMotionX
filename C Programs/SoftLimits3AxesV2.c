#include "KMotionDef.h"

#define XMINUS -100.0
#define XPLUS 100000.0
#define YMINUS -100.0
#define YPLUS 100000.0
#define ZMINUS -100.0
#define ZPLUS 100000.0

int InLimit();
int CheckALimit(CHAN *ch, double Min, double Max);

#define SIGMA 0.01

main()
{
	for (;;)  // loop forever
	{
		if (InLimit())  // check for any limit
		{
			DisableAxis(0);
			DisableAxis(1);
			DisableAxis(2);
			
			Delay_sec(2);
			
			EnableAxisDest(0,ch0->Dest); 
			EnableAxisDest(1,ch1->Dest); 
			EnableAxisDest(2,ch2->Dest); 
		}
	}
}

int InLimit()
{
	WaitNextTimeSlice();
	return CheckALimit(ch0,XMINUS,XPLUS) ||
		   CheckALimit(ch1,YMINUS,YPLUS) ||
		   CheckALimit(ch2,ZMINUS,ZPLUS);
}

// check if in limit while moving in the direction of the limit

int CheckALimit(CHAN *ch, double Min, double Max)
{
	if (ch->Dest < Min && ch->last_vel < -SIGMA) return 1;
	if (ch->Dest > Max && ch->last_vel >  SIGMA) return 1;
	return 0;
}
