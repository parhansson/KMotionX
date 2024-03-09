#include "KMotionDef.h"

// In order to limit maximum torque for a 3phase brushless motor limit the
// Max Output (Voltage) to an allowed amount over the back-emf of the motor.
// The back emf to speed relationship must be determined by observing an
// unloaded high speed and Servo Output. 

#define TIME_SLICES 10 // how many time slices to wait to get a good velocity measurement

#define X_SPEED 150000.0
#define X_OUTPUT 225.0
#define X_ALLOWED 20.0

#define Z_SPEED 128400.0
#define Z_OUTPUT 187.0
#define Z_ALLOWED 20.0


void ServiceMaxOutput(void);

main()
{
	for (;;)
	{
		WaitNextTimeSlice();
		ServiceMaxOutput();
	}
}

// Service limiting Max Allowed Output based on current motor speed

void ServiceMaxOutput(void)
{
	static BOOL FirstTime=TRUE;
	static int i;
	static double T0, X0, Z0;
	double T1,X1,Z1,DX,DZ,iDT;
	
	if (FirstTime)  // first time initialize all
	{
		T0=Time_sec();
		X0=ch0->Position;
		Z0=ch1->Position;
		i=0;
		FirstTime=FALSE;
		return;
	}
	
	if (++i < TIME_SLICES) return;  // wait a few time slices

	T1=Time_sec();
	iDT=1.0/(T1-T0);   // compute inverse delta time
	T0=T1;
	
	X1=ch0->Position;  // compute delta X
	DX=X1-X0;
	X0=X1;

	Z1=ch1->Position;  // compute delta Z
	DZ=Z1-Z0;
	Z0=Z1;
	
	if (DX>0.0) // X going forward ?
		ch0->MaxOutput = DX * iDT * (X_OUTPUT/X_SPEED) + X_ALLOWED; 
	else
		ch0->MaxOutput = DX * iDT * (X_OUTPUT/X_SPEED) - X_ALLOWED; 

	if (DZ>0.0) // Z going forward ?
		ch1->MaxOutput = DZ * iDT * (Z_OUTPUT/Z_SPEED) + Z_ALLOWED; 
	else
		ch1->MaxOutput = DZ * iDT * (Z_OUTPUT/Z_SPEED) - Z_ALLOWED; 

	i=0;  // reset time slice counter
}