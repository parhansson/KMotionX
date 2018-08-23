#include "KMotionDef.h"

#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


main()
{
	int Answer;
	double D;

//	DoPC(PC_COMM_ESTOP);
//	DoPC(PC_COMM_HALT);
//	DoPC(PC_COMM_EXECUTE);
//	DoPC(PC_COMM_SINGLE_STEP);
//	DoPCFloat(PC_COMM_SET_FRO,0.25f);
//	DoPCFloat(PC_COMM_SET_FRO_INC,1.1f);
//	DoPCFloat(PC_COMM_SET_X,0.0);
//	DoPCFloat(PC_COMM_SET_Y,0.0);
//	DoPCFloat(PC_COMM_SET_FRO,1.1);
//	DoPCFloat(PC_COMM_SET_RRO,1.1);
//	DoPCFloat(PC_COMM_SET_FRO_INC,0.90);
//	DoPCFloat(PC_COMM_SET_RRO_INC,1.10);
//	DoPCFloat(PC_COMM_SET_Z,1.25);
//	DoPCInt(PC_COMM_USER_BUTTON,3);
//	DoPCInt(PC_COMM_MCODE,3);

//	Answer = MsgBox("Hello World",MB_YESNO|MB_ICONEXCLAMATION);
//	if (Answer == IDYES)
//		printf("Answer is Yes\n");
//	else
//		printf("Answer is No\n");
	

//	MDI("G0 X1.2 Y2.2 Z3.3");


	// put 3 double values in the persist vars 
	
	SetUserDataDouble(10,123.456);
	SetUserDataDouble(11,1000.0);
	SetUserDataDouble(12,999.9);
	
	// transfer up to the GCode Vars
	SetVars(100,3,10);  // Upload 3 to GCode 100 from persist 10   
	
	MDI("#100 = [#100 + 1]");
		
	// read them back into different persist Vars
	GetVars(100,3,13);  // Download 3 from GCode 100 to persist 13 
	
	printf("%f %f %f\n",GetUserDataDouble(13),GetUserDataDouble(14),GetUserDataDouble(15));
	
	// Request Tool #3 Length placed into persist double #16
	GetToolLength(3-1,&D);
	printf("Tool Length is %f\n",D);

	// Change Tool #3 Length to 12.34 passed up via persist double #16
	SetToolLength(3-1,D);
}
