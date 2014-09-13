#include "KMotionDef.h"

void main() 
{
	int i;
	for (i=0; i<10; i++)
	{
		ClearBit(46);
		Delay_sec(0.1);
		SetBit(46);
		Delay_sec(0.1);
	}
}