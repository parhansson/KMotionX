// PC.c  Command List for PC to DSP Communication
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/
//
// Online help is auto generated from this file  

#pragma DATA_SECTION(Commands,".big");


#include "KMotionDef.h"
#include "PC.h"


char Commands[][MAX_CMD_LENGTH] = {
//start 
"EXECUTE D1 7",								// ExecuteN/Begin execution of thread N/ie. Execute1
"ENTRYPOINT D1 7 H",						// EntryPointN H/Set Execution Start Address of/Thread N to hex address H/ie. Entrypoint1 80070000
"SETSTARTUPTHREAD D1 7 D0 1",     			// SetStartupThreadN B/Turn on or off thread on boot/ie. SetStartupThread0 1
"LOADDATA HH",								// LoadData H N/Poke data into memory hex addr H # of bytes N/up to 64 bytes per line/ie. LoadData 80070000 4/FF FF FF FF
"LOADFLASH HH",								// LoadFlash H N/Poke data into FLASH image hex offset H # of bytes N/up to 64 bytes per line/ie. LoadFlash FF00 4/FF FF FF FF
"CLEARFLASHIMAGE ",							// ClearFlashImage/Prepare to download FLASH image/Sets entire flash image to zero/ie. ClearFlashImage
"PROGFLASHIMAGE ",							// ProgFlashImage/Program entire FLASH image to FLASH Memory/ie. ProgFlashImage
"KILL D1 7",								// KillN/Stop execution of thread N/ie. Kill0
"LED D0 1 =D0 1",							// LEDN=B/Turn LED number 0 or 1 on or off/ie. LED1=1
"PWMC D8 15 =D-1000 1000",					// PWMCN=D/Set PWM channel N to current loop mode/and to value D (range -1000 to 1000)/ie. PWMC0=-99
"PWMR D0 15 =D-511 511",					// PWMRN=D/Set PWM channel N to recirculate mode/and to value D (range -511 to 511)/ie. PWMR0=-99
"PWM D0 15 =D-255 255",  					// PWMN=D/Set PWM channel N to locked anti-phase mode/and to value D (range -256 to 256)/ie. PWM0=-99
"MOVEREL D0 7 =G",							// MoveRelN=D/Move axis N relative to current dest D units/ie. MoveRel0=100.1      
"MOVERELATVEL D0 7 =GG",					// MoveRelAtVelN=D V/Move axis N relative to current dest D units/using specified Velocity/ie. MoveRelAtVel0=100.1 30.0      
"MOVEXYZABC GGGGGG",   						// MoveXYZABC=D D D D D D/Move the 6 axes defined to be x,y,z,a,b,c/(each axis moves independently)/ie. MoveXYZABC 100.1 200.2 300.3 400.4 500.5 600.6
"MOVE D0 7 =G",								// MoveN=D/Move axis N to absolute coordinates D/ie. Move0=100.1            
"MOVEATVEL D0 7 =GG",						// MoveAtVelN=D V/Move axis N to absolute coordinates D at specified Velocity/ie. MoveAtVel0=100.1 30.0            
"MOVEEXP D0 7 =GG",							// MoveExpN=D T/Move axis N to absolute coordinates D exponentially at Time Constant T/ie. MoveExp0=100.1 1.0            
"JOG D0 7 =G",								// JogN=V/Move axis N at velocity V/Uses Accel and Jerk parameters for the axis/Specify zero Vel to decel to stop/ie. Jog0=-200.5
"ADC D0 7",									// ADCN/Display ADC channel N (0 through 7)/range -2048 to 2047/Channels 0-3 are 10V inputs/Channels 4-7 are Motor Currents/ie. ADC0      
"DAC D0 7 =D-2048 2047",					// DACN=M/Set DAC N (0..7) to/value M (-2048..2047)/ie. DAC0=2000
"3PH D0 7 =GG",								// 3PHN=D A/Set PWMs of axis N for Magnitude D phase angle A/Magnitude is -230 ,, +230/Angle in Commutation cycles/ie. 3PH0=230 0.5
"4PH D0 7 =GG",								// 4PHN=D A/Set PWMs of axis N for Magnitude D phase angle A/Magnitude is -250 ,, +250/Angle in Commutation cycles/ie. 4PH0=250 0.5
"FLASH ",									// Flash/Flash current User programs and axis params/ie. Flash      
"ZERO D0 7",								// ZeroN/Clear the measured position of axis N/ie. Zero0      
"DISABLEAXIS D0 7",							// DisableAxisN/Kill Motion and disable Motor on Axis N/ie. DisableAxis0
"ENABLEAXISDEST D0 7 G",           			// EnableAxisDestN D/Set Axis N to specified Dest D and enable the axis/ie. EnableAxisDest0 1000.0
"ENABLEAXIS D0 7",							// EnableAxisN/Set Axis N to Current Measured Position/and enable the axis/ie. Enable0
"GATHERMOVE D0 7 GD1 40000",     			// GatherMoveN D M/Perform a profiled move on axis N with/a distance of D while/Gathering M points of data/ie. GatherMove0 1000.0 2000 
"GATHERSTEP D0 7 GD1 40000",     			// GatherStepN D M/Perform a step on axis N with/a distance of D while/Gathering M points of data/ie. GatherStep0 1000.0 2000       
"CHECKDONEXYZABC ",							// CheckDoneXYZABC/Display 1 if all defined XYZABC axes/have completed their motions, otherwise 0/ie. CheckDoneXYZABC
"CHECKDONEGATHER ",							// CheckDoneGather/Display 1 if data gather is completed, otherwise 0/ie. CheckDoneGather
"CHECKDONEBUF ",							// CheckDoneBuf/Display 1 if all coordinated move/sequences have completed, otherwise 0/ie. CheckDoneBuf
"CHECKDONE D0 7", 							// CheckDoneN/Display 1 if axis N/has completed its motion, otherwise 0/ie. CheckDone0
"CHECKTHREAD D1 7", 						// CheckThreadN/Display 1 if Thread is currently executing, otherwise 0/ie. CheckThread1
"GETINJECT ",								// GetInject/Display results of signal/injection and gathering/ie. GetInject      
"GETSTATUS ",								// GetStatus/Upload Main Status record in hex format/ie. GetStatus      
"GETGATHERHEX D0 1999999 D1 2000000",    	// GetGatherHex O N/Display values (as 32 bit Hex words)/in the Gather Buffer/beginning at decimal offset O/for decimal N words/ie. GetGatherHex 0 100      
"SETGATHERHEX D0 1999999 D1 2000000", 		// SetGatherHex O N/Set values (as 32 bit Hex words)/into the Gather Buffer/beginning at decimal offset O/for decimal N words/ie. SetGatherHex 0 3/FFFFFFFF FFFFFFFF FFFFFFFF      
"SETGATHERDEC D0 1999999 G",   				// SetGatherDec O D/Write a single 32-bit signed decimal word into the Gather Buffer/at decimal offset O/ie. SetGatherDec 1000 32767      
"GETGATHERDEC D0 1999999",   				// GetGatherDec O/Read a single word from the Gather Buffer/at decimal offset O/a single 32-bit value displayed as a signed decimal number/ie. GetGatherDec 1000
"SETPERSISTDEC D0 199 G",   				// SetPersistDec O D/Write a single word into the Persist Array/at decimal offset O/a single 32-bit value specified as a signed decimal number/ie. SetPersistDec 10 32767      
"SETPERSISTHEX D0 199 H",   				// SetPersistHex O D/Write a single word into the Persist Array/at decimal offset O/a single 32-bit value specified as an unsigned hex number/ie. SetPersistHex 10 FFFFFFFF      
"GETPERSISTDEC D0 199",   					// GetPersistDec O/Read a single word from the Persist Array/at decimal offset O/a single 32-bit value displayed as a signed decimal number/ie. GetPersistDec 10      
"GETPERSISTHEX D0 199",   					// GetPersistHex O/Read a single word from the Persist Array/at decimal offset O/a single 32-bit value displayed as an unsigned hex number/ie. GetPersistHex 10      
"GETGATHER D1 40000",						// GetGather N/Upload N data points from previous/GatherMove or GatherStep command/ie. GetGather 1000
"INJECT D0 7 GG",							// InjectN F A/Inject random stimulus into axis N/with cutoff frequency of F (Hz)/of amplitude A (Position units)/ie. InjectN 100.0 20.0      
"SETBITDIRECTION D0 167 *D0 1",				// SetBitDirectionN D/define IO bit N (0..30)/as input (D=0) or output (D=1)/ie. SetBitDirection0=1
"GETBITDIRECTION D0 167",					// GetBitDirectionN/Displays whether an IO bit N (0..30)/is defined as input (0) or output (1)/ie. GetBitDirection0
"SETSTATEBITBUF D0 255 =D0 1",				// SetStateBitBufN=B/Inserts into coordinated move buffer/to set an IO bit N (0..255) high or low/or Virtual IO bits 48-63/(actual IO bits must be defined as an output, see SetBitDirection)/ie. SetStateBitBuf0=1
"SETBITBUF D0 255",							// SetBitBufN/Inserts into coordinated move buffer/command to set an IO bit N(0..255)/or Virtual IO bits 48-63/(actual IO bits must be defined as an output, see SetBitDirection)/ie. SetBitBuf0
"CLEARBITBUF D0 255",						// ClearBitBufN/Inserts into coordinated move buffer/command to clear an IO bit N(0..255)/or a Virtual IO bit 48-63/(actual IO bits must be defined as an output, see SetBitDirection)/ie. ClearBitBuf0
"SETBIT D0 2047",							// SetBitN/sets an IO bit N(0..2047)/or Virtual IO bit 48-63, Extended Virtual 1024-2047/(actual IO bits must be defined as an output, see SetBitDirection)/ie. SetBit0
"CLEARBIT D0 2047",							// ClearBitN/clears an IO bit N(0..2047)/or Virtual IO bit 48-63, Extended Virtual 1024-2047/(actual IO bits must be defined as an output, see SetBitDirection)/ie. ClearBit0
"SETSTATEBIT D0 2047 =D0 1",				// SetStateBitN=B/sets an IO bit N (0..2047) high or low/or Virtual IO bit 48-63, Extended Virtual 1024-2047/(actual I/O bits must be defined as an output, see SetBitDirection)/ie. SetStateBit0=1
"READBIT D0 2047",							// ReadBitN/Displays whether an IO bit N (0..2047)/or Virtual IO bit 48-63, Extended Virtual 1024-2047/is high (1) or low (0)/ie. ReadBit0
"OPENBUF ",									// OpenBuf/Clear and open the buffer for/coordinated linear and circular segments/ie. OpenBuf
"EXECBUF ",									// ExecBuf/Execute the buffer of coordinated/linear and circular segments/ie. ExecBuf
"EXECTIME ",								// ExecTime/Display Total Time (seconds) of segments in the buffer that have already been completed/Negative if buffer halted (starved)/ie. ExecTime
"LINEARHEXEX HHHHHHHHHHHHHHHHHHHHH",		// LinearHexEx x0 y0 z0 a0 b0 c0 u0 v0 x1 y1 z1 a1 b1 c1 u1 v1 a b c d t/place linear interpolated move into buffer/start point, end point, and parametric eq/values are hex floats/ie. LinearHexEx 0 0 0 0 0 0 0 0 3f800000 3f800000 3f800000 3f800000 3f800000 3f800000 3f800000 3f800000 0 0 3f800000 0 3f800000
"LINEARHEX HHHHHHHHHHHHHHHHH",				// LinearHex x0 y0 z0 a0 b0 c0 x1 y1 z1 a1 b1 c1 a b c d t/place linear interpolated move into buffer/start point, end point, and parametric eq/values are hex floats/ie. LinearHex 0 0 0 0 0 0 3f800000 3f800000 3f800000 3f800000 3f800000 3f800000 0 0 3f800000 0 3f800000
"LHEX1 HHHHHHHHHHH",						// LHex1 x1 y1 z1 a1 b1 c1 a b c d t/place linear interpolated move into buffer/start point(from last), end point, and parametric eq/values are hex floats/ie. LHex1 3f800000 3f800000 3f800000 3f800000 3f800000 3f800000 0 0 3f800000 0 3f800000
"LHEX2 HHHHH",								// LHex2 a b c d t/place linear interpolated move into buffer/uses start point, end point from previous command, and parametric eq/values are hex floats/ie. LHex2 0 0 3f800000 0 3f800000
"LINEAREX GGGGGGGGGGGGGGGGGGGGG", 			// LinearEx x0 y0 z0 a0 b0 c0 u0 v0 x1 y1 z1 a1 b1 c1 u1 v1 a b c d t/place linear interpolated move into buffer/3D start point, end point, and parametric eq/ie. Linear 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0 0.0 1.0 0.0 1.0
"LINEAR GGGGGGGGGGGGGGGGG", 				// Linear x0 y0 z0 a0 b0 c0 x1 y1 z1 a1 b1 c1 a b c d t/place linear interpolated move into buffer/3D start point, end point, and parametric eq/ie. Linear 0.0 0.0 0.0 0.0 0.0 0.0 1.0 1.0 1.0 1.0 1.0 1.0 0.0 0.0 1.0 0.0 1.0
"ARCHEX HHHHHHHHHHHHHHHHHHH",   			// ArcHex xc yc rx ry theta0 dtheta z0 a0 b0 c0 z1 a1 b1 c1 a b c d t/place circular interpolated (xy) move into buffer/x center, y center, x radius, y radius,/begin theta, delta theta, begin zabc, end zabc,/and parametric eq/ie. Arc 3f000000 3f000000 3f800000 3f800000 0 3f800000 0 0 0 0 0 0 0 0 0 0 3f800000 0 3f800000
"ARC GGGGGGGGGGGGGGGGGGG",   				// Arc xc yc rx ry theta0 dtheta z0 a0 b0 c00 z1 a1 b1 c1 a b c d t/place circular interpolated (xy) move into buffer/x center, y center, x radius, y radius,/begin theta, delta theta, begin zabc, end zabc,/and parametric eq/ie. Arc 0.5 0.5 1.0 1.0 0.0 3.14 0.0 0.1 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 1.0 0.0 1.0
"ARCHEXZX HHHHHHHHHHHHHHHHHHH", 			// ArcHexZX zc xc rz rx theta0 dtheta y0 a0 b0 c0 y1 a0 b0 c0 a b c d t/place circular interpolated (zx) move into buffer/z center, x center, z radius, x radius,/begin theta, delta theta, begin yabc, end yabc,/and parametric eq/ie. ArcZX 3f000000 3f000000 3f800000 3f800000 0 3f800000 0 0 0 0 0 0 0 0 0 0 3f800000 0 3f800000
"ARCXZ GGGGGGGGGGGGGGGGGGG",   				// ArcZX xc zc rx rz theta0 dtheta y0 a0 b0 c0 y1 a1 b1 c1 a b c d t/place circular interpolated (xz) move into buffer/x center, z center, x radius, z radius,/begin theta, delta theta, begin yabc, end yabc,/and parametric eq/ie. ArcZX 0.5 0.5 1.0 1.0 0.0 3.14 0.0 0.1 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 1.0 0.0 1.0
"ARCHEXYZ HHHHHHHHHHHHHHHHHHH",				// ArcHexYZ yc zc ry rz theta0 dtheta x0 a0 b0 c0 x1 a1 b1 c1 a b c d t/place circular interpolated (yz) move into buffer/y center, z center, y radius, z radius,/begin theta, delta theta, begin xabc, end xabc,/and parametric eq/ie. ArcYZ 3f000000 3f000000 3f800000 3f800000 0 3f800000 0 0 0 0 0 0 0 0 0 0 3f800000 0 3f800000
"ARCYZ GGGGGGGGGGGGGGGGGGG",  				// ArcYZ yc zc ry rz theta0 dtheta x0 a0 b0 c0 x1 a0 b0 c0 a b c d t/place circular interpolated (yz) move into buffer/y center, z center, y radius, z radius,/begin theta, delta theta, begin xabc, end xabc,/and parametric eq/ie. ArcYZ 0.5 0.5 1.0 1.0 0.0 3.14 0.0 0.1 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 1.0 0.0 1.0
"VERSION ",									// Version/Display DSP Firmware Version and Build date and time/ie. Version
"REBOOT! ",									// Reboot!/Reboot from FLASH (Power Up Reset) KMotion Board/ie. Reboot! 
"STOPIMMEDIATE D0 2",						// StopImmediateN/N=0 Stop, N=1 Resume, N=2 Clear/all motion of any Coordinated Motion Axes Immediately/ie. StopImmediate0 
"GETSTOPSTATE ",							// GetStopState/Get State of StopImmediate/0=none,1=stopping indep,2=stopping coord, 3=stopped indep, 4=stopped coord/ie. GetStopState 
"GETSPINDLERPS ",							// GetSpindleRPS/Get measured Spindle RPM in Revs per second/i.e. GetSpindleRPS
"CONFIGSPINDLE D0 1 D0 7 GGG",				// ConfigSpindle T A U W C/Configure Spindle Settings/Type 0=none 1=encoder/Axis used for encoder/UpdateTimeSecs - delta time for measurement/Tau - low pass filter time constant (threading)/CountsPerRev - encoder counts per rev/i.e. ConfigureSpindle 1 0 0.2 0.1 4096.0
"TRIGTHREAD G",								// TrigThread S/Trigger Coordinated Motion Threading Motion/B=Base Spindle Speed (RPS) that coordinated motion was planned for/i.e. TrigThread 10.0
"ECHO ",									// Echo S/Echo String S back to the Console/ie.Echo Hello 
#ifdef KFLOP
"USB ",										// Commands from USB Mode/i.e. USB
"RS232 D9600 115200",						// Commands from RS232 Baud/i.e. RS232 57600
"FPGA D0 1024 D0 255",						// Write 8 bit value directly to FPGA/i.e. FPGA 6 191
"FPGAW D0 1024 D0 65535",					// Write 16 bit value directly to FPGA/i.e. FPGAW 6 191
"WAITBITBUF D0 255",						// WaitBitBufN/Inserts into coordinated move buffer/command to wait for an IO bit N(0..255)/or Virtual IO bits 48-63/to become high/ie. WaitBitBuf0
"WAITNOTBITBUF D0 255",						// WaitNotBitBufN/Inserts into coordinated move buffer/command to wait for an IO bit N(0..255)/or Virtual IO bits 48-63/to become low/ie. WaitNotBitBuf0
#endif
"DEFINECS ?6=ddddddc",						// DefineCS = X Y Z A B C/Define the 6 Axes that make up/the xyzabc Coordinate System/set unused Axes to -1/ie. DefineCS = 0 1 -1 -1 -1 -1
"DEFINECSEX ?8=ddddddddm",					// DefineCSEx = X Y Z A B C U V/Define the 8 Axes that make up/the xyzabcuv Coordinate System/set unused Axes to -1/ie. DefineCSEx = 0 1 -1 -1 -1 -1 -1 -1
"LIMITSWITCH D0 7 ?4=Hi",					// LimitSwitchN=H/Configure Limit Switch Options/Specify Hex value where/Bit 0 1=Stop Motor on Neg Limit, 0=Ignore Neg limit/Bit 1 1=Stop Motor on Pos Limit, 0=Ignore Pos limit/Bit 2 Neg Limit Polarity 0=stop on high, 1=stop on low/Bit 3 Pos Limit Polarity 0=stop on high, 1=stop on low/Bits 4-7/       Action - 0 Kill Motor Drive/       1 Disallow drive in direction of limit/       2 Stop movement/Bits 16-23 Neg Limit Bit number/Bits 24-31 Pos Limit Bit number/ie.LimitSwitch2 0C0D0003  
"INPUTMODE D0 7 ?1=D0 4 i",					// InputModeN=D/Set position input mode for axis/ENCODER_MODE 1/ADC_MODE 2/RESOLVER_MODE 3/USER_INPUT_MODE 4/ie. InputMode0=1
"BACKLASHMODE D0 7 ?1=D0 4 i",				// BacklashModeN=D/Set Backlash mode of operation for axis/BACKLASH_OFF 0/BACKLASH_LINEAR 1/ie. BacklashMode0=1
"OUTPUTMODE D0 7 ?1=D0 8 i", 				// OutputModeN=D/Set motor output mode for axis/MICROSTEP_MODE 1/DC_SERVO_MODE 2/BRUSHLESS_3PH_MODE 3/BRUSHLESS_4PH_MODE 4/DAC_SERVO_MODE 5/STEP_DIR_MODE 6/CL_STEP_DIR_MODE 7/CL_MICROSTEP_MODE 8/ie. SetOutputMode0=1
"DEST D0 7 ?3=Gg",                 			// DestN/Set last commanded Destination for axis N/ie. Dest0      
"POS D0 7 ?3=Gg",							// PosN=P/Set measured position of axis N to P/ie. Pos0=100.0      
"ENABLED D0 7 ?1",							// EnabledN/Display whether the specified axis is enabled/ie. Enabled0      
"VEL D0 7 ?2=F0 1e12 f",					// VelN=V/Get or Set max velocity (for independent moves)/of axis N to V Position units per sec/ie.Vel0=100.0
"ACCEL D0 7 ?2=F0 1e12 f",					// AccelN=A/Get or Set max acceleration (for independent moves and jogs)/of axis N to A Position units per sec2/ie. Accel0=1000.0
"JERK D0 7 ?2=F0 1e12 f",					// JerkN=J/Get or Set max jerk (for independent moves and jogs)/of axis N to J Position units per sec3/ie. Jerk0=10000.0
"FFACCEL D0 7 ?2=F-1e12 1e12 f", 			// FFAccelN=F/Get or Set Acceleration Feed forward for axis N to/F (Output units per Input Units per sec2)/ie. FFAccel0=100.0
"FFVEL D0 7 ?2=F-1e12 1e12 f",				// FFVelN=F/Get or Set Velocity Feed forward for axis N to/F (Output units per Input Units per sec)/ie. FFVel0=100.0
"MAXI D0 7 ?2=F0 1e12 f",					// MaxIN=F/Get or Set Maximum Integrator "wind up" for axis N/Integrator saturates at value F/ie. MaxI0=100.0
"MAXERR D0 7 ?2=F0 1e12 f",					// MaxErr=F/Get or Set Maximum Error for axis N/Limits magnitude of error entering PID to F/ie. MaxErr0=100.0
"MAXOUTPUT D0 7 ?2=F0 1e12 f",				// MaxOutput=F/Get or Set Maximum Output for axis N/Limits magnitude of servo output to F/ie. MaxOutput0=100.0
"DEADBANDGAIN D0 7 ?2=F0 1e12 f",			// DeadBandGainN=G/Get or Set gain for axis N while error is/within the deadband range to G/ie. DeadBandGain0=0.5
"DEADBANDRANGE D0 7 ?2=F0 1e12 f",			// DeadBandRangeN=R/Get or Set range for axis N where deadband gain/is applied to +\- R position units/ie. DeadBandRange0=1.0
"BACKLASHAMOUNT D0 7 ?2=F0 1e12 f",			// BacklashAmountN=G/Get or Set Backlash amount N in motor steps or encoder counts/ie. BacklashAmount0=5.5
"BACKLASHRATE D0 7 ?2=F0 1e12 f",			// BacklashRateN=R/Get or Set Backlash Rate of change int step or counts per second for axis N/ie. BacklashRate=1000.0
"STEPPERAMPLITUDE D0 7 ?2=F0 255 f",		// StepperAmplitudeN=A/Get or Set output magnitude used/for axis N (if in MicroStepping Mode) to/A output units when stopped or with zero Lead/ie. StepperAmplitude0=250 
"LEAD D0 7 ?2=F0 1e4 f",					// LeadN=L/Get or Set axis N Lead Compensation to L/ie. Lead0=10.0 
"INPUTCHAN0 D0 7 ?1=D0 15 i",				// InputChan0N=C/Get or Set first Input Channel of axis N to C/ie. InputChan03=3 
"INPUTCHAN1 D0 7 ?1=D0 15 i",				// InputChan1N=C/Get or Set 2nd Input Channel of axis N to C/ie. InputChan13=4 
"OUTPUTCHAN0 D0 7 ?1=D0 31 i",				// OutputChan0N=C/Get or Set first Output Channel of axis N to C/ie. OutputChan03=3 
"OUTPUTCHAN1 D0 7 ?1=D0 15 i",				// OutputChan1N=C/Get or Set 2nd Output Channel of axis N to C/ie. OutputChan13=3 
"INPUTGAIN0 D0 7 ?2=Gf",					// InputGain0N=G/Get or Set first Input Gain of axis N to G/ie. InputGain03=1.0 
"INPUTGAIN1 D0 7 ?2=Gf",					// InputGain1N=G/Get or Set 2nd Input Gain of axis N to G/ie. InputGain13=1.0 
"INPUTOFFSET0 D0 7 ?2=Gf",					// InputOffset0N=G/Get or Set first Input Offset of axis N to G/ie. InputOffset03=0.0 
"INPUTOFFSET1 D0 7 ?2=Gf",					// InputOffset1N=G/Get or Set 2nd Input Offset of axis N to G/ie. InputOffset13=0.0 
"OUTPUTGAIN D0 7 ?2=Gf",					// OutputGainN=G/Get or Set Output Gain of axis N to G/ie. OutputGain3=-1.0 
"OUTPUTOFFSET D0 7 ?2=Gf",					// OutputOffsetN=G/Get or Set Output Offset of axis N to G/ie. OutputOffset3=100.0 
"SLAVEGAIN D0 7 ?3=Gg",						// SlaveGainN=G/Get or Set SlaveGain (Ratio to follow Master Axis) of axis N to G/ie. SlaveGain3=-0.5 
"MASTERAXIS D0 7 ?1=D-1 7 i",				// MasterAxisN=D/Set which axis the axis should follow/Specify -1 to disable following any axis/ie. MasterAxis0=-1
"INVDISTPERCYCLE D0 7 ?3=Gg",				// InvDistPerCycleN=G/Get or Set distance per cycle (specified as an inverse)/of axis N.  Either a cycle of Stepper of Brushless Motor/ie. InvDistPerCycle0=0.05 
"MAXFOLLOWINGERROR D0 7 ?3=Gg",				// MaxFollowingErrorN=G/Get or Set Maximum allowed servo position error/before disabling motor drives/of axis N/ie. MaxFollowingError0=100.0 
"COMMUTATIONOFFSET D0 7 ?2=Gf",				// CommutationOffsetN=G/Get or Set 3 or 4 phase commutation offset/PhaseA = sin((Position+CommutationOffset)*invDistPerCycle)/ie. CommutationOffset0=100.0 
"IIR D0 7 D0 2 ?5=GGGGGr",					// IIRN I A1 A2 B0 B1 B2/Get or Set IIR Z domain filter of axis N/Filter number I (0..2)/to specified coefficients/ie. IIR0 0 1.5 2.5 -3.5 4.5 5.5      
"P D0 7 ?2=F-1e12 1e12 f",					// PN=G/Get or Set axis N Proportional Gain to G/ie. P0=10.0 
"I D0 7 ?2=F-1e12 1e12 f",					// IN=G/Get or Set axis N Integral Gain to G/ie. I0=10.0 
"D D0 7 ?2=F-1e12 1e12 f",					// DN=G/Get or Set axis N Derivative Gain to G/ie. D0=10.0
//end
};  

int NCommands=sizeof(Commands)/MAX_CMD_LENGTH;
