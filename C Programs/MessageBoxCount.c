#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

main()
{
	persist.UserData[25]++;  // increment the count

	char MyMessage[80];  // String to be created and displayed

	sprintf(MyMessage,"Count = %d",persist.UserData[25]);  // build the message we want to show

	MsgBox(MyMessage,MB_OK);  // Show it
}
