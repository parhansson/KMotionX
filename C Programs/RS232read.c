#include "KMotionDef.h"

void ReceiveChar()
{
	// wait for data in buffer
	while ((FPGA(RS232_STATUS) & 1)==0);
	return FPGA(RS232_DATA);
}

main()
{
	SetBitDirection(45,1);
	FPGA(RS232_BAUD_REG) = RS232_BAUD_38400;
//	FPGA(KAN_TRIG_REG) = 1;  // enable Kanalog to get RS232 working
	for (;;)
	{
		while ((FPGA(RS232_STATUS) & RS232_DATA_READY) == 0) ;
		printf("%X\n",ReceiveChar());
	}
}