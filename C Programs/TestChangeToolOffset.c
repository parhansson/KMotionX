#include "KMotionDef.h"

int DoPC(int cmd);
int DoPCFloat(int cmd, float f);
int DoPCInt(int cmd, int i);
int MsgBox(char *s, int Flags);
int SetVars(int poff, int varoff, int n);
int GetVars(int varoff, int n, int poff);
int GetToolLength(int index, int poff);
int SetToolLength(int index, int poff);


main()
{
	int Answer;
	double *pD = (double *)persist.UserData;

//	DoPC(PC_COMM_ESTOP);
//	DoPC(PC_COMM_HALT);
//	DoPC(PC_COMM_EXECUTE);
//	DoPC(PC_COMM_SINGLE_STEP);
//	DoPCFloat(PC_COMM_SET_FRO,0.25f);
//	DoPCFloat(PC_COMM_SET_FRO_INC,1.1f);
//	DoPCFloat(PC_COMM_SET_X,0.0);
//	DoPCFloat(PC_COMM_SET_Y,0.0);
//	DoPCFloat(PC_COMM_SET_FRO,1.1);
//	DoPCFloat(PC_COMM_SET_FRO_INC,-0.1);
//	DoPCFloat(PC_COMM_SET_Z,1.25);
//	DoPCInt(PC_COMM_USER_BUTTON,3);
//	DoPCInt(PC_COMM_MCODE,3);

//	Answer = MsgBox("Hello World",MB_YESNO|MB_ICONEXCLAMATION);
//	if (Answer == IDYES)
//		printf("Answer is Yes\n");
//	else
//		printf("Answer is No\n");
	

//	MDI("G0 X1.2 Y2.2 Z3.3");


	// put 3 double values in the persist vars 
	
//	pD[10] = 123.456;
//  pD[11] = 1000.0;
//	pD[12] = 999.9;
	
	// transfer up to the GCode Vars
//	SetVars(100,3,10);  // Upload 3 to GCode 100 from persist 10   
	
//	MDI("#100 = [#100 + 1]");
		
	// read them back into different persist Vars
//	GetVars(100,3,13);  // Download 3 from GCode 100 to persist 13 
	
//	printf("%f %f %f\n",pD[13],pD[14],pD[15]);
	
	// Request Tool #3 Length placed into persist double #16
//	GetToolLength(3,16);
//	printf("Tool Length is %f\n",pD[16]);

	// Change Tool #3 Length to 12.34 passed up via persist double #16
	pD[16]=12.34;
	SetToolLength(3-1,16);
}

// Request a Tool Length offset, answer will be placed at persist offset
int GetToolLength(int index, int poff)
{
	persist.UserData[PC_COMM_PERSIST+2] = poff;       	// persist offset (doubles)
	return DoPCInt(PC_COMM_GET_TOOLTABLE_LENGTH,index); // Tool index and Cmd
}

// Change a Tool Length offset, value to be passed up is at specified persist offset
int SetToolLength(int index, int poff)
{
	persist.UserData[PC_COMM_PERSIST+2] = poff;       	// persist offset (doubles)
	return DoPCInt(PC_COMM_SET_TOOLTABLE_LENGTH,index); // Tool index and Cmd
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
	
	printf("Result = %d\n",result);

	return result;
}
