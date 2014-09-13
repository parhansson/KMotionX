#include "KMotionDef.h"

// Detects changes in a hardware encoder and raises/lowers the FRO
// accordingly.  Clamp the encoder range so that if the operator
// keeps turning it will only go so far (slip)

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define ENCODER_AXIS 4
#define ALLOWED_ENC_RANGE 100 // limit the encoder to this range
#define CHANGE_TOL 1.0  // only update if encoder change is more than this
#define CHANGE_TIME 0.05  // don't update more often then this time
#define FRO_MIN 0.1
#define FRO_MAX 2.0

// solve for mapping relationship between encoder and FRO
// assume the linear relationship (form y=ax+b)
// Determine the a and b values that will map encoder of Zero
// to the minimum desired FRO, and the ALLOWED_ENC_RANGE to the max FRO
// But map encoder to the log of the FRO so it changes exponentially
// 
//
// FRO = a * Encoder + b
//
// Given:  when Encoder = 0                   => FRO = FRO_MIN
//              Encoder = ALLOWED_ENC_RANGE   => FRO = FRO_MAX
//
// b = FRO_MIN
// 
// FRO_MAX = a * ALLOWED_ENC_RANGE + FRO_MIN
// a = (FRO_MAX - FRO_MIN)/ALLOWED_ENC_RANGE
//
// determine Encoder value where FRO = 1.0
//
// 1.0 = a * Encoder + b
// Encoder =  (1.0 - b)/a
//
 
double LastFRO=-1;
double LastFROTime=0;
double LastEncoder=0;
double LastUpdateEncoder=1e30;  // force immediate update
double Encoder;

main()
{
	double Pot,T,FRO;
	CHAN *ch=&chan[ENCODER_AXIS];
	double a = (log(FRO_MAX) - log(FRO_MIN))/ALLOWED_ENC_RANGE;
	double b = log(FRO_MIN);


	// Initialize the encoder to give FRO of 1.0
	
	Encoder = (log(1.0) - b)/a;
	
	LastEncoder = ch->Position;
	
	for (;;)
	{
		T = WaitNextTimeSlice();

		Encoder += (ch->Position - LastEncoder);   // accumulate changes
		LastEncoder = ch->Position;
		

		// send message to KMotionCNC if the pot changed significantly
		// and it has been a while since the last message
		if ((Encoder > LastUpdateEncoder+CHANGE_TOL || Encoder < LastUpdateEncoder-CHANGE_TOL) && 
			T > LastFROTime+CHANGE_TIME)
		{
			// limit the Range
			if (Encoder > ALLOWED_ENC_RANGE) Encoder = ALLOWED_ENC_RANGE;
			if (Encoder < 0) Encoder = 0;

			// map the encoder range to the desired FRO range
			FRO = exp(a * Encoder + b);
			if (FRO > FRO_MAX) FRO=FRO_MAX;
			if (FRO < FRO_MIN) FRO=FRO_MIN;

			DoPCFloat(PC_COMM_SET_FRO,FRO);
			LastUpdateEncoder=Encoder;
			LastFROTime=T;
		}
	}
}
