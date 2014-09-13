#include "KMotionDef.h"

void SimpleHome(int axis,float speed,int dir,int bit,int polarity, float offset);

//Plugin calls for Mach3 Home (actually Purge) Commands

main()
{
	int flags = persist.UserData[5];  // Mach3 flags bit0=X, bit1=Y, Bit2=Z, etc...

	printf("Mach3 Home Call, flags = %d\n",flags); 
	
	if (flags==1)
	{
		// do x homing here
		SimpleHome( 0,   	// axis
					100, 	// speed steps/sec
					-1,  	// direction to home
					168, 	// home input bit
					0,   	// bit polarity (0 or 1) when tripped
					1000);	// amount to move back inside (counts)
	}

	if (flags==2)
	{
		// do y homing here
		SimpleHome( 1,   	// axis
					100, 	// speed steps/sec
					-1,  	// direction to home
					169, 	// home input bit
					0,   	// bit polarity (0 or 1) when tripped
					1000);	// amount to move back inside (counts)
	}

	if (flags==4)
	{
		// do z homing here
		SimpleHome( 2,   	// axis
					100, 	// speed steps/sec
					1,  	// direction to home
					170, 	// home input bit
					0,   	// bit polarity (0 or 1) when tripped
					1000);	// amount to move back inside (counts)
	}

	printf("Done\n"); 
}

//simple routine to:
//
//  #1 disable limits
//  #2 jog to a limit at speed
//  #3 stop 
//  #4 zero the axis
//  #5 move back inside
//  #6 enable limits 

void SimpleHome(int axis,float speed,int dir,int bit,int polarity, float offset)
{
	int SaveLimits;  //place to save limit switch settings

	DisableAxis(axis);
	
	// disable the limits (first save how they were set) 
	SaveLimits = chan[axis]->LimitSwitchOptions;
	chan[axis]->LimitSwitchOptions = 0;
	
	EnableAxis(axis);	// enable axis and begin servoing where we are 

 	// Home - jog until it sees the limit

	Jog(axis,speed*dir);      	// jog slowly 
	while (!ReadBit(bit)) ;  	// loop until IO bit goes high
	Jog(axis,0);				// stop
	while (!CheckDone(axis)) 	// loop until motion completes
		if (!chan[axis].Enable) return; 	// abort/exit if diasabled 
	DisableAxis(axis);			// disable the axis
	Zero(axis);					// Zero the position
	EnableAxis(axis);			// re-enable
	Move(axis,-dir * offset);	// move some amount inside the limits
	while (!CheckDone(axis)) ; 	// loop until motion completes 
	chan[axis]->LimitSwitchOptions = SaveLimits;  // restore limit settings
}
