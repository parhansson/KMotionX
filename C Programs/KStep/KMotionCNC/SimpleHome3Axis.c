#include "KMotionDef.h"
#include "..\..\SimpleHomeFunction.c"

// Homing program for a 3 axis System
// Limit switches are disabled and used as a home switch
// then they are re-enabled

#define XHOME 168
#define YHOME 169
#define ZHOME 170

main()
{
	// do z homing
	SimpleHome( 2,   	// axis
				100, 	// speed steps/sec
				1,  	// direction to home
				170, 	// home input bit
				0,   	// bit polarity (0 or 1) when tripped
				1000);	// amount to move back inside (counts)

	// do x homing
	SimpleHome( 0,   	// axis
				100, 	// speed steps/sec
				-1,  	// direction to home
				168, 	// home input bit
				0,   	// bit polarity (0 or 1) when tripped
				1000);	// amount to move back inside (counts)

	// do y homing
	SimpleHome( 1,   	// axis
				100, 	// speed steps/sec
				-1,  	// direction to home
				169, 	// home input bit
				0,   	// bit polarity (0 or 1) when tripped
				1000);	// amount to move back inside (counts)
}
