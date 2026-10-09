#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

//Reads current line of GCode

int main()
{
	char s[250];
	
	GetGCodeLine(1000, s);  // get the line of GCode
	printf("%s\n",s); // rint it
}
