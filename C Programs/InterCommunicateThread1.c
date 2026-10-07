#include "KMotionDef.h"

int MyData[300]; // declare some data to share
int *pMyData=MyData; // declare a pointer to the data

main()
{
	int result=0;
	//give other Threads the address by passing in persist variable 10
	persist.UserData[10] = pMyData; 

	while(1){ 
		Delay_sec(1.0);
		pMyData[299] = result++;
		if(result>10000) result=0;
  	}
}
