#include "KMotionDef.h"

#define TAP_VAR 50 // start of where parameters should be placed

#define SPINDLE_AXIS 4
#define CNTS_PER_REV 4096
#define Z_DIST 0.625 //inches
#define TAU 0.001

double SlaveGain,ToCut,TotalCut,Z0,S0;
void DoSlave(int Axis);
void DoTap(int Axis, double Pitch, double Dist, double RPM);

main()
{
	// Set variables

	// Call MCode M119 to do Rigid Tapping
	// 
	// Var+0 - (int) 119 (MCode number)
	// Var+1 - (float) Bottom Z in User Units
	// Var+2 - (float) Retract Position in User Units
	// Var+3 - (float) Distance per Peck in User Units (0 if unspecified)
	// Var+4 - (float) Feed Rate F Number (User Units/Rev)
	// Var+5 - (float) Spindle RPM to Tap
	// Var+6 - (int) Units (INCHES=1 MM=2)
	// Var+7 - (int) KFLOP axis to move (derived from active GCode Plane and Coord Motion System defs)
	// Var+8 - (float) Axis Resolution Counts/inch

	float Depth;
//	int *pFeedhold = &persist.UserData[TAP_VAR+0];
	float Bottom = *(float *)&persist.UserData[TAP_VAR+1];
	float Retract = *(float *)&persist.UserData[TAP_VAR+2];
	float Peck = *(float *)&persist.UserData[TAP_VAR+3];
	float Pitch = *(float *)&persist.UserData[TAP_VAR+4];
	float RPM = *(float *)&persist.UserData[TAP_VAR+5];
	int Units = persist.UserData[TAP_VAR+6];
	int Axis = persist.UserData[TAP_VAR+7];
	float AxisRes = *(float *)&persist.UserData[TAP_VAR+8];

	//printf("Bottom = %f\n",Bottom); 
	//printf("Retract = %f\n",Retract); 
	//printf("Peck = %f\n",Peck); 
	//printf("Pitch = %f\n",Pitch); 
	//printf("RPM = %f\n",RPM); 
	if (Units == 1)
		printf("Units = Inches\n"); 
	else
		printf("Units = mm\n"); 
	//printf("Axis = %d\n",Axis); 
	//printf("AxisRes = %f\n",AxisRes); 		

	
	printf("Bottom = %f\n",Bottom);	// Print Bottom
	printf("Pitch = %f\n",Pitch);	// Print pitch

	if (Units == 2) // if metric convert units 
	{
		Bottom = Bottom / 25.4;
		Retract = Retract / 25.4;
		Pitch = Pitch / 25.4;
		Peck = Peck / 25.4;
	}
	
    Depth = Bottom - Retract;	// Depth equal to bottom and retract height
	if (Peck == 0) Peck = Depth * -1.1;	// if peck = zero, set larger then depth so its ignored	
	
	// Slave the Z Axis to the Spindle
	SlaveGain = AxisRes * Pitch / CNTS_PER_REV;
	Z0 = chan[Axis].Dest;
	S0 = chan[SPINDLE_AXIS].Dest;

	// in case there is significant spindle position error move there first
	Move(Axis,(chan[SPINDLE_AXIS].Position-S0)*SlaveGain+Z0);
	while (!CheckDone(Axis)) ;
	
	TotalCut=0.0;
	while (TotalCut < Bottom)
	{
		if (TotalCut + Peck > Bottom) // last feed
		{
			// yes, do any remaining
			DoTap(Axis, Pitch, Bottom-TotalCut, RPM);
			// retract fully
			DoTap(Axis, Pitch, -Bottom, RPM);
			TotalCut=Bottom;
		}
		else
		{
			// no, just cut a bit
			DoTap(Axis, Pitch, Peck, RPM);
			DoTap(Axis, Pitch, -Retract, RPM);
			TotalCut+=Peck-Retract;
		}
	}
		
	Delay_sec(1.0);
	Move(Axis,Z0);   // move back to where we started 
	while (!CheckDone(Axis)) ;
}

void DoTap(int Axis, double Pitch, double Dist, double RPM)
{
	// Tap down
	MoveRelAtVel(SPINDLE_AXIS, Dist*CNTS_PER_REV/Pitch, RPM*CNTS_PER_REV*60.0);
	
	while(!CheckDone(SPINDLE_AXIS))
		DoSlave(Axis);
}

void DoSlave(int Axis)
{
	MoveExp(Axis,(chan[SPINDLE_AXIS].Dest-S0)*SlaveGain+Z0, TAU);
	WaitNextTimeSlice();
}
