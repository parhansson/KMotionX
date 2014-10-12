// SimpleConsole.cpp 

#include "stdlib.h"
#include <string>
#include <CMutex.h>
#include "../../KMotionDLL/KMotionDLL.h"  // KMOtion DLL Header

#define BYTE_BIN_PAT "%d%d%d%d%d%d%d%d"
#define BYTE_BIN(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)
#define IB_P BYTE_BIN_PAT " " BYTE_BIN_PAT " " BYTE_BIN_PAT " " BYTE_BIN_PAT
#define IB(byte) BYTE_BIN(byte>>24),BYTE_BIN(byte>>16),BYTE_BIN(byte>>8),BYTE_BIN(byte>>24)

CKMotionDLL *KM;                // one instance of the interface class

// Display Error and terminate

void MyError()
{
	printf("testRunner error exiting\n");
	exit(1);
}

int console(const char* msg){
	printf("Console> %s\n",msg );
	return 0;
}
void testConvertToOut(int thread,const char *file);
void testExtractCoffVersion();
void testCompile();
void testTypes();
void testString();
void testStatus();
void testVersion();
int testCallback();
void testListLocation();
void testMovement();
void testBitDirection();
void testCheckForReady();

int main(int argc, char* argv[])
{
	KM = new CKMotionDLL(0);  // create as board 0
	//testCheckForReady();

	//testVersion();
	//testListLocation();
	//testBitDirection();

	//testTypes();
	//testMovement();

//	testCompile();
	/*
	testConvertToOut(1,"/Users/bul.tp");
	testConvertToOut(0, "/Users/bull.c");
	testConvertToOut(1, "/Users/Home/bull.c");
	testConvertToOut(1, "/Users/bulle.cpP");
	testConvertToOut(1, "/Users/bull.txT");
	testConvertToOut(1, "/Users/bulle.cpp");

	testExtractCoffVersion();
*/


	/*
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
	*/


	return 0;
}
void testVersion(){
	if(KM->CheckKMotionVersion()){
		printf("CheckKMotionVersion Failed\n");
	} else {
		printf("CheckKMotionVersion Succeeded\n");
	}


}
int testCallback(){
	int result = KM->SetConsoleCallback(console);
	if(result){
		printf("SetConsoleCallback Failed\n");
	} else {
		printf("SetConsoleCallback Succeeded\n");
	}
	return result;
}

void testBitDirection(){
	printf("Status before:\n");
	testStatus();
	if (KM->WriteLine( "SetBitDirection0=1"))	MyError();
	if (KM->WriteLine( "SetBitDirection1=1"))	MyError();
	if (KM->WriteLine( "SetBitDirection2=0"))	MyError();
	if (KM->WriteLine( "SetBitDirection3=0"))	MyError();
	if (KM->WriteLine( "SetBit0"))	MyError();
	if (KM->WriteLine( "SetBit1"))	MyError();
	//if (KM->WriteLine( "SetStartupThread1 0"))	MyError();


	usleep(1000*1000);
	printf("Status after:\n");
	testStatus();
}

void testMovement(){
	if(!testCallback()){
		char response[MAX_LINE];
		//int BoardType=BOARD_TYPE_UNKNOWN;

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
		KM->ServiceConsole();

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


	}
}

