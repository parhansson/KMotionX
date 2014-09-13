#include "KMotionDef.h"

main()
{
	float speed = *(float *)&persist.UserData[0];  // value stored is actually a float 
	printf("Spindle Set to %f\n",speed);  // print the desired speed
}
