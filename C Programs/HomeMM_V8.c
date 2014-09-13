#include "KMotionDef.h"

#if 1 // set to 1 for diagnostic messages to be printed
#define	PRINTF printf
#else
#define	PRINTF no_output_printf
#endif


#define GET_VALUE(source_value, start, length) ((source_value >> start) & ((1 << length) - 1))
#define SET_VALUE(source_value, index, new_val) *source_value = (new_val > 0 ? (*source_value | (1 << index)) : (*source_value & (~(1 << index))))
#define LENGTH(T) (sizeof(T)/sizeof(*(T)))


#define MAX_AXIS 8

#define CHAN_AXIS_0 0
#define CHAN_AXIS_1 1
#define CHAN_AXIS_2 2
#define CHAN_AXIS_3 3
#define CHAN_AXIS_4 4
#define CHAN_AXIS_5 5
#define CHAN_AXIS_6 6
#define CHAN_AXIS_7 7

#define HOME_DIR_POSITIVE 0
#define HOME_DIR_NEGATIVE 1

//Homing Sequence Types
#define HOME_TO_LIMIT_THEN_ROLL_OFF 0               //Simple Roll off limit
#define HOME_TO_LIMIT_THEN_ROLL_TO_NEXT_EDGE_RISE 1 //Roll to next rising edge of encoder
#define HOME_TO_LIMIT_THEN_ROLL_TO_NEXT_EDGE_FALL 2 //Roll to next falling edge of encoder
#define HOME_TO_LIMIT_THEN_MOVE_TO_POSITION 3       //Move to a preset location 


#define REPEAT_HOME_AT_SLOWER_RATE 1

#define REQUESTED_HOME_AXIS_FLAGS 20
#define HOME_NEGATIVE_FLAGS 21
#define REPEAT_HOME_FLAGS 22
#define INPUT_HOME_STATES 23


#define HOME_TYPE_0 24
#define HOME_TYPE_1 25
#define HOME_TYPE_2 26
#define HOME_TYPE_3 27
#define HOME_TYPE_4 28
#define HOME_TYPE_5 29
#define HOME_TYPE_6 30
#define HOME_TYPE_7 31

#define INPUT_HOME_AXIS_0 32
#define INPUT_HOME_AXIS_1 33
#define INPUT_HOME_AXIS_2 34
#define INPUT_HOME_AXIS_3 35
#define INPUT_HOME_AXIS_4 36
#define INPUT_HOME_AXIS_5 37
#define INPUT_HOME_AXIS_6 38
#define INPUT_HOME_AXIS_7 39

#define INPUT_ENCODER_AXIS_0 40
#define INPUT_ENCODER_AXIS_1 41
#define INPUT_ENCODER_AXIS_2 42
#define INPUT_ENCODER_AXIS_3 43
#define INPUT_ENCODER_AXIS_4 44
#define INPUT_ENCODER_AXIS_5 45
#define INPUT_ENCODER_AXIS_6 46
#define INPUT_ENCODER_AXIS_7 47

#define FAST_VEL_AXIS_0 48
#define FAST_VEL_AXIS_1 49
#define FAST_VEL_AXIS_2 50
#define FAST_VEL_AXIS_3 51
#define FAST_VEL_AXIS_4 52
#define FAST_VEL_AXIS_5 53
#define FAST_VEL_AXIS_6 54
#define FAST_VEL_AXIS_7 55

#define SLOW_VEL_AXIS_0 56
#define SLOW_VEL_AXIS_1 57
#define SLOW_VEL_AXIS_2 58
#define SLOW_VEL_AXIS_3 59
#define SLOW_VEL_AXIS_4 60
#define SLOW_VEL_AXIS_5 61
#define SLOW_VEL_AXIS_6 62
#define SLOW_VEL_AXIS_7 63

#define ORIGIN_AXIS_0 64
#define ORIGIN_AXIS_1 65
#define ORIGIN_AXIS_2 66
#define ORIGIN_AXIS_3 67
#define ORIGIN_AXIS_4 68
#define ORIGIN_AXIS_5 69
#define ORIGIN_AXIS_6 70
#define ORIGIN_AXIS_7 71

