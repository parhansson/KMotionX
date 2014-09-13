//
//  KMotionDef.h - MAIN HEADER FILE FOR USER C PROGRAMS
//
//  Copyright Dynomotion 2/20/2004
//
//  Include this header file in User C programs that execute
//  in the DSP to define all User accessible KMotion functions,
//  constants, and data structures

#ifndef KMotionDef_h
#define KMotionDef_h

#define KFLOP
#define C6722

#define BOARD "KFLOP"

extern const char VersionAndBuildTime[];  // string with version and build time

#define CLOCKFREQ   50.0e6   // 200 MHz/4   

typedef int BOOL;

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef NULL
#define NULL    0
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif    
#ifndef PI
#define PI 3.14159265358979323846264
#endif    
#ifndef PI_F
#define PI_F 3.1415926535f
#endif    
#ifndef TWO_PI_F
#define TWO_PI_F (2.0f * 3.1415926535f)
#endif    
#ifndef PI_2F
#define PI_2F (3.1415926535f * 0.5f) 
#endif    
#ifndef TWO_PI
#define TWO_PI (2.0 * 3.14159265358979323846264)
#endif    


#include "PC-DSP.h"   // contains common structures shared by PC and DSP

// Global Host Status that the PC Application can specify as it Requests Global Status
// to inform KFLOP User threads of its current state.  ie Job Actively running
// up to 32 bits of status can be specified.  See PC-DSP.h for normal bit defines
extern int volatile HostStatus;

#define JOB_ACTIVE (HostStatus & HOST_JOB_ACTIVE_BIT)



// standard math funtions

extern float sqrtf (float x);
extern float expf  (float x);
extern float logf  (float x);
extern float log10f(float x);
extern float powf  (float x, float y);
extern float sinf  (float x);
extern float cosf  (float x);
extern float tanf  (float x);
extern float asinf (float x);
extern float acosf (float x);
extern float atanf (float x);
extern float atan2f(float y, float x); 
extern float fast_fabsf (float y); 

extern double sqrt (double x);
extern double exp  (double x);
extern double log  (double x);
extern double log10(double x);
extern double pow  (double x, double y);
extern double sin  (double x);
extern double cos  (double x);
extern double tan  (double x);
extern double asin (double x);
extern double acos (double x);
extern double atan (double x);
extern double atan2(double y, double x); 
extern double fast_fabs (double y); 


#define FPGA_ADDR ((volatile unsigned char *)0x91000000)   // Base of FPGA addresses
#define FPGA(X) (FPGA_ADDR[(X)*4+2])
#define FPGA_ADDRW ((volatile unsigned short int *)0x91000000)   // Base of FPGA addresses
#define FPGAW(X) (FPGA_ADDRW[(X)*2+1])
    
// This structure is set to default every time the program runs UNTIL
// the program image has been FLASHED using the FLASH command
// from then on it will not be cleared, so data that was present at the
// time it was flashed will persist     

#define N_USER_DATA_VARS 200
                    
typedef struct
{
    int RunOnStartUp; // word Bits 1-7 selects which threads to execute on startup   
    unsigned int EntryPoints[N_USER_THREADS];   // each downloaded program's entry point
    int UserData[N_USER_DATA_VARS]; // General purpose way to share and or save user program data
} PERSIST;

extern PERSIST persist;    

extern int StatusRequestCounter;  // increments each time host requests status


// data gathering variables

#define MAX_GATHER_VALUES 32

#define GATHER_NULL_TYPE 7
#define GATHER_ADC_TYPE 6
#define GATHER_DOUBLE_TYPE 5
#define GATHER_FLOAT_TYPE 4
#define GATHER_LASTPWMC_TYPE 3
#define GATHER_LASTPWM_TYPE 2
#define GATHER_INT_TYPE 1
#define GATHER_END_TYPE 0

typedef struct  // define a size and address to store
{  
    int type;    // GATHER_XXXXX_TYPE, 0=end of list
    void *addr;
} GATHER_VALUE_DEF;
 
typedef struct
{
    double *bufptr;         // data gathering buffer pointer
    double *endptr;         // done if bufptr = endptr    
    
    double Dest;             // Save where the injection will be relative to
    
    int Inject;             // if set, 1st address is to be copied from buffer to the address 
    
    GATHER_VALUE_DEF list[MAX_GATHER_VALUES];   // list of addresses to gather (null after last value)
} GATHER;

extern GATHER gather;    
                     

#define MAX_GATHER_DATA 1000000 // Size of gather buffer (number of doubles, 8 bytes each).                   
extern double *gather_buffer;   // Large buffer for data gathering, Bode plots, or User use

#define N_CPLX 2048

extern float *input;

void SetupGatherAllOnAxis(int c, int n_Samples);  // Prepares to gather info for an axis
void TriggerGather();                             // starts gathering defined items into gather buffer


// MOTION TRAJECTORY STRUCTURES
    
