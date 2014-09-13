#include "KMotionDef.h"

// Assumes a Tool Changer State Machine is running in a forever loop
// (which is necessary to allow external buttons to change tools)

#define TOOL_STATE_VAR 8  	// Tool changer state is saved globally in this Var
#define TOOL_VAR 9        	// Tool changer desired new tool Var
// define state that the tool changer may be in
enum {T_IDLE,T_START,T_WAIT_UNCLAMP,T_WAIT_MOVE,T_WAIT_CLAMP};

main()
{
	int *ChangerState    = &persist.UserData[TOOL_STATE_VAR];
	int Tool = persist.UserData[TOOL_VAR];  // value stored is an integer 

	if (*ChangerState == T_IDLE)
	{
		*ChangerState = T_START;  // Go!
		printf("Tool %d Change Initiated\n",Tool);

		// wait till finished
		while (*ChangerState != T_IDLE)
			WaitNextTimeSlice();
	}
	else
	{
		printf("Error Tool Changer not idle");
	}					
}
