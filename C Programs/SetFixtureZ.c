#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define Zaxis 2

main()
{
	int FixtureIndex;
	double NewOriginOffset,OriginOffsetZ;
	double DROx, DROy, DROz, DROa, DROb, DROc;

	GetFixtureIndex(&FixtureIndex);

	GetOriginOffset(&OriginOffsetZ, FixtureIndex, Zaxis);

	GetDROs(&DROx, &DROy, &DROz, &DROa, &DROb, &DROc);
	
	// Adjust Origin Offset to make DRO zero
	NewOriginOffset = OriginOffsetZ + DROz;

	pD[TMP]=NewOriginOffset;
	SetVars(5201+FixtureIndex*20+Zaxis, 1, TMP);

	DoPC(PC_COMM_UPDATE_FIXTURE);
}
