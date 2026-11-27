//*************************** Home, Goto, Probe, Routines *************************************

#include "KMotionDef.h"

#define Verbose FALSE

//**********************************************************************************************
// Note : printf sends a message to the Chip app which logs the message in the trace buffer.
//        Because KFlop library routines also use printf the messages need to be distguished from
//        messages sent from KFlop C programs, like this one.
//		  If the first character of the message is '@' or '*' then the Chip app knows that the
//        message is from a KFLop C program. '@' signifies a status or debug message and '*' an
//        error messaage. If you use printf, then include these in the message.
//**********************************************************************************************


//**********************************************************************************************
// Note : All definitions must match the ones in the PC application and all other KFlop programs
//**********************************************************************************************

#define X_Counts_Per_Inch 16000
#define Y_Counts_Per_Inch 16000
#define Z_Counts_Per_Inch 16256


#define Big_Number 9999

#define Fail -999999
#define To_Switch 1
#define From_Switch -1
#define No_Limit 99999

#define Normally_Closed 0
#define Normally_Open   1


#define Probe_Toolsetter_Saftety_Thread 4

//******************************************************************
#define Z_Max 6
#define Z_Rapid 30

//******************************************************************
// Channel definitions
#define X_Axis 0
#define Y_Axis 1
#define Z_Axis 2
#define X_Axis_Slave 3
#define Y_Axis_Slave 4

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
#define Command           0
#define Status            1    //See below for status values
#define Sync_Flag         2 //Used to sync multiple steps. Kflop sets Flag, PC clears it
#define Thread_Status     3 //Used to communicate across threads

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


#define X_Pos_Soft_Limit 40
#define X_Neg_Soft_Limit 41
#define Y_Pos_Soft_Limit 42
#define Y_Neg_Soft_Limit 43
#define Z_Pos_Soft_Limit 44
#define Z_Neg_Soft_Limit 45
            
 
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
#define Disable_Limit_Switches 14
#define Enable_Limit_Switches  15
#define Disable_Soft_Limits    16
#define Enable_Soft_Limits     17
#define Move_Y_Master          18
#define Test                   19
        
//******************************************************************
// Note : Status back to the Chip app is derived from two numbers added together
// The Status Category defines the major category
// The Status Qualifier is added to the Status Category to define a particular status. see following defines
//******************************************************************
// Status Categories       
  
#define No_Status                 	  0

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
// Status Qualifiers
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


//******************************************************************
// Thread Status
#define Thread_Status_Clear	0
#define Probe_Saftey	 	1
#define Toolsetter_Saftey	2


//*************************************************************************

int Initialized = FALSE;
int Main_Command;

int Save_Limit_Options;
int Save_Slave_Limit_Options;



void Initialize()
{
	//Put any initialization code here
	persist.UserData[Thread_Status] = Thread_Status_Clear;
} // end Initialize


float Get_User_Data_Float(int Address)
{
	float d;
	d = *(float *) &persist.UserData[Address]; 
    return d;
} // end Get_User_Data_Double


//*************************************************************************

void Set_User_Data_Float(int Address, float Value)
{
    float d=Value;
	persist.UserData[Address] = *(int *) & d;
} // end Set_User_Data_Double

//*************************************************************************

float Get_Pos(int Axis, int Address)
{	

	switch (Axis)
	{
	  case X_Axis :
		    return Get_User_Data_Float(Address) * X_Counts_Per_Inch;
	  break;
	  
	  case Y_Axis :
		    return Get_User_Data_Float(Address) * Y_Counts_Per_Inch;
	  break;
  
	  case Z_Axis :
		    return Get_User_Data_Float(Address) * Z_Counts_Per_Inch;
	  break;
	}
		
} // end Get_Pos


