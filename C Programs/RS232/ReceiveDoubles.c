#include "KMotionDef.h"

// RS232 Buffered Receive Doubles example
// Protocol = StartCode, Command Code, 8 byte double, then acknowledge 

#define STARTCODE 0xAA
#define ACKCODE 0xBB

// GetChar with timeout, return 1 if Timeout, 0 otherwise
int GetCharTimeOut(unsigned char *c)
{
	double TF;

	// wait 1 second for response
	TF = Time_sec() + 1.0;
	while (pRS232RecIn == pRS232RecOut) // wait for data in buffer
		if (Time_sec() > TF) return 1;
	
	*c = RS232_GetChar();
	return 0;
}

// receive floating point double as 8 bytes
// return 0 if successful, 2 Timeout
int ReceiveDouble(double *v)
{
	int i;

	for (i=0;i<8;i++)
		if (GetCharTimeOut(((unsigned char *)v)+i)) return 2;
		
	// send acknowledge
	RS232_PutChar(ACKCODE);
	return 0;
}

void main()
{
	double a,b;
	unsigned char c;
	
	EnableRS232Cmds(RS232_BAUD_57600);
	DoRS232Cmds = FALSE;  // turn off processing RS232 input as commands
	
	for (;;)
	{
		c=RS232_GetChar();
		printf("Character received %02X\n",c);
		
		if (c == STARTCODE)  // ignore if not start code
		{
			printf("Start Code Received\n");
			if (!GetCharTimeOut(&c))  // get command code
			{
				printf("Command %d Received\n", c);
			
				if (c == 1)
				{
					ReceiveDouble(&a);
					printf("a = %f\n", a);
				}
				if (c == 2)
				{
					ReceiveDouble(&b);
					printf("b = %f\n", b);
				}
			}
		}
	}
}
