#include "KMotionDef.h"
#include "ModBusSlave.h"


int main()
{
	unsigned short mycopy[N_MB_RW_REGISTERS];
	unsigned int myvbits;
	unsigned int mybits[10];
	int i=0;

	printf("Start Monitoring:\n");

	while(1)
	{
		double LastInTime=Time_sec();
		while (LastInTime+1>Time_sec())
			;
		for (i=0;i<N_MB_RW_REGISTERS;i++)
			if (mycopy[i]!=MBRWReg[i])
			{
				printf("MBRWReg[%d] was %04x now %04x\n",i,mycopy[i],MBRWReg[i]);
				mycopy[i]=MBRWReg[i];
			}
		if (myvbits!=VirtualBits)
		{
			printf("VirtualBits was %08x now %08x\n",myvbits,VirtualBits);
			myvbits=VirtualBits;
		}
		for (i=0;i<4;i++)
			if (mybits[i]!=VirtualBitsEx[i])
			{
				printf("VirtualBitsEx[%d] was %08x now %08x\n",i,mybits[i],VirtualBitsEx[i]);
				mybits[i]=VirtualBitsEx[i];
			}
	}
}