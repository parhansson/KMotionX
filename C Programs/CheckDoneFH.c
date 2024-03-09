#include "KMotionDef.h"

int CheckDoneFH(int ch)
{
	return CheckDone(ch) && CS0_StoppingState==0;
}

main()
{
	DefineCoordSystem(0,1,2,-1);
	MoveRel(0,300000);
	MoveRel(1,300000);
	while (!CheckDoneFH(0) && !CheckDoneFH(1)) ;
	printf("Done\n");
	DefineCoordSystem(0,1,2,-1);
}


