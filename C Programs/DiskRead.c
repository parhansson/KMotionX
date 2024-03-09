#include "KMotionDef.h"

// Read PC Disk Text File Example

main()
{
	FILE *f;
	char s[256];
	
	// open some text file in read text mode
	f=fopen("c:\\KMotion433k\\C Programs\\Print.c","rt");
	
	if (!f)  // opened sucessfully??
	{
		printf("Unable to open file\n");
		return;
	}
	
	// loop getting/printing lines to the Console Screen
	
	while (!feof(f)) // end of file?
	{
		fgets(s,100,f);  // no, try to read a line
		
		if (!feof(f)) // was it at the end of file? 
			printf("%s\n",s); // no. print it to Console Screen
	}
	
	fclose(f);
}
