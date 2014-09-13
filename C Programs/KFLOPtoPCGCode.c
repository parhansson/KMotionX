#include "KMotionDef.h"

int DoPC(int cmd);
int DoPCInt(int cmd, int i);
#define GATH_OFF 0  // define the offset into the Gather buffer where strings are passed


main()
{
	MDI("G92.1");
}






// put the MDI string (Manual Data Input - GCode) in the 
// gather buffer and tell the App where it is
int MDI(char *s)
{
	char *p=(char *)gather_buffer+GATH_OFF*sizeof(int);
	int i;
	
	do // copy to gather buffer w offset 0
	{
		*p++ = *s++;
	}while (s[-1]);
	
	// issue the command an wait till it is complete
	// (or an error - such as busy)
	return DoPCInt(PC_COMM_MDI,GATH_OFF);
}


// Put an integer as a parameter and pass the command to the App
int DoPCInt(int cmd, int i)
{
	int result;
	persist.UserData[PC_COMM_PERSIST+1] = i;
	return DoPC(cmd);
}

// Pass a command to the PC and wait for it to handshake
// that it was received by either clearing the command
// or changing it to a negative error code
int DoPC(int cmd)
{
	int result;
	
	persist.UserData[PC_COMM_PERSIST]=cmd;
	
	do
	{
		WaitNextTimeSlice();	
	}while (result=persist.UserData[PC_COMM_PERSIST]>0);
	
	printf("Result = %d\n",result);

	return result;
}
