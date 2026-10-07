#include "KMotionDef.h"

void *memcpy(void *s1, const void *s2, size_t n);  // copy bytes from s2 -> s1 


void main()
{
	char *gb = (char *)gather_buffer;  // Pointer of 1 bytes starting at beginning of gather_buffer
	int nBytes = (ParametricIndex+1)*sizeof(ParametricCoeffs[0]); //Quantity of bytes to save
	
	if (ParametricIndexWrapped)
	{
		printf("Error Motion Buffer wrapped data lost\n");
		return;
	}
	
	printf("Flashing Coord Motion Buffer %d bytes\n",nBytes);

	if (nBytes > 1024*1024 - 4 - 8) // check if will fit in Flash
	{
		printf("Error buffer won't fit in Flash\n");
		return;
	}
	
	memcpy(gb+0,(char *)&nBytes, 4);  // Put size at beginning as 4 byte integer
	memcpy(gb+4,(char *)&CS0_TimeDownloaded, 8);  // Put Buffer Time as double
	memcpy(gb+12,(char *)ParametricCoeffs, nBytes);  // copy data after length into gather buffer
	
    // write bytes of the buffer to the 2nd MByte of the Flash Memory as 16 bit words
    if (ProgramFlashNoMessages((volatile char *)gather_buffer,(nBytes+12)/2,FLASH_USER))
		printf("Error Flashing memory\n");
	else
		printf("Success Flashing memory\n");
}
