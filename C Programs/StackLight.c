#include "KMotionDef.h"

#define RED 46
#define YELLOW 47
#define GREEN 48

int main()
{
	for(;;) // forever loop
	{
	if (CS0_StoppingState != 0)  // Feed hold ?
	{
		SetBit(YELLOW);
		ClearBit(GREEN);
		ClearBit(RED);
	}
	else if (JOB_ACTIVE)  // Job Active ?
	{
		SetBit(GREEN);
		ClearBit(YELLOW);
		ClearBit(RED);
	}
	else // otherwise
	{
		SetBit(RED);
		ClearBit(YELLOW);
		ClearBit(GREEN);
	}
}
	

}
