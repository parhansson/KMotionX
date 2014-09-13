#include "KMotionDef.h"

// RS232 Buffered input/output example to echo out any received character

main()
{
	char c;
	
	EnableRS232Cmds(RS232_BAUD_57600);
	
	DoRS232Cmds = FALSE;  // turn off processing RS232 input as commands
	
	while (pRS232RecIn == pRS232RecOut); // wait for data in buffer

	c=RS232_GetChar();
	
	RS232_PutChar(c);
}
