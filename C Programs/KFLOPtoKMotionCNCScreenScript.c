#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

//For example "WinMsg:Keyboard;;KeyPress;120" to press F9
//For example "WinMsg:DlgName; IDC_SpindleOnCW; BM_CLICK;" to click the Spindle on cw button
//For example "Action : 5; 3; 0; 0; 0; 0; SpindleUsingJogs\CSS\OnCWJog.c" to execute/wait the seec prog in Thread 3 Var 0

void main()
{
	if (ScreenScript("SScript:SearchM6Down_SubScript.scr"))
		printf("Screen Script Failed\n");
	else
		printf("Screen Script Success\n");
}