void testConvertToOut(int thread,const char *file){
	char outfile[256];
	char path[256];
	memset (path,'\0',256);
	KM->ConvertToOut(thread, file,outfile,256);
	//KM->ExtractPath(file,path); //Change to public before testing function
	//CString p;
	//p= KM->ExtractPath(file); //Change to public before testing function

	printf("In %s\nOut: %s\nPath: %s\n\n",file,outfile,path);
}
void testCheckForReady(){
	if(KM->CheckForReady()){
		printf("CheckForReady failed\n");
	} else {
		printf("CheckForReady succeded\n");
	}
}
void testCompile(){
	char file[256];
	strcpy(file,KM->MainPathRoot);
	strcat(file,"/C Programs/FanOFF.c");
	KM->CompileAndLoadCoff(file, 1);

}
void testExtractCoffVersion(){
	char file[256];
		strcpy(file,KM->MainPathRoot);
		strcat(file,"/DSP_KFLOP/DSPKFLOP.out");
	char Version[81];
	if(KM->ExtractCoffVersionString(file, Version)){
		printf("ExtractCoffVersionString failed\n");
	} else {
		printf("CoffVersionString: %s\n",Version);
	}
}
void testTypes(){
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

}
void testString(){
	//std::string f = "adsfadf";
	//#ifdef BOOST_POSIX_API   //workaround for user-input files
	//std::replace(f.begin(), f.end(), '\\', '/');
	//#endif

}
void testListLocation(){

	int nlocations;
	int list[8];
	KM->BoardID=2;
	KM->ListLocations(&nlocations,list);
	for (int var = 0; var < nlocations; var++) {
		printf("Location[%d] Board nr: %d\n",var, list[var]);
	}
	KM->USBLocation();

}
void testStatus(){
	MAIN_STATUS status;
	if(KM->GetStatus(status,1)){
		printf("GetStatus failed\n");
		return;
	}


	printf("VersionAndSize          %d %d\n",status.VersionAndSize>>16,status.VersionAndSize>>24);//bits 16-23 = version, bits 0-15 = size in words
/*
	int ADC[N_ADCS+2*N_ADCS_SNAP];
	int DAC[N_DACS];
	int PWM[N_PWMS+2*N_PWMS_SNAP];
	double Position[N_CHANNELS];
	double Dest[N_CHANNELS];
	unsigned char OutputChan0[N_CHANNELS];
*/
	printf("InputModes              " IB_P "\n",IB(status.InputModes));		// 4 bits for each axis
	printf("InputModes2             " IB_P "\n",IB(status.InputModes2));	// 4 bits for each axis

	printf("OutputModes             " IB_P "\n",IB(status.OutputModes));	// 4 bits for each axis
	printf("OutputModes2            " IB_P "\n",IB(status.OutputModes2));	// 4 bits for each axis

	printf("Enables                 " IB_P "\n",IB(status.Enables)); 		// 4 bits for each axis
	printf("AxisDone                " IB_P "\n",IB(status.AxisDone)); 		// 4 bits for each axis

	printf("BitsDirection           " IB_P " " IB_P "\n", 				// KMotion - 64 bits of I/O direction 1 = output
			IB(status.BitsDirection[1]),IB(status.BitsDirection[0]));
	printf("BitsState               " IB_P " " IB_P "\n",				// KMotion - 64 bits of state lsb=I/O bit0
			IB(status.BitsState[1]),IB(status.BitsState[0]));

	printf("SnapBitsDirection0      " IB_P "\n",IB(status.SnapBitsDirection0));		// Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 0
	printf("SnapBitsDirection1      " IB_P "\n",IB(status.SnapBitsDirection1));		// Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 1
	printf("SnapBitsState0          " IB_P "\n",IB(status.SnapBitsState0));			// Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 0
	printf("SnapBitsState1          " IB_P "\n",IB(status.SnapBitsState1));			// Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 1
	printf("KanalogBitsStateInputs  " IB_P "\n",IB(status.KanalogBitsStateInputs));	// Kanalog - 16 bits 128-143
	printf("KanalogBitsStateOutputs " IB_P "\n",IB(status.KanalogBitsStateOutputs));// Kanalog - 24 bits 144-167


	printf("RunOnStartUp            " IB_P "\n",IB(status.RunOnStartUp));		// word Bits 1-7 selects which threads to execute on startup

	printf("ThreadActive            " IB_P "\n",IB(status.ThreadActive));		// one bit for each thread, 1=active, bit 0 - primary
	printf("StopImmediateState      " IB_P "\n",IB(status.StopImmediateState));	// Status of Stop Coordinated Motion Immediately

	printf("TimeStamp               %f\n",status.TimeStamp);// Time in seconds (since KFlop boot) this status was aquired

	//int	PC_comm[N_PC_COMM_PERSIST];// 8 persist Variables constantly uploaded to send misc commands/data to PC

	printf("VirtualBits             " IB_P "\n",IB(status.VirtualBits));		// Virtual I/O bits simulated in memory
	printf("VirtualBitsEx0          " IB_P "\n",IB(status.VirtualBitsEx0));		// only upload 32 1024 Expanded Virtual Bits
}

