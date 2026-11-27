#include "KMotionDef.h"

#define BB 13
int main()
{
	int i=0;
	SetBitDirection(BB,1);
	for(;;)
	{
		SetStateBit(BB,i);
		i=1-i;
		Delay_sec(0.1);
	}
}
