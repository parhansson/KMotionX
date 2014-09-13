#include "KMotionDef.h"
int DoPC(int cmd);
int DoPCFloat(int cmd, float f);
int DoPCInt(int cmd, int i);
int MsgBox(char *s, int Flags);
int SetVars(int poff, int varoff, int n);
int GetVars(int varoff, int n, int poff);

main()
{
	int Answer;
	int tool = persist.UserData[9];  // value stored is actually a float 
	char s[100];
	
	ClearBit(153); // Stop spindle
	ClearBit(154); // Stop spindle
	sprintf(s,"Change to Tool %d",tool);
	Answer = MsgBox(s,MB_ICONEXCLAMATION|MB_OKCANCEL);

	if (Answer != IDOK)
	{
		DoPC(PC_COMM_HALT);
	}
}
int SetVars(int varoff, int n, int poff)
{
   persist.UserData[PC_COMM_PERSIST+2] = n;       // number of elements
   persist.UserData[PC_COMM_PERSIST+3] = poff;    // persist offset (doubles)
   return DoPCInt(PC_COMM_SET_VARS,varoff);       // Var index and Cmd
}
int GetVars(int varoff, int n, int poff)
{
   persist.UserData[PC_COMM_PERSIST+2] = n;       // number of elements
   persist.UserData[PC_COMM_PERSIST+3] = poff;    // persist offset (doubles)
   return DoPCInt(PC_COMM_GET_VARS,varoff);       // Var index and Cmd
}

#define GATH_OFF 0  // define the offset into the Gather buffer where strings are passed
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
// Put a Float as a parameter and pass the command to the App
int DoPCFloat(int cmd, float f)
{
   int result;
   persist.UserData[PC_COMM_PERSIST+1] = *(int*)&f;
   return DoPC(cmd);
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
   
   //printf("Result = %d\n",result);
   return result;
}