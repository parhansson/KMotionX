#include "KMotionDef.h"

#define TMP 10 // which spare persists to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define NVARS 6
void DoVarsToDROs(void);

main()
{
	for (;;)
	{
		DoVarsToDROs();
		Delay_sec(0.5);
	}
}

// Get some Gcode Variables and put them into on-screen DRO as a Watch Diagnostic

void DoVarsToDROs(void)
{
	int i;
	char s[80];
	
	// Download Variables from GCode #1 to persists 
	GetVars(1,NVARS,TMP); 
	
	for (i=0;i<NVARS; i++)
	{
		// Format the variable to a string
		sprintf(s,"%11.3f",GetUserDataDouble(TMP+i));
		
		// Put it onto the Screen into DROs 160..
		DROLabel(1000+50*i, 160+i, s);
	}
}
