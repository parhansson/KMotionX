#include "KMotionDef.h"

int *pMyData; // declare a pointer to the data

main()
{
	int msg;
	pMyData=persist.UserData[10]; //get address from other Thread
	
	while(1){ 
		Delay_sec(0.5);
		msg = pMyData[299];
		printf("msg:%d\n", msg);
	}
}
