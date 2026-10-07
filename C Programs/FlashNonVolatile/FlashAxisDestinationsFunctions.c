#include "KMotionDef.h"

#define FLASHD ((volatile double *)0x90100000)

void SaveDestinations(void);
int RestoreDestinations(void);
void ServiceDestinationSave(void);

#define DEST_SIGNATURE 123.456
#define SAVE_DELAY 10.0 // seconds after host stops to flash Dests
#define NONVOL_FLAGVAR 150 // persist var to remember we restored

double DestData[10];  // room for 8 axes, count, signature
double DestStable[8]; // used to watch for changes

// for testing
//main()
//{
//	for (;;)
//	{
//		WaitNextTimeSlice();
//		ServiceDestinationSave();
//	}
//}


// Flash the Destinations of all the axes after all host applications
// stop requesting status and nothing is moving for a period of Time_sec
// and the last flashed positions are different from the current

void ServiceDestinationSave(void)
{
	static int PrevStatusRequestCounter=-1;
	static double FlashTime=0;
	int i;
	double T=Time_sec();
	
	// check if Host is requesting Status
	if (StatusRequestCounter != PrevStatusRequestCounter)
	{
		// yes, save time 
		FlashTime = T + SAVE_DELAY;
		for (i=0; i<8; i++) DestStable[i]=chan[i].Dest;  // copy dest data to array
		PrevStatusRequestCounter=StatusRequestCounter;
	}
	else
	{
		for (i=0; i<8; i++) // check if the dests have been stable
			if (DestStable[i]!=chan[i].Dest)
				break;
				
		if (i!=8)  
		{
			// something moved
			PrevStatusRequestCounter=StatusRequestCounter-1;  //force timer restart
		}
		else
		{
			if (T > FlashTime)  // time to save?
			{
				SaveDestinations(); 
				PrevStatusRequestCounter=StatusRequestCounter-1;  //force timer restart
			}
		}
	}
}

void SaveDestinations(void)
{	
	int i;
	
	SetFlashBank(FLASH_USER);  // Flash must be bank selected

	for (i=0; i<8; i++) // check if the dests are all the same and if so exit
		if (DestData[i]!=chan[i].Dest)
			break;
	
	if (i==8) return;
	
	for (i=0; i<8; i++) DestData[i]=chan[i].Dest;  // copy dest data to array
	
	DestData[8] = FLASHD[8]+1; // count number of times written
	DestData[9] = FLASHD[9];

	if (DestData[9] != DEST_SIGNATURE) // if something wrong reset count
	{
		DestData[8] = 0.0;
		DestData[9] = DEST_SIGNATURE;
	}
	
	// write 10 x 8 bytes of the buffer to the 2nd MByte of the Flash Chip
	ProgramFlash((volatile char *)DestData,80,FLASH_USER,""); 

	// Blink LED to indicate save
	for (i=0; i<8; i++) {SetStateBit(46,i&1);Delay_sec(0.1);}
}

int RestoreDestinations(void)
{	
	int i;
	
	if (persist.UserData[NONVOL_FLAGVAR]!=0) return;  // only allow them to be restored once on power up

	persist.UserData[NONVOL_FLAGVAR]=1;
	
	SetFlashBank(FLASH_USER);  // Flash must be bank selected

	if (FLASHD[9] != DEST_SIGNATURE) return 1; // if something wrong exit

	// copy Flash to Dests and Positions
	for (i=0; i<8; i++) chan[i].Position = chan[i].Dest = FLASHD[i];  // copy Flash to Dests
	
	return 0;
}


