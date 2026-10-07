#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

void main()
{
	if (ScreenScript("ID:IDC_But10,Show:0"))
		printf("Screen Script Failed\n");
	else
		printf("Screen Script Success\n");
}
