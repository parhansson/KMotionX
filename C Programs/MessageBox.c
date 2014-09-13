#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

main()
{
	int Answer;

	Answer = MsgBox("Hello World",MB_YESNO|MB_ICONEXCLAMATION);
	if (Answer == IDYES)
		printf("Answer is Yes\n");
	else
		printf("Answer is No\n");
	
}
