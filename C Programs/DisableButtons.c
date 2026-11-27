#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"
#include "..\PC VC Examples\KMotionCNC\resource.h"

int main() 
{
	int *p=(char *)gather_buffer+GATH_OFF*sizeof(int);  // pointer to list
	
	*p++ = IDC_SpindleOnCW;
	*p++ = IDC_SpindleOnCCW;
	*p++ = IDC_SpindleOff;
	*p++ = IDC_SaveFile;
	*p++ = IDC_SaveAs;
	*p++ = IDC_But0;
	*p++ = IDC_GO;
	*p++ = 0;  // terminate list
	
//	DoPCInt(PC_COMM_DISABLE_CONTROLS,GATH_OFF);
	DoPCInt(PC_COMM_ENABLE_CONTROLS,GATH_OFF);

	
 DoPC(PC_COMM_ENABLE_JOG_KEYS);
// DoPC(PC_COMM_FORCE_DISABLE_JOG_KEYS);
}