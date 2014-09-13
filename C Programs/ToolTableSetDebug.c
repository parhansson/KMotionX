#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define Zaxis 2


main()
{
	int FixtureIndex,Units, TWORD, HWORD, DWORD;
	double NewToolLength,Length,OriginOffsetZ,AxisOffsetZ,XRes,YRes,ZRes;
	double Machinex,Machiney,Machinez,Machinea,Machineb,Machinec;
	double DROx,DROy,DROz,DROa,DROb,DROc;

	DoPCInt(PC_COMM_GETAXISRES, TMP);
	XRes = *(float *)&persist.UserData[TMP];
	YRes = *(float *)&persist.UserData[TMP+1];
	ZRes = *(float *)&persist.UserData[TMP+2];
	printf("XRes=%f\n", XRes);
	printf("YRes=%f\n", YRes);
	printf("ZRes=%f\n", ZRes);
	
	GetMiscSettings(&Units, &TWORD, &HWORD, &DWORD);
	printf("Units=%d T=%d  H=%d  D=%d\n",Units, TWORD, HWORD, DWORD);

	// Request Current Tool Length
	GetToolLength(TWORD,&Length);
	printf("Current Tool Length is %f\n",Length);

	GetFixtureIndex(&FixtureIndex);
	printf("Fixture Index = %d\n",FixtureIndex);

	GetOriginOffset(&OriginOffsetZ, FixtureIndex, Zaxis);
	printf("Origin Offset Z = %f\n",OriginOffsetZ);

	GetAxisOffset(&AxisOffsetZ, Zaxis);
	printf("Axis Offset Z = %f\n",AxisOffsetZ);
	
	GetMachine(&Machinex,&Machiney,&Machinez,&Machinea,&Machineb,&Machinec);
	printf("Machine Coordinates %f %f %f %f %f %f\n",Machinex,Machiney,Machinez,Machinea,Machineb,Machinec);

	GetDROs(&DROx,&DROy,&DROz,&DROa,&DROb,&DROc);
	printf("DROs %f %f %f %f %f %f\n",DROx,DROy,DROz,DROa,DROb,DROc);

	// Compute Tool Offset to make DRO zero when Tool Length selected and enabled
	//
	// Since Machine = DRO + OriginOffset + AxisOffset + ToolOffset
	//
	// Set DRO = 0 and solve for ToolOffset
	//
	NewToolLength = Machinez - OriginOffsetZ - AxisOffsetZ;

	// Change Currently Selected Tool Length
	SetToolLength(TWORD,NewToolLength);
	printf("Tool #%d Length Set to %f\n",TWORD,NewToolLength);
}
