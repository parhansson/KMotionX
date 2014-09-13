#include "KMotionDef.h"

main()
{
	printf("P = %f Q = %f R = %f\n",
		*(float *)&persist.UserData[0],
		*(float *)&persist.UserData[1],
		*(float *)&persist.UserData[2]);

}
