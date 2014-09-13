#include "KMotionDef.h"

void SendChar(char c)
{
	while (FPGA(RS232_STATUS) & RS232_TRANSMIT_FULL) ;
	FPGA(RS232_DATA) = c;
}



main()
{
	int i;
	SetBitDirection(45,1);
	FPGA(RS232_BAUD_REG) = RS232_BAUD_38400;
//	FPGA(KAN_TRIG_REG) = 1;  // enable Kanalog to get RS232 working
	for (i=0;i<100;i++)
	{
		SendChar('A');
	}
}
