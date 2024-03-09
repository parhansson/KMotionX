#include "KMotionDef.h"

// RS232 Buffered Send Doubles example
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


// send floating point double as 8 bytes
// return 0 if successful, 1 fail, 2 Timeout
int SendDouble(double *v, unsigned char cmd)
{
	int i;
	unsigned char c;

	RS232_PutChar(STARTCODE);	
	RS232_PutChar(cmd);	

	for (i=0;i<8;i++)
		RS232_PutChar(((char *)v)[i]);
		
	// check for acknowledge
	if (GetCharTimeOut(&c)) return 2;
	if (c != ACKCODE) return 1;
	return 0;
}


void main()
{
	double a=123.456, b=9e6;
	
	EnableRS232Cmds(RS232_BAUD_57600);
	DoRS232Cmds = FALSE;  // turn off processing RS232 input as commands

	if (SendDouble(&a, 1))   //Send a
		printf("Failed\n");

	if (SendDouble(&b, 2))   //Send b
		printf("Failed\n");
}
