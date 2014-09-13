#include "KMotionDef.h"

// This program will slowly rotate a brushless motor using an Amplifier
// that requires 2 analog inputs that are driven 120 degrees out-of-phase
//

#define AXIS_CHAN 4       	// Axis channel to be rotated
#define AMPLITUDE 100.0f  	// Set how hard to drive the coils DAC counts
#define Z_BIT_NUMBER 68		// What bit the Z index is connected to

#define RAD120DEG (TWO_PI_F*0.3333333f)

main()
{
	int k=0,dk=1,ignore=300,zmark;
	CHAN *ch = &chan[AXIS_CHAN];
	float theta;
	
	for (;;)
    {
		Delay_sec(0.001);  // wait a millisecond 
		k+= dk;
		
		theta = k/1000.0;  // move the pole 
		
		DAC(AXIS_CHAN*2  ,(int)(AMPLITUDE * sinf(theta)));
		DAC(AXIS_CHAN*2+1,(int)(AMPLITUDE * sinf(theta+RAD120DEG)));

		zmark = ReadBit(Z_BIT_NUMBER);
		
		if (!zmark && ignore>0) ignore--;
		
		if (ignore==0 && zmark)  // check for index mark
				break;
    }

	DAC(AXIS_CHAN*2,0);    // turn off the coils
	DAC(AXIS_CHAN*2+1,0);    
}

