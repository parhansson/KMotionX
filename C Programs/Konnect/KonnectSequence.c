#include "KMotionDef.h"

main()
{
	int i=0;
	for(;;)
	{
		Delay_sec(0.1);
		ClearBit(i+48);
		ClearBit(i+49);
		i+=8;
		if (i>=16)i-=16;
		SetBit(i+48);
		SetBit(i+49);
		i-=6;
		if (i<0)i+=16;
	}

}
