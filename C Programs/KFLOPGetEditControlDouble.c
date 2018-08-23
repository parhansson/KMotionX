#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


main()
{
	double d;
	// Read String from a KMotionCNC Edit Control
	// Persist Var identifies the Control and contents specifies 
	// where the string data should be placed in the 
	// Gather Buffer as an offset in words
	if (GetEditControlDouble(&d, 160, 1000))
	{
		printf("Error Reading Edit Control Var=160\n");
	}
	else
	{
		printf("Double=%f\n",d);
	}
	
}