//*************************************************************************
float Get_Feed(int Axis, int Address)
{
	
	switch (Axis)
	{
	  case X_Axis :
	     return Get_User_Data_Float(Address) * (X_Counts_Per_Inch / 60);
	  break;
	  
	  case Y_Axis :
	     return Get_User_Data_Float(Address) * (Y_Counts_Per_Inch / 60);
	  break;
  
	  case Z_Axis :
	     return Get_User_Data_Float(Address) * (Z_Counts_Per_Inch / 60);
	  break;
	}
	
} // end Get_Feed

//*************************************************************************

void Update_Status(int New_Status)
{
	persist.UserData[Status] = New_Status; 
	Sync();
}

void Sync(void) //Notifies Chip app that status has changed, and waits for response
{
	persist.UserData[Sync_Flag] = 1;
	while (persist.UserData[Sync_Flag] != 0)
	{
		WaitNextTimeSlice();
	}
} // end Sync


//*************************************************************************

void Wait_Until_Axis_Stopped(int Axis)
{
	while(!CheckDone(Axis))
	{
		WaitNextTimeSlice();
	}
}

float Move_To_From_Switch(int Axis, int Switch, int To_From, float Feed, float Overtravel)
{
	//Note : Overtravel is a relative distance from the start position
	
	int Done, Switch_Sense, Toolsetter_Touch;
	float Limit,  Limit_Position, End_Position;

	if (Verbose==TRUE)
	{
		if (To_From==TRUE) printf("@Move_To_Switch -- "); else printf("@Move_From_Switch -- "); 
		printf("@Axis: %d, Switch: %d, To_From %d, Feedrate: %3.4f, Overtravel: %3.4f\n", 
									   Axis, Switch, To_From, Feed, Overtravel);
	}

	Limit = Overtravel;
	End_Position = 0;
	
	
	if (Feed < 0) Limit_Position = chan[Axis].Dest - Limit; else Limit_Position = chan[Axis].Dest + Limit;

 	Done = 0;		                       
	Jog(Axis, Feed); 
	
	while (Done == 0)
	{
		WaitNextTimeSlice();
		
		Switch_Sense = ReadBit(Switch);
		
		if (Switch == Fixed_Toolsetter_Switch) 
		{
			Toolsetter_Touch = ReadBit(Movable_Toolsetter_Switch);
			if (Toolsetter_Touch == 1)	
			{
				Jog(Axis,0);
				printf("!Movable toolsetter touched instead of Fixed toolsetter\n");
				return 6;
			}
		}
		
		if (Switch == Movable_Toolsetter_Switch) 
		{
			Toolsetter_Touch = ReadBit(Fixed_Toolsetter_Switch);
			if (Toolsetter_Touch == 1)	
			{
				Jog(Axis,0);
				printf("!Fixed toolsetter touched instead of movable toolsetter\n");
				return 6;
			}
		}
		
		
		if (To_From >= 0) 
		{
			if (Switch_Sense == 0) Switch_Sense = 1; else  Switch_Sense = 0;
		}
	
		if (Switch_Sense == 1)
		{
		
			switch (Axis)
			{
			  case X_Axis :
				End_Position = chan[Axis].Dest / X_Counts_Per_Inch;
			  break;
			  
			  case Y_Axis :
				End_Position = chan[Axis].Dest / Y_Counts_Per_Inch;
			  break;
			  
			  case Z_Axis :
					End_Position = chan[Axis].Dest / Z_Counts_Per_Inch;
			  break;
			}
						
			Done = 1 ;
		}
		else
		{
			if (Feed < 0)
			{
				if (chan[Axis].Dest < Limit_Position) Done = -1;
			}
			else
			{   
				if (chan[Axis].Dest > Limit_Position) Done = -1 ;
			}
		}// else
			
	} // while

	Jog(Axis,0);
	
	if (Done > 0)
	{
		if (Verbose==TRUE)
		{
			if (To_From==TRUE) printf("@Move_To_Switch -- "); else printf("@Move_From_Switch -- "); 
			printf("@Done\n");
		}
	}
	else
	{
		if (Verbose==TRUE) printf("@Move_To_From_Switch Failed\n");
		End_Position = Fail;
	}

	if (Verbose==TRUE) printf("@Touch at %3.4f \n",End_Position);

	return End_Position;

} // end Move_To_From_Switch

