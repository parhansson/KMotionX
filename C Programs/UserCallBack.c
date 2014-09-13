#include "KMotionDef.h"

double T0=0,T1=0,Delta=0;

// A User Program Call Back can be defined to be called every Servo Sample
// Set to Non-NULL for the the Callback to be made.  The Call Back Routine
// must return within a few micro seconds or the system may become unstable
//
//typedef void USERCALLBACK(void);
//extern USERCALLBACK *UserCallBack;


void CallBack(void)
{
	T0=Time_sec();
	Delta = T0-T1;
	T1=T0;
}

main()
{
	int i;
	
	// sample the call back activity a few times
		
	for (i=0;i<10;i++)
	{
		if (i==4) UserCallBack = CallBack;  // start the callback
		
		Delay_sec(1);
		
		printf("Delta Time between last two callbacks = %.3fus\n",Delta*1e6);
	}
	
	// disable the Call Back.  note: if we don't do this the callbacks to the routine in
	// this Threadspace will continue even after this tread terminates and loading different
	// code into this thread may cause a crash
	
	UserCallBack = NULL;
}


