#include "KMotionDef.h"
#define TMP 10					// which spare persist to use to transfer data
#include "..\..\KflopToKMotionCNCFunctions.c"

//-----------------------------------------
//      Rotary TOOL CHANGING
//-----------------------------------------
#define AXISA 3

#define TOOL_VAR 9				// Tool changer desired new tool Var

// Tool changer Last tool loaded is saved globally in this Var
#define LAST_TOOL_VAR 8			//  -1=Spindle empty, 0=unknown, 1-4 Tool Slot loaded into Spindle
#define TOOL_DISK_FILE "c:\\Temp\\ToolChangerData.txt"

#define CNT_PER_DEGREE_A 1000.0

// function prototypes
int DoToolChange(int ToolSlot);
int GetCurrentTool(int *tool);
int SaveCurrentTool(int tool);
BOOL ToolNumberValid(int tool);
int UnloadTool(int CurrentTool);
int LoadNewTool(int CurrentTool, int Tool);


main()
{
	int ToolSlot = persist.UserData[TOOL_VAR];	// Requested tool to load (value stored an integer) 

	if (DoToolChange(ToolSlot))	// perform Tool Change
	{
		// error, Halt Job
		DoPC(PC_COMM_HALT);
	}
}

// Perform Tool Change.  Return 0=Success, 1=Failure
int DoToolChange(int ToolSlot)
{
	int CurrentTool;

	if (GetCurrentTool(&CurrentTool))
		return 1;				//  -1=Spindle empty, 0=unknown, 1-4 Tool Slot loaded into Spindle

	printf("Load Tool Slot %d requested, Current Tool %d\n", ToolSlot, CurrentTool);

	if (!ToolNumberValid(ToolSlot))	// check if invalid
	{
		char s[80];
		sprintf(s, "Invalid Tool Change Number %d\n", ToolSlot);
		printf(s);
		MsgBox(s, MB_ICONHAND | MB_OK);
		return 1;
	}

	// load requested tool
	if (LoadNewTool(CurrentTool, ToolSlot))
		return 1;

	SaveCurrentTool(ToolSlot);	// save the one that has been loaded
	return 0;					// success
}


// - Load new Tool by adjusting A Axis machine coordinate appropriately
int LoadNewTool(int CurrentTool, int Tool)
{
	if (CurrentTool != Tool)
	{
		double Adjustment = (Tool - CurrentTool) * 90.0 * CNT_PER_DEGREE_A;

		DisableAxis(AXISA);
		WaitNextTimeSlice();	// make sure we don't get interrupted
		chan[AXISA].Position -= Adjustment;	// Adjust machine coordinates
		EnableAxisDest(AXISA, chan[AXISA].Position);	// re-enable where we now are
	}
	return 0;					//success
}




// Get the last loaded tool.  Parameter points to where to return tool
// First try to get from KFLOP memory
// if memory is invalid, try to read from disk
// if can't read disk then ask Operator
// returns 0 on success, 1 on fail or Operator asked to abort

int GetCurrentTool(int *ptool)
{
	int success, Answer, result, tool;
	float value;

	tool = persist.UserData[LAST_TOOL_VAR];
	success = ToolNumberValid(tool);	// check if valid

	if (!success)				// invalid after power up, try to read from PC Disk File
	{
		// Try to open file
		FILE *f = fopen(TOOL_DISK_FILE, "rt");
		if (f)					// did file open?
		{
			// read a line and convert it
			result = fscanf(f, "%d", &tool);
			fclose(f);

			if (result == 1 && ToolNumberValid(tool))
			{
				printf("Read Disk File Value of %d\n", tool);
				success = TRUE;	// success if one value converted
			}
		}

		if (!success)
			printf("Unable to open/read file:%s\n", TOOL_DISK_FILE);
	}

	if (!success)				// if still no success ask Operator
	{
		Answer = InputBox("Tool in Spindle or -1", &value);
		if (Answer)
		{
			printf("Operator Canceled\n");
			return 1;
		}
		else
		{
			tool = value;
			printf("Operator Entered Value of %d\n", tool);
		}
	}

	if (!ToolNumberValid(tool))	// check if invalid
	{
		char s[80];
		sprintf(s, "Invalid Current Tool Number %d\n", tool);
		printf(s);
		MsgBox(s, MB_ICONHAND | MB_OK);
		return 1;
	}

	printf("Current tool = %d\n", tool);
	*ptool = tool;				// return result to caller
	return 0;					//success
}

// save the tool number to KFLOP global Variable and to PC Disk file in case we loose power
int SaveCurrentTool(int tool)
{
	persist.UserData[LAST_TOOL_VAR] = tool;
	FILE *f = fopen(TOOL_DISK_FILE, "wt");
	fprintf(f, "%d\n", tool);
	fclose(f);
	return 0;
}

// check if Current Tool number Valid
// -1 = no tool loaded
// 1-4 = valid tool
BOOL ToolNumberValid(int tool)
{
	return tool == -1 || (tool >= 1 && tool <= 4);
}
