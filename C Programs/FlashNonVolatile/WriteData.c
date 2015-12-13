#include "KMotionDef.h"

double DestData[2];  // room for some data

main()
{
	double T0,T1;
	
	// put some data in a buffer that we want to save
	DestData[0]=3.0;
	DestData[1]=4.0;
	
	T0=Time_sec();
	// write 16 bytes of the buffer to the 2nd MByte of the Flash Chip
	ProgramFlash((volatile char *)DestData,16/sizeof(short int),FLASH_USER,""); 
	T1=Time_sec();
	
	printf ("Flash Complete Erase+Programming time = %f seconds\n",T1-T0);

}
