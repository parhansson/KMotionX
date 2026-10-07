#include "KMotionDef.h"

void main()
{
	int FreezeIndex,LastIndex = -1;
	for (;;)
	{
		do
		{
			FreezeIndex=ParametricIndex+1; 
		}
		while (FreezeIndex == LastIndex) ;
		printf("ParametricIndex=%d (%d bytes) %.2f %% TimeDownloaded=%.3fsec\n",FreezeIndex, 
			FreezeIndex*sizeof(ParametricCoeffs[0]),
			FreezeIndex*sizeof(ParametricCoeffs[0])/(1024.0*1024.0-4.0)*100.0,
			CS0_TimeDownloaded);
		LastIndex = FreezeIndex;
		Delay_sec(0.5);
	}
}
