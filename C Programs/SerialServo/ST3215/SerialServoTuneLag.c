// SerialServoTuneLag.c
//
// Reduce serial servo tracking lag, and compensate what is left.
//
// The STS3215 closes its own position loop, and with factory gains that loop
// responds with roughly 115ms of lag.  In CL_SERIAL_SERVO_MODE the KFLOP loop
// cannot simply "gain that away" - lag of that size inside a feedback loop
// limits useful bandwidth to about 1.6Hz, so raising P/D just oscillates.
//
// MEASURED RESULT (12V C018): the P sweep below moved lag only 115ms -> 105ms
// for P = 32 -> 128.  Fitting lag = deadtime + K/P gives K ~ 427 and
// deadtime ~ 102ms, so only ~13ms was ever gain dependent.  Step 1 is
// therefore nearly useless on this servo - the lag is dominated by dead time,
// which no feedback gain of any kind can remove.  Run SerialServoSweepLag.c to
// hunt the dead time (ServoAccel=0 is the prime suspect), and rely on step 2.
//
//   1) REDUCE the lag at its source by raising the servo's own position loop
//      P gain (register 0x15, factory 32).  Costs nothing in KFLOP loop
//      stability because it is inside the servo - but see above, it barely
//      helps here.
//
//   2) COMPENSATE the remaining lag with feed forward, not with PID gain.
//      In CL_SERIAL_SERVO_MODE the servo loop Output is added to the commanded
//      position, and Output includes Vel*FFVel + Accel*FFAccel.  So FFVel has
//      units of SECONDS and acts as a pure command lead:
//
//           FFVel   ~= lag (sec)          cancels velocity proportional error
//           FFAccel ~= lag*lag (sec^2)    cancels acceleration induced error
//
//      Feed forward sits outside the feedback path, so unlike P/D it costs no
//      stability margin and may be applied generously.
//
// This program measures the lag, then applies both.  Run it with the axis
// already configured and enabled (see SerialServoAxisInit.c).

#include "KMotionDef.h"

#define AXIS    0
#define SERVO   0

#define TEST_VEL   1000.0    // counts/sec used for the lag measurement
#define TEST_DIST  800.0     // counts

// one servo register write through the DSP protocol engine's aux mailbox
int ServoAuxWrite(int id, int reg, int data, int two)
{
	int t;

	ServoAuxID = id;
	ServoAuxReg = reg;
	ServoAuxData = data;
	ServoAuxTwo = two;
	ServoAuxRead = 0;
	ServoAuxGo = 1;

	for (t = 0; t < 200 && ServoAuxGo; t++) Delay_sec(0.001);
	if (ServoAuxGo) { ServoAuxGo = 0; return -2; }
	return ServoAuxResult;
}

// set the servo's internal position loop gains (EPROM - unlock, write, lock)
int ServoSetPID(int ch, int P, int D, int I)
{
	if (ServoAuxWrite(ch, STS_REG_LOCK, 0, 0)) return -1;
	Delay_sec(0.02);
	if (ServoAuxWrite(ch, STS_REG_POS_P, P, 0)) return -1;
	Delay_sec(0.02);
	if (ServoAuxWrite(ch, STS_REG_POS_D, D, 0)) return -1;
	Delay_sec(0.02);
	if (ServoAuxWrite(ch, STS_REG_POS_I, I, 0)) return -1;
	Delay_sec(0.02);
	if (ServoAuxWrite(ch, STS_REG_LOCK, 1, 0)) return -1;
	Delay_sec(0.02);
	return 0;
}

// Move at a constant velocity and report the steady state following error.
// error / velocity IS the effective lag in seconds.
double MeasureLag(void)
{
	double t, sum=0.0, start;
	int n=0;
	
	// move to have room to Jog
	Move(AXIS, 500);
	while (!CheckDone(AXIS)) ;
	Delay_sec(0.5);

	// accelerate up to speed over a short distance, then sample mid-move
	Jog(AXIS, TEST_VEL);
	Delay_sec(0.35);              // let the ramp and the servo settle

	start = Time_sec();
	while (Time_sec() - start < 0.3)
	{
		// measure against the OFFICIAL commanded reference (DelayedDest
		// equals Dest when CmdDelay is off).  With CmdDelay/CmdLead active
		// this reports the residual tracking error the preview leaves.
		sum += chan[AXIS].DelayedDest - chan[AXIS].Position;
		n++;
		Delay_sec(0.002);
	}

	Jog(AXIS, 0.0);
	Delay_sec(0.5);

	if (n == 0) return 0.0;

	t = (sum / n) / TEST_VEL;      // steady state error / velocity = lag, sec
	return t;
}

main()
{
	double lag0, lag1;
	int P;

	// this tunes the Feetech STS_REG_POS_P/POS_D registers - on a Dynamixel
	// those addresses are Homing Offset EEPROM
	if (ServoProtocol)
	{
		printf("ServoProtocol=1 (Dynamixel) - use DxlConfig.c SET_POS_P instead\n");
		return;
	}

	if (!ServoPosValid[SERVO])
	{
		printf("serial servo %d has no position feedback - run the axis init first\n", SERVO);
		return;
	}

	if (!chan[AXIS].Enable)
	{
		printf("axis %d is not enabled\n", AXIS);
		return;
	}

	// measure with whatever gains are in the servo now
	lag0 = MeasureLag();
	printf("starting lag %.1f ms (%.0f counts of error at %.0f counts/sec)\n",
		lag0*1000.0, lag0*TEST_VEL, TEST_VEL);

	// ---- step 1: raise the servo's own position loop P gain ----
	//
	// Factory P is 32.  Work up while watching for buzzing, whining, or
	// hunting at rest - back off one step if any appears.  D helps damp the
	// higher P; leave I at 0 for now (feed forward handles steady state).

	for (P = 48; P <= 128; P += 16)
	{
		if (ServoSetPID(SERVO, P, 40, 0))
		{
			printf("failed writing servo gains\n");
			return;
		}

		lag1 = MeasureLag();
		printf("  servo P=%3d  ->  lag %.1f ms\n", P, lag1*1000.0);

		// stop if it stops improving (diminishing returns or instability)
//		if (lag1 > lag0 * 0.95 && P > 48) break;

		lag0 = lag1;
	}

	// ---- step 2: compensate the remaining lag with feed forward ----
	//
	// FFVel in seconds, FFAccel in seconds squared.  Output must have room to
	// apply the correction, so MaxOutput has to exceed FFVel*MaxVelocity.

	chan[AXIS].FFVel   = (float)lag0;
	chan[AXIS].FFAccel = (float)(lag0 * lag0);

	if (chan[AXIS].MaxOutput < 500.0f)
		chan[AXIS].MaxOutput = 500.0f;   // servo counts of correction authority

	printf("final lag %.1f ms -> FFVel %.4f sec, FFAccel %.6f sec^2\n",
		lag0*1000.0, chan[AXIS].FFVel, chan[AXIS].FFAccel);
	printf("set OutputMode = CL_SERIAL_SERVO_MODE (%d) to apply the correction\n",
		CL_SERIAL_SERVO_MODE);
	printf("then start P=0 I=0 D=0 and add only a little P - the lag limits\n");
	printf("useful loop bandwidth to roughly %.1f Hz\n", 1.0/(6.0*lag0));
}
