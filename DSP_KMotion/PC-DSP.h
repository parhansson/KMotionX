// PC and DSP common shared definitions     
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#ifndef __PCDSP_H
#define __PCDSP_H

#define KMOTION_VER "4.33k"  

#define ABORT_CHAR 0x03  // ctrl-c clears/aborts any command


#define TIMEBASE    0.00009  // 90 us    

#define USER_PROG_ADDRESS_KMOTION 0x80038000  // where first user program is loaded
#define MAX_USER_PROG_SIZE_KMOTION 0x8000     // space between each thread/user program
#define USER_PROG_ADDRESS_KFLOP 0x80050000  // where first user program is loaded
#define MAX_USER_PROG_SIZE_KFLOP 0x10000    // space between each thread/user program
#define N_USER_THREADS 7              // how many user programs/threads there are
#define MAX_SEGMENTS 35000            // max number of parametric segments
#define N_USER_THREADS 7              // how many user programs/threads there are
#define N_CHANNELS 8                  // number of channels/board
#define N_IIR_FILTERS 3               // number of IIR Filters per axis
#define N_BYTES_PER_LINE 64			  // number of bytes/line for coff downloads

#define N_DACS 8      	// KMotion
#define N_ADCS 8 		
#define N_PWMS 8
#define N_ENCS 8

#define N_ADCS_SNAP 8  	// per snap amp 
#define N_PWMS_SNAP 4 	// per snap amp
#define N_ENCS_SNAP 4

#define N_BITS 48           // total # bits defined in KFLOP FPGA (space for 6 ports)
#define N_BITS_KMOTION 32   // total # bits defined in KMOTION FPGA (space for 4 ports)
#define N_VIRTUAL_BITS  16	// Number of virtual I/O bits, starting at bit 48 (48-63)
#define N_SNAPAMP_BITS0 32	// Number of SnapAmp I/O bits, 1st board, starting at bit  64 (64-95)  8-OPTO 8-DIFFENTIAL 14-LVTTK
#define N_SNAPAMP_BITS1 32	// Number of SnapAmp I/O bits, 2nd board, starting at bit  96 (96-127) 8-OPTO 8-DIFFENTIAL 14-LVTTK
#define N_KANALOG_INPUTBITS 16	// Number of Kanalog Input bits, starting at bit  128 (128-143) 8-GP 8-Opto in
#define N_KANALOG_OUTPUTBITS 24	// Number of Kanalog Output bits, starting at bit  144 (144-167) 8-Opto out, 8-GP, 8-Opto out
#define KSTEP_OPTO_IN_BITS 168 // KStep muxes in 16 inputs to these virtual inputs (168-183)
#define N_KSTEP_INPUTBITS 16	// Number of KStep Input bits, starting at bit  168 (168-183) 16-Opto In
#define VIRTUAL_BITS_EX 1024    // Starting number of first extended Virtual IO bit
#define N_VIRTUAL_BITS_EX  1024	// Number of Extended virtual I/O bits, starting at bit 1024 (1024-2047)



// Macros to convert KMotion ADC channels 0-3 readings to Volts

#define FULL_RANGE_VOLTAGE_KMOTION (-10.0f)
#define OFFSET_VOLTAGE_KMOTION 0.0f

#define KMOTION_CONVERT_ADC_TO_VOLTS(x)  (((x)-OFFSET_VOLTAGE_KMOTION)/2048.0f*FULL_RANGE_VOLTAGE_KMOTION)

// Macros to convert KMotion ADC channels 4-7 readings to Amps

#define FULL_RANGE_CURRENT_KMOTION 4.85f
#define OFFSET_CURRENT_KMOTION (-2048.0f)

#define KMOTION_CONVERT_ADC_TO_AMPS(x)  (((x)-OFFSET_CURRENT_KMOTION)/4096.0f*FULL_RANGE_CURRENT_KMOTION)

#define FULL_RANGE_VOLTAGE_KMOTION_4V (4.0f)

#define KMOTION_CONVERT_DAC_TO_VOLTS_4V(x)  ((x)/4096.0f*FULL_RANGE_VOLTAGE_KMOTION_4V)


// Macros to convert SnapAmp ADC readings to Amps/Volts

#define FULL_RANGE_CURRENT_SNAP 35.0f
#define OFFSET_CURRENT_SNAP 8192.0f

#define FULL_RANGE_VOLTAGE_SNAP 102.6f
#define OFFSET_VOLTAGE_SNAP 256.0f

