#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


main()
{
	char s[80];
	// Read String from a KMotionCNC Edit Control
	// Persist Var identifies the Control and contents specifies 
	// where the string data should be placed in the 
	// Gather Buffer as an offset in words
	if (GetEditControl(s, 160, 1000))
	{
		printf("Error Reading Edit Control Var=160\n");
	}
	else
	{
		printf("String=%s\n",s);
	}
	
}
