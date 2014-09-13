#include "KMotionDef.h"

//Plugin calls for Mach3 NotifyPlugins Commands

#define X 0
#define Y 1
#define Z 2

float DecelTimeForAxis(int axis);

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
	{          
		// User wants to enable Z (switch to CL)
		DisableAxis(Z);
		chan[Z].OutputMode = CL_STEP_DIR_MODE;
		
		EnableAxisDest(Z,chan[Z].Position);
	}

	// handles probing
	//
	// flag is 0 - while watching for probe hit
	// flag is 1 - if probe was already set from start
	// flag is 2 - after successful probe hit
	// flag is 3 - Tells Plugin to upload status (3) to
	//             DRO 1100 and let User handle the error
	//
	// returns the captured results in User Variables
	// X - 50+51
	// Y - 52+53
	// Z - 54+55
	// A - 56+57
	// B - 58+59
	// C - 60+61
	// status result 62
	
	#define PROBE_BIT 0
	#define PROBE_ACTIVE_STATE 1
	#define PROBE_ERROR_HANDLING 0  // 0 Stops Mach3 on probe error
//	#define PROBE_ERROR_HANDLING 3  // 3 User must query DRO 1100 and handle error
	
	if (msg==20000) 
	{
		double *d = (double *)&persist.UserData[MACH3_PROBE_RESULTS_VAR];
		int flag=1;
		
		persist.UserData[MACH3_PROBE_STATUS_VAR]=PROBE_ERROR_HANDLING;
       
		while (ReadBit(PROBE_BIT)!=PROBE_ACTIVE_STATE)
		{
			flag=2;  
			WaitNextTimeSlice();
		}
		
		if (CS0_axis_x>=0) d[0]=chan[CS0_axis_x].Dest;
		if (CS0_axis_y>=0) d[1]=chan[CS0_axis_y].Dest;
		if (CS0_axis_z>=0) d[2]=chan[CS0_axis_z].Dest;
		if (CS0_axis_a>=0) d[3]=chan[CS0_axis_a].Dest;
		if (CS0_axis_b>=0) d[4]=chan[CS0_axis_b].Dest;
		if (CS0_axis_c>=0) d[5]=chan[CS0_axis_c].Dest;
		
		persist.UserData[MACH3_PROBE_STATUS_VAR]=flag;
		StopCoordinatedMotion();
	}
}
