#include "KMotionDef.h"
#define TMP 10 
#include "KflopToKMotionCNCFunctions.c"// verify correct path 

void main()
{
	#define XAXIS 0
	#define YAXIS 2
	#define ZAXIS 3

	// **** WARNING ****
	// Be sure to check which bit # your machine uses for touch off block. Code below set to use bit PROBE.

	#define PROBE 6
	
	#define PROBE_ACTIVE 0  // triggered state = 0 or 1

	
	// The values below must be set for each CNC maching and touch-off block
	double Xcpi=10846.0; // Counts per inch for X axis
	double Ycpi=10846.0; // Counts per inch for Y axis
	double Zcpi=21694.0; // Counts per inch for Z axis

	double ProbeSpd=1.0;  // Speed to move in inches per second
	double ProbeSpdSlow=0.1;  // Speed to move in inches per second

    double XBlockOffset=0.394;	// Distance of edge of 3-axis touch-off block from desired X-axis 0 point.
    double YBlockOffset=0.4;	// Distance of edge of 3-axis touch-off block from desired Y-axis 0 point.
    double ZBlockThickness=0.118;	// Thickness of 3-axis touch-off block from desired 0 point for Z-axis.
	double XYDropdownOffset=1.20;	// Distance to move in the X & Y directions from starting point of Z to drop down for XY check
	double XYDropdownDistance=0.5;  // Distance to drop below the top of touch-off block before checking for X&Y edge
	double ZClearanceDistance=0.2;  // Distance above touch-off block when bit moves between checking for X&Y edge
	
	double BitDia=0.25;			// Default value of bit diameter (update InputBox prompt if you change this!)
	
	//****************************************************************************
	//****************************************************************************
	//****************************************************************************
	//
	// Changing things below here means you are a programmer!
	// Be sure to share any good updates.
	// You can keep the bad ones to yourself... :-)
	//
	//****************************************************************************
	//****************************************************************************
	//****************************************************************************

	float InpBitDia;			// Bit diameter input by user
	
	if (!InputBox("Dia. of bit [dflt=0.25""]", &InpBitDia)) {
		if ((InpBitDia < 0.01) || (InpBitDia > 1.0)) {
			MsgBox("Invalid bit diameter. (Must be >0.01 or <1.0) Exiting.", MB_OK || MB_ICONEXCLAMATION);
			return;
		}
		// MsgBox("Using user input for bit diameter.", MB_OK);
		BitDia = InpBitDia;
	}
	else {
		// MsgBox("Using default for bit diameter.", MB_OK);
	}

	// return;
	
    KStepPresent=TRUE;  // enable KSTEP input multiplexing
    FPGA(STEP_PULSE_LENGTH_ADD) = 63 + 0x80;

	//home Z
    Jog(ZAXIS,-ProbeSpdSlow*Zcpi);			// jog slowly negative
    while (ReadBit(PROBE) != PROBE_ACTIVE);	// loop until IO bit goes low (bit touches block)
    Jog(ZAXIS,0);               // stop moving
    while (!CheckDone(ZAXIS));	// loop until motion completes 
  	DoPCFloat(PC_COMM_SET_Z,ZBlockThickness); // Set Z-axis location to touch-off block thickness

	// Move up to setup for X check
	MoveRelAtVel(ZAXIS, ZClearanceDistance*Zcpi, ProbeSpd*Zcpi);	// Move up to clearance distance above touch-off block
    while (!CheckDone(ZAXIS)); // loop until motion completes 

	// Move over and drop to check X
	MoveRelAtVel(XAXIS, (-(0.5*BitDia)-XYDropdownOffset)*Xcpi, ProbeSpd*Xcpi);	// Move out in -X direction (plus 1/2 bit diameter) for drop down
    while (!CheckDone(XAXIS)); // loop until motion completes 
	
	// Drop down to approach height while checking if hit block
	MoveRelAtVel(ZAXIS, (-ZClearanceDistance-XYDropdownDistance)*Zcpi, ProbeSpdSlow*Zcpi);	// Move down to X approach height
    while (!CheckDone(ZAXIS) && ReadBit(PROBE) != PROBE_ACTIVE); // loop until motion completes or touches block (BAD!)
	
	// If hit touch-off block, display message and halt.
	if (ReadBit(PROBE) == PROBE_ACTIVE) {
		StopCoordinatedMotion();
		ClearStopImmediately();
		MsgBox("Touch-off block hit when dropping down. Initial start location bad. Halting.", MB_OK || MB_ICONEXCLAMATION);
		return;
	}

	// **** Find X edge of block ****
 	// Jog X until touch block and set location
    Jog(XAXIS,ProbeSpdSlow*Xcpi);             // jog slowly positive
    while (ReadBit(PROBE) != PROBE_ACTIVE);   // loop until IO bit goes low
    Jog(XAXIS,0);                // stop X
    while (!CheckDone(XAXIS));  // loop until motion completes 
  	DoPCFloat(PC_COMM_SET_X,-XBlockOffset-(0.5*BitDia)); // Set X-axis location to touch-off block -X Offset

	// Move away from block and position for Y Check
	MoveRelAtVel(XAXIS, -0.1*Xcpi, ProbeSpd*Xcpi);	// Move away .1" in -X direction from touch-off block
    while (!CheckDone(XAXIS)); // loop until motion completes 
	MoveRelAtVel(ZAXIS, (ZClearanceDistance+XYDropdownDistance)*Zcpi, ProbeSpd*Zcpi);	// Move up to clearance distance above block
    while (!CheckDone(ZAXIS)); // loop until motion completes
	MoveRelAtVel(XAXIS, ((0.5*BitDia)+XYDropdownOffset)*Xcpi, ProbeSpd*Xcpi);	// Move over in X direction for drop down
    MoveRelAtVel(YAXIS, (-(0.5*BitDia)-XYDropdownOffset)*Ycpi, ProbeSpd*Ycpi);	// Move over in Y direction for drop down
    while (!CheckDone(XAXIS) && !CheckDone(YAXIS)); // loop until motion completes 

	// Drop down to approach height while checking if hit block
	MoveRelAtVel(ZAXIS, (-ZClearanceDistance-XYDropdownDistance)*Zcpi, ProbeSpdSlow*Zcpi);	// Move down to X approach height
    while (!CheckDone(ZAXIS) && ReadBit(PROBE) != PROBE_ACTIVE); // loop until motion completes or touches block (BAD!)
	
	// If hit touch-off block, display message and halt.
	if (ReadBit(PROBE) == PROBE_ACTIVE) {
		StopCoordinatedMotion();
		ClearStopImmediately();
		MsgBox("Touch-off block hit when dropping down. Initial start location bad. Halting.", MB_OK || MB_ICONEXCLAMATION);
		return;
	}

	
	// **** Find Y edge of block ****
 	// Jog Y until touch block and set location
    Jog(YAXIS,ProbeSpdSlow*Ycpi);             // jog slowly positive
    while (ReadBit(PROBE) != PROBE_ACTIVE);   // loop until IO bit goes low
    Jog(YAXIS,0);                // stop Y
    while (!CheckDone(YAXIS));  // loop until motion completes 
  	DoPCFloat(PC_COMM_SET_Y,-YBlockOffset-(0.5*BitDia)); // Set Y-axis location to touch-off block -Y Offset
	
	// Move away from block and go to 0,0,1
	MoveRelAtVel(YAXIS, -0.1*Ycpi, ProbeSpd*Ycpi);	// Move away .1" in -Y direction from touch-off block
    while (!CheckDone(YAXIS)); // loop until motion completes 

	// Move up to clearance distance
	MoveRelAtVel(ZAXIS, (ZClearanceDistance+XYDropdownDistance)*Zcpi, ProbeSpd*Zcpi);	// Move up to clearance distance above block
    while (!CheckDone(ZAXIS)); // loop until motion completes
	
	// Move to 0,0,1
	MoveRelAtVel(XAXIS, (-XYDropdownOffset+XBlockOffset+0.1)*Xcpi, ProbeSpd*Xcpi);	// Move X to 0
	MoveRelAtVel(YAXIS, (YBlockOffset+(0.5*BitDia)+0.1)*Ycpi, ProbeSpd*Ycpi);	// Move Y to 0
	MoveRelAtVel(ZAXIS, (1.0-ZBlockThickness-ZClearanceDistance)*Zcpi, ProbeSpd*Zcpi);	// Move Z to 1
	while (!CheckDone(XAXIS) && !CheckDone(YAXIS) && !CheckDone(ZAXIS)); // loop until motion completes 
}
