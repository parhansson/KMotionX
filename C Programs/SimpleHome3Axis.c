#include "KMotionDef.h"

// Configuration and Homing program for a 3 axis System
// Limit switches are disabled and used as a home switch
// then they are re-enabled


main()
{
	int SaveXLimits,SaveYLimits,SaveZLimits;  //place to save limit switch settings
	
	DisableAxis(0);  // Disable all axes
	DisableAxis(1);
	DisableAxis(2);

	// Set the axis parameters here
	// after everything is configured in the KMotion Screens
	// use copy C Code to clipboard on the configuration screen
	// then paste here.  Repeat for each axis
	
	
	
	
	
	
	
	// disable the limits (first save how they were set) 
	SaveXLimits = ch0->LimitSwitchOptions;
	SaveYLimits = ch1->LimitSwitchOptions;
	SaveZLimits = ch2->LimitSwitchOptions;
	ch0->LimitSwitchOptions = 0;
	ch1->LimitSwitchOptions = 0;
	ch2->LimitSwitchOptions = 0;
	
	// enable all 3 axes and begin servoing where we are 
	
	EnableAxis(0);
	EnableAxis(1);
	EnableAxis(2);

 
	// Home Z up first - jog until it sees the limit

    Jog(2,100);             // jog slowly positive
    while (!ReadBit(10)) ;  // loop until IO bit goes high
    Jog(2,0);				// stop
    while (!CheckDone(2)) ; // loop until motion completes 
	DisableAxis(2);			// disable the axis
    Zero(2);				// Zero the position
	EnableAxis(2);			// re-enable the ServoTick
	Move(2,-1000.0);		// move some amount inside the limits
    while (!CheckDone(2)) ; // loop until motion completes 
	ch2->LimitSwitchOptions = SaveZLimits;  // restore limit settings


	// Home X next - jog until it sees the limit

    Jog(0,-100);  			// jog slowly negative
    while (!ReadBit(10)) ;  // loop until IO bit goes high
    Jog(0,0);				// stop
    while (!CheckDone(0)) ; // loop until motion completes 
	DisableAxis(0);			// disable the axis
    Zero(0);				// Zero the position
	EnableAxis(0);			// re-enable the ServoTick
	Move(0,1000.0);			// move some amount inside the limits
    while (!CheckDone(0)) ; // loop until motion completes 
	ch0->LimitSwitchOptions = SaveXLimits;  // restore limit settings


	// Home Y next - jog until it sees the limit

    Jog(1,-100);  			// jog slowly negative
    while (!ReadBit(9)) ;  	// loop until IO bit goes high
    Jog(1,0);				// stop
    while (!CheckDone(1)) ; // loop until motion completes 
	DisableAxis(1);			// disable the axis
    Zero(1);				// Zero the position
	EnableAxis(1);			// re-enable the ServoTick
	Move(1,1000.0);			// move some amount inside the limits
    while (!CheckDone(1)) ; // loop until motion completes 
	ch1->LimitSwitchOptions = SaveYLimits;  // restore limit settings

	DefineCoordSystem(0,1,2,-1);  //Define XYZ coordinated motion axes
}
