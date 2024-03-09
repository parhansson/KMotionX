#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


#define VALUE2_DVAR 11
main()
{
	double d;
	// Read double from a KMotionCNC Edit Control
	// Persist Var identifies the Control and contents specifies 
	// where the string data should be placed in the 
	// Gather Buffer as an offset in words
	if (GetEditControlDouble(&d, 161, 1000))
	{
		printf("Error Reading Edit Control Var=161\n");
		return;
	}
	
	SetUserDataDouble(VALUE2_DVAR,d);  //Save it in a global persist double variable
	
	printf("Variable set to %f\n",GetUserDataDouble(VALUE2_DVAR));  // print it as a test
}
