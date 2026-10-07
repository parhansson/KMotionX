#include "KMotionDef.h"

void ServiceWatchdogStatus(void);
void WatchdogTripped(void);
void WatchdogOK(void);

#define WATCHDOG_DELAY 5.0 // seconds after host stops to Trigger

// for testing
main()
{
	for (;;)
	{
		WaitNextTimeSlice();
		ServiceWatchdogStatus();
	}
}


// Watchdog Trips after all host applications stop requesting status
// Watchdog OK is called when communication and status requests Resume

void ServiceWatchdogStatus(void)
{
	static int Alive=FALSE;
	static int PrevStatusRequestCounter=-1;
	static double WatchdogTime=0;
	double T=Time_sec();
	
	// check if Host is requesting Status
	if (StatusRequestCounter != PrevStatusRequestCounter)
	{
		// yes, save time 
		WatchdogTime = T + WATCHDOG_DELAY;
		PrevStatusRequestCounter=StatusRequestCounter;
		if (!Alive) WatchdogOK();
		Alive=TRUE;
	}
	else
	{
		if (T > WatchdogTime)  // time to trigger?
		{
			if (Alive) WatchdogTripped();
			Alive=FALSE; 
		}
	}
}

void WatchdogOK(void)
{
	ClearBit(47);  // KFLOP LED off
	Delay_sec(1);
	SetBit(47);
	Delay_sec(1);
}

void WatchdogTripped(void)
{	
	ClearBit(46);  // KFLOP LED off
	Delay_sec(1);
	SetBit(46);
	Delay_sec(1);
}