//*************************************************************************

void Home_Axis(int Axis)
{
	int    Save_Limit;
	int    Switch, Params, Home_Status;
	float  Save_Pos_Soft_Limit;
	float  Save_Neg_Soft_Limit;
	float  Fast_Feed, Slow_Feed;
	float  Latch_Distance;
	float  Backoff_Distance;
	
	switch (Axis)
	{
	  case X_Axis :
		Save_Limit = ch0->LimitSwitchOptions;
		ch0->LimitSwitchOptions = 0;
		ch3->LimitSwitchOptions = 0; // X Slave
		ch0->SoftLimitNeg = -Big_Number * X_Counts_Per_Inch;
		ch0->SoftLimitPos= Big_Number * X_Counts_Per_Inch;
		Switch = X_Axis_Home_Switch;
		Params = X_Params;
		Home_Status = Home_X_Status;
		
  		Set_User_Data_Float(Params + Feed_1, -10); 
		Set_User_Data_Float(Params + Feed_2, -0.5); 
		Set_User_Data_Float(Params +  Dest_1, -0.05); 
		Set_User_Data_Float(Params + Dest_2, 0.1); 
	
   	  break;
	  
	  case Y_Axis :
		Save_Limit = ch1->LimitSwitchOptions;
		ch1->LimitSwitchOptions = 0;
		ch4->LimitSwitchOptions = 0; // Y Slave
		ch1->SoftLimitNeg = -Big_Number * Y_Counts_Per_Inch;
		ch1->SoftLimitPos = Big_Number * Y_Counts_Per_Inch;;
		Switch = Y_Axis_Home_Switch;
		Params = Y_Params;
		Home_Status = Home_Y_Status;
		
 		Set_User_Data_Float(Params + Feed_1, -10); 
		Set_User_Data_Float(Params + Feed_2, -0.5); 
		Set_User_Data_Float(Params +  Dest_1, -0.05); 
		Set_User_Data_Float(Params + Dest_2, 0.1); 
		
	  break;
	  
	  case Z_Axis :
		Save_Limit = ch2->LimitSwitchOptions;
		ch2->LimitSwitchOptions = 0;
		ch2->SoftLimitPos = Big_Number * Z_Counts_Per_Inch;
		ch2->SoftLimitNeg = -Big_Number * Z_Counts_Per_Inch;
		Switch = Z_Axis_Home_Switch;
		Params = Z_Params;
		Home_Status = Home_Z_Status;

 		Set_User_Data_Float(Params + Feed_1, 10); 
		Set_User_Data_Float(Params + Feed_2, 0.5); 
		Set_User_Data_Float(Params +  Dest_1, 0.001); 
		Set_User_Data_Float(Params + Dest_2, -0.1); 	  break;
    }

    Fast_Feed = Get_Feed(Axis, Params + Feed_1);
	Slow_Feed = Get_Feed(Axis, Params + Feed_2);
	
    Latch_Distance = Get_Pos(Axis, Params + Dest_1);
	Backoff_Distance = Get_Pos(Axis, Params + Dest_2);
	                              
   	if (Verbose==TRUE) 
   	{
   		printf("@Axis %d \n", Axis);
        printf("@Fast Feed: Param=%3.4f Feed=%3.4f \n", Get_User_Data_Float(Params + Feed_1), Fast_Feed);
   		printf("@Slow Feed: Param=%3.4f Feed=%3.4f \n", Get_User_Data_Float(Params + Feed_2), Slow_Feed);
        printf("@Latch Distance: Param=%3.4f Counts=%3.4f \n", Get_User_Data_Float(Params + Dest_1),Latch_Distance);
        printf("@Backoff Distance: Param=%3.4f Counts=%3.4f \n", Get_User_Data_Float(Params + Dest_2),Backoff_Distance);
    }	
 
   	// Move until home switch is touched
	if (Verbose==TRUE) printf("@Status Moving to Home switch %d\n", Home_Status + Moving_To_Switch);  
	Update_Status(Home_Status + Moving_To_Switch); 

	Move_To_From_Switch(Axis, Switch, To_Switch, Fast_Feed, No_Limit); // Move towards switch
	Wait_Until_Axis_Stopped(Axis);
    Zero(Axis);

	// Move onto switch to latch it
	if (Verbose==TRUE) printf("@Status Moving to Latch %d\n", Home_Status + Moving_To_Latch);  

	if (Slow_Feed<0)
	{
		//MoveRelAtVel(Axis, Latch_Distance, -Fast_Feed); 
		MoveRelAtVel(Axis, Latch_Distance, -Slow_Feed); 
	}
	else
	{
		//MoveRelAtVel(Axis, Latch_Distance, Fast_Feed); 
		MoveRelAtVel(Axis, Latch_Distance, Slow_Feed); 
	}

	Update_Status(Home_Status + Moving_To_Latch); 
	Wait_Until_Axis_Stopped(Axis);
    Zero(Axis);

	// Move back off switch
	if (Verbose==TRUE) printf("@Status Moving Off Switch %d\n", Home_Status + Moving_Off_Switch);  
	Update_Status(Home_Status + Moving_Off_Switch); 
	Move_To_From_Switch(Axis, Switch, From_Switch, -Slow_Feed, No_Limit); // Move off switch
 	Wait_Until_Axis_Stopped(Axis);
    Zero(Axis);
    
    // Back away from switch
    if (Verbose==TRUE) printf("@Moving to backoff distance \n");
 
	if (Slow_Feed<0)
	{
		MoveRelAtVel(Axis, Backoff_Distance, -Fast_Feed); 
	}
	else
	{
		MoveRelAtVel(Axis, Backoff_Distance, Fast_Feed); 
	}

	Update_Status(Home_Status + Moving_To_Backoff); 
	Wait_Until_Axis_Stopped(Axis);
  	Zero(Axis);

    if (Verbose==TRUE) printf("@Backing off Done \n");

    Enable_XYZ_Soft_Limits();

   	switch (Axis)
	{
	  case X_Axis :
	    ch0->LimitSwitchOptions = Save_Limit;
   	    ch3->LimitSwitchOptions = Save_Limit; // X Slave
   	    
	  break;
	  case Y_Axis :
	    ch1->LimitSwitchOptions = Save_Limit;
	    ch4->LimitSwitchOptions = Save_Limit; // Y Slave
	  break;
	  case Z_Axis :
	    ch2->LimitSwitchOptions = Save_Limit;
	  break;
    }
    
  	Update_Status(Home_Status + Homed);

 	if (Verbose==TRUE) printf("@Status Homed %d\n", Home_Status +  Homed);  

 } // End Home Axis

