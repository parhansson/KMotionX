#include "KMotionDef.h"

// Demo for testing Active Damping of Resonant Vibrations
//
// Technique used is to feed back a portion of the Position Error delayed in time.
//
// GAIN is the portion that is to be fed BACKLASH_OFF
// Freq and DELAY_CYCLE determines the amount of Time Delay
// (Typical vales are Freq = Resonant Frequency of system and 0.75 of a cycle)
//
// This program sets up a Servo Sample callback to perform the calculations.
// Note that once the program sets up variables and the callback it Terminates
// However the callback continues to execute.  Downloading a new or modified
// program into the Thread space before stopping the Callback (UserCallBack = NULL)
// can cause a crash.  The program is setup to toggle the Callback On and Off each
// time it is executed 
// 
// A circular buffer is used to delay the signal for a Freq=137Hz and Delay_Cycle=0.75
// the signal is delayed 61 Servo Samples
//
// The program also contains anti-dither code where below a certain speed (100uSteps/sec)
// for at least 0.03sec the Closed Loop and Damping Feedback is disabled 
//
// The Damping can also be enabled and disabled by seatting/clearing persist variable 100  
 

#define Freq 137.0 			// Resonant Frequency to be supressed
#define DELAY_CYCLE 0.75	// Fraction of cycle to be delayed
#define GAIN 0.25f			// portion of error to be fed back

#define MAXD 1024

int nDelay;
float Delay[MAXD];

float TimeDelay(float I);
void DoDither(CHAN *ch, float P_original, float P_low, float I_original, double *T0, double T1);

// Anti dither variables
double T1,T0_0=0;  
float P_original_0;
float P_low_0;
float I_original_0;

// Called before every servo Sample
void CallBack(void)
{
	// Compute Error and Time Delay it
	ch0->DestOffset = TimeDelay(ch0->Dest-ch0->Position)*GAIN;

	// Force no Damping if Stopped (Integrator off) or User on/off
	if (ch0->I < 1e-6f || persist.UserData[100]==0.0) ch0->DestOffset=0.0;
	
	// maintain Dither mode
	T1=Time_sec();  // returns current time
	DoDither(ch0, P_original_0, P_low_0, I_original_0, &T0_0, T1);
}


main()
{
	int i;
	
	for (i=0;i<MAXD;i++)Delay[i]=0.0f;  // clear delay buffer
	
	// Compute number of samples signal should be delayed
	nDelay = (int)(DELAY_CYCLE*((1.0/Freq)/TIMEBASE)+0.5);
	printf("nDelay = %d\n",nDelay);
	
	// define Closed Loop PI parameters to be used while moving and when stopped
	P_original_0 = 0.05;
	P_low_0      = 0.0;
	I_original_0 = 0.0008;
	
	//Toggle Callback On and Off
	if (UserCallBack)
	{
		printf("Callback Off\n");
		UserCallBack = NULL;  //Stop the callback
	}
	else
	{
		printf("Callback On\n");
		UserCallBack = CallBack;  // start the callback
	}
}

// Performs Time Delay by saving New Value into a Circular
// buffer and retrieving older value delayed by nDelay samples 
float TimeDelay(float I)
{
	static int iDelay=0;
	int i;

	// Save new value
	Delay[iDelay]=I;
	
	// Get old value
	i=iDelay++ - nDelay;
	if (iDelay>=MAXD)iDelay=0;

	if (i<0) i+=MAXD;  // wrap?
	return Delay[i];
}


#define SMALL_VEL 100 // step/sec
#define LONG_TIME 0.03

// Change Gains for one Axis Based on being nearly stopped for a least some period of time
void DoDither(CHAN *ch, float P_original, float P_low, float I_original, double *T0, double T1)
{
	if (fast_fabs(ch->last_vel) > SMALL_VEL) *T0=T1;

	if (T1 - *T0 > LONG_TIME)// error for a long time
	{
		ch->P = P_low;   // reduced gain
		ch->I = 0.0;     // no integrator
	}
	else
	{
		ch->P = P_original;    // normal gain
		ch->I = I_original;    // normal integrator
	}
}
