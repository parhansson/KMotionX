#include "KMotionDef.h"

// Services Tool Changer using non-blocking State Machine approach
// The Service routines maintain their state so they can always return
// immediately and later resume from their previous state.

// T O O L   C H A N G E R   S E Q U E N C E
//
// define state that the tool changer may be in
enum {
T_IDLE,
T_START,
T_WAIT_Z_UP,
T_WAIT_MOVE_XY_UNLOAD,
T_WAIT_MOVE_Z_DOWN_FAST,
T_WAIT_MOVE_Z_DOWN_SLOW,
T_WAIT_MOVE_Z_DOWN_VERYSLOW,
T_WAIT_OPENCLAMP,
T_WAIT_Z_UP2,
T_WAIT_MOVE_XY_LOAD,
T_WAIT_MOVE_Z_DOWN_FAST2,
T_WAIT_MOVE_Z_DOWN_SLOW2,
T_WAIT_MOVE_Z_DOWN_VERYSLOW2,
T_WAIT_CLOSECLAMP,
T_WAIT_Z_UP3,
T_WAIT_MOVE_XY_PROBE
};

#define TOOL_VAR 9        	// Tool changer desired new tool Var
#define TOOL_STATE_VAR 8  	// Tool changer state is saved globally in this Var
#define LAST_TOOL_VAR 7   	// Tool changer Last tool position is saved globally in this Var
#define TOOL_CLAMP_BIT 46 	// Output bit number to clamp tool
#define TOOL_CLAMP_STATUS_BIT 8 // Input bit number to check clamp

#define X 0
#define Y 1
#define Z 2

#define CNTS_MM 300.0 // axis resolutions

float ToolPositionX[5] = {3005.0*CNTS_MM, 3005.0*CNTS_MM, 3005.0*CNTS_MM, 3005.0*CNTS_MM, 3005.0*CNTS_MM};
float ToolPositionY[5] = {   0.0*CNTS_MM,  100.0*CNTS_MM,  100.0*CNTS_MM,  100.0*CNTS_MM,  100.0*CNTS_MM};

#define ProbeX (1000.0*CNTS_MM)
#define ProbeY (1000.0*CNTS_MM)



int *ChangerState 	= &persist.UserData[TOOL_STATE_VAR];
int *Tool     		= &persist.UserData[TOOL_VAR];
int *LastTool 		= &persist.UserData[LAST_TOOL_VAR];

double ToolTime;

// Services Tool Change Sequence
void ServiceToolChange(void)
{
	switch (*ChangerState)
	{
	case T_IDLE:
		{
			break;
		}
	case T_START:
		{
			if (*LastTool==0)
			{
				printf("Error Last Tool Never defined\n");
				*ChangerState = T_IDLE;  // go idle
			}
			else if (*LastTool > 5 || *LastTool < 1)
			{
				printf("Invalid LastTool Number %d\n",*Tool);
				*ChangerState = T_IDLE;  // go idle
			}
			else if (*Tool > 5 || *Tool < 1)
			{
				printf("Invalid Tool Number %d\n",*Tool);
				*ChangerState = T_IDLE;  // go idle
			}
			else
			{
				printf("Tool Change Z Up\n");
				Move(Z,10000);
				*ChangerState = T_WAIT_Z_UP;
			}
			break;
		}

	case T_WAIT_Z_UP:
		{
			if (CheckDone(Z))
			{
				printf("Tool Change Move XY to Unload\n");
				Move(X,ToolPositionX[*LastTool]);
				Move(Y,ToolPositionY[*LastTool]);
				*ChangerState = T_WAIT_MOVE_XY_UNLOAD;
			}
			break;
		}

	case T_WAIT_MOVE_XY_UNLOAD:
		{
			if (CheckDone(X) && CheckDone(Y))
			{
				printf("Tool Change Z Down\n");
				Move(Z,10000);  // start Z moving down fast
				*ChangerState = T_WAIT_MOVE_Z_DOWN_FAST;
			}
			break;
		}

	case T_WAIT_MOVE_Z_DOWN_FAST:
		{
			if (chan[Z].Dest < 1000.0)
			{
				MoveAtVel(Z,10000,100.0);  // slow down Z
				*ChangerState = T_WAIT_MOVE_Z_DOWN_SLOW;
			}
			break;
		}

	case T_WAIT_MOVE_Z_DOWN_SLOW:
		{
			if (chan[Z].Dest < 500.0)
			{
				MoveAtVel(Z,10000,50.0);  // slow down Z more
				*ChangerState = T_WAIT_MOVE_Z_DOWN_VERYSLOW;
			}
			break;
		}

	case T_WAIT_MOVE_Z_DOWN_VERYSLOW:
		{
			if (CheckDone(Z))
			{
				printf("Tool Change Unclamp\n");
				SetBit(TOOL_CLAMP_BIT);
				*ChangerState = T_WAIT_OPENCLAMP;
			}
			break;
		}


	case T_WAIT_OPENCLAMP:
		{
			if (ReadBit(TOOL_CLAMP_STATUS_BIT))
			{
				printf("Tool Change Z Up #2\n");
				Move(Z,10000);
				*ChangerState = T_WAIT_Z_UP2;
			}
			break;
		}

	case T_WAIT_Z_UP2:
		{
			if (CheckDone(Z))
			{
				printf("Tool Change Move XY to Load\n");
				Move(X,ToolPositionX[*Tool]);
				Move(Y,ToolPositionY[*Tool]);
				*ChangerState = T_WAIT_MOVE_XY_LOAD;
			}
			break;
		}

	case T_WAIT_MOVE_XY_LOAD:
		{
			if (CheckDone(X) && CheckDone(Y))
			{
				printf("Tool Change Z Down\n");
				Move(Z,10000);  // start Z moving down fast
				*ChangerState = T_WAIT_MOVE_Z_DOWN_FAST2;
			}
			break;
		}

	case T_WAIT_MOVE_Z_DOWN_FAST2:
		{
			if (chan[Z].Dest < 1000.0)
			{
				MoveAtVel(Z,10000,100.0);  // slow down Z
				*ChangerState = T_WAIT_MOVE_Z_DOWN_SLOW2;
			}
			break;
		}

	case T_WAIT_MOVE_Z_DOWN_SLOW2:
		{
			if (chan[Z].Dest < 500.0)
			{
				MoveAtVel(Z,10000,50.0);  // slow down Z more
				*ChangerState = T_WAIT_MOVE_Z_DOWN_VERYSLOW2;
			}
			break;
		}

	case T_WAIT_MOVE_Z_DOWN_VERYSLOW2:
		{
			if (CheckDone(Z))
			{
				SetBit(TOOL_CLAMP_BIT);
				*ChangerState = T_WAIT_CLOSECLAMP;
			}
			break;
		}

	case T_WAIT_CLOSECLAMP:
		{
			if (!ReadBit(TOOL_CLAMP_STATUS_BIT))
			{
				printf("Tool Change Z Up #2\n");
				Move(Z,10000);
				*ChangerState = T_WAIT_Z_UP3;
			}
			break;
		}

	case T_WAIT_Z_UP3:
		{
			if (CheckDone(Z))
			{
				printf("Tool Change Move XY to Probe\n");
				Move(X,ProbeX);
				Move(Y,ProbeY);
				*ChangerState = T_WAIT_MOVE_XY_PROBE;
			}
			break;
		}

	case T_WAIT_MOVE_XY_PROBE:
		{
			if (CheckDone(X) && CheckDone(Y))
			{
				printf("Tool Change Complete\n");
				*LastTool = *Tool;  // remember where we are
				*ChangerState = T_IDLE;
			}
			break;
		}
	}
}

