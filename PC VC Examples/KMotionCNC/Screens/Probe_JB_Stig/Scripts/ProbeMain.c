#include "KMotionDef.h"
#define TMP 10					// which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


// ProbeScreen created by Stig Østvang
// Special thanks to Jeremy Brown for the framework for probing
// and Tom Kerekes for the screen templates, cleaning up and help

int MoveDirection(int Direction, double Distance, int CheckProbe);
void Probe(int Direction);
void FindZ();
int FindOutCenterNS();
int FindOutCenterWE();
int FindInCenterNS();
int FindInCenterWE();
int FindOutTopLeft();
int FindOutTop();
int FindOutTopRight();
int FindOutLeft();
int FindOutRight();
int FindOutBottomLeft();
int FindOutBottom();
int FindOutBottomRight();
int GetStockSize();

// Globals
float xRes, yRes, zRes;
// float StockWidth, StockDepth;
float RESETDRO;
float FSpeed, SSpeed, MSpeed;
double STOCKWIDTH, STOCKDEPTH, ToUserUnits;
double nTouch, sTouch, wTouch, eTouch, tempX, tempY, Trig;
float CalcStockWidth, CalcStockDepth;
int UnitsType, TWORD, HWORD, DWORD;
int Direction, Axis;
char *Units;

#define PERSIST_VAR 0					// defines which button code was pushed    	
#define PROBEBIT 1032					// Bit for probe
#define PROBESPEED 20.0f				// Probespeed in in/min
#define XAXIS 0							// 
#define YAXIS 1							// 
#define ZAXIS 2							// 
#define MOVESPEED 80.0f					// Rapid speed in in/min
#define PROBETIPDIAMETER 0.125f 		// Diameter of probe tip
#define SAFEZ 1.0f						// 
#define SLOWSPEED 5.0f		    		// Reset speed in in/min
#define BUFFERDISTANCE 0.2f				// 
#define PROBEDEPTH 0.1f					// Depth below Z0 to probe
#define DISTANCETOMOVE 0.5f				// 
#define RESETDISTANCE 0.05f				// Distance to move to reset probe

#define STOCKWIDTHVAR 160				// Var for Stock Width
#define STOCKDEPTHVAR 161				// Var for Stock Depth
#define RESETDROVAR 1248				// Var for Reset DRO
#define PROBE_ACTIVE 0  				// triggered state = 0 or 1
#define ASK_PROBE_INSTALLED FALSE

enum {North, East, South, West, Down, Up};

// a "direction" is defined by which axis, which way, the axis resolution, and name 

typedef struct {
	int Axis, Dir;
	float *Res;
	char *Name;
} DIRECTION;

DIRECTION Directions[6]=   // define how to move in each direction
{YAXIS, 1, &xRes, "North",
 XAXIS, 1, &yRes, "East",
 YAXIS,-1, &yRes, "South",
 XAXIS,-1, &xRes, "West",
 ZAXIS,-1, &zRes, "Down",
 ZAXIS, 1, &zRes, "Up"};

// Button codes

enum {OUT_TOP_LEFT, OUT_TOP,    OUT_TOP_RIGHT,  // 0 1 2
      OUT_LEFT,     OUT_CENTER, OUT_RIGHT,		// 3 4 5
      OUT_BOT_LEFT, OUT_BOT,    OUT_BOT_RIGHT,	// 6 7 8
	  OUT_CENTER_HOR,	OUT_CENTER_VERT,		// 9 10
	  IN_TOP_LEFT,	IN_TOP,		IN_TOP_RIGHT,	// 11 12 13
	  IN_LEFT,		IN_CENTER,	IN_RIGHT,		// 14 15 16
	  IN_BOT_LEFT,	IN_BOT,		IN_BOT_RIGHT,	// 17 18 19
	  IN_CENTER_HOR,IN_CENTER_VERT,FIND_Z};		// 20 21 22
      
