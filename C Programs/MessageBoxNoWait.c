#include "KMotionDef.h"
#define GATH_OFF 0  // define the offset into the Gather buffer where strings are passed

// Pass a command to the PC and wait for it to handshake
// that it was received by either clearing the command
// or changing it to a negative error code
int DoPC(int cmd)
{
	int result;
	
	persist.UserData[PC_COMM_PERSIST]=cmd;
	
//	do
//	{
//		WaitNextTimeSlice();	
//	}while (result=persist.UserData[PC_COMM_PERSIST]>0);
	
//	printf("Result = %d\n",result);

	return result;
}


// Put an integer as a parameter and pass the command to the App
int DoPCInt(int cmd, int i)
{
	int result;
	persist.UserData[PC_COMM_PERSIST+1] = i;
	return DoPC(cmd);
}


// Trigger a message box on the PC to be displayed
// defines for MS Windows message box styles and Operator
// response IDs are defined in the KMotionDef.h file 
int MsgBox(char *s, int Flags)
{
	char *p=(char *)gather_buffer+GATH_OFF*sizeof(int);
	int i;
	
	do // copy to gather buffer w offset 0
	{
		*p++ = *s++;
	}while (s[-1]);
	
	persist.UserData[PC_COMM_PERSIST+2] = Flags;  // set options
	DoPCInt(PC_COMM_MSG,GATH_OFF);
	return persist.UserData[PC_COMM_PERSIST+3];
}


main()
{
	int Answer;

	MsgBox("Hello World",MB_OK|MB_ICONEXCLAMATION);
}
