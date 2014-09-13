double *pD = (double *)persist.UserData;

#define CANON_UNITS_INCHES 1
#define CANON_UNITS_MM 2


int GetDROs(double *DROx, double *DROy, double *DROz, double *DROa, double *DROb, double *DROc)
{
	if (DoPCInt(PC_COMM_GET_DROS,TMP)) return 1;       // Var index and Cmd
	*DROx=pD[TMP];
	*DROy=pD[TMP+1];
	*DROz=pD[TMP+2];
	*DROa=pD[TMP+3];
	*DROb=pD[TMP+4];
	*DROc=pD[TMP+5];
}

int GetMachine(double *Machinex, double *Machiney, double *Machinez, double *Machinea, double *Machineb, double *Machinec)
{
	if (DoPCInt(PC_COMM_GET_MACHINE_COORDS,TMP)) return 1;       // Var index and Cmd
	*Machinex=pD[TMP];
	*Machiney=pD[TMP+1];
	*Machinez=pD[TMP+2];
	*Machinea=pD[TMP+3];
	*Machineb=pD[TMP+4];
	*Machinec=pD[TMP+5];
}

int GetMiscSettings(int *Units, int *TWORD, int *HWORD, int *DWORD)
{
	if (DoPCInt(PC_COMM_GET_MISC_SETTINGS,TMP)) return 1; 
	
	*Units = persist.UserData[TMP];
	*TWORD = persist.UserData[TMP+1];
	*HWORD = persist.UserData[TMP+2];
	*DWORD = persist.UserData[TMP+3];
}

int GetFixtureIndex(int *FixtureIndex)
{
	if (GetVars(5220,1,TMP)) return 1;  // Download to persist TMP
	*FixtureIndex=pD[TMP];
	return 0;
}

int GetOriginOffset(double *OriginOffset, int FixtureIndex, int Axis)
{
	if (GetVars(5200+FixtureIndex*20+Axis+1,1,TMP)) return 1;  // Download to persist TMP
	*OriginOffset=pD[TMP];
	return 0;
}

int GetAxisOffset(double *AxisOffset, int Axis)
{
	if (GetVars(5200+Axis+11,1,TMP)) return 1;  // Download to persist TMP
	*AxisOffset=pD[TMP];
	return 0;
}

// Request a Tool Length offset, answer will be placed at persist offset
int GetToolLength(int index, double *Length)
{
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
	if (DoPCInt(PC_COMM_GET_TOOLTABLE_LENGTH,index)) return 1; // Tool index and Cmd
	*Length=pD[TMP];
}

// Change a Tool Length offset, value to be passed up is at specified persist offset
int SetToolLength(int index, double Length)
{
	pD[TMP]=Length;
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
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
	
//	printf("Result = %d\n",result);

	return result;
}
