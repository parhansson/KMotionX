// SerialServoAxisInit.c
//
// Example Kogna User C init program configuring axis 0 to drive a Feetech
// STS3215 serial servo as a normal KFLOP/Kogna axis.  The servos daisy chain
// on a single wire (IO0, or EX_IO_13 via ServoBusEXIO) and the Input/Output
// Channel is the servo's ID on that bus (0-7, assign with SerialServoSetID.c).
//
// Input  Mode SERIAL_SERVO_INPUT_MODE - Position comes from the servo's encoder
// Output Mode SERIAL_SERVO_MODE       - commanded position is sent to the servo
//                                       (use CL_SERIAL_SERVO_MODE to also run
//                                        the KFLOP servo loop as a correction)
//
// See SERIAL_SERVO_Axis_Mode.md for the full description.

#include "KMotionDef.h"

#define AXIS    0     // KFLOP/Kogna axis channel
#define SERVO   0     // servo ID on the bus (0 - 7)

// one servo control table access through the DSP protocol engine
int ServoReg(int id, int reg, int *data, int two, int rd)
{
	int t;

	ServoAuxID = id;
	ServoAuxReg = reg;
	ServoAuxData = *data;
	ServoAuxTwo = two;
	ServoAuxRead = rd;
	ServoAuxGo = 1;

	for (t = 0; t < 200 && ServoAuxGo; t++) Delay_sec(0.001);
	if (ServoAuxGo) { ServoAuxGo = 0; return -2; }

	if (rd) *data = ServoAuxData;
	return ServoAuxResult;
}

int ServoWrite(int id, int reg, int data) { return ServoReg(id, reg, &data, 0, 0); }

main()
{
	int t;

	// this example drives Feetech STS servos.  The STS_REG_* control table
	// numbers used below mean something completely different on a Dynamixel
	// (STS_REG_TORQUE = 0x28 is not even a valid XL430 address), so refuse
	// rather than write to the wrong registers.  ServoProtocol is a sticky
	// global - it stays 1 after any Dxl* program has run.
	if (ServoProtocol)
	{
		printf("ServoProtocol=1 (Dynamixel) - use DxlAxisInit.c instead\n");
		return;
	}

	ServoBusEXIO = 0;                 // bus on IO0 (set 1 for EX_IO_13)
	ServoCycleTicks = 0;              // bus cycles back to back (~1ms each)

	// how hard the servo is allowed to chase the commanded position.  KFLOP
	// does the trajectory planning, so leave these at maximum.  NEVER set a
	// small nonzero ServoAccel - it badly throttles the servo's own loop.
	ServoSpeed = 3400;
	ServoAccel = 0;

	ch0->InputMode  = SERIAL_SERVO_INPUT_MODE;
	ch0->InputChan0 = SERVO;
	ch0->InputChan1 = 0;
	ch0->InputGain0 = 1.0;            // axis units per servo count (4096/rev)
	ch0->InputOffset0 = 0.0;
	ch0->InputOffset1 = 0.0;

	ch0->OutputMode  = SERIAL_SERVO_MODE;
	ch0->OutputChan0 = SERVO;
	ch0->OutputChan1 = 0;
	ch0->OutputGain  = 1.0;           // servo counts per axis unit
	// To home a pose to read zero set InputOffset0 = -P0 and OutputOffset = +P0
	// (negatives of each other) where P0 is the raw servo position at the pose.
	// See "Homing with axis offsets" in SERIAL_SERVO_Axis_Mode.md
	ch0->OutputOffset = 0.0;

	// Output is the position correction added to the command, so MaxOutput is
	// the biggest correction allowed, in servo counts.  It is unused in
	// SERIAL_SERVO_MODE, but it MUST be nonzero in CL_SERIAL_SERVO_MODE or the
	// correction (including feed forward) is clamped away and closed loop does
	// nothing at all.
	ch0->MaxOutput   = 500.0;         // servo counts of correction authority

	// the servo's own loop lags the command roughly 80ms, which converts to
	// following error in proportion to speed.  Allow for it - this still
	// catches a jam or an unplugged servo.
	ch0->MaxFollowingError = 4000.0;   // servo counts

	// feed forward is the lag compensation - see SERIAL_SERVO_Axis_Mode.md.
	// measure with SerialServoTuneLag.c; ~0.08 sec is typical.
	ch0->FFVel = 0.0;
	ch0->FFAccel = 0.0;

	// EXACT lag compensation by command preview (see the CmdDelay section of
	// SERIAL_SERVO_Axis_Mode.md): delay the official reference globally and
	// command this axis ahead by its measured lag.  Uncomment to use:
	// CmdDelay = 889;            // ~80ms in 90us samples - whole machine
	// ch0->CmdLead = 889;        // this axis commanded 80ms ahead

	ch0->SoftLimitPos = 1.0e9;
	ch0->SoftLimitNeg = -1.0e9;
	ch0->LimitSwitchOptions = 0;
	ch0->BacklashMode = BACKLASH_OFF;
	ch0->MasterAxis = -1;
	ch0->invDistPerCycle = 1.0;
	ch0->StepperAmplitude = 0;

	// no filters needed for SERIAL_SERVO_MODE (no servo loop runs).  For
	// CL_SERIAL_SERVO_MODE set the usual gains here; the measured position is
	// a staircase (one telemetry update per bus cycle per claimed servo), so
	// filter it (e.g. 2nd order low pass ~40Hz) before using any D gain.
	ch0->iir[0].B0=1.0; ch0->iir[0].B1=0.0; ch0->iir[0].B2=0.0;
	ch0->iir[0].A1=0.0; ch0->iir[0].A2=0.0;
	ch0->iir[1].B0=1.0; ch0->iir[1].B1=0.0; ch0->iir[1].B2=0.0;
	ch0->iir[1].A1=0.0; ch0->iir[1].A2=0.0;
	ch0->iir[2].B0=1.0; ch0->iir[2].B1=0.0; ch0->iir[2].B2=0.0;
	ch0->iir[2].A1=0.0; ch0->iir[2].A2=0.0;

	// configuring the axis makes the DSP protocol engine claim the bus and
	// begin polling telemetry.  Wait for the first response so the measured
	// position is known before enabling.

	for (t = 0; t < 500; t++)
	{
		if (ServoPosValid[SERVO]) break;
		Delay_sec(0.002);
	}

	if (!ServoPosValid[SERVO])
	{
		printf("no response from serial servo ID %d - check power, wiring, ID\n", SERVO);
		return;
	}

	printf("servo %d at %d counts, %4.1fV %dC\n", SERVO, ServoPosition[SERVO],
		SERVO_CONVERT_TO_VOLTS(ServoVolts[SERVO]), ServoTemp[SERVO]);

	// the servo will not move until its torque is enabled
	if (ServoWrite(SERVO, STS_REG_TORQUE, 1))
	{
		printf("failed to enable servo torque\n");
		return;
	}

	// enable at the measured position so enabling never causes motion
	EnableAxis(AXIS);

	// axis 0 alone as a coordinate system
	DefineCoordSystem(AXIS,-1,-1,-1);

	printf("axis %d enabled on serial servo %d\n", AXIS, SERVO);
}
