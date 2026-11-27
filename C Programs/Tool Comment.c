#include "KMotionDef.h"
#define TMP 10					// which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


int FixtureIndex, Units, TWORD, HWORD, DWORD;


int main()
{
	char s[200];
	
	// Set Tool Table Comment
	GetMiscSettings(&Units, &TWORD, &HWORD, &DWORD);
	SetToolComment(0, TWORD, "Junk 1100 World");

	// Read Tool Table Comment
	GetToolComment(0, TWORD, s);
	printf("%s\n",s);
}
