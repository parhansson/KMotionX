#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "..\..\KflopToKMotionCNCFunctions.c"

//-----------------------------------------
//		LINEAR TOOL CHANGING
//-----------------------------------------
#define AXISX 0
#define AXISY 1
#define AXISZ 2

//---------Absolute position of tool holders
#define HOLDER_X_1 100
#define HOLDER_X_2 200
#define HOLDER_X_3 300
#define HOLDER_X_4 400
#define HOLDER_Y 1300
#define HOLDER_Z -200

// absolute position of the tool height setting plate
#define TOOL_HEIGHT_PLATE_X 50 
#define TOOL_HEIGHT_PLATE_Y 50 

// absolute position to move to that is permanently unobstructed, and safe to move down in Z
#define TOOL_CHANGE_SAFE_POS_X 250  
#define TOOL_CHANGE_SAFE_POS_Y 1200

#define AXIS_SAFE_DISTANCE_Y 100  // distance in mm to approach tool holder
//---------

//--------- Spindle IO bits
#define CLAW_EJECT 58		// IO bit to eject tool from spindle (KONNECT OUTPUT 10)
#define SPINDLE_CLEAN 59	// IO bit to blow air out of spindle taper (KONNECT OUTPUT 11)
#define CLAW_LOOSE 1048		// IO bit to sense whether the claw has ejected (KONNECT INPUT 24)
#define TOOL_SENSE 1049		// IO bit to sense whether the a tool is in the spindle (KONNECT INPUT 24)
//---------

#define TOOL_VAR 9        	// Tool changer desired new tool Var

// Tool changer Last tool loaded is saved globally in this Var
#define LAST_TOOL_VAR 8   	//  -1=Spindle empty, 0=unknown, 1-4 Tool Slot loaded into Spindle
#define TOOL_DISK_FILE "c:\\Temp\\ToolChangerData.txt"


#define CLAMP_TIME 1.0    	// seconds to wait for the clamp/unclamp
#define TOOL_HEIGHT_BIT	1055	//bit to read tool height plate (KONNECT INPUT 31)

#define SAFE_HEIGHT_Z 100   // relative distance in mm to move to clear the top of the tool taper
#define TOOL_RETRACT_SPEED_Z 5.0	//speed in mm/second to move spindle up after tool has been ejected

#define SlowSpeed 10.0 //mm/sec

#define CNT_PER_MM_X 800.0 
#define CNT_PER_MM_Y 800.0 
#define CNT_PER_MM_Z 800.0 

// function prototypes
int DoToolChange(int ToolSlot);
int GetCurrentTool(int *tool);
int SaveCurrentTool(int tool);
BOOL ToolNumberValid(int tool);
float ToolPositionX(int tool);
int MoveXY(float x, float y, float Speed);
int MoveZ(float z, float Speed);
int UnloadTool(int CurrentTool);
int LoadNewTool(int Tool);
int EjectTool(void);



main()
{
	int ToolSlot = persist.UserData[TOOL_VAR];  // Requested tool to load (value stored an integer) 

	if (DoToolChange(ToolSlot))  // perform Tool Change
	{
		// error, Halt Job
		DoPC(PC_COMM_HALT);
	}
}

// Perform Tool Change.  Return 0=Success, 1=Failure
int DoToolChange(int ToolSlot)
{
	int CurrentTool;

	if (GetCurrentTool(&CurrentTool)) return 1;  //  -1=Spindle empty, 0=unknown, 1-4 Tool Slot loaded into Spindle

	printf("Load Tool Slot %d requested, Current Tool %d\n",ToolSlot, CurrentTool);
	
	if (!ToolNumberValid(ToolSlot))  // check if invalid
	{
		char s[80];
		sprintf(s,"Invalid Tool Change Number %d\n",ToolSlot);
		printf(s);
		MsgBox(s, MB_ICONHAND | MB_OK);
		return 1;
	}
	
	if (CurrentTool!=-1) // is there a tool in the Spindle??
		if (UnloadTool(CurrentTool)) return 1;  // yes, unload it
		
	// Now Spindle is empty, load requested tool
	if (LoadNewTool(ToolSlot)) return 1;
	
	SaveCurrentTool(ToolSlot);  // save the one that has been loaded
	return 0;  // success
}


