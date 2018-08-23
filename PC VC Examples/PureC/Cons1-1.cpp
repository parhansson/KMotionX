//---------------------------------------------------------------------------

#include <windows.h>
#include <string>

extern "C"  int __stdcall KM_dotnet_Interop_New(int *handle, int board);
extern "C"  int __stdcall KM_dotnet_Interop_WriteLine(void *handle, char *s);
extern "C"  int __stdcall KM_dotnet_Interop_WriteLineReadLine(void *handle, char *s, char **r);
extern "C" 	int  __stdcall KM_dotnet_Interop_Free(int *handle);
extern "C" 	bool  __stdcall KM_dotnet_Interop_MainStatus_GetStatus(void *handle, bool lock,
	int *versionandsize,
	int adc[24],
	int dac[8],
	int pwm[16],
	double position[8],
	double destination[8],
	unsigned char *outputchan0,
	int *inputmodes,
	int *inputmodes2,
	int *outputmodes,
	int *outputmodes2,
	int *enables,
	int *axisdone,
	int bitsdirection[2],
	int bitsstate[2],
	int *snapbitsdirection0,
	int *snapbitsdirection1,
	int *snapbitsstate0,
	int *snapbitsstate1,
	int *kanalgobitsstateinputs,
	int *kanalogbitsstateoutputs,
	int *runonstartup,
	int *threadactive,
	int *stopimmediatestate,
	double *timestamp,
	int pccomm[8],
	int *virtualbits,
	int *virtualbitsex0);

int *H1;

// global main status variables 

int versionandsize;
int adc[24];
int dac[8];
int pwm[16];
double position[8];
double destination[8];
unsigned char outputchan0[8];
int inputmodes;
int inputmodes2;
int outputmodes;
int outputmodes2;
int enables;
int axisdone;
int bitsdirection[2];
int bitsstate[2];
int snapbitsdirection0;
int snapbitsdirection1;
int snapbitsstate0;
int snapbitsstate1;
int kanalgobitsstateinputs;
int kanalogbitsstateoutputs;
int runonstartup;
int threadactive;
int stopimmediatestate;
double timestamp;
int pccomm[8];
int virtualbits;
int virtualbitsex0;

int main()
{
	KM_dotnet_Interop_New((int *)&H1, 0);
	KM_dotnet_Interop_WriteLine(H1, "ClearBit46");
	KM_dotnet_Interop_WriteLine(H1, "Zero0");
	KM_dotnet_Interop_WriteLine(H1, "Enable0");
	KM_dotnet_Interop_WriteLine(H1, "Move0=123456.123456");  // command axis 0 to move

	// loop until Axis is done moving
	char response[80], *presponse = response;
	do
	{
		KM_dotnet_Interop_WriteLineReadLine(H1, "CheckDone0", &presponse);
	} while (response[0] != '1');


	// read global status
	KM_dotnet_Interop_MainStatus_GetStatus(H1, true,
		&versionandsize,
		adc,
		dac,
		pwm,
		position,
		destination,
		outputchan0,
		&inputmodes,
		&inputmodes2,
		&outputmodes,
		&outputmodes2,
		&enables,
		&axisdone,
		bitsdirection,
		bitsstate,
		&snapbitsdirection0,
		&snapbitsdirection1,
		&snapbitsstate0,
		&snapbitsstate1,
		&kanalgobitsstateinputs,
		&kanalogbitsstateoutputs,
		&runonstartup,
		&threadactive,
		&stopimmediatestate,
		&timestamp,
		pccomm,
		&virtualbits,
		&virtualbitsex0);

	// display final Destination of Axis 0 in a message box
	MessageBox(NULL, std::to_string(destination[0]).c_str(), "KMotion", MB_OK);

	KM_dotnet_Interop_Free((int *)&H1);
}

