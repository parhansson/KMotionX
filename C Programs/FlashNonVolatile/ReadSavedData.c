#include "KMotionDef.h"

void SetFlashBank(volatile unsigned short *add);

main()
{
	double *p=(double*)FLASH_USER;  // pointer to the FLASH Chip
		
	SetFlashBank(p); // the DSP external bus is banked, this routine selects the bank for the address 
	// now we can access the data 
	printf("Saved 0 1 2 = %f %f next word = %X\n",p[0],p[1],*(int*)(&p[2]));
}
