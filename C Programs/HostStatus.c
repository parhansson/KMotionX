#include "KMotionDef.h"


main()
{
	int LastState=-1;
	for (;;)  // loop forever
	{
		WaitNextTimeSlice();
		if (LastState!=HostStatus)
		{
			LastState=HostStatus;
			printf("Host State = %X\n",LastState);
		}
	}
}

