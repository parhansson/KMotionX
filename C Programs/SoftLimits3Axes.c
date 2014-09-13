#include "KMotionDef.h"

#define XMINUS -100.0
#define XPLUS 100000.0
#define YMINUS -100.0
#define YPLUS 100000.0
#define ZMINUS -100.0
#define ZPLUS 100000.0

int InLimit();

main()
{
	for (;;)  // loop forever
	{
		if (InLimit())  // check for any limit
		{
			DisableAxis(0);
			DisableAxis(1);
			DisableAxis(2);
			
			while (InLimit()) ;  //wait until back within limits
		}
	}
}

int InLimit()
{
	WaitNextTimeSlice();
	return 	ch0->Dest < XMINUS || ch0->Dest > XPLUS ||
			ch1->Dest < YMINUS || ch1->Dest > YPLUS ||
			ch2->Dest < ZMINUS || ch2->Dest > ZPLUS;
}
