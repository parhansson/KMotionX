#include "KMotionDef.h"
#include "ModBusSlave.h"

// Simple example to read an ModBus register that is 
// being updated by the KFLOP Modbus Slave Process 

int main()
{
	printf("Last Modbus Register 40003 = %d\n", MBRWReg[2]);
}