#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define ORIGINAL_SPEED_VARS_X 30 // variable to save original X Speed as a float
#define ORIGINAL_SPEED_VARS_Y 31 // variable to save original Y Speed as a float

main()
{
	DoPC(PC_COMM_GET_JOG_OVERRIDE_X);
	persist.UserData[ORIGINAL_SPEED_VARS_X]=persist.UserData[PC_COMM_PERSIST+1];
	printf("XJog Speed=%f\n", *(float *)&persist.UserData[ORIGINAL_SPEED_VARS_X]);  // print it

	DoPC(PC_COMM_GET_JOG_OVERRIDE_Y);
	persist.UserData[ORIGINAL_SPEED_VARS_Y]=persist.UserData[PC_COMM_PERSIST+1];
	printf("YJog Speed=%f\n", *(float *)&persist.UserData[ORIGINAL_SPEED_VARS_Y]);  // print it
}