#define SENSOR_OFFSET_AXIS_0 72
#define SENSOR_OFFSET_AXIS_1 73
#define SENSOR_OFFSET_AXIS_2 74
#define SENSOR_OFFSET_AXIS_3 75
#define SENSOR_OFFSET_AXIS_4 76
#define SENSOR_OFFSET_AXIS_5 77
#define SENSOR_OFFSET_AXIS_6 78
#define SENSOR_OFFSET_AXIS_7 79

#define SEQUENCE_NUM_AXIS_0 80
#define SEQUENCE_NUM_AXIS_1 81
#define SEQUENCE_NUM_AXIS_2 82
#define SEQUENCE_NUM_AXIS_3 83
#define SEQUENCE_NUM_AXIS_4 84
#define SEQUENCE_NUM_AXIS_5 85
#define SEQUENCE_NUM_AXIS_6 86
#define SEQUENCE_NUM_AXIS_7 87 

#define HOMING_COMPLETE_FLAGS 88



typedef enum 
{
	IDLE,
	ON_SENSOR, 
	MOVING_TO_SENSOR,
	MOVING_FROM_SENSOR,
	MOVING_TO_FINAL,
	STOPPING_AFTER_ENCODER,
	COMPLETE,
	ERROR
} HOMING_STATE;


typedef struct
{ 
	int HomeNegative;     //Determines the direction of the limit switch
	int RepeatHomeSlower; //Sets whether to 'double tap' the switch and at a slower rate on the second pass 
	int HomeSequenceType; //Final action to take after the switch has been indexed
	int AxisChannel;      //Axis Channel to use for this task
	int HomeLimitBit;     //Bit of IO the home switch is connected to
	int HomeLimitState;   //State the home switch IO is when tripped
	int EncoderBit;       //Bit of IO the encoder is connected to(if any)
	HOMING_STATE Status;

	float HomeFastVel;    //Velocity(in counts) for initial calibration
	float HomeSlowVel;    //Velocity(in counts) for final positioning  *Except for Encoder Edge types that use 1 for ther velo
	float Origin;         //Optional position to move axis after homing sequence
	float SensorOffset;   //Distance(in counts) to perform a relative move off a sensor when on the sensor
	 
}HOMING_PARAMS;


typedef struct
{
	int Count;
	HOMING_PARAMS HomingTasks[MAX_AXIS];
}HOMING_SEQ;



HOMING_SEQ HomingSequences[MAX_AXIS];
HOMING_STATE GlobalStatus;

int SequenceId=0;
int AxisChannel=0;


int	WaitAllDone(HOMING_PARAMS *Tasks, int Count);
void CreateHomeTask(int axischannel, int hometype, int switchdir, int repeathome, int slowspeed, int fastspeed, int limitbit, int encoderbit, int origin, int offset);
void LoadUserData(void);
void Init(void);
void GenerateHomingSequences(void);
void DoHome(void);
int DoSequence(HOMING_PARAMS *Tasks, int Count);
void MoveToSensor(int axis, int isneg, float speed);
void MoveFromSensor(int axis, int isneg, float speed);
void MoveToSensorOffset(int axis, int isneg, float distance, float speed);
void Validate(void);
int no_output_printf(const char *format, ...);




main()
{
	GlobalStatus = IDLE;
	Init();
	LoadUserData();
	PRINTF("Requested Flags = %x\n",persist.UserData[REQUESTED_HOME_AXIS_FLAGS]);
	GenerateHomingSequences();
	DoHome();
	Validate();
}

//Use this function to manually load the persist.UserData[]
void LoadUserData(void)
{  
	persist.UserData[REQUESTED_HOME_AXIS_FLAGS]=0;
	persist.UserData[HOME_NEGATIVE_FLAGS]=0;  
	persist.UserData[REPEAT_HOME_FLAGS]=0;  
	persist.UserData[INPUT_HOME_STATES]=0;  

	SequenceId=0;
	
	//Set Z axis to home first for safety
	AxisChannel = 2;
	CreateHomeTask(
		HOME_TO_LIMIT_THEN_MOVE_TO_POSITION,  	// home type 
		HOME_DIR_NEGATIVE,	// switch dir
		FALSE,				// repeat home
		100, 				// slowspeed
		400, 				// fastspeed
		32, 				// limitbit
		1, 					// limitstate
		0,					// encoderbit
		1000, 				// origin (amount to move inside)
		2000);				// sensor offset (amount to move if on sensor)

	//Now home X and Y in the same sequence
	//	SequenceId++;
	//	CreateHomeTask(0, 0, 1, 1, 100, 400, 32, 0, 0, 80);
	//	CreateHomeTask(1, 0, 1, 1, 100, 400, 32, 0, 0, 80);
	
}

