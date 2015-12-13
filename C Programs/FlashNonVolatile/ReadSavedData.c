#include "KMotionDef.h"

#define FLASHC ((volatile char *)0x90100000)  //  2nd Meg of Flash 
void SetFlashBank(volatile unsigned short *add);

main()
{
	double *p=(double*)FLASHC;  // pointer to the FLASH Chip
		
	SetFlashBank(FLASHC); // the DSP external bus is banked, this routine selects the bank for the address 
	// now we can access the data 
	printf("Saved 0 1 2 = %f %f %X\n",p[0+0x100000/sizeof(double)],
                                      p[1+0x100000/sizeof(double)],
                                      *(int*)(&p[2+0x100000/sizeof(double)]));
}
