// SimpleConsole.cpp 

#include "stdafx.h"
#include "stdlib.h"
#include "..\..\KMotionDll\KMotionDll.h"  // KMOtion DLL Header

CKMotionDLL *KM;                // one instance of the interface class

void MyError();


int main(int argc, char* argv[])
{
	KM = new CKMotionDLL(0);  // create as board 0
	char response[MAX_LINE];

	// Setup Axis for a simple stepper motor

	if (KM->WriteLine( "DisableAxis1=0"))	MyError(); // Set board 0, Axis 1 to OFF while changing parameters
	if (KM->WriteLine( "Zero1=0"))			MyError(); // Zero position board 0, Axis 1
	if (KM->WriteLine( "StepperAmplitude1=80"))MyError(); // Set nominal Sine wave amplitude to 80/256 of Motor supply voltage
	if (KM->WriteLine( "Vel1=100"))			MyError(); // Set Max Velocity to 100 cycles/sec (4 full steps/sec = 1 cycle)
	if (KM->WriteLine( "Accel1=50"))			MyError(); // Set Max Acceleration to 50 cycles/sec2 
	if (KM->WriteLine( "Jerk1=2000"))		MyError(); // Set Max Jerk to 2000 cycles/sec3 
	if (KM->WriteLine( "Lead1=10"))			MyError(); // Set Inductance Lead compensation to 10 * dV/dt 
	if (KM->WriteLine( "OutputMode1=1"))		MyError(); // Set board 0, Axis 1 to MICROSTEP_MODE (see KMotionDef.h)
	if (KM->WriteLine( "EnableAxisDest1 0"))	MyError(); // Zero position board 0, Axis 1
	if (KM->WriteLine( "Echo Setup Complete")) MyError(); // Echo a string to console 


	// move board 0, axis 1, 1000 counts relative

	if (KM->WriteLine( "MoveRel1=100"))  MyError();

	// loop until complete status = 1

	do
	{
		if (KM->WriteLineReadLine("CheckDone1",response))
			MyError();
	} 
	while (response[0] != '1');

	if (KM->WriteLine("Echo Move Complete")) MyError(); // Echo a string to console 

	return 0;
}

// Display Error and terminate

void MyError()
{
	printf("Unable to communicate with KMotion board");
	exit(1);
}

