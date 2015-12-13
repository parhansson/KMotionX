#include "KMotionDef.h"

// Bit bang output to 8-bit SPI Device ~450Kbps

#define pMOSI 0
#define pSS 1
#define pMISO 2
#define pSCK 3

int SPI_OUT(int data);

#define N 1000


main()
{
	int i;
	double TX,T0,T1,T2,T3,T4;
	
	TX=Time_sec();
	T0=Time_sec();
	SPI_OUT(0xAA);
	T1=Time_sec();
	SPI_OUT(0xAA);
	T2=Time_sec();
	SPI_OUT(0xAA);
	T3=Time_sec();
	for (i=0;i<N;i++)
		SPI_OUT(0xAA);
	T4=Time_sec();

	printf("Times %f %f %f %f us\n",(T1-T0-(T0-TX))*1e6,
									(T2-T1-(T0-TX))*1e6,
									(T3-T2-(T0-TX))*1e6,
									(T4-T3-(T0-TX))*1e6/N);
}

int SPI_OUT(int data)
{
	int i;
	int dataIn=0;

   	FPGA(BIT_CLR+(pSS>>3)) = ~(1<<(pSS&7)); 
   	
   	
	if (data & 0x80)
		FPGA(BIT_SET+(pMOSI>>3)) = 1<<(pMOSI&7);
	else
		FPGA(BIT_CLR+(pMOSI>>3)) = ~(1<<(pMOSI&7));
		
	dataIn = (dataIn << 1) | ((FPGA(BIT_READ+(pMISO>>3))>>(pMOSI&7)) & 1);

	FPGA(BIT_SET+(pSCK>>3)) = 1<<(pSCK&7);
	FPGA(BIT_CLR+(pSCK>>3)) = ~(1<<(pSCK&7));


	if (data & 0x40)
		FPGA(BIT_SET+(pMOSI>>3)) = 1<<(pMOSI&7);
	else
		FPGA(BIT_CLR+(pMOSI>>3)) = ~(1<<(pMOSI&7));
		
	dataIn = (dataIn << 1) | ((FPGA(BIT_READ+(pMISO>>3))>>(pMOSI&7)) & 1);
		
	FPGA(BIT_SET+(pSCK>>3)) = 1<<(pSCK&7);
	FPGA(BIT_CLR+(pSCK>>3)) = ~(1<<(pSCK&7));

	if (data & 0x20)
		FPGA(BIT_SET+(pMOSI>>3)) = 1<<(pMOSI&7);
	else
		FPGA(BIT_CLR+(pMOSI>>3)) = ~(1<<(pMOSI&7));
		
	dataIn = (dataIn << 1) | ((FPGA(BIT_READ+(pMISO>>3))>>(pMOSI&7)) & 1);
		
	FPGA(BIT_SET+(pSCK>>3)) = 1<<(pSCK&7);
	FPGA(BIT_CLR+(pSCK>>3)) = ~(1<<(pSCK&7));

	if (data & 0x10)
		FPGA(BIT_SET+(pMOSI>>3)) = 1<<(pMOSI&7);
	else
		FPGA(BIT_CLR+(pMOSI>>3)) = ~(1<<(pMOSI&7));
		
	dataIn = (dataIn << 1) | ((FPGA(BIT_READ+(pMISO>>3))>>(pMOSI&7)) & 1);
		
	FPGA(BIT_SET+(pSCK>>3)) = 1<<(pSCK&7);
	FPGA(BIT_CLR+(pSCK>>3)) = ~(1<<(pSCK&7));

	if (data & 0x08)
		FPGA(BIT_SET+(pMOSI>>3)) = 1<<(pMOSI&7);
	else
		FPGA(BIT_CLR+(pMOSI>>3)) = ~(1<<(pMOSI&7));
		
	dataIn = (dataIn << 1) | ((FPGA(BIT_READ+(pMISO>>3))>>(pMOSI&7)) & 1);
		
	FPGA(BIT_SET+(pSCK>>3)) = 1<<(pSCK&7);
	FPGA(BIT_CLR+(pSCK>>3)) = ~(1<<(pSCK&7));

	if (data & 0x04)
		FPGA(BIT_SET+(pMOSI>>3)) = 1<<(pMOSI&7);
	else
		FPGA(BIT_CLR+(pMOSI>>3)) = ~(1<<(pMOSI&7));
		
	dataIn = (dataIn << 1) | ((FPGA(BIT_READ+(pMISO>>3))>>(pMOSI&7)) & 1);
		
	FPGA(BIT_SET+(pSCK>>3)) = 1<<(pSCK&7);
	FPGA(BIT_CLR+(pSCK>>3)) = ~(1<<(pSCK&7));

	if (data & 0x02)
		FPGA(BIT_SET+(pMOSI>>3)) = 1<<(pMOSI&7);
	else
		FPGA(BIT_CLR+(pMOSI>>3)) = ~(1<<(pMOSI&7));
		
	dataIn = (dataIn << 1) | ((FPGA(BIT_READ+(pMISO>>3))>>(pMOSI&7)) & 1);
		
	FPGA(BIT_SET+(pSCK>>3)) = 1<<(pSCK&7);
	FPGA(BIT_CLR+(pSCK>>3)) = ~(1<<(pSCK&7));

	if (data & 0x01)
		FPGA(BIT_SET+(pMOSI>>3)) = 1<<(pMOSI&7);
	else
		FPGA(BIT_CLR+(pMOSI>>3)) = ~(1<<(pMOSI&7));
		
	dataIn = (dataIn << 1) | ((FPGA(BIT_READ+(pMISO>>3))>>(pMOSI&7)) & 1);
		
	FPGA(BIT_SET+(pSCK>>3)) = 1<<(pSCK&7);
	FPGA(BIT_CLR+(pSCK>>3)) = ~(1<<(pSCK&7));


   	FPGA(BIT_SET+(pSCK>>3)) = 1<<(pSCK&7); 

	return dataIn;
}