//Initialize Collections
void Init(void)
{
	int i,j;

	PRINTF("Init\n");
	for(i = 0; i < MAX_AXIS; i++)//Loop through each sequence collection
	{
		HomingSequences[i].Count = 0;
		for(j = 0; j < MAX_AXIS; j++)//Loop through each axis in sequence
		{
			HomingSequences[i].HomingTasks[j].AxisChannel = -1;
		}
	}
}

void CreateHomeTask(int hometype, int switchdir, int repeathome, int slowspeed, int fastspeed, 
                    int limitbit, int limitstate, int encoderbit, int origin, int offset)
{ 
	SET_VALUE(&persist.UserData[REQUESTED_HOME_AXIS_FLAGS], AxisChannel, 1);
	SET_VALUE(&persist.UserData[HOME_NEGATIVE_FLAGS], AxisChannel, switchdir);  
	SET_VALUE(&persist.UserData[REPEAT_HOME_FLAGS], AxisChannel, repeathome);  
	SET_VALUE(&persist.UserData[INPUT_HOME_STATES], AxisChannel, limitstate);  

	PRINTF("CreateHomeTask axis %d limit bit %d\n",AxisChannel,limitbit);

	persist.UserData[SEQUENCE_NUM_AXIS_0+AxisChannel] = SequenceId;
	persist.UserData[HOME_TYPE_0+AxisChannel] = hometype;

	persist.UserData[SLOW_VEL_AXIS_0+AxisChannel] = slowspeed;
	persist.UserData[FAST_VEL_AXIS_0+AxisChannel] = fastspeed;

	persist.UserData[INPUT_HOME_AXIS_0+AxisChannel] = limitbit;
	persist.UserData[INPUT_ENCODER_AXIS_0+AxisChannel] = encoderbit;

	persist.UserData[ORIGIN_AXIS_0+AxisChannel] = origin;
	persist.UserData[SENSOR_OFFSET_AXIS_0+AxisChannel] = offset;
}

//Populate data structures for performing the home routines later
void GenerateHomingSequences(void)
{
	int Axis,Home,NegativeHome,RepeatHome,HomeType,LimitBit,LimitState,EncoderBit;
	float FastVel,SlowVel,Origin,SensorOffset;
	HOMING_PARAMS *Task;

	PRINTF("GenerateHomingSequences\n");

	for (Axis=0;Axis<MAX_AXIS;Axis++)
	{
		Home = GET_VALUE(persist.UserData[REQUESTED_HOME_AXIS_FLAGS], Axis, 1);

		//Load Parameters for Axis
		if(Home)
		{
			NegativeHome = GET_VALUE(persist.UserData[HOME_NEGATIVE_FLAGS], Axis, 1);
			RepeatHome = GET_VALUE(persist.UserData[REPEAT_HOME_FLAGS], Axis, 1);
			HomeType = persist.UserData[HOME_TYPE_0+Axis];
			SequenceId = persist.UserData[SEQUENCE_NUM_AXIS_0+Axis];

			LimitBit = persist.UserData[INPUT_HOME_AXIS_0+Axis];
			LimitState = GET_VALUE(persist.UserData[INPUT_HOME_STATES], Axis, 1);
			
			EncoderBit = persist.UserData[INPUT_ENCODER_AXIS_0+Axis];

			FastVel = persist.UserData[FAST_VEL_AXIS_0+Axis];
			SlowVel = persist.UserData[SLOW_VEL_AXIS_0+Axis];

			Origin = persist.UserData[ORIGIN_AXIS_0+Axis];
			SensorOffset = persist.UserData[SENSOR_OFFSET_AXIS_0+Axis];
			
			Task = &HomingSequences[SequenceId].HomingTasks[HomingSequences[SequenceId].Count];

			Task->HomeNegative = NegativeHome;
			Task->RepeatHomeSlower = RepeatHome;
			Task->HomeSequenceType = HomeType;
			Task->AxisChannel = Axis;

			Task->HomeLimitBit = LimitBit;
			Task->HomeLimitState = LimitState;
			
			Task->EncoderBit = EncoderBit;

			Task->HomeFastVel = FastVel;
			Task->HomeSlowVel = SlowVel;

			Task->Origin = Origin;
			Task->SensorOffset = SensorOffset;

			HomingSequences[SequenceId].Count ++;
		}
	}
}

