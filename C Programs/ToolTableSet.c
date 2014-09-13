#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define Zaxis 2

main()
{
	int FixtureIndex,Units, TWORD, HWORD, DWORD;
	double NewToolLength,OriginOffsetZ,AxisOffsetZ;
	double Machinex,Machiney,Machinez,Machinea,Machineb,Machinec;

	GetMiscSettings(&Units, &TWORD, &HWORD, &DWORD);

	GetFixtureIndex(&FixtureIndex);

	GetOriginOffset(&OriginOffsetZ, FixtureIndex, Zaxis);

	GetAxisOffset(&AxisOffsetZ, Zaxis);
	
	GetMachine(&Machinex,&Machiney,&Machinez,&Machinea,&Machineb,&Machinec);

	// Compute Tool Offset to make DRO zero when Tool Length selected and enabled
	//
	// Since Machine = DRO + OriginOffset + AxisOffset + ToolOffset
	//
	// Set DRO = 0 and solve for ToolOffset
	//
	NewToolLength = Machinez - OriginOffsetZ - AxisOffsetZ;

	// Change Currently Selected Tool Length
	SetToolLength(TWORD,NewToolLength);
}
