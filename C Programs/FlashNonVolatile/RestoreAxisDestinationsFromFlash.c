#include "KMotionDef.h"

#define FLASHD ((volatile double *)FLASH_USER)

void SetFlashBank(volatile unsigned short *add);
int RestoreDestinations(void);

#define DEST_SIGNATURE 123.456
#define NONVOL_FLAGVAR 150 // persist var to remember we restored

double DestData[10];  // room for 8 axes, count, signature

main()
{
	RestoreDestinations();
}

int RestoreDestinations(void)
{	
	int i;
	
	if (persist.UserData[NONVOL_FLAGVAR]!=0) return;  // only allow them to be restored once on power up

	persist.UserData[NONVOL_FLAGVAR]=1;
	
	SetFlashBank(FLASH_USER);  // Flash must be bank selected

	for (i=0;i<80;i++) printf("%d %x\n",i,FLASH_USER[i]);
	for (i=0;i<10;i++) printf("%d %g\n",i,FLASHD[i]);
	
	if (FLASHD[9] != DEST_SIGNATURE) return 1; // if something wrong exit

	// copy Flash to Dests and Positions
	for (i=0; i<8; i++) chan[i].Position = chan[i].Dest = FLASHD[i];  // copy Flash to Dests
	
	return 0;
}
