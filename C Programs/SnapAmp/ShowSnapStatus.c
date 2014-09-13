#include "KMotionDef.h"

main()
{
	int s;

	s=ReadSnapAmp(SNAP0+SNAP_STATUS);

	printf("Fault          = %d\n",(s>>0)&1);
	printf("Over Current 0 = %d\n",(s>>1)&1);
	printf("Over Current 1 = %d\n",(s>>2)&1);
	printf("Over Temp 0    = %d\n",(s>>3)&1);
	printf("Over Temp 1    = %d\n",(s>>4)&1);
	printf("Fan            = %d\n",(s>>5)&1);
}
