#include "KMotionDef.h"

//Plugin calls for Mach3 NotifyPlugins Commands

main()
{
	int msg = persist.UserData[6];  // Mach3 notify Message 10000-10999

	printf("Mach3 Notify Call, Message = %d\n",msg); 
	
	Delay_sec(2.0);
	
	printf("Done\n"); 
}