// - Load new Tool (Spindle must be empty)
int LoadNewTool(int Tool)
{
	// - Move to position of requested tool
	// - Rapid move to absolute position of new tool only in X and Y
	if (MoveXY(ToolPositionX(Tool),HOLDER_Y,SlowSpeed)) return 1;

	// - Move to tool Z position at TOOL_RETRACT_SPEED_Z
	if (MoveZ(HOLDER_Z,SlowSpeed)) return 1;

	// - Engage new tool
	// - CLAW_EJECT and SPINDLE_CLEAN bits are currently high from tool removal operation
	// - Turn off CLAW_EJECT and SPINDLE_CLEAN bits to engage tool
	ClearBit(CLAW_EJECT);
	ClearBit(SPINDLE_CLEAN);

	// - Wait for time in seconds defined by CLAMP_TIME
	Delay_sec(CLAMP_TIME);

	// - Check to see if CLAW_LOOSE and TOOL_SENSE are high; if either are not, 
	//		something has gone wrong; halt everything and display message indicating failure
	// - Tool has been engaged
	if (!ReadBit(CLAW_LOOSE))
	{
		printf("Claw Still Loose Error\n");
		MsgBox("Claw Still Loose Error\n", MB_ICONHAND | MB_OK);
		return 1;
	}
	if (!ReadBit(TOOL_SENSE))
	{
		printf("Tool Sense Error\n");
		MsgBox("Tool Sense Error\n", MB_ICONHAND | MB_OK);
		return 1;
	}

	// - Leave tool holder by moving Y axis by the negative value of Y_AXIS_SAFE_DISTANCE
	// - Move to position of requested tool
	// - Rapid move to absolute position of new tool only in X and Y
	if (MoveXY(ToolPositionX(Tool),HOLDER_Y+AXIS_SAFE_DISTANCE_Y,SlowSpeed)) return 1;

	// - Rapid to Z home
	if (MoveZ(0.0,SlowSpeed)) return 1;

	return 0; //success
}


// - Remove tool in spindle by going to holder of current tool
int UnloadTool(int CurrentTool)
{
	// - Rapid to Z Home to clear any work that may be on the table
	if (MoveZ(0.0,SlowSpeed)) return 1;

	// - Rapid to TOOL_CHANGE_SAFE_POS to execute a safe negative Z move
	if (MoveXY(TOOL_CHANGE_SAFE_POS_X,TOOL_CHANGE_SAFE_POS_Y,SlowSpeed)) return 1;
	
	// - Approach tool holder by matching Z height of tool flange currently in spindle with tool holder                            claw
	if (MoveZ(HOLDER_Z,SlowSpeed)) return 1;

	// - After matching height above, approach tool holder by moving to holder X position
	if (MoveXY(ToolPositionX(CurrentTool),TOOL_CHANGE_SAFE_POS_Y,SlowSpeed)) return 1;

	// - After matching X position, match tool Y position
	if (MoveXY(ToolPositionX(CurrentTool),HOLDER_Y,SlowSpeed)) return 1;

	// - Move only in Y position until current position matches tool holder position (maybe disable X)                          axis?)
	// ???
	
	// - Eject tool
	if (EjectTool()) return 1;


	return 0; //success
}


// - Eject tool
int EjectTool(void)
{ 
	// - Turn on CLAW_EJECT bit to remove tool from spindle
	SetBit(CLAW_EJECT);

	// - Turn on SPINDLE_CLEAN bit to remove any debris from taper and tools
	SetBit(SPINDLE_CLEAN);

	// - Wait for time in seconds defined by CLAMP_TIME
	Delay_sec(CLAMP_TIME);
	
	// - Read CLAW_LOOSE bit to see whether the tool is loose, to make a safe Z move without  
    //      destroying tool holder
	// - If CLAW_LOOSE bit is high, something has gone wrong;
	//		halt everything and display message indicating failure
	if (ReadBit(CLAW_LOOSE))
	{
		printf("Claw Loose Error\n");
		MsgBox("Claw Loose Error\n", MB_ICONHAND | MB_OK);
		return 1;
	}

	// - Move Z axis up at speed defined by 'Z_TOOL_RETRACT_SPEED', to Z_SAFE_HEIGHT
	if (MoveZ(HOLDER_Z+SAFE_HEIGHT_Z,TOOL_RETRACT_SPEED_Z)) return 1;

	// - Read TOOL_SENSE bit to see whether the tool has been successfully ejected from the spindle
	// - If TOOL_SENSE bit is high, something has gone wrong; 
	//		halt everything and display message indicating failure
	if (ReadBit(TOOL_SENSE))
	{
		printf("Tool Sense Release Error\n");
		MsgBox("Tool Sense Release Error\n", MB_ICONHAND | MB_OK);
		return 1;
	}
	return 0; // success
}



