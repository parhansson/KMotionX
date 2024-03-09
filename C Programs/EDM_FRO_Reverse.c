#include "KMotionDef.h"

#define M3_BIT 15
#define FORWARD_BIT 7
#define REVERSE_BIT 6

main()
{
	int Forward,Reverse,M3;
	for (;;)
	{
		WaitNextTimeSlice();
		
		M3=ReadBit(M3_BIT);
		Forward=!ReadBit(FORWARD_BIT);
		Reverse=!ReadBit(REVERSE_BIT);
		
		// M3 and Forward or Reverse?
		if (M3 && (Forward || Reverse))
		{
			if (Forward)
				SetFRO(1.0);  //Full speed ahead
			else
				SetFRO(-0.5); //Reduced speed reverse
		}
		else
		{
			SetFRO(0.0);  //Stop
		}
	}
}

