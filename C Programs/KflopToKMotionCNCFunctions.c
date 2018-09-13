#define CANON_UNITS_INCHES 1
#define CANON_UNITS_MM 2

double GetUserDataDouble(int i)
{
	double d;
	
	((int*)(&d))[0] = persist.UserData[i*2];
	((int*)(&d))[1] = persist.UserData[i*2+1];
	return d;
}

void SetUserDataDouble(int i, double v)
{
	double d=v;
	persist.UserData[i*2]   = ((int*)(&d))[0];
	persist.UserData[i*2+1] = ((int*)(&d))[1] ;
}


double RoundToReasonable(double v, int Units)
{
	if (Units==CANON_UNITS_INCHES)			// for inches round to 6 digits
	{
		if (v<0)
			return ((int)(-v * 1e6 + 0.5)) / -1e6;
		else
			return ((int)(v * 1e6 + 0.5)) / 1e6;
	}
	else									// for mm round to 4 digits
	{
		if (v<0)
			return ((int)(-v * 1e4 + 0.5)) / -1e4;
		else
			return ((int)(v * 1e4 + 0.5)) / 1e4;
	}
}


int GetDROs(double *DROx, double *DROy, double *DROz, double *DROa, double *DROb, double *DROc)
{
	if (DoPCInt(PC_COMM_GET_DROS,TMP)) return 1;       // Var index and Cmd
	*DROx=GetUserDataDouble(TMP);
	*DROy=GetUserDataDouble(TMP+1);
	*DROz=GetUserDataDouble(TMP+2);
	*DROa=GetUserDataDouble(TMP+3);
	*DROb=GetUserDataDouble(TMP+4);
	*DROc=GetUserDataDouble(TMP+5);
}

int GetMachine(double *Machinex, double *Machiney, double *Machinez, double *Machinea, double *Machineb, double *Machinec)
{
	if (DoPCInt(PC_COMM_GET_MACHINE_COORDS,TMP)) return 1;       // Var index and Cmd
	*Machinex=GetUserDataDouble(TMP);
	*Machiney=GetUserDataDouble(TMP+1);
	*Machinez=GetUserDataDouble(TMP+2);
	*Machinea=GetUserDataDouble(TMP+3);
	*Machineb=GetUserDataDouble(TMP+4);
	*Machinec=GetUserDataDouble(TMP+5);
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
	*FixtureIndex=GetUserDataDouble(TMP);
	return 0;
}

int GetOriginOffset(double *OriginOffset, int FixtureIndex, int Axis)
{
	if (GetVars(5200+FixtureIndex*20+Axis+1,1,TMP)) return 1;  // Download to persist TMP
	*OriginOffset=GetUserDataDouble(TMP);
	return 0;
}

int GetAxisOffset(double *AxisOffset, int Axis)
{
	if (GetVars(5200+Axis+11,1,TMP)) return 1;  // Download to persist TMP
	*AxisOffset=GetUserDataDouble(TMP);
	return 0;
}

// Request a Tool Length offset, answer will be placed at persist offset
int GetToolLength(int index, double *Length)
{
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
	if (DoPCInt(PC_COMM_GET_TOOLTABLE_LENGTH,index)) return 1; // Tool index and Cmd
	*Length=GetUserDataDouble(TMP);
}

// Change a Tool Length offset, value to be passed up is at specified persist offset
int SetToolLength(int index, double Length)
{
	SetUserDataDouble(TMP,Length);
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
	return DoPCInt(PC_COMM_SET_TOOLTABLE_LENGTH,index); // Tool index and Cmd
}

// Request a Tool Diameter, answer will be placed at persist offset
int GetToolDiameter(int index, double *Diameter)
{
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
	if (DoPCInt(PC_COMM_GET_TOOLTABLE_DIAMETER,index)) return 1; // Tool index and Cmd
	*Diameter=GetUserDataDouble(TMP);
}

// Change a Tool Diameter, value to be passed up is at specified persist offset
int SetToolDiameter(int index, double Diameter)
{
	SetUserDataDouble(TMP,Diameter);
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
	return DoPCInt(PC_COMM_SET_TOOLTABLE_DIAMETER,index); // Tool index and Cmd
}

// Request a Tool X offset, answer will be placed at persist offset
int GetToolOffsetX(int index, double *OffsetX)
{
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
	if (DoPCInt(PC_COMM_GET_TOOLTABLE_OFFSETX,index)) return 1; // Tool index and Cmd
	*OffsetX=GetUserDataDouble(TMP);
}

// Change a Tool X offset, value to be passed up is at specified persist offset
int SetToolOffsetX(int index, double OffsetX)
{
	SetUserDataDouble(TMP,OffsetX);
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
	return DoPCInt(PC_COMM_SET_TOOLTABLE_OFFSETX,index); // Tool index and Cmd
}

