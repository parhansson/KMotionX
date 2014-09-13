#include "KMotionDef.h"

main()
{
	int tool = persist.UserData[9];  // value stored is actually a float 
	printf("Tool Set to %d\n",tool);  // print the desired speed
}
