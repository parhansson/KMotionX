#include "KMotionDef.h"

#define FLASH ((volatile char *)0x90000000)
void ProgramFlash(volatile char *src, int Length, volatile char *dest, char *message);

main()
{
	// put some data in a buffer that we want to save
	gather_buffer[0]=99999.4;
	gather_buffer[1]=444.1;
	gather_buffer[2]=-1.0;
	
	// write 256 bytes of the buffer to the 2nd MByte of the Flash Chip
	ProgramFlash((volatile char *)gather_buffer,256,FLASH+0x100000,""); 
}
