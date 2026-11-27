// DxlAxisInit.c
//
// Configure axis 0 to run a Dynamixel XL430 (Protocol 2.0) on the serial
// servo bus in the LOW LAG mode - the servo must already be set up with
// DxlSetup.c (1 Mbaud, ID = SERVO, Extended Position mode, profiles 0).
//
// Differences from the Feetech axis init:
//  - ServoProtocol = 1 selects the Dynamixel engine
//  - torque must be EXPLICITLY enabled (register 64) - the servo silently
//    rejects position goals with torque off
//  - no CmdDelay/CmdLead preview: with the profile generator off the servo
//    acts on each goal immediately.  Measure the residual lag with
//    SerialServoLagVsVel.c before deciding any lead is needed at all.
//
// Position units are servo counts (4096/rev), multi turn +/-256 revs.

#include "KMotionDef.h"

#define AXIS   0
#define SERVO  0           // bus ID = axis channel number
#define USE_EXIO  0

int DxlReg(int id, int reg, int *data, int size, int rd)
{
	int t;

	ServoAuxID = id;
	ServoAuxReg = reg;
	ServoAuxData = *data;
	ServoAuxTwo = size;
	ServoAuxRead = rd;
	ServoAuxGo = 1;

	for (t = 0; t < 600 && ServoAuxGo; t++) Delay_sec(0.001);
	if (ServoAuxGo) { ServoAuxGo = 0; return -2; }

	if (rd) *data = ServoAuxData;
	return ServoAuxResult;
}

main()
{
	CHAN *ch = &chan[AXIS];
	int v, t;

	ServoProtocol = 1;  // Dynamixel
	ServoBusEXIO = USE_EXIO;
	ServoBaud576 = 0;

	// prove the servo answers before configuring the axis

	v = 0;
	if (DxlReg(SERVO, DXL_REG_MODEL, &v, 1, 1))
	{
		printf("no Dynamixel answers at ID %d\n", SERVO);
		return;
	}
	printf("servo %d: model %d\n", SERVO, v);

	// tuned recipe (2026-08-05, XL430, no load): servo P = 10000 leaves a
	// constant ~5.5ms tracking lag; FFVel (seconds in this mode) cancels a
	// constant TIME lag exactly - measured +/-4 counts at 1500 counts/s
	// with CmdDelay = 0 and no CmdLead (no preview needed at all)

	ch->InputMode  = SERIAL_SERVO_INPUT_MODE;
	ch->OutputMode = CL_SERIAL_SERVO_MODE;
	ch->InputChan0 = SERVO;
	ch->OutputChan0 = SERVO;
	// To home a pose to read zero set InputOffset0 = -P0 and OutputOffset = +P0
	// (negatives of each other) where P0 is the raw servo position at the pose.
	// See "Homing with axis offsets" in SERIAL_SERVO_Axis_Mode.md
	ch->InputGain0 = 1;
	ch->InputOffset0 = 0;
	ch->OutputGain = 1;
	ch->OutputOffset = 0;
	ch->MasterAxis = -1;
	ch->Vel = 2500;
	ch->Accel = 2500;
	ch->Jerk = 25000;
	ch->MaxFollowingError = 4000;
	ch->CmdLead = 0;              // low lag mode - no preview
	ch->P = 0;                    // no KFLOP PID assist - FFVel only
	ch->I = 0;
	ch->D = 0;
	ch->FFVel = 0.0055;           // = the servo's residual constant lag, sec
	ch->FFAccel = 0;
	ch->MaxI = 200;
	ch->MaxErr = 1e6;
	ch->MaxOutput = 200;          // MUST be nonzero or FFVel is clamped away
	ch->DeadBandGain = 1;
	ch->DeadBandRange = 0;
	ch->SlaveGain = 1;
	ch->BacklashMode = BACKLASH_OFF;
	ch->invDistPerCycle = 1;
	ch->Lead = 0;
	ch->LimitSwitchOptions = 0x100;
	ch->SoftLimitPos =  1e9;
	ch->SoftLimitNeg = -1e9;

	// the servo's P gain is RAM and RESETS TO 640 AT SERVO POWER UP - it
	// must be restored every session or the axis runs with ~47ms lag

	v = 10000;
	if (DxlReg(SERVO, DXL_REG_POS_P, &v, 1, 0))
		printf("WARNING: servo P gain write failed\n");

	// enable torque - REQUIRED: goals are silently rejected without it

	v = 1;
	if (DxlReg(SERVO, DXL_REG_TORQUE, &v, 0, 0))
	{
		printf("torque enable failed\n");
		return;
	}

	// wait for the first telemetry so the enable interlock passes

	{
		double t0 = Time_sec();
		while (!ServoPosValid[SERVO] && Time_sec() - t0 < 1.0)
			Delay_sec(0.001);
		if (!ServoPosValid[SERVO])
		{
			printf("no telemetry from servo %d - axis not enabled\n", SERVO);
			return;
		}
		Delay_sec(0.01);
	}

	EnableAxisDest(AXIS, ch->Position);

	printf("axis %d enabled on Dynamixel %d  pos %d  %.1fV  %dC\n",
		AXIS, SERVO, ServoPosition[SERVO],
		SERVO_CONVERT_TO_VOLTS(ServoVolts[SERVO]), ServoTemp[SERVO]);
	printf("note: 'Current' telemetry reports Present Load (0.1%% units) -\n");
	printf("the XL430 has no current sensor\n");
}
