#include "KMotionDef.h"

// Services Tool Changer using non-blocking State Machine approach
// The Service routines maintain their state so they can always return
// immediately and later resume from their previous state.

// T O O L   C H A N G E R   S E Q U E N C E
//
// define state that the tool changer may be in
enum {T_IDLE,T_START,T_WAIT_UNCLAMP,T_WAIT_MOVE,T_WAIT_CLAMP};

#define TOOL_VAR 9        	// Tool changer desired new tool Var
#define TOOL_STATE_VAR 8  	// Tool changer state is saved globally in this Var
#define LAST_TOOL_VAR 7   	// Tool changer Last tool position is saved globally in this Var
#define TOOL_CLAMP_BIT 20 	// IO bit number to clamp turret
#define TOOL_DIST 4250.0	// Steps/counts to move turret to next tool
#define CLAMP_TIME 1.5    	// seconds to wait for the clamp/unclamp
#define TURRET_AXIS 2     	// axis channel for the turret motor

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
				printf("Error Turret Position Never defined\n");
				*ChangerState = T_IDLE;  // go idle
			}
			else if (*Tool > 8 || *Tool < 1)
			{
				printf("Invalid Tool Number %d\n",*Tool);
				*ChangerState = T_IDLE;  // go idle
			}
			else
			{
				printf("Tool Change Unclamp\n");
				ClearBit(TOOL_CLAMP_BIT);  // Release Clamp
				ToolTime = Time_sec() + CLAMP_TIME;  // wait until this time
				*ChangerState = T_WAIT_UNCLAMP;
			}
			break;
		}
	case T_WAIT_UNCLAMP:
		{
			if (Time_sec() > ToolTime)
			{
				// time to move.  Compute distance

				int delta = *Tool - *LastTool;  // how far to move 
				if (delta>4)  delta-=8;		// optimize direction
				if (delta<-4) delta+=8;

				printf("Turret Moved From %d to %d, delta = %d\n", *LastTool,*Tool,delta);
				*LastTool = 0;  // set as undefined while moving
				MoveRel(TURRET_AXIS,delta*TOOL_DIST);

				*ChangerState = T_WAIT_MOVE;
			}
			break;
		}
	case T_WAIT_MOVE:
		{
			// check if disabled
			if (!chan[TURRET_AXIS].Enable)
			{
				*ChangerState = T_IDLE;
			}
			else if (CheckDone(TURRET_AXIS))
			{
				printf("Tool Change Clamp\n");
				SetBit(TOOL_CLAMP_BIT);  // Clamp
				ToolTime = Time_sec() + CLAMP_TIME;  // wait until this time
				*ChangerState = T_WAIT_CLAMP;
			}
			break;
		}
	case T_WAIT_CLAMP:
		{
			// if clamped go idle
			if (Time_sec() > ToolTime)
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
