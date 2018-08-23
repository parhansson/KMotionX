#include "KMotionDef.h"
#include "CoordMotionInKFLOPFunctions.c"

void ServiceFeedHold(void);
int Debounce(int n, int *cnt, int *last, int *lastsolid);

// state variables for switch debouncing
int flast=0,flastsolid=-1,fcount=0;

#define FEEDHOLDBIT 162

main()
{
    // Do an example xy coordinated Move
	do
	{
		DefineCoordSystem(0,1,2,-1);  // x,y,z
	   
		VM=10000; // Max allowed feed rate counts/sec
		A=4000; // Max acceleration counts/sec2
	   
	   
		OpenBuf();  // open the coordinated motion buffer
	   
		x0=y0=z0=0.0; // starting point
		x1=30000;
		y1=20000;
		z1=50000.0; // ending point
		DoLinear();

		//now move back
	   
		x0=30000;
		y0=20000;
		z0=50000.0; // starting point
		x1=y1=z1=0.0; // ending point
		DoLinear();



		CS0_Flushed=TRUE;	// Allow Motion to run to end
		ExecBuf();  // launch the motion
	   
		while (!CheckDoneBuf())  // Service Feed Hold while waiting till finished 
			ServiceFeedHold(); 
			
    }while(TRUE); // loop forever
}

void ServiceFeedHold(void)
{	
	int result;

	// Handle FeedHold/Resume
	result = Debounce(ReadBit(FEEDHOLDBIT),&fcount,&flast,&flastsolid);
	if  (result == 1)
	{
		if (CS0_StoppingState == 0)
			StopCoordinatedMotion();
		else
			ResumeCoordinatedMotion();
	}
}



// Debounce a bit
//
// return 1 one time when first debounced high 
// return 0 one time when first debounced low 
// return -1 otherwise 
#define DBTIME 300

int Debounce(int n, int *cnt, int *last, int *lastsolid)
{
	int v = -1;
	
	if (n == *last)  // same as last time?
	{
		if (*cnt == DBTIME-1)
		{
			if (n != *lastsolid)
			{
				v = *lastsolid = n;  // return debounced value
			}
		}
		if (*cnt < DBTIME)	(*cnt)++;
	}
	else
	{
		*cnt = 0;  // reset count
	}
	*last = n;
	return v;
}

