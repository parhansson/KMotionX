#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

main()
{
	int Answer;
	float value;

	Answer = InputBox("Enter Distance",&value);
	if (Answer)
		printf("Operator Canceled\n");
	else
		printf("Operator Entered Value of %.3f\n",value);
	
}
