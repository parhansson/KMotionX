#include "KMotionDef.h"

#define AXIS 0 // axis to home
#define DIR -1 // direction to home
#define HSPEED 100 // speed and direction to home
#define THRESHOLD 10.0 // Following Error threshold to detect hard stop
#define INSIDE_DIST 100.0 // Distance to move and zero inside hardstop
main()
{
	Zero(AXIS);  // Zero the encoder  
	EnableAxisDest(AXIS,0);
	Jog(AXIS,HSPEED*DIR);     // start moving toward hardstop
	while (fast_fabs(chan[AXIS].Position - chan[AXIS].Dest) < THRESHOLD) ; // wait till magnitude of following error exceeds threshold
	MoveAtVel(0,chan[AXIS].Position+INSIDE_DIST,HSPEED);		// move to position inside hardstop
	while (CheckDone(AXIS)) ; //wait until stopped at inside position
	Zero(AXIS); //Zero the destination and position here
}