#define SNAP_CONVERT_ADC_TO_AMPS(x)  (((x)-OFFSET_CURRENT_SNAP)/8192.0f*FULL_RANGE_CURRENT_SNAP)
#define SNAP_CONVERT_AMPS_TO_ADC(x)  (((x)/FULL_RANGE_CURRENT_SNAP*8192.0f+OFFSET_CURRENT_SNAP))
#define SNAP_CONVERT_ADC_TO_VOLTS(x) (((x)-OFFSET_VOLTAGE_SNAP)/65536.0f*FULL_RANGE_VOLTAGE_SNAP)

#define SNAP_CONVERT_VOLTS_TO_ADC(x) ((int)((x)/FULL_RANGE_VOLTAGE_SNAP*65536.0f+OFFSET_VOLTAGE_SNAP))


// Macros to convert Kanalog DAC/ADC settings to/from Volts

#define KANALOG_CONVERT_DAC_TO_VOLTS(x)  ((x)*(-10.0f/2048.0f))
#define KANALOG_CONVERT_ADC_TO_VOLTS(x)  ((x)*( 10.0f/2048.0f))

// Common codes to be passed from KFLOP to the PC to issue specific commands for certain Apps
// storing one of these codes into the PC_COMM_PERSIST variable will be uploaded to the PC
// with the status, the PC will perform the requested function, then the PC will either clear the 
// persist variable to indicate success or set to a negative error code to indicate failure.
#define PC_COMM_IDLE 0
#define PC_COMM_ESTOP 1
#define PC_COMM_HALT 2
#define PC_COMM_EXECUTE 3
#define PC_COMM_SINGLE_STEP 4
#define PC_COMM_SET_FRO 5 // Persist+1 is the FRO as a float
#define PC_COMM_SET_FRO_INC 6 // Persist+1 is the factor to change it as float
#define PC_COMM_SET_X 7  // Persist+1 is the value (32-bit float) to set the DRO to
#define PC_COMM_SET_Y 8  // Persist+1 is the value (32-bit float) to set the DRO to
#define PC_COMM_SET_Z 9  // Persist+1 is the value (32-bit float) to set the DRO to
#define PC_COMM_SET_A 10 // Persist+1 is the value (32-bit float) to set the DRO to
#define PC_COMM_SET_B 11 // Persist+1 is the value (32-bit float) to set the DRO to
#define PC_COMM_SET_C 12 // Persist+1 is the value (32-bit float) to set the DRO to
#define PC_COMM_USER_BUTTON 13 // Persist+1 is which User Button to push
#define PC_COMM_MCODE 14 // Persist+1 is which MCode to execute

// MessageBox Persist+1 = gather buffer offset (32-bit words) to message string
//            Persist+2 = Message Box Options (Icons and Buttons)
//            Persist+3 = MessageBox Result returned
#define PC_COMM_MSG 15   // MessageBox Persist+1=string,+2=Options,+3=result

// Get GCode Interpreter Persist+1 = first #Var to get
//                       Persist+2 = How many consecutive #Vars to get 
//                       Persist+3 = where to place them in KFLOP persist (double offset)
//     (GCode #Vars -> KFLOP persist) note: vars are transferred as doubles 2 persists each
#define PC_COMM_GET_VARS 16 // Persist+1=#Var,+2=N, +3=Dest   

// Set GCode Interpreter Persist+1 = first #Var to put
//                       Persist+2 = How many consecutive #Vars to put 
//                       Persist+3 = where to get them from KFLOP persist (double offset)
//     (KFLOP persist -> GCode #Vars) note: vars are transferred as doubles 2 persists each
#define PC_COMM_SET_VARS 17 // Persist+1=#Var,+2=N, +3=Dest   

// MDI Persist+1 = gather buffer offset (32-bit words) to GCode String
//     Persist+2 = Result returned
#define PC_COMM_MDI 18 // Persist+1=string, +2=result   

// Requests the resolution in counts/inch for all 6 Axes as 32-bit floating point numbers
#define PC_COMM_GETAXISRES 19 // Persist+1 is which where to place them in the UserData vars. 

#define PC_COMM_RESTART 20 // Resets the GCode back to line 1 

#define PC_COMM_SET_SSO 21 // Persist+1 is the SSO as a float
#define PC_COMM_SET_SSO_INC 22 // Persist+1 is the factor to change it as float

// Get GCode Tool Table Length	Persist+1 = Tool Index
//								Persist+2 = where to set value to KFLOP persist (double offset) 
//     (GCode #Vars -> KFLOP persist) note: vars are transferred as doubles 2 persists each
#define PC_COMM_GET_TOOLTABLE_LENGTH 23 // Persist+1=#ToolIndex,+2=Dest   

