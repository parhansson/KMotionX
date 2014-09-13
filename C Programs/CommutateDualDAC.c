#include "KMotionDef.h"

// This program will commutate a brushless motor using an Amplifier
// that requires 2 analog inputs that are driven 120 degrees out-of-phase
//
// The axis should be configured as if it is a brush motor and driving
// some unused DAC channel (ie 7).  And Max Output should be configured
// less than 2047
//
// The program loops and continuously takes the axis's desired output
// and writes it to two DACS that are commutated from the position
// based on the invDistPerCycle and CommutationOffset
//
// DACS 0 and 1 will be used for ch0
// DACS 2 and 3 will be used for ch1
// DACS 3 and 4 will be used for ch2

void Commutate2Phase(int ch);


main()
{
	// since we are using a 500 count encoder (2000 cnts)
	ch0->invDistPerCycle = 1.0/2000.0;
	
	// this must be determined somehow
	ch0->CommutationOffset = 0;
	
	while (1)  //loop forever
	{
		WaitNextTimeSlice();
		Commutate2Phase(0);     // do axis 0
	}
}

#define RAD120DEG (TWO_PI_F*0.3333333f)

void Commutate2Phase(int chN)
{
	CHAN *ch=&chan[chN];
	float theta;
	
	theta = (ch0->Position + ch->CommutationOffset) * ch->invDistPerCycle * TWO_PI_F;
	
	DAC(chN*2  ,(int)(ch->Output * sinf(theta)));
	DAC(chN*2+1,(int)(ch->Output * sinf(theta+RAD120DEG)));
}
