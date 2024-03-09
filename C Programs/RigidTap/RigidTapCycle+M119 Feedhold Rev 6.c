#include "KMotionDef.h"

#define TAP_VAR 50 // start of where parameters should be placed




//  *********CAUTION!!!!!********
// if Feedhold is pushed while tapping, the target position will continue to increase with every turn of spindle and machine will likely crash when resuming motion!!
// This program will trigger E-stop after a maximum slave error is exceeded to prevent this. Be sure to setup Estop bit properly to use this safety feature!
// There is also an option to set a persist variable to block feedhold button on line 105. Init file loop will need to be modified to work with this.

// There is an option on line 133 to index spindle if you have a signal available. 



//  R I G I D   T A P P I N G    Rev 5

#define ZAXIS 2			// Axis to slave
#define MAXERROR 0.05	// MAX slave error in inches
#define SPINDLE_AXIS 3	// Spindle Axis With Encoder Feedback 
#define SPINDLECW 147	// SPINDLE CW Bit number
#define SPINDLECCW 148	// SPINDLE CCW Bit number
#define Z_CPI 20320		// Counts per inch of slave axis
#define CPR 8192		// Counts per rev of spindle encoder
#define TAU 0.001		// Smoothing time
#define MAXTAPSPEED 850 // Maximum Tapping speed in rpm
#define OSFACTOR 0.0077	// Overshoot Factor (higher number reduces overshoot at higher rpms)
#define OSCONSTANT 0.034// Overshoot Constant (constant amount of compensation applied at all rpms, in inches)
#define ESTOP 137		// Emergency stop bit number 
#define ESTOPSTATE 0	// Emergency stop triggered by ClearBit = 0, SetBit = 1
#define STOPFACTOR 2.0	// Stop delay Factor (increase number to allow more time for spindle to stop while still slaving after hole in finished)
#define SPINDLE_STOP_TIME 1.0 // for Job aborts, don't exit to keep Slaving until this much time after the Spindle was turned off 
double SlaveGain,ToCut,Z0,S0,OS,SpindleStopTime;

void Slave(void);


