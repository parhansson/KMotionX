#include "KMotionDef.h"

int CheckDoneFH(int ch)
{
	return CheckDone(ch) && CS0_StoppingState==0;
}

main()
{
	DefineCoordSystem(-1,-1,-1,-1);
	MoveRel(0,300000);
	while (!CheckDoneFH(0)) ;
	printf("Done\n");
	DefineCoordSystem(0,1,2,-1);
}


