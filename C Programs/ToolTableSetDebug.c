#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define Zaxis 2


main()
{
	int FixtureIndex,Units, TWORD, HWORD, DWORD;
	double NewToolLength,Length,OriginOffsetZ,AxisOffsetZ,XRes,YRes,ZRes;
	double NewToolDiameter,ToolDiameter,NewToolOffsetX,ToolOffsetX,NewToolOffsetY,ToolOffsetY;
	double Machinex,Machiney,Machinez,Machinea,Machineb,Machinec;
	double DROx,DROy,DROz,DROa,DROb,DROc;

	DoPCInt(PC_COMM_GETAXISRES, TMP);
	XRes = *(float *)&persist.UserData[TMP];
	YRes = *(float *)&persist.UserData[TMP+1];
	ZRes = *(float *)&persist.UserData[TMP+2];
	printf("XRes=%g\n", XRes);
	printf("YRes=%g\n", YRes);
	printf("ZRes=%g\n", ZRes);
	
	GetMiscSettings(&Units, &TWORD, &HWORD, &DWORD);
	printf("Units=%d T=%d  H=%d  D=%d\n",Units, TWORD, HWORD, DWORD);

	// Request Current Tool Length
	GetToolLength(TWORD,&Length);
	printf("Current Tool Length is %g\n",Length);

	GetFixtureIndex(&FixtureIndex);
	printf("Fixture Index = %d\n",FixtureIndex);

	GetOriginOffset(&OriginOffsetZ, FixtureIndex, Zaxis);
	printf("Origin Offset Z = %g\n",OriginOffsetZ);

	GetAxisOffset(&AxisOffsetZ, Zaxis);
	printf("Axis Offset Z = %g\n",AxisOffsetZ);
	
	GetMachine(&Machinex,&Machiney,&Machinez,&Machinea,&Machineb,&Machinec);
	printf("Machine Coordinates %.17g %.17g %.17g %.17g %.17g %.17g\n",Machinex,Machiney,Machinez,Machinea,Machineb,Machinec);

	GetDROs(&DROx,&DROy,&DROz,&DROa,&DROb,&DROc);
	printf("DROs %.17g %.17g %.17g %.17g %.17g %.17g\n",DROx,DROy,DROz,DROa,DROb,DROc);

	// Compute Tool Offset to make DRO zero when Tool Length selected and enabled
	//
	// Since Machine = DRO + OriginOffset + AxisOffset + ToolOffset
	//
	// Set DRO = 0 and solve for ToolOffset
	//
	NewToolLength = RoundToReasonable(Machinez - OriginOffsetZ - AxisOffsetZ,Units);

	// Change Currently Selected Tool Length
	SetToolLength(TWORD,NewToolLength);
	printf("Tool #%d Length Set to %.17g\n",TWORD,NewToolLength);

	GetToolDiameter(TWORD,&ToolDiameter);
	GetToolOffsetX(TWORD,&ToolOffsetX);
	GetToolOffsetY(TWORD,&ToolOffsetY);
	printf("Original Tool #%d Diameter=%.17g OffsetX=%.17g OffsetY=%.17g\n",
		TWORD,ToolDiameter,ToolOffsetX,ToolOffsetY);

	SetToolDiameter(TWORD,0.1);
	SetToolOffsetX(TWORD,0.2);
	SetToolOffsetY(TWORD,0.3);

	GetToolDiameter(TWORD,&NewToolDiameter);
	GetToolOffsetX(TWORD,&NewToolOffsetX);
	GetToolOffsetY(TWORD,&NewToolOffsetY);
	printf("New Tool #%d Diameter=%.17g OffsetX=%.17g OffsetY=%.17g\n",
		TWORD,NewToolDiameter,NewToolOffsetX,NewToolOffsetY);

}
