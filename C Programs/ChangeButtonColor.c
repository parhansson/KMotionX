#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

int main()
{
	for (;;)
	{
		if (ScreenScript("ID:IDC_But12,Colors:ff;ff0000;FFFFFF;800000"))  // background red
			printf("Screen Script Failed\n");
		else
			printf("Screen Script Success\n");
		
		Delay_sec(1);
		
		if (ScreenScript("ID:IDC_But12,Colors:ff;00ff00;FFFFFF;800000"))  // background green
			printf("Screen Script Failed\n");
		else
			printf("Screen Script Success\n");
		
		Delay_sec(1);
	}	
}
