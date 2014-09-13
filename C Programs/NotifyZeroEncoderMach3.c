#include "KMotionDef.h"

//Plugin calls for Mach3 NotifyPlugins Commands

#define X 0
#define Y 1
#define Z 2


main()
{
	int msg = persist.UserData[6];  // Mach3 notify Message 10000-10999

	printf("Mach3 Notify Call, Message = %d\n",msg); 
	
	if (msg==10100)
	{
		// adjust the commanded position to match the glass scale encoder
		DisableAxis(X);
		EnableAxisDest(X,chan[X].Position);
	}
	if (msg==10101)
	{
		// adjust the commanded position to match the glass scale encoder
		DisableAxis(Y);
		EnableAxisDest(Y,chan[Y].Position);
	}
	if (msg==10102)
	{
		// adjust the commanded position to match the glass scale encoder
		DisableAxis(Z);
		EnableAxisDest(Z,chan[Z].Position);
	}
	if (msg==10500)
	{
		if (CS0_StoppingState == 0)
			StopCoordinatedMotion();
		else
			ResumeCoordinatedMotion();
	}
 if (msg==10300)
 {
            // User wants to disable Z (switch to OL)

            DisableAxis(Z);
            chan[Z].OutputMode = STEP_DIR_MODE;

            EnableAxisDest(Z,chan[Z].Position);
}
 if (msg==10301)
 {          // User wants to enable Z (switch to CL)

            DisableAxis(Z);
            chan[Z].OutputMode = CL_STEP_DIR_MODE;

            EnableAxisDest(Z,chan[Z].Position);
}

}
