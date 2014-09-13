#include "KMotionDef.h"

//Plugin calls for Mach3 NotifyPlugins Commands


void Tap(void);

main()
{
	int msg = persist.UserData[6];  // Mach3 notify Message 10000-10999

	printf("Mach3 Notify Call, Message = %d\n",msg); 
	
	if (msg==10084)
	{
		Tap();
	}
}

//  R I G I D   T A P P I N G 

#define ZAXIS 7
#define SPINDLE_AXIS 6
#define Z_CNTS_PER_INCH -20000.0
#define CNTS_PER_REV (8192*14/16)
#define TAU 0.001

double SlaveGain,ToCut,TotalCut,Z0,S0;
void DoSlave(void);
void DoTap(double Dist, double Rate, double TPI);

void Tap(void)
{
	//  #10  1010    18 19          TPI
	//  #11  1011    20 21          Feed Rate in/min
	//  #12  1012    22 23          Retract Rate in/min
	//  #13  1013    24 25          Z depth Total inches
	//  #14  1014    26 27          Z depth Forward per Motion
	//  #15  1015    28 29          Z depth Retract per Motion

	double TPI 			= *(double *)&persist.UserData[18];
	double FeedRate 	= *(double *)&persist.UserData[20]/60.0;
	double RetractRate 	= *(double *)&persist.UserData[22]/60.0;
	double ZDist 		= *(double *)&persist.UserData[24];
	double ZForward		= *(double *)&persist.UserData[26];
	double ZReverse		= *(double *)&persist.UserData[28];
	
	printf("TPI = %f\n",TPI);
	printf("FeedRate = %f\n",FeedRate);
	printf("RetractRate = %f\n",RetractRate);
	printf("ZDist = %f\n",ZDist);
	printf("ZForward= %f\n",ZForward);
	printf("ZReverse = %f\n",ZReverse);

	// Slave the Z Axis to the Spindle
	SlaveGain = Z_CNTS_PER_INCH/(CNTS_PER_REV * TPI);
	Z0 = chan[ZAXIS].Dest;
	S0 = chan[SPINDLE_AXIS].Dest;

	// in case there is significant spindle position error move there first
	Move(ZAXIS,(chan[SPINDLE_AXIS].Position-S0)*SlaveGain+Z0);
	while (!CheckDone(ZAXIS)) ;
	
	TotalCut=0.0;
	while (TotalCut < ZDist)
	{
		if (TotalCut + ZForward > ZDist) // last feed
		{
			// yes, do any remaining
			DoTap(ZDist-TotalCut, FeedRate, TPI);
			// retract fully
			DoTap(-ZDist, RetractRate, TPI);
			TotalCut=ZDist;
		}
		else
		{
			// no, just cut a bit
			DoTap(ZForward, FeedRate, TPI);
			DoTap(-ZReverse, RetractRate, TPI);
			TotalCut+=ZForward-ZReverse;
		}
	}
		
	Delay_sec(1.0);
	Move(ZAXIS,Z0);   // move back to where we started 
	while (!CheckDone(ZAXIS)) ;
}

void DoTap(double Dist, double Rate, double TPI)
{
	// Tap down
	MoveRelAtVel(SPINDLE_AXIS, Dist*TPI*CNTS_PER_REV, Rate*TPI*CNTS_PER_REV);
	
	while(!CheckDone(SPINDLE_AXIS))
		DoSlave();
}

void DoSlave(void)
{
	MoveExp(ZAXIS,(chan[SPINDLE_AXIS].Dest-S0)*SlaveGain+Z0, TAU);
	WaitNextTimeSlice();
}