// E X T E R N A L   T O O L   B U T T O N   M A N A G E M E N T 
//
// define states that the tool button service may be in
enum {T_IDLE,T_ONE_BUTTON_PUSHED};

double ToolButtonTime;
int ButtonState = T_IDLE;
int LastButton;

#define FIRST_TOOL_BUTTON 36
#define NUM_TOOL_BUTTONS 8

#define LONGPUSH 4.0  	// Push button this long to force tool number
#define SHORTPUSH 1.0  	// Push button this long to do tool change

// returns button number 0-N if one single button pushed
// otherwise returns -1 if no buttons pushed
// or -2 if more than one button pushed

int CheckForOneButtonPushed(void)
{
	int i,b,n=0;

	for (i=0; i<NUM_TOOL_BUTTONS; i++)
	{
		if (ReadBit(FIRST_TOOL_BUTTON+i))
		{
			b=i; // rememeber last one
			n++; // remember how many
		}
	}
	if (n==1) return b; 	// one pushed, return which one
	if (n==0) return -1; 	// none pushed, return -1
	return -2; 				// more than one, return -2
}

// Services Some Operator buttons to perform two functions
// A short duration push of a single button will cause a tool change
// A long duration push of a single button will set the current turret
// position as that tool number
void ServiceToolButtons(void)
{
	int NewButton=CheckForOneButtonPushed();
	double T=Time_sec();

	switch (ButtonState)
	{
	case T_IDLE:
		{
			// Single button pushed?
			if (NewButton >= 0)
			{
				LastButton=NewButton;
				ToolButtonTime=T;
				ButtonState = T_ONE_BUTTON_PUSHED;
			}
			break;
		}
	case T_ONE_BUTTON_PUSHED:
		{
			// same one still pushed?
			if (NewButton != LastButton)
			{
				// no, check for more than one pushed
				if (NewButton==-2)
				{
					// more than one, do nothing and start over
				}
				// check how long it had been pushed
				else if (T > ToolButtonTime + LONGPUSH)
				{
					*LastTool = LastButton+1;  // set tool number
					printf("Turret Set as Tool %d\n",*LastTool);
				}
				else if (T > ToolButtonTime + SHORTPUSH)
				{
					// short push, initiate a ToolChange if
					// Tool Changer is idle
					if (*ChangerState == T_IDLE)
					{
						*Tool = LastButton+1;  // set new tool to load
						*ChangerState = T_START;  // Go!
						printf("Tool %d Change Initiated\n",*Tool);
					}					
				}
				ButtonState = T_IDLE;  // go back to idle state
			}
			break;
		}
	}
}

// Loop servicing things
main()
{
	for (;;)  // loop forever
	{
		WaitNextTimeSlice();
		ServiceToolChange();
		ServiceToolButtons();
	}
}
