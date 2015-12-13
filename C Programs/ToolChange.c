#include "KMotionDef.h"

main()
{
	int slot = persist.UserData[9];  // value stored is an int 
	int id = persist.UserData[9+1];  // value stored is an int 
	printf("Tool Set to slot %d id %d\n",slot,id);  // print the slot and id
}
