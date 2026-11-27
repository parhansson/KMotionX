#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

//Reads current date/time

int main()
{
	char s[250];
	
	GetDateTime(1000, s);  // get the date time string
	printf("%s\n",s); // print it
}