// Set GCode Tool Table Length	Persist+1 = Tool Index
//								Persist+2 = where to get value from KFLOP persist (double offset) 
//     (KFLOP persist -> GCode #Vars) note: vars are transferred as doubles 2 persists each
#define PC_COMM_SET_TOOLTABLE_LENGTH 24 // Persist+1=#ToolIndex,+2=Source   


// Get GCode Misc Settings	Persist+1 = where to set values to KFLOP persist (32-bit integers) 
//     (GCode #Vars -> KFLOP persist)  4 values are sent to KFLOP,  
//											Inches(1)/Metric(2) Flag
//											Tool Slot T number
//											Tool Index H number (length index)
//											Tool Index D number (radius index)
#define PC_COMM_GET_MISC_SETTINGS 25 // Persist+1=Dest for values  

// Get GCode Interpreter DROS Persist+1 = where to place them in KFLOP persist (double offset)
//     (GCode Machine Coordinates -> KFLOP persist) note: vars are transferred as doubles 2 persists each
#define PC_COMM_GET_MACHINE_COORDS 26 // Persist+1=Dest   

// Get GCode Interpreter DROS Persist+1 = where to place them in KFLOP persist (double offset)
//     (GCode DROs -> KFLOP persist) note: vars are transferred as doubles 2 persists each
#define PC_COMM_GET_DROS 27 // Persist+1=Dest  

#define PC_COMM_UPDATE_FIXTURE 28 // Update any change in currently selected Fixture offsets


#define PC_COMM_PERSIST 100  // First Persist Variable that is uploaded in status
#define N_PC_COMM_PERSIST 8  // Number of Persist Variables that are uploaded in status


#define MACH3_PROBE_STATUS_VAR 62
#define MACH3_PROBE_RESULTS_VAR 50


// This status contains the majority of all status
// so that it can be uploaded as a bulk transfer
#define STAT_VERSION 308
typedef struct
{
	int VersionAndSize;   //bits 16-23 = version, bits 0-15 = size in words
	int ADC[N_ADCS+2*N_ADCS_SNAP];
	int DAC[N_DACS];
	int PWM[N_PWMS+2*N_PWMS_SNAP];
	double Position[N_CHANNELS];  
	double Dest[N_CHANNELS];
	unsigned char OutputChan0[N_CHANNELS];
	  
	int InputModes;      // 4 bits for each axis 
	int InputModes2;     // 4 bits for each axis 
	int OutputModes;     // 4 bits for each axis 
	int OutputModes2;    // 4 bits for each axis 
	int Enables;         // 1 bit  for each axis 
	int AxisDone;        // 1 bit  for each axis 
	
	int BitsDirection[2];// KMotion - 64 bits of I/O direction 1 = output
	int BitsState[2];    // KMotion - 64 bits of state lsb=I/O bit0

	int SnapBitsDirection0;   // Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 0
	int SnapBitsDirection1;   // Snap - 32 bits of I/O direction 1=output 16-29 GPIO only, Card 1
	int SnapBitsState0;       // Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 0
	int SnapBitsState1;       // Snap - 32 bits of state  16-29 GPIO 0-7 Diff 8-15 OPTO, Card 1
	
	int KanalogBitsStateInputs;   // Kanalog - 16 bits 128-143
	int KanalogBitsStateOutputs;  // Kanalog - 24 bits 144-167

	int RunOnStartUp;    // word Bits 1-7 selects which threads to execute on startup   
	
	int ThreadActive;    		// one bit for each thread, 1=active, bit 0 - primary
	int StopImmediateState;    // Status of Stop Coordinated Motion Immediately

	double TimeStamp;	// Time in seconds (since KFlop boot) this status was aquired
	int	PC_comm[N_PC_COMM_PERSIST];// 8 persist Variables constantly uploaded to send misc commands/data to PC

	int VirtualBits;		// Virtual I/O bits simulated in memory
	int VirtualBitsEx0;		// only upload 32 1024 Expanded Virtual Bits 
} MAIN_STATUS;   


// Axis Input Modes

#define NO_INPUT_MODE 0
#define ENCODER_MODE 1
#define ADC_MODE 2	             
#define RESOLVER_MODE 3
#define USER_INPUT_MODE 4
#define NUM_INPUT_MODES 4

