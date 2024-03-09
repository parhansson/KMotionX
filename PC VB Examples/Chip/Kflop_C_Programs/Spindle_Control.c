//*************************** Spindle Control *************************************

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


//**********************************************************************************************
// Note : All definitions must match the ones in the PC application and all other KFlop programs
//**********************************************************************************************

#define X_Counts_Per_Inch 16000
#define Y_Counts_Per_Inch 16000
#define Z_Counts_Per_Inch 16256

#define Fail -999999
#define To_Switch 1
#define From_Switch -1
#define No_Limit 99999

//******************************************************************
// Channel definitions
#define X_Axis 0
#define Y_Axis 1
#define Z_Axis 2

//******************************************************************
// Limit switches
#define X_Axis_Home_Switch 			168
#define Y_Axis_Home_Switch 			170
#define Z_Axis_Home_Switch 			172
#define Probe_Switch 				178
#define Movable_Toolsetter_Switch 	179
#define Fixed_Toolsetter_Switch 	180
#define E_Stop_Switch 	            181

//******************************************************************
// User data locations
//Integer
#define Command 0
#define Status 1    //See below for status values
#define Sync_Flag 2 //Used to sync multiple steps. Kflop sets Flag, PC clears it
#define Probe_In_Progress 3

//Float
#define Spindle_Speed    5 //Used to set spindle speed from PC application
#define Spindle_Override 6

//float, see following Offsets into user data locations
#define X_Params    10
#define Y_Params    15
#define Z_Params    20

//feedback status to Chip app
#define Pos_1       30
#define Pos_2       31
#define Spindle_RPM 32
                
//******************************************************************
// Offsets into user data locations
#define Dest_1              0
#define Dest_2              1
#define Feed_1              2
#define Feed_2              3

//******************************************************************
// Commands
#define Idle 				    0
#define Set_Spindle_RPM 	    1
#define Go_To 				    2
#define Home_X 				    3
#define Home_Y 				    4
#define Home_Z 				    5
#define Home_All 			    6
#define Probe_X_Plus 		    7
#define Probe_X_Minus 		    8
#define Probe_Y_Plus 		    9
#define Probe_Y_Minus 		   10
#define Probe_Z_Minus 		   11
#define Tool_Setter_Fixed      12
#define Tool_Setter_Movable    13
#define Disable_Limits         14
#define Enable_Limits          15
#define Test                   16
        
//******************************************************************
// Status Blocks       
#define Idle                      	  0

#define Move_Status  			 	 10

#define Home_X_Status 				 20
#define Home_Y_Status 				 30
#define Home_Z_Status 				 40

#define Probe_X_Plus_Status			 50
#define Probe_X_Minus_Status		 60
#define Probe_Y_Plus_Status			 70
#define Probe_Y_Minus_Status		 80
#define Probe_Z_Minus_Status 	 	 90
#define Fixed_Toolsetter_Status		100	
#define Movable_Toolsetter_Status	110	
#define Spindle_Status				120

//******************************************************************
// Offsets into Status
#define Moving_To_Position_1  	1
#define Moving_To_Position_2  	2
#define Moving_To_Position_3 	3
#define Moving_To_Position_4	4
#define Move_Done 	 			5
#define Move_Error 	 			6
        
#define Moving_To_Switch   		1
#define Moving_To_Latch         2 
#define Moving_Off_Switch	 	3
#define Moving_To_Backoff       4
#define Homed 			 		5

#define Probe_Extend 			1
#define Probe_Retract 			2
#define Probe_Touch 			3
#define Probe_Release 			4
#define Probe_Done 				5
#define Probe_Overtravel		6
#define Probe_In_Contact 		7

#define RPM_Change  			1
#define RPM_Too_High  			2
      
       
//******************************************************************

void Set_User_Data_Float(int Address, float Value)
{
    float d=Value;
	persist.UserData[Address] = *(int *) & d;
} // end Set_User_Data_Double

//*************************************************************************

#define RPM_FACTOR 500.0 // RPM for full duty cycle (max analog out)
#define Max_RPM 24000    // Maximum spindle RPM for error check

float V[]=
{
0.001,  // count = 0
0.031,  // count = 1
0.044,  // count = 2
0.054,  // count = 3
0.062,  // count = 4
0.069,  // count = 5
0.074,  // count = 6
0.079   // count = 7
};

float V240=0.970;

int CorrectAnalog(float v)
{
	int r;
	float v2=2.0f*v;
	// compare with half way points to determine closest count
	if (v2 < V[1]+V[0]) return 0;
	if (v2 < V[2]+V[1]) return 1;
	if (v2 < V[3]+V[2]) return 2;
	if (v2 < V[4]+V[3]) return 3;
	if (v2 < V[5]+V[4]) return 4;
	if (v2 < V[6]+V[5]) return 5;
	if (v2 < V[7]+V[6]) return 6;
	
	// must be 7 or higher do linear interpolation
	
	r = (int)(7.5 + (v-V[7])/(V240-V[7])*(240.0f-7.0f));
	if (r>255) r=255;
	return r;
}

//*************************************************************************

void Update_Status(int New_Status)
{
	persist.UserData[Status] = New_Status; 
	Sync();
}

void Sync(void) //Notifies Chip app that status has changed, and waits for response
{
	persist.UserData[Sync_Flag] = 1;
	while (persist.UserData[Sync_Flag] != 0);
} // end Sync

//*************************************************************************


main ()
{
	float Commanded_Speed;
	float Speed;
	float Override;
	float Ratio;
	float FPGA_Command;

	Ratio = 500.0 / Max_RPM;   
	
	if (Verbose==TRUE) printf("@Spindle Control Program\n");
	
    Commanded_Speed = *(float *)& persist.UserData[Spindle_Speed];  
    Override = *(float *)& persist.UserData[Spindle_Override]; 
    Override = Override / 100;
 	Speed = Commanded_Speed * Override;
    FPGA_Command = Speed * Ratio;
    
    if (Verbose==TRUE) 
	{
		printf("@Speed Command: %3.4f\n", Commanded_Speed);
		printf("@Override : %3.4f\n", Override);
		printf("@Speed : %3.4f\n", Speed);
		printf("@-------------------------------\n");
	}
	
   if (Speed > Max_RPM)
    {
    	Speed = Max_RPM;
    	Set_User_Data_Float(Spindle_RPM, Speed);
		Update_Status(Spindle_Status + RPM_Too_High);
		return;
    }
    
  	Set_User_Data_Float(Spindle_RPM, Speed);
	Update_Status(Spindle_Status + RPM_Change);

	FPGA(KAN_TRIG_REG)=4;  			// Mux PWM0 to JP7 Pin5 IO 44 for KSTEP 
	SetBitDirection(44,1);  		// define bit as an output
	FPGA(IO_PWMS_PRESCALE) = 46;  	// divide clock by 46 (1.4 KHz)
	FPGA(IO_PWMS+1) = 1;  			// Enable
	FPGA(IO_PWMS) = CorrectAnalog(FPGA_Command/RPM_FACTOR);  	// Set PWM
	
	//FPGA(IO_PWMS) = 255; // Max duty
	 //SetBit(44); //Turns on output full on
}