extern double CS0_TimeBase;      	// how much coordinated motion is advanced each tick
extern double CS0_TimeBaseDelta;  	// how much coordinated motion time rate is changed per tick
extern float CS0_DecelTime;			// how long to take to stop the coordinated motion (computed by StopMotion)
extern int CS0_StoppingState; 		// emergency stop in progress, 0 = not stopping, 1=stopping coord motion, 2=stopping indep, 3=fully stopped, 4=ind stopped
void StopCoordinatedMotion(void);  	// bring any coordinated motion to a emergency stop ASAP
void ResumeCoordinatedMotion(void);	// resume coordinated/Indep motion after an emergency stop
void ClearStopImmediately(void);	// Clear Stop Condition without resuming
void UpdateStoppingState(void);		// Update Stopping Status (only required for indep stopping)



#define TRAJECTORY_OFF 0          // no trajectory generation
#define TRAJECTORY_INDEPENDENT 1  // simple independent axis (3rd order funtion of time)
#define TRAJECTORY_LINEAR 2       // linear interpolated from coord system 0    (x = c*p+d)
#define TRAJECTORY_CIRCULAR 3     // circular interpolated from coord system 0  (x = c*sin(p*a+b)+d)
#define TRAJECTORY_SPECIAL 4      // Special Command to clear/set IO bit
								  // 	command 0 = ClearBit command 1 = SetBit
								  //    param 0 = bit number
#define TRAJECTORY_EXPONENTIAL 5  // independent axis (exponentially approach Dest a=Ratio per tick, b=Dest)

#define LAST_MOTION_JOG 0		  // type of last independent motion was a jog
#define LAST_MOTION_MOVE 1		  // type of last independent motion was a move
#define LAST_MOTION_MULTI 2		  // type of last independent motion was a multi axis move (G0 type)
#define LAST_MOTION_EXP 3		  // type of last independent motion was an exponential


typedef struct  // linear (only c and d are used),or sine equation
{  
    double a;    
    double b;    
    double c;    
    double d;   // always t^0 constant coefficient (starting position)
} TRIP_CIRCLE_LINEAR;

typedef struct  // linear (only c and d are used)
{  
    double c;    
    double d;   // always t^0 constant coefficient (starting position)
} TRIP_LINEAR;

//
// Motion structure for a coordinate system
//
// 3rd order polynomial for the parametric parameter 
//
// defines a parametric parameter p that varies from 0->1 over the
// segment of motion as a function of time.  All the associated axes 
// derive their position from this (either circular or linear) 
// using either a circular formula or linear formula

typedef struct  
{   
    char trajectory_mode;    // Off, circular, or linear
    unsigned char x_axis;    // associated x axis / or special command 
    unsigned char y_axis;    // associated y axis
    unsigned char z_axis;    // associated z axis
    unsigned char a_axis;    // associated a axis
    unsigned char b_axis;    // associated b axis
    unsigned char c_axis;    // associated c axis
    unsigned char u_axis;    // associated u axis
    unsigned char v_axis;    // associated v axis
	short int special_param; // special param 0
	double t;   // time duration (in sec) of trip state
    double a;   // t^3 coefficient  (Jerk)
    double b;   // t^2 coefficient  (initial acceleration)
    double c;   // t^1 coefficient  (initial velocity)
    double d;   // t^0 constant coefficient (starting position) 
    TRIP_CIRCLE_LINEAR X,Y;
    TRIP_LINEAR Z,A,B,C,U,V;
} PARAMETRIC_COEFF;




typedef struct  // 3rd order polynomial for a single trip state
{  
    int trajectory_mode;
    double t;   // time duration (in sec) of trip state
    double a;   // t^3 coefficient  (Jerk)
    double b;   // t^2 coefficient  (initial acceleration)
    double c;   // t^1 coefficient  (initial velocity)
    double d;   // t^0 constant coefficient (starting position)
} TRIP_COEFF;
            
typedef struct  // 2nd order IIR Filter
{
    float A1;   // output coefficient Z-1
    float A2;   // output coefficient Z-2
    float B0;   // input  coefficient Z-0   
    float B1;   // input  coefficient Z-1
    float B2;   // input  coefficient Z-2
    float d0,d1; // delay values
} IIR;

            

#define MAX_TRIP 20  // max trip states for individual axis moves
extern TRIP_COEFF TripCoeffs[N_CHANNELS][MAX_TRIP];  // Trip Coeff lists for each channel

// Limit Switch Options

// Bit 0 1=Stop Motor on Neg Limit, 0=Ignore Neg limit
// Bit 1 1=Stop Motor on Pos Limit, 0=Ignore Pos limit
// Bit 2 Neg Limit Polarity 0=stop on high, 1=stop on low
// Bit 3 Pos Limit Polarity 0=stop on high, 1=stop on low
//
// Bits 4-7 Action - 0 Kill Motor Drive
//                   1 Disallow drive in direction of limit
//                   2 Stop movement
//
// Bit 8=use Extended Limit Bit Numbers (LimitNegSwitchBit,LimitPosSwitchBit)
//
// (for legacy support allow packed 8-bit numbers)
// Bits 16-23 Neg Limit I/O Bit number
// Bits 24-31 Pos Limit I/O Bit number




