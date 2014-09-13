#include "KMotionDef.h"

main()
{
	for (;;) // loop forever
	{
		WaitNextTimeSlice();
		
		if (ReadBit(46))  // Watch an external input switch
		{
			StopCoordinatedMotion();  //feedhold
		}
	}
}
