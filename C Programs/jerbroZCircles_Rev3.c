#include "KMotionDef.h"
#define TMP 10					// which spare persist to use to transfer data
#include "..\KflopToKMotionCNCFunctions.c"

// Special thanks to Jeremy Brown for sharing

// Probe Example to find top, left, bottom of part

void MoveNorth(double distance);
void MoveSouth(double distance);
void MoveWest(double distance);
void MoveEast(double distance);
void MoveUp(double distance);
void MoveDown(double distance);

void MoveXToDest(double destination);
void MoveYToDest(double destination);

void ProbeDown();
void ProbeNorth();
void ProbeSouth();
void ProbeWest();
void ProbeEast();

// Globals
float xRes, yRes, zRes;

#define PROBEBIT 171			// #110
#define PROBESPEED 500.0f		// #111
#define XAXIS 0					// #112
#define YAXIS 1					// #113
#define ZAXIS 2					// #114
#define UPSPEED 2000.0f			// #115
#define DOWNSPEED (PROBESPEED * -1.0f)	// #116
#define WESTSPEED -1000.0f		// #117
#define EASTSPEED 1000.0f		// #118
#define NORTHSPEED 1000.0f		// #119
#define SOUTHSPEED -1000.0f		// #120
#define PLATETHICKNESS 0.621f	// #121
#define SAFEZ 3.0f				// #122
#define PLATEDEPTH 3.0f			// #123
#define PLATEWIDTH 3.0f			// #124
#define BUFFERDISTANCE 0.1f		// #125
#define PROBEDEPTH 0.6f			// #126
#define RIGHTCORNEROFFSET 0.5f	// #127
#define MAXIMUMTOOLDIAMETER 0.5f	// #128
#define LEFTCORNEROFFSET 0.75f	// #129
#define PROBE_ACTIVE 1  // triggered state = 0 or 1


void main()
{
	printf("\n\nStarting main\n");

	double nTouch, sTouch, wTouch, eTouch;
	double nHole, sHole, wHole, eHole;
	double approxXAxisHoleZero, approxYAxisHoleZero;
	double toolWidth, toolDepth;

	printf("Getting Axis Resolution\n");
	DoPCInt(PC_COMM_GETAXISRES, TMP);

	xRes = *(float *)&persist.UserData[TMP];
	yRes = *(float *)&persist.UserData[TMP+1];
	zRes = *(float *)&persist.UserData[TMP+2];

	printf("Axis Resolutions Found:\n\tx: %f\n\ty: %f\n\tz: %f\n", xRes, yRes, zRes);

	// Find Z Height
	ProbeDown();
	printf("Setting Z DRO to plate thickness: %f\n", PLATETHICKNESS);
	DoPCFloat(PC_COMM_SET_Z, PLATETHICKNESS);

	// Find N side of plate
	MoveUp(BUFFERDISTANCE);
	MoveNorth(PLATEDEPTH - LEFTCORNEROFFSET + BUFFERDISTANCE);
	MoveDown(PROBEDEPTH);
	ProbeSouth();

	printf("Setting nTouch to ch1->Dest: %f\n", chan[YAXIS].Dest);
	nTouch = chan[YAXIS].Dest;

	// Find E side of plate
	MoveNorth(BUFFERDISTANCE);
	MoveUp(PROBEDEPTH);
	MoveSouth(BUFFERDISTANCE + RIGHTCORNEROFFSET);
	MoveEast(PLATEWIDTH - LEFTCORNEROFFSET + BUFFERDISTANCE);
	MoveDown(PROBEDEPTH);
	ProbeWest();

	printf("Setting eTouch to chan[XAXIS].Dest: %f\n", chan[XAXIS].Dest);
	eTouch = chan[XAXIS].Dest;

	// Find W side of plate
	MoveEast(BUFFERDISTANCE);
	MoveUp(PROBEDEPTH);
	MoveWest(BUFFERDISTANCE + PLATEWIDTH + BUFFERDISTANCE + MAXIMUMTOOLDIAMETER);
	MoveDown(PROBEDEPTH);
	ProbeEast();

	printf("Setting wTouch to chan[XAXIS].Dest: %f\n", chan[XAXIS].Dest);
	wTouch = chan[XAXIS].Dest;

	// Calculate Tool Width
	toolWidth = (eTouch - wTouch) / xRes - PLATEWIDTH;
	printf("toolWidth calculated as %f\n", toolWidth);

	// Find S side of plate
	MoveWest(BUFFERDISTANCE);
	MoveUp(PROBEDEPTH);
	MoveEast(BUFFERDISTANCE + PLATEWIDTH - RIGHTCORNEROFFSET + (toolWidth / 2.0));
	MoveSouth(PLATEDEPTH + MAXIMUMTOOLDIAMETER);
	MoveDown(PROBEDEPTH);
	ProbeNorth();

	printf("Setting sTouch to chan[YAXIS].Dest: %f\n", chan[YAXIS].Dest);
	sTouch = chan[YAXIS].Dest;

	toolDepth = (nTouch - sTouch) / yRes - PLATEDEPTH;
	printf("toolDepth calculated as %f\n", toolDepth);

	// Move to Hole Center
	MoveSouth(BUFFERDISTANCE);
	MoveUp(PROBEDEPTH);
	MoveNorth(BUFFERDISTANCE + LEFTCORNEROFFSET + (toolDepth / 2.0));
	MoveWest(PLATEWIDTH - RIGHTCORNEROFFSET - LEFTCORNEROFFSET);
	MoveDown(PROBEDEPTH);

	printf("Setting approxXAxisHoleZero to %f\n", chan[XAXIS].Dest);
	approxXAxisHoleZero = chan[XAXIS].Dest;
	printf("Setting approxYAxisHoleZero to %f\n", chan[YAXIS].Dest);
	approxYAxisHoleZero = chan[YAXIS].Dest;

	// Find Exact Hole Center
	ProbeWest();
	wHole = chan[XAXIS].Dest;
	MoveXToDest(approxXAxisHoleZero);
	ProbeEast();
	eHole = chan[XAXIS].Dest;
	MoveWest(((eHole - wHole) / 2.0) / xRes);

	printf("Setting X DRO to Zero\n");
	DoPCFloat(PC_COMM_SET_X, 0.0f);

	ProbeNorth();
	nHole = chan[YAXIS].Dest;
	MoveYToDest(approxYAxisHoleZero);
	ProbeSouth();
	sHole = chan[YAXIS].Dest;
	MoveNorth(((nHole - sHole) / 2.0) / yRes);

	printf("Setting Y DRO to Zero\n");
	DoPCFloat(PC_COMM_SET_Y, 0.0f);

	MoveUp(SAFEZ);

	printf("Done\n");
}