//  M A I N   S T R U C T U R E   T H A T   D E F I N E S   A N   A X I S 

typedef struct
{   
    int ChanNumber;                 // channel number 0-3
    int Enable;                     // enables feedback
    int InputMode;                  // sets position input mode (See Axis Input Modes)
    int OutputMode;					// sets servo/motor mode    (See Axis Output Modes)
    int LimitSwitchOptions;         // see above for description
    int LimitSwitchNegBit;			// Neg Limit I/O Bit number
    int LimitSwitchPosBit;			// Pos Limit I/O Bit number
	int MasterAxis;					// -1 if none, else master axis channel to slave to
	double SlaveGain;				// Multiplicative Factor for slave motion 
    double MaxFollowingError;       // Kill motor if error exceeds this value
    double LastFollowingError;      // Last Measured Following Error
    double t;                       // current time in secs within the trip state
    double Dest;                    // current dest position for servo
	double UnfilteredDest;			// unfiltered current dest position for servo (before CM smoothing)
    double DestOffset;              // Additional offset to position (used by Injection)
    float Vel;                      // max velocity for the move trajectory
    float Accel;                    // max Acceleration for the move trajectory
    float Jerk;                     // max Jerk (rate of change of Accel) for the move
    float FFAccel;                  // Acceleration feed forward
    float FFVel;                    // Velocity feed forward
    double Position;                // encoder, ADC, Resolver, reading
    double invDistPerCycle;         // for stepper distance for one complete cycle (4 full steps)
                                    // for brushless 3-4 phase encoder counts for one complete cycle
                                    // (saved as reciprical for speed)      
    float StepperAmplitude;         // Microstepper Amplitude in PWM counts to apply (moving slow, without lead comp)
    float Output;                  	// Value output to PWM or DAC or CL Stepper offset 
    float prev_output;              // previous Output so Slave can track Master when in CL Stepper 
    float Lead;                     // Lead compensation to correct for step motor inductance
    TRIP_COEFF *pcoeff;             // pointer to coeff that interrupt routine uses, NULL if done 
    double last_position;           // last measured error
    double last_dest;               // last destination from beginning of prev servo interrupt
	double prev_dest;				// prev destination from end of prev servo interrupt (will incl User changes to Dest)
    float last_vel;                 // last destination velocity
    float x1last,x2last;            // used with lead compensation 
    int   OutputChan0,OutputChan1;  // pwm or DAC channels to use     
    int   InputChan0,InputChan1;    // Encoder or ADC channels to use 
    float InputOffset0,InputGain0;  // offsets and gains for Resolver Input 0,1 (x'=ax+b)
    float InputOffset1,InputGain1;  //   or for ADC, or (InputGain0=-1 reverses encoder)
	float OutputGain;				// Scale or Reverse Output Magnitude or Direction
	float OutputOffset;				// Offset the output
    float CommutationOffset;        // 3 or 4 phase commutation offset, PhaseA = sin((Position+CommutationOffset)*invDistPerCycle)
    float last_theta;               // last resolver theta reading
    float theta_correction;         // resolver correction offset to correct for nonlinearities
    signed char last_enc;           // last fpga encoder reading
    float P,I,D;                    // pid gain values
    float MaxI;                     // max integrator windup
    float MaxErr;                   // error saturates at this value
    float integrator;               // current integrator vlue
    float MaxOutput;                // max allowed servo output
    float DeadBandRange;            // Range about zero where gain change occurs
    float DeadBandGain;             // Additional gain within DeadBand Range
	int LastMotionType;				// Type of last move - used in Immediate Stop/Resume
	double LastMotionDest;			// Where last move was to go - used in Immediate Stop/Resume
	float ExpMotionVc;				// Velocity point on exp curve where Max Accel is obtained
	float ExpMotionXc;				// Distance point on exponential curve where Max Accel is obtained
	float SoftLimitNeg;				// Negative Soft Limit (counts)
	float SoftLimitPos;				// Positive Soft Limit (counts)
	int BacklashMode;				// Type of correction:  Currently only BACKLASH_OFF, BACKLASH_LINEAR
	float BacklashAmount;			// Amount of Backlash to be applied
	float BacklashRate;				// Rate Backlash shoule be applied, counts/sec
	int BacklashDirection;			// Last non zero direction moved
	float PrevBacklashDest;			// Prev Destination where backlash was determined to allow small hysteresis
	float Backlash;					// current amount of compensation being applied

    TRIP_COEFF *c;               	// move profile polynomial coefficients list (up tp MAX_TRIP)
    IIR iir[N_IIR_FILTERS];         // several IIR filters
}CHAN;

// continuously sent by DMA to DACs
extern short int DAC_Buffer[N_DACS];   //  format    12 bits data 
#define DAC(ch, v) DAC_Buffer[ch]=((v-2048)&0xfff)  // set DAC channel to value (range -2048/+2047)

extern int ADC_BufferIn[N_ADCS];     //  format   12 bits data 
#define ADC(ch) (ADC_BufferIn[ch]-2048)   // return ADC reading of specified channel (range -2048/2047)extern int ADC_BufferIn[N_ADCS];     //  format   4-dummy bits 12 bits data 16 dummy 

