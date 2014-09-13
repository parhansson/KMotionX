#include "KMotionDef.h"

#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

#define XSELECT  16  // these inputs indicate which axis is to be moved
#define YSELECT  17
#define ZSELECT  18

#define MAX_SPEED_X 500.0
#define MAX_SPEED_Y 500.0
#define MAX_SPEED_Z 500.0

#define MIN_THRESHOLD 0.03

void DoSpeedAxis(int ch, int Select, double *LastSpeed, double Speed);

double T0=0.0,T1;

main()
{
	double Pot,LastSpeed_X=0.0,LastSpeed_Y=0.0,LastSpeed_Z=0.0;
	
	printf("%f\n",KANALOG_CONVERT_ADC_TO_VOLTS(ADC(0)));
	
	for (;;)
	{
		T1=WaitNextTimeSlice();

		if (T1-T0 > 0.05)  // only change speed every so often
		{
			// convert 0-10V input to -1.0 to 1.0 range
			// Pot mid range of 5V will be zero
			Pot=KANALOG_CONVERT_ADC_TO_VOLTS(ADC(0)) * (1.0/5.0) - 1.0;
			
			// force small values to exactly zero (stopped)
			if (Pot < MIN_THRESHOLD && Pot > -MIN_THRESHOLD) Pot=0;

			DoSpeedAxis(XAXIS, XSELECT, &LastSpeed_X, Pot*MAX_SPEED_X);
			DoSpeedAxis(YAXIS, YSELECT, &LastSpeed_Y, Pot*MAX_SPEED_Y);
			DoSpeedAxis(ZAXIS, ZSELECT, &LastSpeed_Z, Pot*MAX_SPEED_Z);
		}
	}
}

void DoSpeedAxis(int ch, int Select, double *LastSpeed, double Speed)
{
	if (ReadBit(Select))  //is this axis selected ??
	{
		Jog(ch,Speed);
		*LastSpeed = Speed;
		T0=T1;  // save last time speed was changed
	}
	else if (*LastSpeed != 0.0) //not selected so stop if was moving
	{
		 Jog(ch,0.0);
		*LastSpeed=0.0;
	}
}