//*************************************************************************

void Move_Axis(int Axis, float Position, float Feedrate)
{
	float Velocity;
	Velocity = (float) Feedrate;
	MoveAtVel(Axis, Position, Velocity);
	Wait_Until_Axis_Stopped(Axis);
} // end Move_Axis

void Move_Axis_Rel(int Axis, float Position, float Feedrate)
{
	float Velocity;
	Velocity = (float) Feedrate;
	persist.UserData[Status] = Move_Status + Moving_To_Position_1;
	MoveRelAtVel(Axis, Position, Velocity);
	Wait_Until_Axis_Stopped(Axis);
} // end Move_Axis

//*************************************************************************
void Disable_XYZ_Limit_Switches()
{
	Save_Limit_Options = ch0->LimitSwitchOptions;
	Save_Slave_Limit_Options = ch3->LimitSwitchOptions;
	ch0->LimitSwitchOptions = 0; // X
	ch1->LimitSwitchOptions = 0; // Y
	ch3->LimitSwitchOptions = 0; // X Slave
	ch4->LimitSwitchOptions = 0; // Y Slave
	if (Verbose==TRUE) printf("@XY Limit Switches Disabled \n");
} // Disable_XY_Limit_Switches

//*************************************************************************
void Enable_XYZ_Limit_Switches()
{
	ch0->LimitSwitchOptions = Save_Limit_Options; // X
	ch1->LimitSwitchOptions = Save_Limit_Options; // Y
	ch3->LimitSwitchOptions = Save_Slave_Limit_Options; // X Slave
	ch4->LimitSwitchOptions = Save_Slave_Limit_Options; // Y Slave
	if (Verbose==TRUE) printf("@XY Limit Switches Enabled \n");
} // Disable_XY_Limit_Switches

