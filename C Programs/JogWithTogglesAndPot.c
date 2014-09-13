#include "KMotionDef.h"

#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

#define XPLUS  16
#define XMINUS 17
#define YPLUS  18
#define YMINUS 19
#define ZPLUS  20
#define ZMINUS 21

#define MAX_SPEED_X 500.0
#define MAX_SPEED_Y 500.0
#define MAX_SPEED_Z 500.0

void DoSpeedAxis(int ch, int Plus, int Minus, double *LastSpeed, double Speed);

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
			Pot=KANALOG_CONVERT_ADC_TO_VOLTS(ADC(0)) * (1.0/5.0);

			DoSpeedAxis(XAXIS, XPLUS, XMINUS, &LastSpeed_X, Pot*MAX_SPEED_X);
			DoSpeedAxis(YAXIS, YPLUS, YMINUS, &LastSpeed_Y, Pot*MAX_SPEED_Y);
			DoSpeedAxis(ZAXIS, ZPLUS, ZMINUS, &LastSpeed_Z, Pot*MAX_SPEED_Z);
		}
	}
}

void DoSpeedAxis(int ch, int Plus, int Minus, double *LastSpeed, double Speed)
{
	if (ReadBit(Plus))  //toggle to plus direction?
	{
		Jog(ch,Speed);
		*LastSpeed = Speed;
		T0=T1;  // save last time speed was changed
	}
	else if (ReadBit(Minus)) //toggle to minus direction
	{
		Jog(ch,-Speed);
		*LastSpeed = -Speed;
		T0=T1; // save last time speed was changed
	}
	else  //neither toggle so stop, if axis isn't already
	{
		if (*LastSpeed != 0.0) Jog(ch,0.0);
		*LastSpeed=0.0;
	}
}