main()
{

// Set variables

	// Call MCode M119 to do Rigid Tapping
	// 
	// Var+0 - (int) 119 (MCode number)
	// Var+1 - (float) Bottom Z in User Units
	// Var+2 - (float) Retract Position in User Units
	// Var+3 - (float) Distance per Peck in User Units (0 if unspecified)
	// Var+4 - (float) Feed Rate F Number (User Units/Rev)
	// Var+5 - (float) Spindle RPM to Tap
	// Var+6 - (int) Units (INCHES=1 MM=2)
	// Var+7 - (int) KFLOP axis to move (derived from active GCode Plane and Coord Motion System defs)
	// Var+8 - (float) Axis Resolution Counts/inch

	int *pFeedhold = &persist.UserData[TAP_VAR+0];
	float Bottom = *(float *)&persist.UserData[TAP_VAR+1];
	float Retract = *(float *)&persist.UserData[TAP_VAR+2];
	float Peck = *(float *)&persist.UserData[TAP_VAR+3];
	float Pitch = *(float *)&persist.UserData[TAP_VAR+4];
	float RPM = *(float *)&persist.UserData[TAP_VAR+5];
	int Units = persist.UserData[TAP_VAR+6];
	int Axis = persist.UserData[TAP_VAR+7];
	float AxisRes = *(float *)&persist.UserData[TAP_VAR+8];

	//printf("Bottom = %f\n",Bottom); 
	//printf("Retract = %f\n",Retract); 
	//printf("Peck = %f\n",Peck); 
	//printf("Pitch = %f\n",Pitch); 
	//printf("RPM = %f\n",RPM); 
	if (Units == 1)
		printf("Units = Inches\n"); 
	else
		printf("Units = mm\n"); 
	//printf("Axis = %d\n",Axis); 
	//printf("AxisRes = %f\n",AxisRes); 		

	
	printf("Bottom = %f\n",Bottom);	// Print Bottom
	printf("Pitch = %f\n",Pitch);	// Print pitch

	if (Units == 2) // if metric convert units 
	{
		Bottom = Bottom / 25.4;
		Retract = Retract / 25.4;
		Pitch = Pitch / 25.4;
		Peck = Peck / 25.4;
	}

    float Depth = Bottom - Retract;	// Depth equal to bottom and retract height
	if (Peck == 0) Peck = Depth * -1.1;	// if peck = zero, set larger then depth so its ignored	
	

// Check if Max Speed exceeded

	if (RPM > MAXTAPSPEED) 
	{
		SetStateBit(ESTOP,ESTOPSTATE);	// e-stop
		printf("Max Tap Speed Exceeded\n");		// Print error
		return 0;  					// End
	}

// Disable Feedhold/reset buttons

	persist.UserData[163] = 1;				// Set varable to indicate tapping active
	//ResumeCoordinatedMotion(); 				// Clear feedhold

// Prep Tapping Parameters
	
	float i = RPM * STOPFACTOR; 				// Set variable for stopping delay
	OS = ((RPM * RPM) * OSFACTOR * Pitch * Z_CPI / 1000);// Overshoot calculation
	OS = OS + (OSCONSTANT *	Z_CPI);				// Add overshoot constant for all rpms
	SlaveGain = Z_CPI * Pitch * -1 / CPR;		// Set Slave factor		
	Z0 = chan[ZAXIS].Dest;					// Record Z start position
	float ToCut = Depth * Z_CPI * -1;           // Calculted incremental distance to cut
	float StopPoint = Z0 - ToCut + OS;			// Position where spindle is command to stop (Absolute Position)
	float PeckCounts = Peck * Z_CPI;			// Calculate peckcounts
	float PeckPoint = Z0 - PeckCounts + OS;		// Calculate first peckpoint
	Zero(SPINDLE_AXIS);					// Zero Spindle Axis
	float Zmin = 0;						// Set Zmin zero
	float Zmininches = 0;				// Set Zmininches zero
	int Done = 0;						// Tap cycle not complete
	int StopPointReached = 0;			// Stoppoint not reached

	BOOL FeedholdActive=FALSE;
	BOOL HaltDetected=FALSE;
	
// Start Spindle
	
 	ClearBit(SPINDLECCW); 		// turn off spindle counterclockwise
	SetBit(SPINDLECW); 		// spindle on cw

	persist.UserData[164] = 3;				// Set variable to indicate spindle restart direction
	Delay_sec(.3);


// Record spindle position

	// while (!ReadBit(1045)) WaitNextTimeSlice(); 	// Option to wait for orientation signal to index spindle	
	S0 = chan[SPINDLE_AXIS].Position;	// Record Spindle encoder position 


// Begin Tapping Operation
	
    while(Done == 0) // Tap while Done = 0
	{
		// Handle any Feedhold/Resumes
		if (CS0_StoppingState != 0)  // somehow motion feedhold is active
		{
			ResumeCoordinatedMotion();  // release it
			*pFeedhold = -2;  // simulate a tapping feedhold
		}

		if (!FeedholdActive && *pFeedhold == -2)  // check if Operator requested Tapping Feedhold
		{
			FeedholdActive = TRUE;
			SetBit(SPINDLECW); 			// These two lines turn off Troys Hurco spindle
			SetBit(SPINDLECCW);
			SpindleStopTime = Time_sec() + SPINDLE_STOP_TIME;
		}

		if (FeedholdActive && *pFeedhold == 0)  // check if Operator requested Tapping Resume
		{
			FeedholdActive = FALSE;
			if (persist.UserData[164] == 3)  //check appropriate direction
			{
				ClearBit(SPINDLECCW); 	// turn off spindle counterclockwise
				SetBit(SPINDLECW); 		// spindle on cw
			}
			else
			{
				ClearBit(SPINDLECW);		// turn off spindle clockwise	
				SetBit(SPINDLECCW); 		// turn on spindle counterclockwise  
			}
		}

		// if Job was Halted during the Tap, Feedhold should have been asserted and
		// the spindle turned off. If the Job ends make the decision to exit right away
		// but delay the exit to keep slaving long enough to where the spindle should
		// be stopped in order not to break the Tap

		if (FeedholdActive && !JOB_ACTIVE)
			HaltDetected=TRUE;

		if (HaltDetected && Time_sec() > SpindleStopTime)
		{
			Jog(ZAXIS,0);  // stop any possible MoveExp motion
			printf("Tap Aborted\n");
			return 0;
		}

		Slave();	// Slave Z axis 

		if (chan[ZAXIS].Dest <= PeckPoint && ReadBit(SPINDLECW))	// If peckpoint Reached and spindle on CW
		{					
			ClearBit(SPINDLECW);		// turn off spindle clockwise	
            SetBit(SPINDLECCW); 		// turn on spindle counterclockwise  
			persist.UserData[164] = 4;	// Set variable to indicate spindle restart direction
			PeckPoint = PeckPoint - PeckCounts; 			
		}		
		if ((chan[ZAXIS].Dest > Z0) && ReadBit(SPINDLECCW) && (StopPointReached == 0)) 	// If Tap is out of hole, in reverse and stoppoint not reached
		{				
			ClearBit(SPINDLECCW); 	// turn off spindle counterclockwise
			SetBit(SPINDLECW); 		// spindle on cw
			persist.UserData[164] = 3;	// Set variable to indicate spindle restart direction
		}      	
		if (chan[ZAXIS].Dest <= StopPoint && ReadBit(SPINDLECW)) // If stoppoint Reached while being driven CW
		{					
			ClearBit(SPINDLECW);		// turn off spindle clockwise	
            SetBit(SPINDLECCW); 		// turn on spindle counterclockwise  
			persist.UserData[164] = 4;	// Set variable to indicate spindle restart direction
			StopPointReached = 1; 				
		}
		if ((chan[ZAXIS].Dest > Z0) && (StopPointReached == 1) && ReadBit(SPINDLECCW)) // If Tap is out of hole and stop point has been reached and being driven CCW
		{				
			//ClearBit(SPINDLECCW); 	// turn off spindle counterclockwise

			SetBit(SPINDLECW); 			// These two lines added to turn off Troys Hurco spindle
			SetBit(SPINDLECCW);
			persist.UserData[164] = 3;	// Set variable to indicate spindle restart direction
			i--;				// Delay while spindle slows down
			if (i<=1) Done = 1;		// Done, exit loop
		}
		if (chan[ZAXIS].Dest < Zmin)		// If Z is lower the Zmin
		{
			Zmin = chan[ZAXIS].Dest;	// Record new Zmin
		}
		if (ESTOPSTATE == ReadBit(ESTOP)) // If e stop triggered, exit loop
		{
			return 0;	
		}
	}	
	
	Jog(ZAXIS, 0);	// finish MoveExp
	while (!CheckDone(ZAXIS));	// wait for Z to finish
	Zmininches = (Z0 - Zmin) / Z_CPI * -1;	// Calculate Z min in inches
	Zmininches = Zmininches + Retract;	// Add retract to Zmin	
	if (Units == 2)	Zmininches = Zmininches * 25.4;	// If metric multiply by 25.4

	persist.UserData[163] = 0;	// Tapping complete
	
	printf("Tap Complete. Minimium Depth Reached = %f\n",Zmininches);
}	    


void Slave(void)
{
	float Destination = (chan[SPINDLE_AXIS].Position-S0) * SlaveGain + Z0;	// Set new destination
	MoveExp(ZAXIS, Destination, TAU);	// Move to new destination
	WaitNextTimeSlice();	
	float slaveerror = (chan[ZAXIS].Dest - Destination) / Z_CPI;	// Calculate slave error
	if (slaveerror < 0) slaveerror = slaveerror * -1;	// if error is negative, make positive
	if (slaveerror > MAXERROR)  // If slaveerror exceeds max
	{
		SetStateBit(ESTOP,ESTOPSTATE);	// e-stop
		printf("Tap sync failed\n");			// Print error							
	}
}
