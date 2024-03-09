#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


main()
{
	double d1,d2;
	char s[80];
	// Read double from a KMotionCNC Edit Control
	// Persist Var identifies the Control and contents specifies 
	// where the string data should be placed in the 
	// Gather Buffer as an offset in words
	if (GetEditControlDouble(&d1, 160, 1000))
	{
		printf("Error Reading Edit Control Var=160\n");
		DROLabel(1000, 162, "???");
		return;
	}
	// Read double from a KMotionCNC Edit Control
	// Persist Var identifies the Control and contents specifies 
	// where the string data should be placed in the 
	// Gather Buffer as an offset in words
	if (GetEditControlDouble(&d2, 161, 1100))
	{
		printf("Error Reading Edit Control Var=161\n");
		DROLabel(1000, 162, "???");
		return;
	}
	
	// Now compute and form result
	sprintf(s,"%g",d1+d2);
	
	// Put it onto the Screen
	DROLabel(1000, 162, s);
}