extern int ADC_BufferInSnap[2*N_ADCS_SNAP];   //  Snap Amp Current ADC format  16-bits data 

#define FULL_RANGE_CURRENT 4.85f
#define MeasuredAxisAmps(axis) ((ADC(axis+4)+2048)*(FULL_RANGE_CURRENT/4096.0f))  // returns measured current in an axis (Amperes)

// On board Power Amp PWM control

#define MAX_PWMR_VALUE 400		// Max value for PWMs in Recirculate mode
void WritePWMR(int ch, int v);  // Write to PWM - Recirculate mode (+ or - power then shorted)
#define MAX_PWM_VALUE 230		// Max value for PWMs in antiphase mode
void WritePWM(int ch, int v);   // Write to PWM - locked anti-phase mode (+ power then - power)

#define MAX_PWMC_VALUE 1000		// Max value for PWMs in Current Mode (SnapAmps only)
void WritePWMC(int ch, int v);  // Write to PWM - Current Loop mode - Always optimal decay

extern int SnapAmpPresent;     				// 1 = SnapAmp Present 0= Not Present
void WriteSnapAmp(int add, int data);		// write a 16-bit word directly to SnapAmp FPGA 
int ReadSnapAmp(int add);					// read a 16-bit word directly from SnapAmp FPGA


// Digital I/O bit PWM control (8 I/O bits on KFlop JP6 may be pulsed)

#define N_IO_PWMS 8				// Number of pwms that may be assigned to GPIO bits		
#define IO_PWMS 0xD0  			// FPGA offset to IO PWM registers (2 bytes each - value, enable(bit0))
#define IO_PWMS_PRESCALE 0x2f  	// FPGA offset to IO PWM Pre-Scale clock divider 0-255, 0 = 16.6MHz, 1=8.33MHz, ...
#define IO_PWM_MAX_VALUE 255    // 0 = 0%, 255 = 100 % duty cycle

// addr to r/w encoder noise rejection filter value (0..255), 
// Bit8 switches Encoders Ch4-7 from JP5 to JP6, 
// Bit9 switches Encoders Ch0-3 from JP7 to JP4
#define ENC_NOISE_FILTER_ADD  0x05   
#define ENC_0_3_JP4 0x200
#define ENC_4_7_JP6 0x100
#define ENC_NOISE_FILTER_DEFAULT_VAL 7 // noise rejection filter default value (100MHz/3/7/2 = 2MHz)


#define ENC_NOISE_ERR_ADD 0x08  // encoder sudden change by 2 error address
#define ENC_NOISE_ERR_BIT0 4 	// encoder sudden change by 2 error bit for encoder 0
#define ENC_NOISE_ERR_BIT1 5 	// encoder sudden change by 2 error bit for encoder 1
#define ENC_NOISE_ERR_BIT2 6 	// encoder sudden change by 2 error bit for encoder 2
#define ENC_NOISE_ERR_BIT3 7 	// encoder sudden change by 2 error bit for encoder 3



// FPGA Step and Direction Frequency Generators (8) are available
//
// 
#define NSTEPDIR 8

// address of 6 bit pulse length 0-63= # 16.666MHz clocks, 
// bit6 muxes generators 0-3 from JP7 to JP4 and JP6, 
// bit7 reverses polarity (0=pulses low & Pos Dir high, 1=pulses high & Pos Dir Low)
#define STEP_PULSE_LENGTH_ADD 0x06 

#define STEP_PULSE_LENGTH_DEFAULT 32 // default pulse length of ~ 2us

#define STEP_RATE_ADD 0x3c // write a 32 bit word - Bit31=enable, Bit27=Drive, Bits24-26=chan, 0-23= signed fraction of 16.666MHz
#define STEP_POSITION_ADD0 0x40 // read a 16 bit word - step count0 - 9 bits signed position and 7 bits of fraction
#define STEP_POSITION_ADD1 0x42 // read a 16 bit word - step count1 - 9 bits signed position and 7 bits of fraction
#define STEP_POSITION_ADD2 0x44 // read a 16 bit word - step count2 - 9 bits signed position and 7 bits of fraction
#define STEP_POSITION_ADD3 0x46 // read a 16 bit word - step count3 - 9 bits signed position and 7 bits of fraction
#define STEP_POSITION_ADD4 0x48 // read a 16 bit word - step count4 - 9 bits signed position and 7 bits of fraction
#define STEP_POSITION_ADD5 0x4a // read a 16 bit word - step count5 - 9 bits signed position and 7 bits of fraction
#define STEP_POSITION_ADD6 0x4c // read a 16 bit word - step count6 - 9 bits signed position and 7 bits of fraction
#define STEP_POSITION_ADD7 0x4e // read a 16 bit word - step count7 - 9 bits signed position and 7 bits of fraction


extern int KanalogPresent;     // 1=Kanalog Present
extern int KStepPresent;       // 1=KStep Present - set this to mux inputs into virtual bits 48-63