//Perform homing routines
//The routines are broken into 'Sequences' to enable situations like homing the Z axis on a mill before homing the X and Y simultaneously
void DoHome(void)
{
	int i;

	PRINTF("DoHome\n");
	for(i = 0; i < MAX_AXIS; i++)//Loop though each sequence collection
	{
		PRINTF("Do Sequence %d\n", i);
		if(HomingSequences[i].Count > 0)//Check to skip unpopulated sequences
		{
			DoSequence(HomingSequences[i].HomingTasks,HomingSequences[i].Count);
		}
	}
}

int DoSequence(HOMING_PARAMS *Tasks, int Count)
{	
	int j, Axis, SequenceAxes;

	for(j = 0; j < Count; j++)//First, check to see if we are on the home sensor.  Move to offset if we are
	{
		PRINTF("Do Task %d\n", j);
		PRINTF("Checking Home Limit Bit %d\n", Tasks[j].HomeLimitBit);
		if(ReadBit(Tasks[j].HomeLimitBit))
		{
			PRINTF("Move Off Sensor\n");
			Tasks[j].Status = ON_SENSOR;
			MoveToSensorOffset(Tasks[j].AxisChannel, Tasks[j].HomeNegative, Tasks[j].SensorOffset, Tasks[j].HomeFastVel);
			Tasks[j].Status = MOVING_FROM_SENSOR; 
		}
	}

	// wait for all axes in the sequence are off sensor
	WaitAllDone(Tasks,Count);
	
	GlobalStatus = MOVING_TO_SENSOR;

	PRINTF("Moving to Sensor\n");
	//First pass to home to sensor at high velocity
	//Here we want to get all axes in current sequence moving simultaneously
	for(j = 0; j < Count; j++)
	{
		Tasks[j].Status = MOVING_TO_SENSOR;
		MoveToSensor(Tasks[j].AxisChannel, Tasks[j].HomeNegative, Tasks[j].HomeFastVel);
	}

	//Reset Sequence Counter
	SequenceAxes = 0;

	//Poll each axis in sequence to see if we have reached the sensor
	//Stop immediately
	while(GlobalStatus != ON_SENSOR)
	{
		for(j = 0; j < Count; j++)
		{
			if(ReadBit(Tasks[j].HomeLimitBit) == Tasks[j].HomeLimitState)
			{
				PRINTF("Stopped axis=%d state=%d\n",Tasks[j].AxisChannel,Tasks[j].HomeLimitState);
				Jog(Tasks[j].AxisChannel,0.0);
				Tasks[j].Status = ON_SENSOR;
				SequenceAxes++;
			}
			if(!chan[Tasks[j].AxisChannel].Enable)
			{
				Tasks[j].Status = ERROR;
				break;
			}
		}
		//All axes in sequence have reached the sensor, exit loop
		if(SequenceAxes == Count)
		{
			GlobalStatus = ON_SENSOR;
		}
	}
	
	PRINTF("All Axes Stopped\n");

	//Check each axis in sequence to see if double tap on switch is needed
	//Same as previous homing operation, just at slower speed
	//First move back off sensor to start offset
	for(j = 0; j < Count; j++)
	{
		if(Tasks[j].RepeatHomeSlower == REPEAT_HOME_AT_SLOWER_RATE)
		{ 
			Tasks[j].Status = MOVING_FROM_SENSOR;
			PRINTF("Repeat Home Slower axis %d\n",Tasks[j].AxisChannel);
			MoveToSensorOffset(Tasks[j].AxisChannel, Tasks[j].HomeNegative, Tasks[j].SensorOffset, Tasks[j].HomeFastVel);
		}
	}
	
	// wait for all axes in the sequence to be finished moving
	WaitAllDone(Tasks,Count);

	//Reset Sequence Counter
	SequenceAxes = 0;

	//Home to sensor at low velocity
	//Here we want to get all axes in current sequence moving simultaneously
	for(j = 0; j < Count; j++)
	{
		if(Tasks[j].RepeatHomeSlower == REPEAT_HOME_AT_SLOWER_RATE)
		{
			GlobalStatus = MOVING_TO_SENSOR;
			Tasks[j].Status = MOVING_TO_SENSOR;
			MoveToSensor(Tasks[j].AxisChannel, Tasks[j].HomeNegative, Tasks[j].HomeSlowVel);
			//Keep track of how many axes use the second switch
			SequenceAxes++;
		}
	}	

	//Poll each axis in sequence to see if we have reached the sensor
	//Stop immediately
	while(GlobalStatus != ON_SENSOR)
	{
		for(j = 0; j < Count; j++)
		{
			if(ReadBit(Tasks[j].HomeLimitBit) == Tasks[j].HomeLimitState)
			{
				Jog(Tasks[j].AxisChannel, 0.0);
				Tasks[j].Status = ON_SENSOR;
				SequenceAxes--;
			}
			if(!chan[Tasks[j].AxisChannel].Enable)
			{
				Tasks[j].Status = ERROR;
				PRINTF("Error #1 Axis Disabled %d\n",Tasks[j].AxisChannel);
				SequenceAxes = -1;
				break;
			}
		}

		PRINTF("Axis Stopped axis=%d\n",Tasks[j].AxisChannel);

		//Keep track of how many axes use the second switch
		if(SequenceAxes < 1)
		{
			GlobalStatus = ON_SENSOR;
		}
	} 
	SequenceAxes = 0;
	//Final home routine
	//Since we are on the same thread, we have to perform the final move one axis at a time
	//It should not be that much of a concern, as the first move to sensor was simultaneous and that was the largest travel distance
	for(j = 0; j < Count; j++)
	{
		int Axis=Tasks[j].AxisChannel;
		
		//Move away from sensor and stop as soon as axis is off
		//All sequences perform this first
		Tasks[j].Status = MOVING_FROM_SENSOR;
		MoveFromSensor(Axis, Tasks[j].HomeNegative, Tasks[j].HomeSlowVel);
	}

	PRINTF("Moving to Final\n");
	while(GlobalStatus != MOVING_TO_FINAL)
	{
		for(j = 0; j < Count; j++)
		{
			Axis = Tasks[j].AxisChannel;
			
			if(ReadBit(Tasks[j].HomeLimitBit) == Tasks[j].HomeLimitState)
			{
				Jog(Axis,0.0);
				Tasks[j].Status = MOVING_TO_FINAL;
				SequenceAxes++;
			}
			if(!chan[Axis].Enable)
			{
				Tasks[j].Status = ERROR;
				break;
			}
		}
		//All axes in sequence have reached the sensor, exit loop
		if(SequenceAxes == Count)
		{
			GlobalStatus = MOVING_TO_FINAL;
		}
	}
	
	//Start Final Positioning
	PRINTF("Start Final Positioning\n");
	for(j = 0; j < Count; j++)
	{ 
		Axis = Tasks[j].AxisChannel;
		
		if(Tasks[j].Status != ERROR)
		{
			//Determine final routine and perform it
			switch(Tasks[j].HomeSequenceType)
			{
				//We are already here
			case HOME_TO_LIMIT_THEN_ROLL_OFF:
				//Already commanded... will finalize in the next waiting loop
				break;
				//Travel to next rising encoder edge
			case HOME_TO_LIMIT_THEN_ROLL_TO_NEXT_EDGE_RISE:
				Tasks[j].Status = MOVING_TO_FINAL;
				MoveFromSensor(Axis, Tasks[j].HomeNegative, Tasks[j].HomeSlowVel); 
				break;
				//Travel to next falling encoder edge
			case HOME_TO_LIMIT_THEN_ROLL_TO_NEXT_EDGE_FALL:
				Tasks[j].Status = MOVING_TO_FINAL;
				MoveFromSensor(Axis, Tasks[j].HomeNegative, Tasks[j].HomeSlowVel);
				break;
				//Move to a predefined positioned
			case HOME_TO_LIMIT_THEN_MOVE_TO_POSITION:
				Tasks[j].Status = MOVING_TO_FINAL;
				MoveRelAtVel(Axis, Tasks[j].Origin, Tasks[j].HomeSlowVel);
				break;
			default:
				break;
			}
		}
	}
	
	SequenceAxes = 0;
	//Wait Final Positioning
	PRINTF("Wait Final Positioning\n");
	while(GlobalStatus != COMPLETE)
	{
		for(j = 0; j < Count; j++)
		{ 
			Axis=Tasks[j].AxisChannel;
			
			if(!chan[Axis].Enable)
			{
				Tasks[j].Status = ERROR;
				SequenceAxes++;
				break;
			}
			
			if(Tasks[j].Status != ERROR && Tasks[j].Status != COMPLETE)
			{
				//Determine final routine and perform it
				switch(Tasks[j].HomeSequenceType)
				{
					//We are already here
				case HOME_TO_LIMIT_THEN_ROLL_OFF:
					if(CheckDone(Axis))  // wait til really stopped
					{
						Tasks[j].Status = COMPLETE; 
						Zero(Axis);
						SequenceAxes++;
					}
					break;
					//Travel to next rising encoder edge
				case HOME_TO_LIMIT_THEN_ROLL_TO_NEXT_EDGE_RISE:
					if (Tasks[j].Status == STOPPING_AFTER_ENCODER)
					{
						Tasks[j].Status = COMPLETE; 
						Zero(Axis);
						SequenceAxes++;
					}
					else if(ReadBit(Tasks[j].EncoderBit))
					{
						Jog(Axis, 0.0);
						Tasks[j].Status = STOPPING_AFTER_ENCODER; 
					} 
					break;
					//Travel to next falling encoder edge
				case HOME_TO_LIMIT_THEN_ROLL_TO_NEXT_EDGE_FALL:
					if (Tasks[j].Status == STOPPING_AFTER_ENCODER)
					{
						Tasks[j].Status = COMPLETE; 
						Zero(Axis);
						SequenceAxes++;
					}
					else if(!ReadBit(Tasks[j].EncoderBit))
					{
						Jog(Axis, 0.0);
						Tasks[j].Status = STOPPING_AFTER_ENCODER; 
					} 
					break;
					//Move to a predefined positioned
				case HOME_TO_LIMIT_THEN_MOVE_TO_POSITION:
					if (CheckDone(Axis))
					{
						Tasks[j].Status = COMPLETE; 
						Zero(Axis);
						SequenceAxes++;
					}
					break;
				default:
					break;
				}
			}
		}
		if(SequenceAxes == Count)
		{
			GlobalStatus = COMPLETE;
		}
	}
}