// Request a Tool Y offset, answer will be placed at persist offset
int GetToolOffsetY(int index, double *OffsetY)
{
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
	if (DoPCInt(PC_COMM_GET_TOOLTABLE_OFFSETY,index)) return 1; // Tool index and Cmd
	*OffsetY=GetUserDataDouble(TMP);
}

// Change a Tool Y offset, value to be passed up is at specified persist offset
int SetToolOffsetY(int index, double OffsetY)
{
	SetUserDataDouble(TMP,OffsetY);
	persist.UserData[PC_COMM_PERSIST+2] = TMP;       	// persist offset (doubles)
	return DoPCInt(PC_COMM_SET_TOOLTABLE_OFFSETY,index); // Tool index and Cmd
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


// returns both result of communication and if communication
// successful the User Response;
int MsgBoxGetResponse(int *response)
{
	*response=-1;
	int result=persist.UserData[PC_COMM_PERSIST];
	if (result==0)
		*response=persist.UserData[PC_COMM_PERSIST+3];

	return result;
}

// Trigger a message box on the PC to be displayed
// defines for MS Windows message box styles and Operator
// response IDs are defined in the KMotionDef.h file 
// (Don't wait for response)
int MsgBoxNoWait(char *s, int Flags)
{
	char *p=(char *)gather_buffer+GATH_OFF*sizeof(int);
	int i;
	
	do // copy to gather buffer w offset 0
	{
		*p++ = *s++;
	}while (s[-1]);
	
	persist.UserData[PC_COMM_PERSIST+2] = Flags;  // set options
	DoPCIntNoWait(PC_COMM_MSG,GATH_OFF);
	return 0;
}

// Trigger a dialog box on the PC to request a floating point
// value to be entered by Operator.  returns 1 if the Operator
// selected cancel.  returns 0 if the operator selected "Set" 
int InputBox(char *s, float *value)
{
	char *p=(char *)gather_buffer+GATH_OFF*sizeof(int);
	int i;
	
	do // copy to gather buffer w offset 0
	{
		*p++ = *s++;
	}while (s[-1]);
	
	DoPCInt(PC_COMM_INPUT,GATH_OFF);
	*value = *(float *)&persist.UserData[PC_COMM_PERSIST+2];  // return the value
	return persist.UserData[PC_COMM_PERSIST+3];
}


// Write a string to a DRO Label on the screen.
// Put the string into the gather buffer at the specified offset (in words)
// Then place the offset in the specified persit variable
// KMotionCNC will upload and display the message and then
// clear the persist variable.
//
// in order to avoid any possibility of an unterminated message
// write the message in reverse so the termination is added first

void DROLabel(int gather_offset, int persist_var, char *s)
{
	char *p=(char *)gather_buffer+gather_offset*sizeof(int);
	int i,n;
	
	// first find length of string
	for (n=0; n<256; n++) if (s[n]==0) break;

	// now copy string backwards
	for (i=n; i>=0; i--) p[i]=s[i]; 
	
	persist.UserData[persist_var] = gather_offset; // set gather offset
	return;
}

// Write a float to a Persist Variable

void WriteVarFloat(int persist_var, float v)
{
	float f=v;
	persist.UserData[persist_var] = *(int *)&f;
}


// Read String from a KMotionCNC Edit Control
// Persist Var identifies the Control and specifies 
// where the string data should be placed in the 
// Gather Buffer as an offset in words
int GetEditControl(char *s, int Var, int offset)
{
	char *p=(char *)gather_buffer+offset*sizeof(int);
	int result;
	
	persist.UserData[Var]=offset;
	if (DoPCInt(PC_GET_EDIT_CELL,Var)<0)
	{
		return -1;
	}
	
	do // copy from gather buffer 
	{
		*s++ = *p++;
	}while (s[-1]);
	
	return 0;
}

// Read String from a KMotionCNC Edit Control and convert to a double
// Persist Var identifies the Control and specifies 
// where the string data should be placed in the 
// Gather Buffer as an offset in words
int GetEditControlDouble(double *d, int Var, int offset)
{
	int result;
	char s[80];
	if (GetEditControl(s, Var, offset)) return 1;
	result = sscanf(s,"%lf",d);
	if (result!=1) return 1;
	return 0;
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
		result=persist.UserData[PC_COMM_PERSIST];
	}while (result>0);
	
//	printf("Result = %d\n",result);

	return result;
}

// Put an integer as a parameter and pass the command to the App
// (Don't wait for response)
int DoPCIntNoWait(int cmd, int i)
{
	int result;
	persist.UserData[PC_COMM_PERSIST+1] = i;
	return DoPCNoWait(cmd);
}

// Pass a command to the PC and wait for it to handshake
// that it was received by either clearing the command
// or changing it to a negative error code
// (Don't wait for response)

int DoPCNoWait(int cmd)
{
	int result;
	
	persist.UserData[PC_COMM_PERSIST]=cmd;
	
	return 0;
}
