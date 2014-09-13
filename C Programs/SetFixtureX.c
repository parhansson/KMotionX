#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define Xaxis 0

main()
{
	int FixtureIndex;
	double NewOriginOffset,OriginOffsetX,AxisOffsetX;
	double Machinex,Machiney,Machinez,Machinea,Machineb,Machinec;

	GetFixtureIndex(&FixtureIndex);

	GetOriginOffset(&OriginOffsetX, FixtureIndex, Xaxis);

	GetAxisOffset(&AxisOffsetX, Xaxis);
	
	GetMachine(&Machinex,&Machiney,&Machinez,&Machinea,&Machineb,&Machinec);

	// Compute Origin Offset to make DRO zero
	//
	// Since Machine = DRO + OriginOffset + AxisOffset
	//
	// Set DRO = 0 and solve for OriginOffset
	//
	NewOriginOffset = Machinex -  AxisOffsetX;

	pD[TMP]=NewOriginOffset;
	SetVars(5201+FixtureIndex*20+0, 1, TMP);

	DoPC(PC_COMM_UPDATE_FIXTURE);
}