void main()
{
	printf("\n\nStarting main\n");
	
	int Button = *(float *)&persist.UserData[PERSIST_VAR]; // button code pushed

#if ASK_PROBE_INSTALLED
	int Answer = MsgBox("Is Probe installed in Spindle?",MB_YESNO|MB_ICONEXCLAMATION);
	if (Answer == IDNO)
	{
		printf("Probe is NOT installed\n");
		return;
	}
	else
		printf("Probe is installed\n");
#endif

	// Check if probe is already activated
	if (!ReadBit(PROBEBIT) !=PROBE_ACTIVE)
	{
		MsgBox("Probe is already activated!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	
	
	printf("Getting Axis Resolution\n");
	DoPCInt(PC_COMM_GETAXISRES, TMP);

	xRes = *(float *)&persist.UserData[TMP];
	yRes = *(float *)&persist.UserData[TMP+1];
	zRes = *(float *)&persist.UserData[TMP+2];

	printf("Axis Resolutions Found:\n\tx: %f\n\ty: %f\n\tz: %f\n", xRes, yRes, zRes);

	if (ReadBit(RESETDROVAR))
		printf("ResetDRO is Set\n");
	else
		printf("ResetDRO is NOT Set\n");
		
	// Selecting Probe Tool offset
	double Length;

    MDI("G43T99H99");  // select probe tool and enable G43
    
    GetMiscSettings(&UnitsType, &TWORD, &HWORD, &DWORD);
    GetToolLength(TWORD, &Length);
    printf("T = %d Length = %f\n",TWORD, Length);

		
	if (UnitsType == CANON_UNITS_INCHES)
	{
		Units = "Inches";
		ToUserUnits = 1.0;
	}
	else
	{
		Units = "mm";
		ToUserUnits = 25.4;
	}
	
	printf("Units are %s\n", Units);

	switch (Button)
	{
		case FIND_Z:
		{
			printf("FindZ\n");
			FindZ();
			break;
		}
		
		case OUT_TOP_LEFT:
		{
			printf("OutTopLeft\n");
			FindZ();
			if (FindOutTopLeft()) return;
			break;
		}
		
		case OUT_TOP:
		{
			printf("OutTop\n");
			FindZ();
			if (FindOutTop()) return;
			break;
		}
		
		case OUT_TOP_RIGHT:
		{
			printf("OutTopRight\n");
			FindZ();
			if (FindOutTopRight()) return;
			break;
		}
		
		case OUT_LEFT:
		{
			printf("OutLeft\n");
			FindZ();
			if (FindOutLeft()) return;
			break;
		}
		
		case OUT_CENTER:
		{
			printf("OutCenter\n");
			if (GetStockSize()) return;
			FindZ();
			MoveDirection(Up, BUFFERDISTANCE, 0);
			if (FindOutCenterNS()) return;
			if (FindOutCenterWE()) return;
			MoveDirection(Up, SAFEZ, 0);
			break;
		}
		
		case OUT_RIGHT:
		{
			printf("OutRight\n");
			FindZ();
			if (FindOutRight()) return;
			break;
		}
		
		case OUT_BOT_LEFT:
		{
			printf("OutBotLeft\n");
			FindZ();
			if (FindOutBottomLeft()) return;
			break;
		}
		
		case OUT_BOT:
		{
			printf("OutBot\n");
			FindZ();
			if (FindOutBottom()) return;
			break;
		}
		
		case OUT_BOT_RIGHT:
		{
			printf("OutBotRight\n");
			FindZ();
			if (FindOutBottomRight()) return;
			break;
		}
		
		case OUT_CENTER_HOR:
		{
			printf("OutCenterHor\n");
			if (GetStockSize()) return;
			FindZ();
			MoveDirection(Up, BUFFERDISTANCE, 0);
			if (FindOutCenterNS()) return;
			MoveDirection(Up, SAFEZ, 0);
			break;
		}
		
		case OUT_CENTER_VERT:
		{
			printf("OutCenterVert\n");
			if (GetStockSize()) return;
			FindZ();
			MoveDirection(Up, BUFFERDISTANCE, 0);
			if (FindOutCenterWE()) return;
			MoveDirection(Up, SAFEZ, 0);
			break;
		}
		
		case IN_TOP_LEFT:
		{
			printf("InTopLeft\n");
			Probe(North);						// Find N edge
			MoveDirection(South, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
			while (!CheckDone(YAXIS));
			if (MoveDirection(North, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{	
				printf("Setting Y DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_Y, 0.0f);
			}
			else
				tempY = chan[YAXIS].Dest;
			if (MoveDirection(South, DISTANCETOMOVE, 1)) return;
			if (MoveDirection(Down, SAFEZ, 1)) return;
			Probe(West);						// Find W edge
			MoveDirection(East, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
			while (!CheckDone(XAXIS));
			if (MoveDirection(West, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting X DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_X, 0.0f);
				MDI("G0X0Y0");
			}
			else
				Move(YAXIS, tempY);
			break;
		}
		
		case IN_TOP:
		{
			printf("InTop\n");
			Probe(North);						// Find N edge
			MoveDirection(South, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
			while (!CheckDone(YAXIS));
			if (MoveDirection(North, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting Y DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_Y, 0.0f);
			}
			break;
		}
		
		case IN_TOP_RIGHT:
		{
			printf("InTopRight\n");
			Probe(North);						// Find N edge
			MoveDirection(South, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
			while (!CheckDone(YAXIS));
			if (MoveDirection(North, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting Y DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_Y, 0.0f);
			}
			else
				tempY = chan[YAXIS].Dest;
			if (MoveDirection(South, DISTANCETOMOVE, 1)) return;
			if (MoveDirection(Down, SAFEZ, 1)) return;
			Probe(East);						// Find E edge
			MoveDirection(West, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
			while (!CheckDone(XAXIS));
			if (MoveDirection(East, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting X DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_X, 0.0f);
				MDI("G0X0Y0");
			}
			else
				Move(YAXIS, tempY);
			break;
		}
		
		case IN_LEFT:
		{
			printf("InLeft\n");
			Probe(West);						// Find W edge
			MoveDirection(East, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
			while (!CheckDone(XAXIS));
			if (MoveDirection(West, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting X DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_X, 0.0f);
			}
			break;
		}
		
		case IN_CENTER:
		{
			printf("InCenter\n");
			if (FindInCenterNS()) return;
			if (FindInCenterWE()) return;
			break;
		}
		
		case IN_RIGHT:
		{
			printf("InRight\n");
			Probe(East);						// Find E edge
			MoveDirection(West, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
			while (!CheckDone(XAXIS));
			if (MoveDirection(East, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting X DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_X, 0.0f);
			}
			break;
		}
		
		case IN_BOT_LEFT:
		{
			printf("InBotLeft\n");
			Probe(South);						// Find S edge
			MoveDirection(North, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
			while (!CheckDone(YAXIS));
			if (MoveDirection(South, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting Y DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_Y, 0.0f);
			}
			else
				tempY = chan[YAXIS].Dest;
			if (MoveDirection(North, DISTANCETOMOVE, 1)) return;
			if (MoveDirection(Down, SAFEZ, 1)) return;
			Probe(West);						// Find W edge
			MoveDirection(East, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
			while (!CheckDone(XAXIS));
			if (MoveDirection(West, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting X DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_X, 0.0f);
				MDI("G0X0Y0");
			}
			else
				Move(YAXIS, tempY);
			break;
		}
		
		case IN_BOT:
		{
			printf("InBot\n");
			Probe(South);						// Find S edge
			MoveDirection(North, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
			while (!CheckDone(YAXIS));
			if (MoveDirection(South, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting Y DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_Y, 0.0f);
			}
			break;
		}
		
		case IN_BOT_RIGHT:
		{
			printf("InBotRight\n");
			Probe(South);						// Find S edge
			MoveDirection(North, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
			while (!CheckDone(YAXIS));
			if (MoveDirection(South, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting Y DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_Y, 0.0f);
			}
			else
				tempY = chan[YAXIS].Dest;
			if (MoveDirection(North, DISTANCETOMOVE, 1)) return;
			if (MoveDirection(Down, SAFEZ, 1)) return;
			Probe(East);						// Find E edge
			MoveDirection(West, RESETDISTANCE, 0);
			MoveDirection(Up, SAFEZ, 0);
			MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
			while (!CheckDone(XAXIS));
			if (MoveDirection(East, PROBETIPDIAMETER/2, 1)) return;
			if (ReadBit(RESETDROVAR))
			{
				printf("Setting X DRO to Zero\n");
				DoPCFloat(PC_COMM_SET_X, 0.0f);
				MDI("G0X0Y0");
			}
			else
				Move(YAXIS, tempY);
			break;
		}
		
		case IN_CENTER_HOR:
		{
			printf("InCenterHor\n");
			if (FindInCenterNS()) return;
			break;
		}
		
		case IN_CENTER_VERT:
		{
			printf("InCenterVert\n");
			if (FindInCenterWE()) return;
			break;
		}
	}
	printf("Done\n");
}

int GetStockSize()
{
	// Read double from a KMotionCNC Edit Control
	// Persist Var identifies the Control and contents specifies 
	// where the string data should be placed in the 
	// Gather Buffer as an offset in words
	if (GetEditControlDouble(&STOCKWIDTH, STOCKWIDTHVAR, 1000))
	{
		printf("Error Reading Edit Control Stock Width\n");
		return 1;
	}
	printf("Stock Width: %f\n", STOCKWIDTH);
	STOCKWIDTH = STOCKWIDTH / ToUserUnits;
	
	// Read double from a KMotionCNC Edit Control
	// Persist Var identifies the Control and contents specifies 
	// where the string data should be placed in the 
	// Gather Buffer as an offset in words
	//if (GetEditControlDouble(&d2, STOCKDEPTHVAR, 1100))
	if (GetEditControlDouble(&STOCKDEPTH, STOCKDEPTHVAR, 1100))
	{
		printf("Error Reading Edit Control Stock Depth\n");
		return 1;
	}
	printf("Stock Depth: %f\n", STOCKDEPTH);
	STOCKDEPTH = STOCKDEPTH / ToUserUnits;
	return 0;
}

void FindZ()
{	// Find Z Height
	Probe(Down);						// Find top of stock
	MoveAtVel(ZAXIS, Trig, PROBESPEED * zRes / 60);
	while (!CheckDone(YAXIS)) ;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting Z DRO to 0\n");
		DoPCFloat(PC_COMM_SET_Z, 0.0f);
	}
}

int FindOutCenterNS()
{
	// Find N side of plate
	if (MoveDirection(North, STOCKDEPTH + DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(South);						// Find S edge
	MoveDirection(North, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
	while (!CheckDone(YAXIS));
		
	printf("Setting nTouch to ch1->Dest: %f\n", chan[YAXIS].Dest);
	nTouch = chan[YAXIS].Dest;

	// Find S side of plate
	if (MoveDirection(South, STOCKDEPTH + DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(North);						// Find N edge
	MoveDirection(South, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
	while (!CheckDone(YAXIS));
		
	printf("Setting sTouch to ch1->Dest: %f\n", chan[YAXIS].Dest);
	sTouch = chan[YAXIS].Dest;
	
	// Calculate Stock Depth
	CalcStockDepth = (nTouch - sTouch) / yRes - PROBETIPDIAMETER;
	printf("Stock Depth calculated as %f\n", CalcStockDepth * ToUserUnits);
	
	if (MoveDirection(North, CalcStockDepth/2 + PROBETIPDIAMETER/2, 1)) return 1;

	if (ReadBit(RESETDROVAR))
	{
		printf("Setting Y DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_Y, 0.0f);
	}
	return 0;
}
	
int FindOutCenterWE()
{
	// Find E side of plate
	if (MoveDirection(East, STOCKWIDTH + DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(West);						// Find W edge
	MoveDirection(East, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
	while (!CheckDone(XAXIS));
		
	printf("Setting eTouch to ch0->Dest: %f\n", chan[XAXIS].Dest);
	eTouch = chan[XAXIS].Dest;
	
	// Find W side of plate
	if (MoveDirection(West, STOCKWIDTH + DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(East);						// Find E edge
	MoveDirection(West, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
	while (!CheckDone(XAXIS));
		
	printf("Setting wTouch to ch0->Dest: %f\n", chan[XAXIS].Dest);
	wTouch = chan[XAXIS].Dest;

	// Calculate Stock Width
	CalcStockWidth = (eTouch - wTouch) / xRes - PROBETIPDIAMETER;
	printf("Stock Width calculated as %f\n", CalcStockWidth * ToUserUnits);
	
	if (MoveDirection(East, CalcStockWidth/2 + PROBETIPDIAMETER/2, 1)) return 1;
	
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting X DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_X, 0.0f);
	}
	return 0;
}

int FindInCenterNS()
{
	// Find N side of plate
	Probe(North);
			
	printf("Setting nTouch to Trig: %f\n", Trig);
	nTouch = Trig;
	
	MoveDirection(South, RESETDISTANCE, 0);

	// Find S side of plate
	Probe(South);
		
	printf("Setting sTouch to Trig: %f\n", Trig);
	sTouch = Trig;
	
	MoveDirection(North, RESETDISTANCE, 0);
	
	// Calculate Stock Depth
	CalcStockDepth = (nTouch - sTouch) / yRes + PROBETIPDIAMETER;
	printf("Stock Depth calculated as %f\n", CalcStockDepth * ToUserUnits);
	
	if (MoveDirection(North, (Trig - chan[YAXIS].Dest)/yRes + CalcStockDepth/2 - PROBETIPDIAMETER/2, 1)) return 1;

	if (ReadBit(RESETDROVAR))
	{
		printf("Setting Y DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_Y, 0.0f);
	}
	return 0;
}

int FindInCenterWE()
{
	// Find E side of plate
	Probe(East);
	
	printf("Setting nTouch to Trig: %f\n", Trig);
	eTouch = Trig;
	
	MoveDirection(West, RESETDISTANCE, 0);
	
	// Find W side of plate
	Probe(West);
	
	printf("Setting wTouch to Trig: %f\n", Trig);
	wTouch = Trig;

	MoveDirection(East, RESETDISTANCE, 0);

	// Calculate Stock Width
	CalcStockWidth = (eTouch - wTouch) / xRes + PROBETIPDIAMETER;
	printf("Stock Width calculated as %f\n", CalcStockWidth * ToUserUnits);
	
	if (MoveDirection(East, (Trig - chan[XAXIS].Dest)/yRes + CalcStockWidth/2 - PROBETIPDIAMETER/2, 1)) return 1;
	
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting X DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_X, 0.0f);
	}
	return 0;
}

int FindOutTopLeft()
{
	MoveDirection(Up, BUFFERDISTANCE, 0);
	// Find N side of plate
	if (MoveDirection(North, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(South);						// Find S edge
	MoveDirection(North, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
	while (!CheckDone(YAXIS));
	if (MoveDirection(South, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting Y DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_Y, 0.0f);
	}
	else
		tempY = chan[YAXIS].Dest;
	// Find W side of plate
	if (MoveDirection(South, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(West, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(East);						// Find E edge
	MoveDirection(West, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
	while (!CheckDone(XAXIS));
	if (MoveDirection(East, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting X DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_X, 0.0f);
		MDI("G0X0Y0");
	}
	else
		Move(YAXIS, tempY);
	MoveDirection(Up, SAFEZ, 0);
	return 0;
}

int FindOutTop()
{
	MoveDirection(Up, BUFFERDISTANCE, 0);
	// Find N side of plate
	if (MoveDirection(North, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(South);						// Find S edge
	MoveDirection(North, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
	while (!CheckDone(YAXIS));
	if (MoveDirection(South, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting Y DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_Y, 0.0f);
	}
	MoveDirection(Up, SAFEZ, 0);
	return 0;
}

int FindOutTopRight()
{
	MoveDirection(Up, BUFFERDISTANCE, 0);
	// Find N side of plate
	if (MoveDirection(North, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(South);						// Find S edge
	MoveDirection(North, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
	while (!CheckDone(YAXIS));
	if (MoveDirection(South, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting Y DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_Y, 0.0f);
	}
	else
		tempY = chan[YAXIS].Dest;
	// Find E side of plate
	if (MoveDirection(South, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(East, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(West);						// Find W edge
	MoveDirection(East, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
	while (!CheckDone(XAXIS));
	if (MoveDirection(West, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting X DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_X, 0.0f);
		MDI("G0X0Y0");
	}
	else
		Move(YAXIS, tempY);
	MoveDirection(Up, SAFEZ, 0);
	return 0;
}

int FindOutLeft()
{
	MoveDirection(Up, BUFFERDISTANCE, 0);
	// Find W side of plate
	if (MoveDirection(West, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(East);						// Find E edge
	MoveDirection(West, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
	while (!CheckDone(XAXIS));
	if (MoveDirection(East, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting X DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_X, 0.0f);
	}
	MoveDirection(Up, SAFEZ, 0);
	return 0;
}

int FindOutRight()
{
	MoveDirection(Up, BUFFERDISTANCE, 0);
	// Find E side of plate
	if (MoveDirection(East, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(West);						// Find W edge
	MoveDirection(East, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
	while (!CheckDone(XAXIS));
	if (MoveDirection(West, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting X DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_X, 0.0f);
	}
	MoveDirection(Up, SAFEZ, 0);
	return 0;
}

int FindOutBottomLeft()
{
	MoveDirection(Up, BUFFERDISTANCE, 0);
	// Find S side of plate
	if (MoveDirection(South, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(North);						// Find N edge
	MoveDirection(South, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
	while (!CheckDone(YAXIS));
	if (MoveDirection(North, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting Y DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_Y, 0.0f);
	}
	else
		tempY = chan[YAXIS].Dest;
	// Find W side of plate
	if (MoveDirection(North, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(West, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(East);						// Find E edge
	MoveDirection(West, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
	while (!CheckDone(XAXIS));
	if (MoveDirection(East, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting X DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_X, 0.0f);
		MDI("G0X0Y0");
	}
	else
		Move(YAXIS, tempY);
	MoveDirection(Up, SAFEZ, 0);
	return 0;
}

int FindOutBottom()
{
	MoveDirection(Up, BUFFERDISTANCE, 0);
	// Find S side of plate
	if (MoveDirection(South, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(North);						// Find N edge
	MoveDirection(South, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
	while (!CheckDone(YAXIS));
	if (MoveDirection(North, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting Y DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_Y, 0.0f);
	}
	MoveDirection(Up, SAFEZ, 0);
	return 0;
}

int FindOutBottomRight()
{
	MoveDirection(Up, BUFFERDISTANCE, 0);
	// Find S side of plate
	if (MoveDirection(South, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(North);						// Find N edge
	MoveDirection(South, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(YAXIS, Trig, PROBESPEED * yRes / 60);
	while (!CheckDone(YAXIS));
	if (MoveDirection(North, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting Y DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_Y, 0.0f);
	}
	else
		tempY = chan[YAXIS].Dest;
	// Find E side of plate
	if (MoveDirection(North, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(East, DISTANCETOMOVE, 1)) return 1;
	if (MoveDirection(Down, BUFFERDISTANCE + PROBEDEPTH, 1)) return 1;
	Probe(West);						// Find W edge
	MoveDirection(East, RESETDISTANCE, 0);
	MoveDirection(Up, BUFFERDISTANCE + PROBEDEPTH, 0);
	MoveAtVel(XAXIS, Trig, PROBESPEED * xRes / 60);
	while (!CheckDone(XAXIS));
	if (MoveDirection(West, PROBETIPDIAMETER/2, 1)) return 1;
	if (ReadBit(RESETDROVAR))
	{
		printf("Setting X DRO to Zero\n");
		DoPCFloat(PC_COMM_SET_X, 0.0f);
		MDI("G0X0Y0");
	}
	else
		Move(YAXIS, tempY);
	MoveDirection(Up, SAFEZ, 0);
	return 0;
}

int MoveDirection(int Direction, double Distance, int CheckProbe)
{
	double tmpDist;
	
	DIRECTION *d = &Directions[Direction];

	float Res = *d->Res;
	
	MSpeed = MOVESPEED * Res / 60.0;
	tmpDist = Distance * Res * d->Dir;
	Axis = d->Axis;
	
	printf("Moving %s %f %s\n", d->Name, Distance * ToUserUnits, Units);
	
	MoveRelAtVel(Axis, tmpDist, MSpeed);

	if (CheckProbe == 0)
	{
		while (!CheckDone(Axis));
	}
	else
	{
		while (!CheckDone(Axis) && ReadBit(PROBEBIT) != PROBE_ACTIVE);
		if (ReadBit(PROBEBIT) == PROBE_ACTIVE)
		{
			Jog(Axis,0);
			MsgBox("Probe hit something!",MB_OK|MB_ICONEXCLAMATION);
			return 1;
		}
	}
	return 0;
}



void Probe(int Direction)
{
	DIRECTION *d = &Directions[Direction];

	float Res = *d->Res * d->Dir;
	FSpeed = PROBESPEED * Res / 60;
	SSpeed = SLOWSPEED * Res / 60;
	Axis = d->Axis;
	
	printf("Probe %s Jogging at speed:%f\n", d->Name, FSpeed);
	Jog(Axis, FSpeed);
	printf("Waiting for probe to activate|\n");
	while (ReadBit(PROBEBIT) != PROBE_ACTIVE);   		
	printf("Stopping\n");
    Jog(Axis, 0);
	printf("Waiting for motion to stop\n");
	while (!CheckDone(Axis));
	printf("Jogging at speed:%f\n", -SSpeed);
	Jog(Axis, -SSpeed);
	printf("Waiting for probe to reset\n");
	while (ReadBit(PROBEBIT) == PROBE_ACTIVE);   		
	printf("Stopping\n");
    Jog(Axis, 0);
	printf("Waiting for motion to stop\n");
	while (!CheckDone(Axis));
	printf("Jogging at speed:%f\n", SSpeed);
	Jog(Axis, SSpeed);
	printf("Waiting for probe to activate\n");
	while (ReadBit(PROBEBIT) != PROBE_ACTIVE);   		
	Trig = chan[Axis].Dest;
	printf("Stopping\n");
    Jog(Axis, 0);
	printf("Waiting for motion to stop\n");
	while (!CheckDone(Axis));
}
