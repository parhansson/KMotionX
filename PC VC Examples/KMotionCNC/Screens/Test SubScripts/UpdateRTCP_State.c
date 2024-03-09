#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

// Displays RTCP mode status as a button text and color
//
// Monitors RTCP mode as part of HostStatus and executes 
// Screen SubScript to change text and color of a button
//
// Also looks for a command (Virtual bit 1200) as a request
// to toggle the mode. If command is detected we request the
// current Tool Number and MDI a G43.4Hn for that tool number
// to turn RTCP on for that tool.
 
void UpdateRTCP(void);

main()
{
	for (;;)
	{
		WaitNextTimeSlice();
		UpdateRTCP();
	}
}

#define ToggleRTCP_BIT 1200 // Set by Button to toggle mode
#define RTCP_UPDATE_TIME 2 // update every 2 sec in case out of sync

void UpdateRTCP(void)
{
	static double LastT=0;
	static int PrevRTCP=-1;
	
	int Slot, ID, HWORD;
	char s[80];


	if (ReadBit(ToggleRTCP_BIT))  // Toggle request
	{
		ClearBit(ToggleRTCP_BIT);  // clear command
		
		if (RTCP_ACTIVE)
		{
			printf("G49\n");
			MDI("G49");  // turn it off
		}
		else
		{
			GetToolSlotAndID(&Slot, &ID); // Get currently selected tool
			printf("Slot = %d ID = %d\n",Slot, ID);
			if (Slot == 0) HWORD=ID; else HWORD=Slot;
			sprintf(s,"G43.4 H%d",HWORD);
			printf("%s\n",s);
			MDI(s);  // turn it on
		}
	}
	
	// if mode changed or has been a while update button
	if (RTCP_ACTIVE != PrevRTCP || Time_sec() > LastT)
	{
		LastT = Time_sec() + RTCP_UPDATE_TIME;
		PrevRTCP = RTCP_ACTIVE;
		if (PrevRTCP)
		{
			printf("ON\n");
			ScreenScript("SScript:ChangeRTCPon.scr");
		}
		else
		{
			printf("OFF\n");
			ScreenScript("SScript:ChangeRTCPoff.scr");
		}
	}
}
