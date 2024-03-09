#include "KMotionDef.h"

#define LAST_TOOL_VAR 7   	// Tool changer Last tool position is saved globally in this Var
int *LastTool 		= &persist.UserData[LAST_TOOL_VAR];

main()
{
	 *LastTool=1;
}
