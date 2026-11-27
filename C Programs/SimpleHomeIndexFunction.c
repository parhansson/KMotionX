//simple routine to:
//
//  #1 disable limits
//  #2 jog to a limit at speed
//  #3 Reverse till index pulse
//  #4 stop
//  #5 zero the axis
//  #6 move back inside
//  #7 enable limits 

int SimpleHomeIndexFunction(int axis,	// axis number to home
			   float speed,  			// speed to move toward home
			   int dir,      			// direction to move toward home (+1 or -1)
			   int limitbit, 			// limit bit number to watch for
			   int limitpolarity,		// limit polarity to wait for (1 or 0)
               float indexspeed,		// speed to move while searching for index
			   int indexbit,        	// index bit number to watch for (use -1 for none)
			   int indexpolarity, 		// index polarity to wait for (1 or 0)
			   float offset)			// amount to move inside limits
{
	int SaveLimits;  //place to save limit switch settings

	DisableAxis(axis);
	
	// disable the limits (first save how they were set) 
	SaveLimits = chan[axis].LimitSwitchOptions;
	chan[axis].LimitSwitchOptions = 0;
	
	EnableAxis(axis);	// enable axis and begin servoing where we are 

 	// Home - jog until it sees the limit

	Jog(axis,speed*dir);      	// jog slowly 
	while (ReadBit(limitbit)!=limitpolarity) // loop until Limit bit goes to specified polarity
		if (!chan[axis].Enable) return 1; 	// abort/exit if disabled

	if (indexbit!=-1)  // index bit specified?
	{
		Jog(axis,-indexspeed*dir);  // reverse and jog slowly 
		while (ReadBit(indexbit)!=indexpolarity) // loop until index bit goes to specified polarity
			if (!chan[axis].Enable) return 1; 	// abort/exit if disabled
	}
	
	Jog(axis,0);				// stop
	while (!CheckDone(axis)) 	// loop until motion completes
		if (!chan[axis].Enable) return 1; 	// abort/exit if disabled
 
	DisableAxis(axis);			// disable the axis
	Zero(axis);					// Zero the position
	EnableAxis(axis);			// re-enable

	Move(axis,-dir * offset);	// move some amount inside the limits
	while (!CheckDone(axis)) ; 	// loop until motion completes 
		if (!chan[axis].Enable) return 1; 	// abort/exit if disabled

	chan[axis].LimitSwitchOptions = SaveLimits;  // restore limit settings
	return 0;
}

