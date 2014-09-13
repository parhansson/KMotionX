#include "KMotionDef.h"

//Plugin calls for Mach3 Home (actually Purge) Commands

main()
{
	int flags = persist.UserData[5];  // Mach3 flags bit0=X, bit1=Y, Bit2=Z, etc...

	printf("Mach3 Home Call, flags = %d\n",flags); 
	
	if (flags==1)
	{
		// do x homing here
	}

	if (flags==2)
	{
		// do y homing here
	}

	if (flags==4)
	{
		// do z homing here
	}

	
	printf("Done\n"); 
}
