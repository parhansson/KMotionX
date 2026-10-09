//---------------------------------------------------------------------------

#include <windows.h>
#include <string>

#define __PureC__
#include "KMotion_dotNet_Interop.h" 

int* H1;

MAIN_STATUS Status;

int main()
{
	KM_dotnet_Interop_New((HANDLE64 *)&H1, 0);
	KM_dotnet_Interop_WriteLine(H1, "ClearBit46");
	KM_dotnet_Interop_WriteLine(H1, "Zero0");
	KM_dotnet_Interop_WriteLine(H1, "EnableAxis0");
	KM_dotnet_Interop_WriteLine(H1, "Move0=123456.123456");  // command axis 0 to move

	// loop until Axis is done moving
	char response[80], *presponse = response;
	do
	{
		KM_dotnet_Interop_WriteLineReadLine(H1, "CheckDone0", &presponse);
	} while (response[0] != '1');


	// read global status
	KM_dotnet_Interop_MainStatus_GetStatus(H1, true, &Status);

	// display final Destination of Axis 0 in a message box
	MessageBox(NULL, std::to_string(Status.Dest[0]).c_str(), "KMotion", MB_OK);

	KM_dotnet_Interop_Free((HANDLE64 *)&H1);
}

