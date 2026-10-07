//*************************** Probe_ aftey Check *************************************

//**********************************************************************************************
// Note : printf sends a message to the Chip app which logs the message in the trace buffer.
//        Because KFlop library routines also use printf the messages need to be distguished from
//        messages sent from KFlop C programs, like this one.
//		  If the first character of the message is '@' or '*' then the Chip app knows that the
//        message is from a KFLop C program. '@' signifies a status or debug message and '*' an
//        error messaage. If you use printf, then include these in the message.
//**********************************************************************************************

#include "KMotionDef.h"

#define Verbose TRUE

//******************************************************************
// Note : All definitions must match the ones in the PC application
//******************************************************************

// User data locations
// Integer
#define Command 0
#define Status 1
#define Sync_Flag 2
#define Thread_Status     3 //Used to communicate across threads


//******************************************************************
// Thread Status
#define Thread_Status_Clear	0
#define Probe_Saftey	 	1
#define Toolsetter_Saftey	2


// Channel definitions
#define X_Axis 0
#define Y_Axis 1
#define Z_Axis 2

// Switch Definitions
#define Probe_Switch 				178
#define Movable_Toolsetter_Switch 	179
#define Fixed_Toolsetter_Switch 	180

//*************************************************************************


main ()
{
	persist.UserData[Thread_Status] = Thread_Status_Clear; 
	
	while(TRUE)
	{
		if (ReadBit(Fixed_Toolsetter_Switch) == 1)
			{
				StopCoordinatedMotion();
				if (persist.UserData[Thread_Status] == 0) printf("*Fixed Toolsetter Saftey Tripped\n");
				persist.UserData[Thread_Status] = Toolsetter_Saftey; 
				while(!CheckDone(X_Axis) || !CheckDone(Y_Axis) || !CheckDone(Z_Axis)) WaitNextTimeSlice();
				ClearStopImmediately();
			} 

		if (ReadBit(Movable_Toolsetter_Switch) == 1)
			{
				StopCoordinatedMotion();
				if (persist.UserData[Thread_Status] == 0) printf("*Movable Toolsetter Saftey Tripped\n");
				persist.UserData[Thread_Status] = Toolsetter_Saftey; 
				while(!CheckDone(X_Axis) || !CheckDone(Y_Axis) || !CheckDone(Z_Axis)) WaitNextTimeSlice();
				ClearStopImmediately();
			} 

		if (ReadBit(Probe_Switch) == 0)
			{
				StopCoordinatedMotion();
				if (persist.UserData[Thread_Status] == 0) printf("*Probe Saftey Tripped\n");
				persist.UserData[Thread_Status] = Toolsetter_Saftey; 
				while(!CheckDone(X_Axis) || !CheckDone(Y_Axis) || !CheckDone(Z_Axis)) WaitNextTimeSlice();
				ClearStopImmediately();
			} 
			
		WaitNextTimeSlice();
	} // End While
	
}
