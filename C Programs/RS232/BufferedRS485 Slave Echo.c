#include "KMotionDef.h"

// RS232 Buffered input/output slave example
// receive data until an 0xAA is received
// transmit back same data but complimented
// discard received data that was transmitted

#define N 10000
unsigned char InBuffer[N];

main()
{
	int i,in,out;
	unsigned char c;
	
	EnableRS422Cmds(100000);
	DoRS422Cmds = FALSE;  // turn off processing RS422 input as commands
    RS422_SetBaudRate(100000, 8, FALSE, FALSE, TRUE);  // set RS485 mode
    
    for (;;)
    {
		in=out=0;
		
		for (;;)  // capture bytes until 
		{
			if (pRS422RecIn != pRS422RecOut) // wait for data in buffer
			{
				c=RS422_GetChar();
				
				InBuffer[in++] = c;
				
				if (c == 0xAA) break;
			}
		}
		printf("Slave received %d characters\n",in);

		for (out=0; out<in; out++) // send back data inverted
		{
			RS422_PutChar(~InBuffer[out]);
		}

		printf("Slave returned %d characters complemented\n",in);

		for (i=0; i<in;) // discard the data we just transmitted
		{
			if (pRS422RecIn != pRS422RecOut) // wait for data in buffer
			{
				c=RS422_GetChar();
				
				InBuffer[i++] = c;
			}
		}
	}
}