//return x position of tool holder as a function of the tool
float ToolPositionX(int tool)
{
	return (HOLDER_X_2-HOLDER_X_1)*(tool-1) + HOLDER_X_1;
}



// Get the last loaded tool.  Parameter points to where to return tool
// First try to get from KFLOP memory
// if memory is invalid, try to read from disk
// if can't read disk then ask Operator
// returns 0 on success, 1 on fail or Operator asked to abort

int GetCurrentTool(int *ptool)
{
	int success,Answer,result,tool;
	float value;

	tool = persist.UserData[LAST_TOOL_VAR];
	success = ToolNumberValid(tool);  // check if valid

	if (!success)   // invalid after power up, try to read from PC Disk File
	{
		// Try to open file
		FILE *f=fopen(TOOL_DISK_FILE,"rt");
		if (f)  // did file open?
		{
			// read a line and convert it
			result=fscanf(f,"%d",&tool);
			fclose(f);
			
			if (result==1 && ToolNumberValid(tool))
			{
				printf("Read Disk File Value of %d\n",tool);
				success=TRUE; // success if one value converted
			}
		}
		
		if (!success) printf("Unable to open/read file:%s\n",TOOL_DISK_FILE);  
	}

	if (!success)   // if still no success ask Operator
	{
		Answer = InputBox("Tool in Spindle or -1",&value);
		if (Answer)
		{
			printf("Operator Canceled\n");
			return 1;
		}
		else
		{
			tool=value;
			printf("Operator Entered Value of %d\n",tool);
		}
	}

	if (!ToolNumberValid(tool))  // check if invalid
	{
		char s[80];
		sprintf(s,"Invalid Current Tool Number %d\n",tool);
		printf(s);
		MsgBox(s, MB_ICONHAND | MB_OK);
		return 1;
	}
	
	printf("Current tool = %d\n",tool);
	*ptool = tool;  // return result to caller
	return 0;  //success
}

// save the tool number to KFLOP global Variable and to PC Disk file in case we loose power
int SaveCurrentTool(int tool)
{
	persist.UserData[LAST_TOOL_VAR]=tool;
	FILE *f=fopen(TOOL_DISK_FILE,"wt");
	fprintf(f,"%d\n",tool);
	fclose(f);
	return 0;
}

// check if Current Tool number Valid
// -1 = no tool loaded
// 1-4 = valid tool
BOOL ToolNumberValid(int tool)
{
	return tool == -1 || (tool>=1 && tool<=4);
}


// Move Axis XY at specified Speed and wait until complete
// return 0 = success, 1 if axis disabled
int MoveXY(float x, float y, float Speed)
{
	MoveAtVel(AXISX, x * CNT_PER_MM_X, Speed * CNT_PER_MM_X);
	MoveAtVel(AXISZ, y * CNT_PER_MM_Y, Speed * CNT_PER_MM_Y);
	
	while (!CheckDone(AXISX) || !CheckDone(AXISY))
	{
		if (!chan[AXISX].Enable)
		{
			printf("Error X Axis Disabled\n");
			MsgBox("Error X Axis Disabled\n", MB_ICONHAND | MB_OK);
			return 1;
		}
		if (!chan[AXISY].Enable)
		{
			printf("Error Y Axis Disabled\n");
			MsgBox("Error Y Axis Disabled\n", MB_ICONHAND | MB_OK);
			return 1;
		}
	}
	return 0;  //success
}

// Move Axis Z at specified Speed and wait until complete
// return 0 = success, 1 if axis disabled
int MoveZ(float z, float Speed)
{
	MoveAtVel(AXISZ, z * CNT_PER_MM_Z, Speed * CNT_PER_MM_Z);
	
	while (!CheckDone(AXISZ))
	{
		if (!chan[AXISZ].Enable)
		{
			printf("Error Z Axis Disabled\n");
			MsgBox("Error Z Axis Disabled\n", MB_ICONHAND | MB_OK);
			return 1;
		}
	}
	return 0;  //success
}


