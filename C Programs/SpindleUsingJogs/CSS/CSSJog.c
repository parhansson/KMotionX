
// Handle CSS (Constant Surface Speed) messages from KMotionCNC)
//
// This code assumes you have an Axis Channel Configured to control
// Spindle speed and Jog Calls can be made to control speed
//
// Include this function into your main Init Thead code and call it
// continuously from a forever loop similar to that shown here
 
//#include "KMotionDef.c"
//#include "MySpindleDefs.h"
//#include "CSSJog.c"
//main()
//{
//	for (;;)
//	{
//		WaitNextTimeSlice();
//		ServiceCSS();
//	}
//}

int   *css_mode = &persist.UserData[PC_COMM_CSS_MODE];			// Mode 1=Normal RPM mode. 2=CSS
float *css_xoff = &persist.UserData[PC_COMM_CSS_X_OFFSET];		// X axis counts for Radius zero
float *css_xfactor = &persist.UserData[PC_COMM_CSS_X_FACTOR];	// X axis factor to convert counts to inches 
float *css_s = &persist.UserData[PC_COMM_CSS_S];				// S speed setting in inches/sec
float *css_max_rpm = &persist.UserData[PC_COMM_CSS_MAX_RPM];	// Limit max RPM to this value as Radius approaches zero

double css_T=0;  // update only every so often
#define CSS_UPDATE_DT 0.05

void ServiceCSS(void)
{
	float rpm;
	double T=Time_sec();

	if (*css_mode == 2 && T > css_T) // check if we are in CSS mode and it is time to update
	{
		css_T=T+CSS_UPDATE_DT;  // determine next time to update
		
		// convert axis position to distance from center in inches
		float radius = fast_fabs((chan[CS0_axis_x].Dest - *css_xoff) * *css_xfactor);
		
		if (radius > 0.0f)
			rpm = *css_s / (radius * (TWO_PI_F/60.0f));
		else
			rpm = *css_max_rpm;

		if (rpm > *css_max_rpm) rpm = *css_max_rpm;
				
		if (persist.UserData[STATEVAR]!=0)  // if spindle is already on, ramp to new speed
			Jog(SPINDLEAXIS,rpm * FACTOR);
		
//		printf("xoff=%f radius= %f xfactor=%f s=%f(ips) maxrpm=%f rpm=%f\n",*css_xoff,radius,*css_xfactor,*css_s,*css_max_rpm,rpm);
	}
}