// Kanalog FPGA Registers for internal use only
#define KAN_TRIG_REG 0xA0		// triggers a transfer to/from Kanalog, 1-enables Kanalog, 2-enables RS232, 3-both
#define KAN_DAC_REGS 0x80  		// 8 - 12 bit r/w regs
#define KAN_FET_OPTO 0x88  		// 1 - 16 bit 15-8 FET drivers, 7-0 Opto Outputs
#define KAN_GPOUT    0x89  		// 1 - 8 bit   7-0 GP 3.3V OUTPUTS
#define KAN_ADC_REGS 0x90  		// 8 - 12 bit r/w regs
#define KAN_OPTOIN_GPIN 0x98  	// 1 - 16 bit 15-8 OptoInputs, 7-0 GP 3.3V inputs

// Kanalog IO Bit numbers
#define KAN_INPUTS 128			// 16 Bits 128-143 (128-135 GPIN, 136-143 Opto in)
#define KAN_NINPUTS 16			// 16 Input Bits
#define KAN_OUTPUTS 144			// 24 Bits 144-167 (144-151 Opto out, 152-159 FET/Relay Drivers, 160-167 GPOUT)
#define KAN_NOUTPUTS 24			// 24 output Bits



// 3 Phase manual control. Angle is specified in cycles.
// cycles is a double precision value an may be a very
// large number.  Only the fractional part will be used.  
#define MAXV 230.0f // Max allowed 3 phase vector (512/2/sin(60) - EOFF*4)
void Write3PH(CHAN *ch0, float v, double angle_in_cycles); // put a voltage v on a 3 Phase motor at specified commutation angle
void Write4PH(CHAN *ch0, float v, double angle_in_cycles); // put a voltage v on a 4 Phase motor at specified commutation angle


extern int LastPWM[N_PWMS+2*N_PWMS_SNAP];    // +/- 255 counts 

int ReadADC(int ch);           // User Programs should use ADC() command instead
void WriteDAC(int ch, int v);  // User Programs should use DAC() command instead



//  S P I N D L E   A N D   T H R E A D I N G   S U P P O R T

// main Spindle data structure which maintains Spindle 
// Position Info and threading control

typedef struct 
{
	double Position;			// position in revs
	double StartPosition;		// position in revs to begin threading
	double LastUpdatePosition;	// last position in revs where speed was computed
	double RevsPerCount;		// inverse of counts/rev
	double UpdateTick;			// which servo tic we should calc RPM
	double DeltaTicks;			// # Servo tics between speed measurement
	double AdjTimeFilt;			// Lead Adjusted, filtered, time
	double LastCSTime;			// Previous Coordinate System Time
	double *pEncoderPos;		// pointer to Spindle Position 
	double K;					// Tau low pass filter coefficient
	float TrueSpeedRPS;			// last measured speed
	float InvBaseSpeedRPS;		// Reciprocal of Base speed of which trajectory was planned
	double InvBaseSpeedRPSK1;	// InvBaseSpeedRPS * (1-K)
	float InvUpdateTime;		// Reciprocal of update time to avoid division
	float Tau;					// Time constant for spindle filtering
	int Type;					// Type=0 None, Type=1 uses encoder to measure spindle position
	int ThreadingActive;		// True when threading is in progress 

} SPINDLE;

extern SPINDLE Spindle;

void ConfigureSpindle(int Type, int Axis, float UpdateTimeSecs, float Tau, float CountsPerRev);  // configures for type of Spindle feedback
void TrigThreading(float BaseSpeedRPS); // triggers threading coordinated motion, BaseSpeed is the ideal Spindle speed thatthe motion was planned for



//  T I M E   F U N C T I O N S

double Time_sec();// returns total time since power up in seconds

void WaitUntil(double time_sec);// wait until a specified time

void Delay_sec(double sec);// Delay time in seconds

// (returns current time)
double WaitNextTimeSlice(void);// wait until a thread's new time slice begins



#define TIMER0  (*(volatile int *)0x42000010) // 32 bit raw hardware timer counts at CLOCKFREQ (see PC_DSP.h)  

extern volatile double ServoTick;         // increments each servo interrupt
extern volatile unsigned int LastTimer0;  // Last timer value when the ServoTick was incremented



 
extern CHAN chan[N_CHANNELS];   // the axes channel related structures
extern CHAN *ch0;               // global pointer to axis 0 
extern CHAN *ch1;               // global pointer to axis 1 
extern CHAN *ch2;               // global pointer to axis 2 
extern CHAN *ch3;               // global pointer to axis 3 
extern CHAN *ch4;               // global pointer to axis 4 
extern CHAN *ch5;               // global pointer to axis 5 
extern CHAN *ch6;               // global pointer to axis 6 
extern CHAN *ch7;               // global pointer to axis 7 


// This status contains the majority of all status
// so that it can be uploaded as a bulk transfer
extern MAIN_STATUS MainStatus;



void DisableAxis(int ch); // Disable the Axis, Servo output is set to zero


void EnableAxisDest(int ch, double Dest); // enable the Axis and set the destination


