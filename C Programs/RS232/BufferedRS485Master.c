#include "KMotionDef.h"

// RS485 Buffered input/output example to Send data to a Slave and get response

#define N 1024

main()
{
	int i,in=0,out=0;
	unsigned char c, r;
	double T=0;
	
	EnableRS422Cmds(1500000);
	DoRS422Cmds = FALSE;  // turn off processing RS422 input as commands
    RS422_SetBaudRate(100000, 8, FALSE, FALSE, TRUE);  // set RS485 mode
    

	while (in < 2*N)
	{
		if (pRS422RecIn != pRS422RecOut) // wait for data in buffer
		{
			c=RS422_GetChar();
			
			if (in == 2*N-1) 
				r = ~0xAA;
			else if (in == N-1) 
				r = 0xAA;
			else if (in<N)
				r = in & 0x7f;
			else
				r = ~(in & 0x7f);

			if (c  != r)
			{
				printf("Error in=%d out=%d %d %d\n",in, out, r, c);
				return;
			}
			in++;
		}
		
		if (out < N)
		{
			if (out == N-1) // last character?
				RS422_PutChar(out & 0xaa);
			else
				RS422_PutChar(out & 0x7f);
			out++;
		}
		
		if (Time_sec() > T)
		{
			static lastin=-1;
			static int lastout=-1;
//			if (in != lastin || lastout != out)
			{
				printf("in=%d out=%d\n",in, out);
				lastin=in;
				lastout=out;
				T = Time_sec() +0.5;
			}
		}
	}
}
