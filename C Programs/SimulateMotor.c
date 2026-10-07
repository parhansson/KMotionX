#include "KMotionDef.h"

double Sum,V=0, P, Pi;
#define N 10
double D[N];
int index=0;

#define floor(x) (x<0.0 ? ((int)x)-1 : (int)x);

void MyCallBack(void)
{
	V += 10000.0 * ch0->Output * 180e-6;  // assume output is an acceleration
	P += V * 180e-6; // assume Vel in counts/sec
	Pi = floor(P);

	Sum = Sum - D[index] + Pi; // remove oldest insert new
	D[index] = Pi;
	if (++index == N) index=0;
//	ch0->Position = Sum * (1.0/N);
	ch0->Position = Pi;
}


main()
{
	int i;
	P=ch0->Position;
	for (i=0;i<N;i++) D[i]=P;
	Sum=N*P;
	printf("Output %f\n",ch0->Output);
	for (;;)
	{
		ch7->Dest=ch0->Output;
		if (ch0->Enable)
		{
			UserCallBack = MyCallBack;
			WaitNextTimeSlice();
		}
		else
		{
			UserCallBack = NULL;
			V=ch0->Position=0;
		}
	}
}