#define TranslateInputMode(x) ( ((x)&0xf) == NO_INPUT_MODE	 ? "No Input" : \
 							    ((x)&0xf) == ENCODER_MODE 	 ? "Encoder" : \
						   		((x)&0xf) == ADC_MODE  		 ? "ADC" :     \
						   		((x)&0xf) == RESOLVER_MODE   ? "Resolver" :\
						   		((x)&0xf) == USER_INPUT_MODE ? "User Input" : \
						                                       "Invalid")

#define InputModeToDefine(x)  ( ((x)&0xf) == NO_INPUT_MODE 	 ? "NO_INPUT_MODE" : \
								((x)&0xf) == ENCODER_MODE 	 ? "ENCODER_MODE" : \
						   		((x)&0xf) == ADC_MODE  		 ? "ADC_MODE" :     \
						   		((x)&0xf) == RESOLVER_MODE   ? "RESOLVER_MODE" :\
						   		((x)&0xf) == USER_INPUT_MODE ? "USER_INPUT_MODE" : \
						                                       "Invalid")


// Axis Output Modes

#define NO_OUTPUT_MODE 0
#define MICROSTEP_MODE 1
#define DC_SERVO_MODE 2	             
#define BRUSHLESS_3PH_MODE 3
#define BRUSHLESS_4PH_MODE 4
#define DAC_SERVO_MODE 5
#define STEP_DIR_MODE 6
#define CL_STEP_DIR_MODE 7
#define CL_MICROSTEP_MODE 8
#define NUM_SERVO_MODES 8

#define TranslateOutputMode(x) ( ((x)&0xf) == NO_OUTPUT_MODE 	   ? "No Output" : \
								 ((x)&0xf) == MICROSTEP_MODE 	   ? "Microstep" : \
						   		 ((x)&0xf) == DC_SERVO_MODE  	   ? "DC Servo" :     \
						   		 ((x)&0xf) == BRUSHLESS_3PH_MODE   ? "3PH Servo" :     \
						   		 ((x)&0xf) == BRUSHLESS_4PH_MODE   ? "4PH Servo" :     \
						   		 ((x)&0xf) == DAC_SERVO_MODE 	   ? "DAC Servo" :     \
						   		 ((x)&0xf) == STEP_DIR_MODE 	   ? "Step Dir" :     \
						   		 ((x)&0xf) == CL_STEP_DIR_MODE 	   ? "CL Step" :     \
						   		 ((x)&0xf) == CL_MICROSTEP_MODE    ? "CL Micro" :     \
						                                             "Invalid")

#define OutputModeToDefine(x) (  ((x)&0xf) == NO_OUTPUT_MODE 	   ? "NO_OUTPUT_MODE" : \
								 ((x)&0xf) == MICROSTEP_MODE 	   ? "MICROSTEP_MODE" : \
						   		 ((x)&0xf) == DC_SERVO_MODE  	   ? "DC_SERVO_MODE" :     \
						   		 ((x)&0xf) == BRUSHLESS_3PH_MODE   ? "BRUSHLESS_3PH_MODE" :     \
						   		 ((x)&0xf) == BRUSHLESS_4PH_MODE   ? "BRUSHLESS_4PH_MODE" :     \
						   		 ((x)&0xf) == DAC_SERVO_MODE 	   ? "DAC_SERVO_MODE" :     \
						   		 ((x)&0xf) == STEP_DIR_MODE 	   ? "STEP_DIR_MODE" :     \
						   		 ((x)&0xf) == CL_STEP_DIR_MODE 	   ? "CL_STEP_DIR_MODE" :     \
						   		 ((x)&0xf) == CL_MICROSTEP_MODE	   ? "CL_MICROSTEP_MODE" :     \
						                                             "Invalid")

// Backlash Compensation Options

#define BACKLASH_OFF 0
#define BACKLASH_LINEAR 1
#define NUM_BACKLASH_MODES 1

#define TranslateBacklashMode(x) (((x)&0xf) == BACKLASH_OFF 	   ? "off" : \
								  ((x)&0xf) == BACKLASH_LINEAR 	   ? "Linear" : \
						                                             "Invalid")

#define BacklashModeToDefine(x) (((x)&0xf) == BACKLASH_OFF 	   ? "BACKLASH_OFF" : \
								 ((x)&0xf) == BACKLASH_LINEAR  ? "BACKLASH_LINEAR" : \
						                                             "Invalid")



// Global Host Status    extern int HostStatus
// As host is requesting Global Status from KFLOP it can also optionally
// specify its status as a 32 bit field.  Bits can indicate its state and
// modes such as whether a a Job is actively running so KFLOP should inhibit
// mpg jogging

#define HOST_JOB_ACTIVE_BIT 1 // bit0 indicates Host Job is Actively running


#endif
