#include "KMotionDef.h"
#define TMP 10					// which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

#define StockWidth 4.0
#define StockDepth 3.0
#define StockHeight 1.0

#define Length 4.0
#define PROBETIPDIAMETER 0.125f 		// #121		Diameter of probe tip

void main()
{
	double x,y,z,xRes,yRes,zRes;
	printf("Getting Axis Resolution\n");
	DoPCInt(PC_COMM_GETAXISRES, TMP);

	xRes = *(float *)&persist.UserData[TMP];
	yRes = *(float *)&persist.UserData[TMP+1];
	zRes = *(float *)&persist.UserData[TMP+2];

	printf("Axis Resolutions Found:\n\tx: %f\n\ty: %f\n\tz: %f\n", xRes, yRes, zRes);

	for (;;)
	{
		x = ch0->Dest / xRes;
		y = ch1->Dest / yRes;
		z = ch2->Dest / zRes;
		SetStateBit(1032, !((fast_fabs(x) > (StockWidth-PROBETIPDIAMETER)/2 || 
                             fast_fabs(y) > (StockDepth-PROBETIPDIAMETER)/2) 
								&&
							(fast_fabs(x) < (StockWidth+PROBETIPDIAMETER) && 
                             fast_fabs(y) < (StockDepth+PROBETIPDIAMETER)) 
								&&
                            fast_fabs(z-Length-StockHeight/2) < StockHeight/2));
	}
}
