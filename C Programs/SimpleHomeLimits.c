#include "KMotionDef.h"

main()
{
	ch0->LimitSwitchOptions=0x00000000; // disable limits
	
	Jog(0,-100);          				// start moving
	while (!Readbit(8)) ; 				// wait for switch (input #8) to change
	
	Jog(0,0);		      				// stop
	while (!CheckDone(0))  ;  			// wait till we are stopped

	Zero(0);                            // Set current position to 0

	Move(0,1000);	      				// move off limit
	while (!CheckDone(0))  ;  			// wait till we are stopped
	
	// re-enable limits, bit 8 both ends, active low, disable all axis
	ch0->LimitSwitchOptions=0x808000f;	
}