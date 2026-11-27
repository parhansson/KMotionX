#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

int main()
{
	if (!ReadBit(48) || !ReadBit(49))  // check if all necessary operations have been completed
	{
		MsgBox("Please Home and Set Tool before running Jobs",MB_OK);
		DoPC(PC_COMM_HALT);
	}
}