void EnableAxis(int ch); // enable the Axis at the current encoder position


// do this before enabling servo or when
// filter coefficients change
void ResetFilters(int ch); // Resets Filter history to known state


void Zero(int ch);  // Zero the Encoder Position and Current Commanded Position

// Basic motion commands to move one axis
void Move(int ch, double x);   // move using absolute coordinates
void MoveAtVel(int chno, double x, float MaxVel);        // move using absolute coordinates and specify the velocity
void MoveRel(int ch, double dx);    // move relative to current destination
void MoveRelAtVel(int chno, double x, float MaxVel);        //  move relative to current destinatio and specify the velocity
void Jog(int ch, double vel);       // move continiously at specified velocity
void MoveExp(int chno, double x, double Tau);  // exponentially approach a target at time constant Tau

int CheckDone(int ch);  // returns 1 if axis is Done, 0 if not, -1 if axis is disabled


// Basic motion commands to move 3 axes 
// 
void MoveXYZABC(double x, double y, double z, double a, double b, double c); // Moves 6 axes (each axis moves independently)
int CheckDoneXYZABC(); // Check if all CS axis have completed , returns 1 if all complete, -1 if any is disabled, otherwise 0


int CheckDoneBuf();   // returns 1 if Done, 0 if not, -1 if any axis in CS disabled
int CheckDoneGather();
void StartMove(int ch);

void SetupForMove(double From, double To, float MaxVel, CHAN *ch, int CoeffOffset,
                                                  int NoJerkControlAtStart, 
                                                  int NoJerkControlAtEnd,
                                                  int Start);
                                                  
void SetupForMotionPause(double x,CHAN *ch,int CoeffOffset, double time);  // stay still                                                   

// coordinate systems #0 - axis definitions
extern int CS0_axis_x; // Axis channel number to use as x
extern int CS0_axis_y; // Axis channel number to use as y  
extern int CS0_axis_z; // Axis channel number to use as z  
extern int CS0_axis_a; // Axis channel number to use as a  
extern int CS0_axis_b; // Axis channel number to use as b  
extern int CS0_axis_c; // Axis channel number to use as c  
extern int CS0_axis_u; // Axis channel number to use as u  
extern int CS0_axis_v; // Axis channel number to use as v  

void DefineCoordSystem(int axisx, int axisy, int axisz, int axisa); // define axis chan numbers to use as x,y,z,a (set -1 to disable)
void DefineCoordSystem6(int axisx, int axisy, int axisz, int axisa, int axisb, int axisc); // define axis chan numbers to use as x,y,z,a,b,c (set -1 to disable)
void DefineCoordSystem8(int axisx, int axisy, int axisz, int axisa, int axisb, int axisc, int axisu, int axisv); // define axis chan numbers to use as x,y,z,a,b,c,u,v (set -1 to disable)


// A Low Pass filter can be applied to all 8 axes of coordinated motion
// by setting the KLP coefficient.  To compute an appropriate coefficient
// from a time constant Tau in seconds use KLP = exp(-TIMEBASE/Tau);
	
extern double KLP;  // coordinated motion low pass filter coefficient

// TauKLP = -TIMEBASE/log(KLP);  automatically computed by ExecBuf command
extern float TauKLP;		    // "smoothing" related time constant used for End Motion

extern float SplineTauFactor;  // Final Spline motion to target will have a time duration of this number of TauKLP (defaults to 2)


// Stop profile generation
// which will stop updating (freeze) the destination 
// new commands can then be placed into the queue

void StopMotion(CHAN *ch); 

// put a trajectory off into the queue 
 
void SetupForMotionEnd(CHAN *ch, int CoeffOffset) ;


// Digital I/O Functions 
 
#define BIT_SET  0x100      // rel address in FPGA where bit set ports reside
#define BIT_CLR  0x110      // rel address in FPGA where bit clear ports reside
#define BIT_DIR  0x120      // rel address in FPGA where bit Direction ports reside
#define BIT_READ 0x130      // rel address in FPGA where bit read ports reside

// Fixed I/O bit definitions

#define LED0 46  // KFLOP LED #0 bit number
#define LED1 47	 // KFLOP LED #1 bit number


// Virtual I/O bits
extern int VirtualBits;   		// Virtual I/O bits simulated in memory, use SetBit/ClearBit/SetStateBit(32-63 to reference)

// Virtual I/O bits Extended 1024-2047
extern int VirtualBitsEx[N_VIRTUAL_BITS_EX/32];	// 1024 Expanded Virtual Bits (1024-2047)

extern int BitDirShadow[2];  	// direction of all 64 I/O bits
extern int BitDirShadowSnap0;  	// direction of 14 Snap Amp, 1st board, I/O bits
extern int BitDirShadowSnap1;  	// direction of 14 Snap Amp, 2nd board, I/O bits