void MoveNorth(double distance)
{
	printf("Moving North by %f\n", distance);
	MoveRel(YAXIS, distance * yRes);
	while (!CheckDone(YAXIS));
}

void MoveSouth(double distance)
{
	printf("Moving South by %f\n", distance);
	MoveRel(YAXIS, -1 * distance * yRes);
	while (!CheckDone(YAXIS));
}

void MoveWest(double distance)
{
	printf("Moving West by %f\n", distance);
	MoveRel(XAXIS, -1 * distance * xRes);
	while (!CheckDone(XAXIS));
}

void MoveEast(double distance)
{
	printf("Moving East by %f\n", distance);
	MoveRel(XAXIS, distance * xRes);
	while (!CheckDone(XAXIS));
}

void MoveUp(double distance)
{
	printf("Moving Up by %f\n", distance);
	MoveRel(ZAXIS, distance * zRes);
	while (!CheckDone(ZAXIS));
}

void MoveDown(double distance)
{
	printf("Moving Down by %f\n", distance);
	MoveRel(ZAXIS, -1 * distance * zRes);
	while (!CheckDone(ZAXIS));
}

void MoveXToDest(double destination)
{
	printf("Moving XAXIS to %f\n", destination);
	Move(XAXIS, destination);
	while (!CheckDone(XAXIS));
}

void MoveYToDest(double destination)
{
	printf("Moving YAXIS to %f\n", destination);
	Move(YAXIS, destination);
	while (!CheckDone(YAXIS));
}

void ProbeDown()
{
	printf("Jogging Down at speed:%f\n", DOWNSPEED);
	Jog(ZAXIS, DOWNSPEED);
	printf("Waiting for probe to read high\n");
	while (ReadBit(PROBEBIT) != PROBE_ACTIVE);
	printf("Stopping\n");
	Jog(ZAXIS, 0);
	printf("Waiting for Z motion to stop\n");
	while (!CheckDone(ZAXIS));
}

void ProbeSouth()
{
	printf("Jogging South at speed:%f\n", SOUTHSPEED);
	Jog(YAXIS, SOUTHSPEED);
	printf("Waiting for probe to read high\n");
	while (ReadBit(PROBEBIT) != PROBE_ACTIVE);
	printf("Stopping\n");
	Jog(YAXIS, 0);
	printf("Waiting for Y motion to stop\n");
	while (!CheckDone(YAXIS));
}

void ProbeNorth()
{
	printf("Jogging North at speed:%f\n", NORTHSPEED);
	Jog(YAXIS, NORTHSPEED);
	printf("Waiting for probe to read high\n");
	while (ReadBit(PROBEBIT) != PROBE_ACTIVE);
	printf("Stopping\n");
	Jog(YAXIS, 0);
	printf("Waiting for Y motion to stop\n");
	while (!CheckDone(YAXIS));
}

void ProbeWest()
{
	printf("Jogging West at speed:%f\n", WESTSPEED);
	Jog(XAXIS, WESTSPEED);
	printf("Waiting for probe to read high\n");
	while (ReadBit(PROBEBIT) != PROBE_ACTIVE);
	printf("Stopping\n");
	Jog(XAXIS, 0);
	printf("Waiting for X motion to stop\n");
	while (!CheckDone(XAXIS));
}

void ProbeEast()
{
	printf("Jogging East at speed:%f\n", EASTSPEED);
	Jog(XAXIS, EASTSPEED);
	printf("Waiting for probe to read high\n");
	while (ReadBit(PROBEBIT) != PROBE_ACTIVE);
	printf("Stopping\n");
	Jog(XAXIS, 0);
	printf("Waiting for X motion to stop\n");
	while (!CheckDone(XAXIS));
}
