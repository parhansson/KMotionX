// SimpleConsole.cpp 

#include "stdlib.h"
#include <CString.h>
#include <CMutex.h>
#include "../../KMotionDLL/KMotionDLL.h"  // KMOtion DLL Header

CKMotionDLL *KM;                // one instance of the interface class

// Display Error and terminate

void MyError()
{
	printf("%s:%d Simpleconsole error lock\n",__FILE__,__LINE__);
	exit(1);
}

int console(const char* msg){
	printf("%s:%d Console %s",__FILE__,__LINE__,msg );
	return 0;
}

int main(int argc, char* argv[])
{

	printf("char size %lu\n",sizeof(char));
	printf("short size %lu\n",sizeof(short));
	printf("int size %lu\n",sizeof(int));
	printf("long size %lu\n",sizeof(long));

	printf("int8_t size %lu\n",sizeof(int8_t));
	printf("int16_t size %lu\n",sizeof(int16_t));
	printf("int32_t size %lu\n",sizeof(int32_t));

	/*
char size 1
short size 2
int size 4
long size 8
int8_t size 1
int16_t size 2
int32_t size 4
	 */

	return 0;
	std::string f = "adsfadf";
	//#ifdef BOOST_POSIX_API   //workaround for user-input files
	            std::replace(f.begin(), f.end(), '\\', '/');
	//#endif

	KM = new CKMotionDLL(0);  // create as board 0
	char response[MAX_LINE];
	int BoardType=BOARD_TYPE_UNKNOWN;
	if(KM->CheckKMotionVersion()){
		printf("%s:%d CheckKMotionVersion failed\n",__FILE__,__LINE__);
	}
	const char* InFile= "/Users/parhansson/git/kflop/KMotionX/DSP_KFLOP/DSPKFLOP.out";
	char Version[81];
	if(KM->ExtractCoffVersionString(InFile, Version)){
		printf("%s:%d ExtractCoffVersionString failed\n",__FILE__,__LINE__);

	} else {
		printf("%s:%d ExtractCoffVersionString: %s\n",__FILE__,__LINE__,Version);
	}
	// Setup Axis for a simple stepper motor
	if(KM->SetConsoleCallback(console)){
		printf("%s:%d SetConsoleCallback Failed\n",__FILE__,__LINE__);
	}
	if(KM->ServiceConsole()){
		printf("%s:%d ServiceConsole Failed\n",__FILE__,__LINE__);
	}
	if (KM->KMotionLock() == KMOTION_LOCKED)  // see if we can get access
	{
		// Get the firmware date from the KMotion Card which
		// will be in PT (Pacific Time)
		KM->ReleaseToken();
		if(KM->WriteLineReadLine("Version",response)){
			printf("%s:%d WriteLineReadLine Version failed\n",__FILE__,__LINE__);

		} else {
			printf("%s:%d Version: %s\n",__FILE__,__LINE__,response);

		}
	} else {
		printf("%s:%d Simpleconsole Failed to get lock\n",__FILE__,__LINE__);
	}


	//KM->CheckKMotionVersion()

	if (KM->WriteLine( "DisableAxis1=0"))	MyError(); // Set board 0, Axis 1 to OFF while changing parameters
	/*
	if (KM->WriteLine( "Zero1=0"))			MyError(); // Zero position board 0, Axis 1
	if (KM->WriteLine( "StepperAmplitude1=80"))MyError(); // Set nominal Sine wave amplitude to 80/256 of Motor supply voltage
	if (KM->WriteLine( "Vel1=100"))			MyError(); // Set Max Velocity to 100 cycles/sec (4 full steps/sec = 1 cycle)
	if (KM->WriteLine( "Accel1=50"))			MyError(); // Set Max Acceleration to 50 cycles/sec2 
	if (KM->WriteLine( "Jerk1=2000"))		MyError(); // Set Max Jerk to 2000 cycles/sec3 
	if (KM->WriteLine( "Lead1=10"))			MyError(); // Set Inductance Lead compensation to 10 * dV/dt 
	if (KM->WriteLine( "OutputMode1=1"))		MyError(); // Set board 0, Axis 1 to MICROSTEP_MODE (see KMotionDef.h)
	*/
	if (KM->WriteLine( "EnableAxisDest1 0"))	MyError(); // Zero position board 0, Axis 1
	if (KM->WriteLine( "Echo Setup Complete")) MyError(); // Echo a string to console 
	KM->ServiceConsole();

	// move board 0, axis 1, 1000 counts relative

	//if (KM->WriteLine( "MoveRel1=100"))  MyError();

	// loop until complete status = 1
/*
	do
	{
		if (KM->WriteLineReadLine("CheckDone1",response))
			MyError();
	} 
	while (response[0] != '1');

	if (KM->WriteLine("Echo Move Complete")) MyError(); // Echo a string to console 
*/
	return 0;
}