//*************************************************************************
void Disable_XYZ_Soft_Limits()
{
	ch0->SoftLimitNeg = -Big_Number * X_Counts_Per_Inch; // X
	ch0->SoftLimitPos = Big_Number * X_Counts_Per_Inch;
	ch1->SoftLimitNeg = -Big_Number * Y_Counts_Per_Inch; //Y
	ch1->SoftLimitPos = Big_Number * Y_Counts_Per_Inch;
	ch2->SoftLimitPos = Big_Number * Z_Counts_Per_Inch;
	
	if (Verbose==TRUE) printf("@Soft Limits Disabled \n");
}	
 // Disable_XY_Limit_Switches

//*************************************************************************
void Enable_XYZ_Soft_Limits()
{
	ch0->SoftLimitNeg = Get_User_Data_Float(X_Neg_Soft_Limit) * X_Counts_Per_Inch; // X
	ch0->SoftLimitPos = Get_User_Data_Float(X_Pos_Soft_Limit) * X_Counts_Per_Inch;
	ch1->SoftLimitNeg = Get_User_Data_Float(Y_Neg_Soft_Limit) * Y_Counts_Per_Inch; // Y
	ch1->SoftLimitPos = Get_User_Data_Float(Y_Pos_Soft_Limit) * Y_Counts_Per_Inch;
	ch2->SoftLimitPos = Get_User_Data_Float(Z_Pos_Soft_Limit) * Z_Counts_Per_Inch;

	if (Verbose==TRUE) printf("@Soft Limits Enabled \n");
}
// Disable_XY_Limit_Switches


//*************************************************************************

void Move_Y_Master_Axis()
{
	printf("@Y Pos: %3.4f\n", Get_User_Data_Float(Y_Params + Dest_1));
	printf("@Y Feed: %3.4f\n", Get_User_Data_Float(Y_Params + Feed_1));
    
	DisableAxis(Y_Axis_Slave);
    Move_Axis(Y_Axis, Get_Pos(Y_Axis, Y_Params + Dest_1), Get_Feed(Y_Axis, Y_Params + Feed_1));
	EnableAxis(Y_Axis_Slave);
	
  	Update_Status(Move_Status + Move_Done);
printf("@X Axis Squared\n");

	if (Verbose==TRUE) printf("@X Axis Squared\n");
}
// Move_Y_Master


