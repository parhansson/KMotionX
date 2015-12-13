#include "KMotionDef.h"

// Creates dual feeback loops for cases such as rotary motor encoder feedback
// with linear scale encoder feedback.  Two KFLOP Servo Axes are required,
// one for each loop.  Output of the outer loop is applied as a velocity
// to the inner loop 

main()
{
    for (;;)  // loop forever
    {
    	WaitNextTimeSlice();
		ch0->Dest += ch2->Output;  // move ch0 at speed of ch2 output
    }
}
