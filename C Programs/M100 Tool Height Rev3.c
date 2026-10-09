#include "KMotionDef.h"
#define TMP 10					// which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


#define AXISX 0
#define AXISY 1
#define AXISZ 2



// absolute position of the tool height setting plate
#define TOOL_HEIGHT_PLATE_X 700
#define TOOL_HEIGHT_PLATE_Y 0



//--------- Spindle IO bits
#define TOOL_SENSE 1041			// Tool Tip Sensor
//---------

#define TOOL_RETRACT_SPEED_Z 0.2	//speed in mm/second to move spindle up after tool has been ejected
#define TOOL_RETRACT_SPEED_Z1 0.1


#define SlowSpeed 60.0			//mm/sec

#define CNT_PER_MM_X 4000.0
#define CNT_PER_MM_Y 4000.0
#define CNT_PER_MM_Z 4000.0
int FixtureIndex, Units, TWORD, HWORD, DWORD;
double NewToolLength, OriginOffsetZ, AxisOffsetZ;
double Machinex, Machiney, Machinez, Machinea, Machineb, Machinec;


int main()
{
	MoveAtVel(AXISZ, 0, SlowSpeed * CNT_PER_MM_Z);	// Z up
	while (!CheckDone(AXISZ));	// wait till finished

	// Move XY to tool sensor
	MoveAtVel(AXISX, TOOL_HEIGHT_PLATE_X * CNT_PER_MM_X, SlowSpeed * CNT_PER_MM_X);
	MoveAtVel(AXISY, TOOL_HEIGHT_PLATE_Y * CNT_PER_MM_Y, SlowSpeed * CNT_PER_MM_Y);
	while (!CheckDone(AXISX) || !CheckDone(AXISY));	// wait till XY finished

	Jog(AXISZ, -TOOL_RETRACT_SPEED_Z * CNT_PER_MM_Z);	// probe
	while (!ReadBit(TOOL_SENSE));  // wait for probe to trigger
	Jog(AXISZ, 0);	// Stop
	Delay_sec(5);

	MoveRel(AXISZ, 3 * CNT_PER_MM_Z);	// move up 3mm
	Delay_sec(5);

	Jog(AXISZ, -TOOL_RETRACT_SPEED_Z1 * CNT_PER_MM_Z);	// probe again slower
	while (!ReadBit(TOOL_SENSE));  // wait for probe to trigger
	Jog(AXISZ, 0);	// Stop

	// Set Tool Table
	GetMiscSettings(&Units, &TWORD, &HWORD, &DWORD);
	GetMachine(&Machinex, &Machiney, &Machinez, &Machinea, &Machineb, &Machinec);
	NewToolLength = RoundToReasonable(Machinez, Units);
	SetToolLength(TWORD, NewToolLength);

	MoveAtVel(AXISZ, 0, SlowSpeed * CNT_PER_MM_Z);	// Z up
	while (!CheckDone(AXISZ));	// wait till finished
}
