#include "KMotionDef.h"
#include "SimpleHomeIndexFunction.c" 

main()
{
	int result;


	// HOME Z
	
	result = SimpleHomeIndexFunction(2,  // axis number to home
			   1000.0,  // speed to move toward home
			   -1,      // direction to move toward home (+1 or -1)
			   138, 	// limit bit number to watch for
			   0,		// limit polarity to wait for (1 or 0)
               100.0,	// speed to move while searching for index
			   -1,		// index bit number to watch for (use -1 for none)
			   1, 		// index polarity to wait for (1 or 0)
			   5000);	// amount to move inside limits
			   
	if (result==0)
	{
		printf("Home Z succeded\n");
	}
	else
	{
		printf("Home Z failed\n");
		return;
	}
	
	// HOME X

	result = SimpleHomeIndexFunction(0,  // axis number to home
			   1000.0,  // speed to move toward home
			   -1,      // direction to move toward home (+1 or -1)
			   136, 	// limit bit number to watch for
			   0,		// limit polarity to wait for (1 or 0)
               100.0,	// speed to move while searching for index
			   -1,		// index bit number to watch for (use -1 for none)
			   1, 		// index polarity to wait for (1 or 0)
			   5000);	// amount to move inside limits
			   
	if (result==0)
	{
		printf("Home X succeded\n");
	}
	else
	{
		printf("Home X failed\n");
		return;
	}
	
	// HOME Y

	result = SimpleHomeIndexFunction(1,  // axis number to home
			   1000.0,  // speed to move toward home
			   -1,      // direction to move toward home (+1 or -1)
			   137, 	// limit bit number to watch for
			   0,		// limit polarity to wait for (1 or 0)
               100.0,	// speed to move while searching for index
			   -1,		// index bit number to watch for  (use -1 for none)
			   1, 		// index polarity to wait for (1 or 0)
			   5000);	// amount to move inside limits
			   
	if (result==0)
	{
		printf("Home Y succeded\n");
	}
	else
	{
		printf("Home Y failed\n");
		return;
	}
}

