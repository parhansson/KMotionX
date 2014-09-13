#include "KMotionDef.h"

main()
{
	int New,Last=ReadBit(46);
	for (;;)
	{
		New = ReadBit(46);
		if (New != Last)
		{
			Last=New;
			printf("X=%f Y=%f\n",ch0->Dest/1000.0,ch1->Dest/1000.0);  // send message to console
		}
	}
}