void SetBitDirection(int bit, int dir); // define bit as input (0) or output (1) 
int GetBitDirection(int bit);           // returns whether bit is defined as input (0) or output (1)
void SetBit(int bit);                   // set a bit high (bit must be defined as an output, see SetBitDirection)
void ClearBit(int bit);                 // set a bit low (bit must be defined as an output, see SetBitDirection)
void SetStateBit(int bit, int state);   // set a bit high or low (bit must be defined as an output, see SetBitDirection)
int ReadBit(int bit);                   // read the state of an I/O bit


// KONNECT AUX PORT FUNCTIONS
// 
// Example:
//
// Configure KFLOP to service Konnect 32 Input 16 output IO board
// Board address is 0, 
// 16 Outputs are mapped to Virtual IO 48-63 (VirtualBits)
// 32 Inputs are mapped to Virtual IO 1024-1055 (VirtualBits[0])
//
//		InitAux();
//		AddKonnect(0,&VirtualBits,VirtualBitsEx);
//

void InitAux(void);  // Initialize the AUX Port KFLOP JP4 or JP6 and clear list of board to be serviced (only one Aux Port may be used at a time)

// Board address, address of 32-bit into to get the Output Bits (in low 16 bits), address of where to put 32 Inputs
void AddKonnect(int BoardAddress, int *OutputAddress, int *InputAddress);  // add a Konnect Board to list of AUX1 Port Boards to be serviced

// Board address, address of 32-bit into to get the Output Bits (in low 16 bits), address of where to put 32 Inputs
void AddKonnect_Aux0(int BoardAddress, int *OutputAddress, int *InputAddress); // add a Konnect Board to list of AUX0 Port Boards to be serviced


                                
// User Print Routines
//
// sends a string to the user console of the KMotion
// application.  Because other threads may be sending
// characters, the strings are buffered in a queue
// and sent by the primary thread as soon as there
// is no PC input to process.  The string prepends 
// an escape so the PC application knows for sure
// to send this to the Console window and not to 
// process it as a response to a command that may
// be in the pipeline.
//
//
// normally the routine exits to the caller quickly
// unless the queue is full, then it must wait

#define MAX_STRING 128                    
#define MAX_NSTRINGS 256  // must be binary    

// Note: standard C language printf 

int printf(const char *format, ...);     		// Print formatted string to console
int sprintf(char *s, const char *format, ...); 	// Print formatted string to string

typedef int FILE;
FILE *fopen(const char*, const char*);		   // Open a text file for writing on the PC (2nd param is ignored) 
int fprintf(FILE *f, const char * format, ...);		   // Print formatted string to the PC's Disk File
int fclose(FILE *f);                           // Close the disk file on the PC

int Print(char *s);                      		// Print a string to the console window
int PrintFloat(char *Format, double v);  		// Print a double using printf format, ex "%8.3f\n"
int PrintInt(char *Format, int v);       		// Print an integer using printf format, ex "result=%4d\n"
                                
int sscanf(const char *_str, const char *_fmt, ...); //scan string and convert to values 

/*
 * MessageBox() Flags thes can be passed to the PC to invoke MessageBoxes
 *              for some applications such as KMotionCNC which monitor upload
 *              status and present message boxes when requested.  See the pc-dsp.h
 *              header for more information
 */
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
#define MB_CANCELTRYCONTINUE        0x00000006L
#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L
#define MB_APPLMODAL                0x00000000L
#define MB_SYSTEMMODAL              0x00001000L
#define MB_TASKMODAL                0x00002000L
#define MB_NOFOCUS                  0x00008000L
#define MB_SETFOREGROUND            0x00010000L
#define MB_DEFAULT_DESKTOP_ONLY     0x00020000L
#define MB_TOPMOST                  0x00040000L
#define MB_RIGHT                    0x00080000L

/*
 * Dialog Box Command IDs
 */
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7



// Misc routines

void DoResolverInput2(CHAN *chx, float x, float y);  // optimized routine to handle sin/cosine resolver input

extern double ResolverFactor; // defaults to 1000.0/TWO_PI converts sine/cosine angle to reported Position


// M U L T I - T H R E A D   S U P P O R T

// user threads are numbered 1 .. n

void StartThread(int thread);  // starts a downloaded program at it's entry point
void PauseThread(int thread);  // stops a thread from executing
int ResumeThread(int thread);  // resumes a tread after a pause 
void ThreadDone(void);         // call to terminate current thread


// A User Program Call Back can be defined to be called every Servo Sample
// Set to Non-NULL for the the Callback to be made.  The Callback Routine
// must return with a few micro seconds or the system may become unstable
typedef void USERCALLBACK(void);
extern USERCALLBACK *UserCallBack;


// used to allow mutual exclusive access to a resourse
// (waits until resourse is available, then locks it)
// if the thread that locked it is no longer active,
// release the lock

void MutexLock(int *mutex);
void MutexUnlock(int *mutex);

// These routines are written in assembly such that
// they are atomic and are un-interruptible by using
// instructions in delayed branching 

void AtomicSet(int *p, int mask);
//{
//	*p = *p | mask;
//}

void AtomicClear(int *p, int mask);
//{
//	*p = *p & mask;
//}

// test a location and if zero, set
// to value.  returns the original
// value.  routine is atomic