//*************************************************************************	
void Probe(int Probe_Command)
{
	int Axis, Switch, Params, Probe_Status, Direction, Input_Sense;
	float Extend_Limit, Retract_Limit, Slow_Feed, Fast_Feed, Start_Position, Touch, Release;
    
	Direction = 1;
	Switch = Probe_Switch;
	Input_Sense = Normally_Closed;
	
 	switch (Probe_Command)
	{
		case Probe_X_Plus:
			Axis = X_Axis;
			Params =  X_Params;
			Probe_Status = Probe_X_Plus_Status;
		break;
		
		case Probe_X_Minus:
			Axis = X_Axis;
			Params = X_Params;
			Probe_Status = Probe_X_Minus_Status;
			Direction = -1;
		break;

		case Probe_Y_Plus:
			Axis = Y_Axis;
			Params = Y_Params;
			Probe_Status = Probe_Y_Plus_Status;
		break;
		
		case Probe_Y_Minus:
			Axis = Y_Axis;
			Params = Y_Params;
			Probe_Status = Probe_Y_Minus_Status;
			Direction = -1;
		break;

		case Probe_Z_Minus:
			Axis = Z_Axis;
			Params = Z_Params;
			Probe_Status = Probe_Z_Minus_Status;
			Direction = -1;
		break;

		case Tool_Setter_Fixed:
			Axis = Z_Axis;
			Params = Z_Params;
			Switch = Fixed_Toolsetter_Switch;
			Input_Sense = Normally_Open;
			Probe_Status = Fixed_Toolsetter_Status;
			Direction = -1;
		break;

		case Tool_Setter_Movable:
			Axis = Z_Axis;
			Params = Z_Params;
			Switch = Movable_Toolsetter_Switch;
			Input_Sense = Normally_Open;
			Probe_Status = Movable_Toolsetter_Status;
			Direction = -1;
		break;
	
	}
	
	if (Input_Sense == Normally_Closed)
	{
		if (ReadBit(Switch) == 0)
		{
			if (Verbose==TRUE) printf("@Probe already in contact.");
			Update_Status(Probe_Status + Probe_In_Contact);
			return;
		}
	}
	else
	{
		if (ReadBit(Switch) == 1)
		{
			if (Verbose==TRUE) printf("@Toolsetter already in contact.");
			Update_Status(Probe_Status + Probe_In_Contact);
			return;
		}
	}
	
	Start_Position = Get_Pos(Axis, chan[Axis].Dest);
	Extend_Limit = Get_Pos(Axis, Params + Dest_1);
	Retract_Limit = Get_Pos(Axis, Params + Dest_2);
	Slow_Feed = Get_Feed(Axis, Params + Feed_1);
	Fast_Feed = Get_Feed(Axis, Params + Feed_2);

  	if (Verbose==TRUE) 
   	{
   		printf("@Axis %d \n", Axis);
        printf("@Slow_Feed: Param=%3.4f Feed=%3.4f \n", Get_User_Data_Float(Params + Feed_1), Slow_Feed);
   		printf("@Fast_Feed: Param=%3.4f Feed=%3.4f \n", Get_User_Data_Float(Params + Feed_2), Fast_Feed);
        printf("@Extend_Limit: Param=%3.4f Counts=%3.4f \n", Get_User_Data_Float(Params + Dest_1), Extend_Limit);
        printf("@Retract_Limit: Param=%3.4f Counts=%3.4f \n", Get_User_Data_Float(Params + Dest_2), Retract_Limit);
    }	
         
         
	// Extend towards surface
	if (Direction < 0)
	{
		Slow_Feed = -Slow_Feed;
		Fast_Feed = -Fast_Feed;
	}
	
	if (Verbose==TRUE) printf("@Probe -- Start\n");
	Update_Status(Probe_Status +  Probe_Extend);

	if (Input_Sense == Normally_Closed)
		Touch = Move_To_From_Switch(Axis, Switch, To_Switch, Fast_Feed, Extend_Limit); 
	else 
		Touch = Move_To_From_Switch(Axis, Switch, From_Switch, Fast_Feed, Extend_Limit); 

	Wait_Until_Axis_Stopped(Axis);
  
	if (Touch == Fail) 
	{
		if (Verbose==TRUE) printf("@Probe Extend Fail : %d\n", Probe_Status + Probe_Overtravel);
		Update_Status(Probe_Status + Probe_Overtravel);
		return;
	}
	else
	{
		if (Verbose==TRUE) printf("@Probe Extended, Touch %3.4f\n", Touch);
		if (Verbose==TRUE) printf("@Actual Stop %3.4f\n", Get_Pos(Axis, chan[Axis].Dest));
		
		Update_Status(Probe_Status + Probe_Touch);
	}
	
	// Retract from surface
	if (Verbose==TRUE) printf("@Retract -- Start\n");
	
	if (Input_Sense == Normally_Closed)
		Release = Move_To_From_Switch(Axis, Switch, From_Switch, -Slow_Feed, Retract_Limit);
	else 
		Release = Move_To_From_Switch(Axis, Switch, To_Switch, -Slow_Feed, Retract_Limit);
	
	if (Release == Fail) 
	{
		Update_Status(Probe_Status + Probe_Overtravel);
		printf("@Probe Retract Fail : %d\n", Probe_Status + Probe_Overtravel);
		return;
	}
	else
	{
		Set_User_Data_Float(Pos_1, Release);
		Update_Status(Probe_Status + Probe_Release);
		if (Verbose==TRUE) printf("@Probe Release at %3.4f\n", Release);
	}
	
	// Go to retract position
	if (Verbose==TRUE) printf("@Backoff -- Start\n");
	if (Direction < 0) Fast_Feed = -Fast_Feed; else	Retract_Limit = -Retract_Limit;
	MoveRelAtVel(Axis, Retract_Limit, Fast_Feed);
	Wait_Until_Axis_Stopped(Axis);
 
 	switch (Probe_Command)
	{
		case Tool_Setter_Fixed:
		    Retract_Limit = Z_Max * Z_Counts_Per_Inch;
			Fast_Feed = Z_Rapid * (Z_Counts_Per_Inch / 60);
		    Move_Axis(Z_Axis, Retract_Limit, Fast_Feed );
		break;

		case Tool_Setter_Movable:
		    Retract_Limit = Z_Max * Z_Counts_Per_Inch;
			Fast_Feed = Z_Rapid * (Z_Counts_Per_Inch / 60);
		    Move_Axis(Z_Axis, Retract_Limit, Fast_Feed );
		break;
	}
 
	Wait_Until_Axis_Stopped(Axis);

	if (Verbose==TRUE) printf("@Probing Done -- \n");
	Update_Status(Probe_Status + Probe_Done);

} // end Probe




