#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define Yaxis 1

main()
{
	int FixtureIndex;
	double NewOriginOffset,OriginOffsetY,AxisOffsetY;
	double Machinex,Machiney,Machinez,Machinea,Machineb,Machinec;

	GetFixtureIndex(&FixtureIndex);

	GetOriginOffset(&OriginOffsetY, FixtureIndex, Yaxis);

	GetAxisOffset(&AxisOffsetY, Yaxis);
	
	GetMachine(&Machinex,&Machiney,&Machinez,&Machinea,&Machineb,&Machinec);

	// Compute Origin Offset to make DRO zero
	//
	// Since Machine = DRO + OriginOffset + AxisOffset
	//
	// Set DRO = 0 and solve for OriginOffset
	//
	NewOriginOffset = Machiney -  AxisOffsetY;

	pD[TMP]=NewOriginOffset;
	SetVars(5201+FixtureIndex*20+1, 1, TMP);

	DoPC(PC_COMM_UPDATE_FIXTURE);
}
