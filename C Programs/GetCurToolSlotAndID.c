#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

int main()
{
	int Slot, ID;
		
	GetToolSlotAndID(&Slot, &ID);
	printf("Slot=%d, ID=%d\n",Slot,ID);
}