int TestAndSet(int *mutex, int value);
//{
//	register result = *mutex;
//	if (result==0) *mutex=value;
//	return result;
//} 


// S N A P   A M P   D E F I N I T I O N S

#define KM_SNAP_READ_LOW 0xc
#define KM_SNAP_READ_HI  0xd
#define KM_SNAP_READ_EXCEPTION 0x10
#define KM_SNAP_CLK_ENA 0x09
#define KM_SNAP_SHIFT_BYTE 0x0a
#define KM_SNAP_WRITE_HIGH_TRIG 0x0b
#define KM_SNAP_READ_ADD_TRIG 0x0c
#define KM_SNAP_READ_ADD_BITMAP 0x28
// FPGA Registers

#define SNAP0 0x40  // Base Addresse SNAP AMP #0 
#define SNAP1 0x60  // Base Addresse SNAP AMP #1  

// write addresses

#define SNAP_PWMS 0 			 	// (4) 16 bit PWMs 
#define SNAP_CLR_ENC_ERRS 8			// any write clears all encoder errors
#define SNAP_CUR_LOOP_GAINS 9		// (4) 8 bit Current Loop Gains Default=16
#define SNAP_SET_BIT 17				// (14) GPIO bits
#define SNAP_CLR_BIT 18				// (14) GPIO bits
#define SNAP_DIR_BIT 19				// (14) GPIO bits
#define SNAP_SUPPLY_CLAMP0 20		// 16 bit power supply clamp setting side A
#define SNAP_SUPPLY_CLAMP_ENA0 21	// 1 bit power supply clamp enable side A
#define SNAP_SUPPLY_CLAMP1 22		// 16 bit power supply clamp setting side B
#define SNAP_SUPPLY_CLAMP_ENA1 23	// 1 bit power supply clamp enable side B
#define SNAP_PEAK_CUR_LIMIT0 24		// 4 bits peak current limit side A
#define SNAP_PEAK_CUR_LIMIT1 25		// 4 bits peak current limit side B

// read addresses

#define SNAP_PWMS 0 		 		// (4) 16 bit PWMs 
#define SNAP_ENC 8 		 			// (4) 16 bit Encoders (bit15=error 7-0=data) 
#define SNAP_CURRENT_A0 12			// Measured Current Side A Lead A (14bits)
#define SNAP_CURRENT_C0 13			// Measured Current Side A Lead C
#define SNAP_CURRENT_A1 14			// Measured Current Side B Lead A
#define SNAP_CURRENT_C1 15			// Measured Current Side B Lead C
#define SNAP_DIFF_IN 16 		 	// 16 bits (15-8= Diff inputs 7-0=encoder inputs) 
#define SNAP_IN_BIT 17				// (14) GPIO bits
#define SNAP_SUPPLY_VOLT0 22		// Measured Supply Voltage Side A
#define SNAP_SUPPLY_VOLT1 23		// Measured Supply Voltage Side B
#define SNAP_TEMP0 24				// Measured Temperature Side A
#define SNAP_TEMP1 25				// Measured Temperature Side B
#define SNAP_STATUS 30				// Status (FAN,OverTemp1,OverTemp0,OVER_CUR1,OVER_CUR0,Fault)
#define SNAP_RESET 31				// Reset

//RS232 FPGA Register Definitions

#define RS232_STATUS 0xc1		// Status Reg Address
#define RS232_DATA 0xc0			// 8 bit data read/write reg address
#define RS232_DATA_READY 0x01	// Data ready to read status mask
#define RS232_TRANSMIT_FULL 0x02// Transmit buffer full status mask

#define RS232_BAUD_REG 0xc1	// Set Baud rate 8-bit divisor Reg Address 
#define RS232_BAUD_115200 	((16666666/115200/16)-1)// 8-bit divisor value to set 115200 baud
#define RS232_BAUD_57600 	((16666666/57600/16)-1)	// 8-bit divisor value to set  57600 baud
#define RS232_BAUD_38400 	((16666666/38400/16)-1)	// 8-bit divisor value to set  38400 baud
#define RS232_BAUD_19200 	((16666666/19200/16)-1)	// 8-bit divisor value to set  19200 baud
#define RS232_BAUD_9600 	((16666666/9600/16)-1)	// 8-bit divisor value to set   9600 baud
#define RS232_BAUD_4800 	((16666666/4800/16)-1)	// 8-bit divisor value to set   4800 baud

void InitRS232(int baud);
void EnableRS232Cmds(int baud);

extern char * volatile pRS232RecIn;  // Buffered Receive Pointer Head
extern char *pRS232RecOut;           // Buffered Receive Pointer Tail
extern char *pRS232TxIn;             // Buffered Transmit Pointer Head
extern char * volatile pRS232TxOut;  // Buffered Transmit Pointer Tail
extern int DoRS232Cmds;              // Enables/disables KFLOP Command processor to/from RS232

char RS232_GetChar(void);   // Get Internally Buffered (1000 chars) RS232 received Data 
void RS232_PutChar(char c); // Put Internally Buffered (1000 chars) RS232 transmit Data



#endif

