#include "KMotionDef.h"

// This program will slowly rotate a brushless motor using an Amplifier
// that requires 2 analog inputs that are driven 120 degrees out-of-phase
//

#define AXIS 0       		// Axis channel to be rotated (assume DACS 0,1 then 2,3 then ..)
#define AMPLITUDE 200.0f  	// Set how hard to drive the coils DAC counts

#define RAD120DEG (TWO_PI_F*0.3333333f)

main()
{
	int k=0,dk=1;
	float theta;
	
	for (;;)
    {
		Delay_sec(0.001);  // wait a millisecond 
		k+= dk;
		
		theta = k/1000.0;  // move the pole 
		
		DAC(AXIS*2  ,(int)(AMPLITUDE * sinf(theta)));
		DAC(AXIS*2+1,(int)(AMPLITUDE * sinf(theta+RAD120DEG)));
    }
}