//*************************************************************************
int Error_Exit()
{
	if (persist.UserData[Thread_Status] != 0) 
	{
		Update_Status(Move_Status + Move_Done);
		persist.UserData[Command] = 0;
		persist.UserData[Status] = Idle;
		WaitNextTimeSlice();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//********************************************************************************************
//************************************ Main Loop *********************************************
//********************************************************************************************
main()
{
      if (Initialized==FALSE)
      {
      	Initialize();
      }
    
	Main_Command = persist.UserData[Command];
	
    if (Verbose==TRUE) printf("@Main_Command %d\n", Main_Command);

printf("@Main_Command %d\n", Main_Command);

	switch (Main_Command)
	{
 
		case Go_To:
			if (Verbose==TRUE) printf("@Go_To X %d Y %d Z-up %d Z-dn %d\n", Get_Pos(X_Axis, X_Params + Dest_1), Get_Pos(Y_Axis, Y_Params + Dest_1), Get_Pos(Z_Axis, Z_Params + Dest_1), Get_Pos(Z_Axis, Z_Params + Dest_2));

printf("@Go_To X %d Y %d Z-up %d Z-dn %d\n", Get_Pos(X_Axis, X_Params + Dest_1), Get_Pos(Y_Axis, Y_Params + Dest_1), Get_Pos(Z_Axis, Z_Params + Dest_1), Get_Pos(Z_Axis, Z_Params + Dest_2));

			Update_Status(Move_Status + Moving_To_Position_1);
		    Move_Axis(Z_Axis, Get_Pos(Z_Axis, Z_Params + Dest_1), Get_Feed(Z_Axis, Z_Params + Feed_1));
	    
	    	if (persist.UserData[Thread_Status] == 0)
			{ 
				Update_Status(Move_Status + Moving_To_Position_2);
				Move_Axis(X_Axis, Get_Pos(X_Axis, X_Params + Dest_1), Get_Feed(X_Axis, X_Params + Feed_1));
			}
			else
			{
				StopCoordinatedMotion();
				persist.UserData[Thread_Status] = 0;
			}
			
	    	if (persist.UserData[Thread_Status] == 0)
			{ 
				Update_Status(Move_Status + Moving_To_Position_3);
				Move_Axis(Y_Axis, Get_Pos(Y_Axis, Y_Params + Dest_1), Get_Feed(Y_Axis, Y_Params + Feed_1));
			}
			else
			{
				StopCoordinatedMotion();
				persist.UserData[Thread_Status] = 0;
			}

	    	if (persist.UserData[Thread_Status] == 0)
			{ 
				Update_Status(Move_Status + Moving_To_Position_4);
				Move_Axis(Z_Axis, Get_Pos(Z_Axis, Z_Params + Dest_2), Get_Feed(Z_Axis, Z_Params + Feed_2));
			}
			else
			{
				StopCoordinatedMotion();
				persist.UserData[Thread_Status] = 0;
			}

			persist.UserData[Thread_Status] = 0;
			Update_Status(Move_Status + Move_Done);

		break;
		
		case Home_X:
			Home_Axis(X_Axis);
			break;
		break;

		case Home_Y:
			Home_Axis(Y_Axis);
		break;

		case Home_Z:
			Home_Axis(Z_Axis);
		break;
		
		case Home_All: 
			Home_Axis(Z_Axis);
			Home_Axis(X_Axis);
			Home_Axis(Y_Axis);
		break;
			
		case Probe_X_Plus:
		case Probe_X_Minus:
		case Probe_Y_Plus:
		case Probe_Y_Minus:
		case Probe_Z_Minus:
			if (Verbose==TRUE) printf("@Probe\n");
			PauseThread(Probe_Toolsetter_Saftety_Thread);
			Probe(Main_Command);
			ResumeThread(Probe_Toolsetter_Saftety_Thread); 
			if (Verbose==TRUE) printf("@Probe Done\n");
		break;
			
		case Tool_Setter_Fixed:
		case Tool_Setter_Movable:
		    if (Verbose==TRUE) printf("@Toolset\n");
			PauseThread(Probe_Toolsetter_Saftety_Thread);
			Probe(Main_Command);
			ResumeThread(Probe_Toolsetter_Saftety_Thread); 
			if (Verbose==TRUE) printf("@Toolset Done\n");
		break;
		
		case Disable_Limit_Switches:
			Disable_XYZ_Limit_Switches();
		break;
		
		case Enable_Limit_Switches:
			Enable_XYZ_Limit_Switches();
		break;
		
		case Disable_Soft_Limits:
			Disable_XYZ_Soft_Limits();
		break;
		
		case Enable_Soft_Limits:
			Enable_XYZ_Soft_Limits();
		break;
		
		case Move_Y_Master:
			Move_Y_Master_Axis();
		break;
		
		case Test:
			printf("@CoordSystem0 = %x\n",CoordSystem0);
			printf("@CS0_Flushed = %d\n",CS0_Flushed);
			printf("@CS0_HoldAtEnd = %d\n",CS0_HoldAtEnd);
			printf("@CS0_TimeBase = %f\n",CS0_TimeBase);
			printf("@CS0_TimeBaseDesired = %f\n",CS0_TimeBaseDesired);
			printf("@CS0_TimeExecuted = %f\n",CS0_TimeExecuted);
		break;

	}
		 					 	 
	persist.UserData[Command] = 0;
	persist.UserData[Status] = Idle;
		
	if (Verbose==TRUE) printf("@Command Done\n");
	
} // end main
