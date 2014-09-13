#include "KMotionDef.h"

main()
{
	for(;;)
	{
		Move(4,8000);
		while (!CheckDone(4));
		Delay_sec(0.2);
		
		Move(4,16000);
		while (!CheckDone(4));
		Delay_sec(0.2);
		
		Move(4,24000);
		while (!CheckDone(4));
		Delay_sec(0.2);
		
		Move(4,32000);
		while (!CheckDone(4));
		Delay_sec(0.2);
		
		Move(4,40000);
		while (!CheckDone(4));
		Delay_sec(0.2);
		
		Move(4,00000);
		while (!CheckDone(4));
	}
}
