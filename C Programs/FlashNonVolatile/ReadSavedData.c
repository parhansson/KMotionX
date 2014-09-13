#include "KMotionDef.h"

#define FLASH ((volatile char *)0x90000000)
void SetFlashBank(volatile unsigned short *add);

main()
{
	double *p=(double*)FLASH;  // pointer to the FLASH Chip
		
	SetFlashBank(p); // the DSP external bus is banked, this routine selects the bank for the address 
	
	printf("Saved 0 1 2 = %f %f %f\n",p[0],p[1],p[2]);  // now we can access the data 
}
