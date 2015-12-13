#include "KMotionDef.h"

// Demo changing individual Flash Blocks

void PrintTwoBlocks(void);

double DestData[10];  // room for data

main()
{
	DestData[0] = 1.0;
	DestData[1] = 2.0;
	// write 2 x 8 bytes of the buffer to the 2nd MByte of the Flash Chip first 64KByte block
	ProgramFlash((volatile char *)DestData,16,FLASH_USER,""); 

	DestData[0] = 3.0;
	DestData[1] = 4.0;
	// write 2 x 8 bytes of the buffer to the 2nd MByte of the Flash Chip 2nd 64KByte Block
	ProgramFlash((volatile char *)DestData,16,FLASH_USER+FLASH_BLOCK_SIZE,""); 
	PrintTwoBlocks();
	
	// change first block
	DestData[0] = 5.0;
	DestData[1] = 6.0;
	// write 2 x 8 bytes of the buffer to the 2nd MByte of the Flash Chip first 64KByte block
	ProgramFlash((volatile char *)DestData,16,FLASH_USER,""); 
	printf("First block changed\n");
	PrintTwoBlocks();

	// change 2nd block
	DestData[0] = 7.0;
	DestData[1] = 8.0;
	// write 2 x 8 bytes of the buffer to the 2nd MByte of the Flash Chip 2nd 64KByte Block
	ProgramFlash((volatile char *)DestData,16,FLASH_USER+FLASH_BLOCK_SIZE,""); 
	printf("2nd block changed\n");
	PrintTwoBlocks();
}

#define FLASHD ((double *)FLASH_USER)

void PrintTwoBlocks(void)
{
	SetFlashBank(FLASH_USER); // read first block
	printf("First Block Values %f %f\n",FLASHD[0], FLASHD[1]);

	SetFlashBank(FLASH_USER+FLASH_BLOCK_SIZE);
	printf("Second block Values %f %f\n",FLASHD[0+FLASH_BLOCK_SIZE/sizeof(double)],FLASHD[1+FLASH_BLOCK_SIZE/sizeof(double)]);
}


