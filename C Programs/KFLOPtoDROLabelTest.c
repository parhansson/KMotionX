#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


main()
{
	int i;
	char s[80];
	
	for (i=0;i<10;i++)
	{
		sprintf(s,"Test %d",i); // format character string
		DROLabel(1000,150,s);  // use gather offset 1000 and persist var 150 to send label
		Delay_sec(0.5);
	}
}