void MoveToSensor(int axis, int isneg, float speed)
{
	PRINTF("Move to sensor axis=%d isneg=%d speed=%f\n",axis,isneg,speed);

	if(isneg == HOME_DIR_NEGATIVE)
	{
		Jog(axis, -speed);
	}
	else
	{
		Jog(axis, speed);
	}
}

void MoveFromSensor(int axis, int isneg, float speed)
{
	PRINTF("Move from sensor axis=%d isneg=%d speed=%f\n",axis,isneg,speed);

	if(isneg == HOME_DIR_NEGATIVE)
	{
		Jog(axis, speed);
	}
	else
	{
		Jog(axis, -speed);
	}
}


void MoveToSensorOffset(int axis, int isneg, float distance, float speed)
{
	PRINTF("Move sensor offset axis=%d isneg=%d distance=%f speed=%f\n",axis,isneg,distance,speed);

	if(isneg == HOME_DIR_NEGATIVE)
	{
		MoveRelAtVel(axis, distance, speed);
	}
	else
	{
		MoveRelAtVel(axis, -distance, speed);
	}
}


//Make Sure Everything homed properly
void Validate(void)
{
	int i,j;

	int success;
	for(i = 0; i < MAX_AXIS; i++)//Loop through each sequence collection
	{ 
		for(j = 0; j < HomingSequences[i].Count; j++)//Loop through each axis in sequence
		{
			success = (HomingSequences[i].HomingTasks[j].Status == COMPLETE) ? 1 : 0;
			SET_VALUE(&persist.UserData[HOMING_COMPLETE_FLAGS], HomingSequences[i].HomingTasks[j].AxisChannel, success); 
		}
	}
}

// wait till all axes in the list of tasks are done
// also return error if any of the axis have become disabled

int	WaitAllDone(HOMING_PARAMS *Tasks, int Count)
{
	int i,Done,Axis;
	do
	{
		WaitNextTimeSlice();

		Done=TRUE;
		
		for (i=0; i<Count; i++)
		{
			Axis = Tasks[i].AxisChannel;
			
			// first check if the axis is enabled, if not return error
			if (!chan[Axis].Enable) return 1; 
			
			// Check if axis is not finished
			if (!CheckDone(Axis)) Done=FALSE;
		}
	}
	while (!Done);
	return 0;
}

int no_output_printf(const char *format, ...)
{
	return 0;	
}

