// SerialServoSweepLag.c
//
// Find where the serial servo's dead time actually comes from.
//
// Sweeping the servo's own position loop P gain (SerialServoTuneLag.c) showed
// lag falling only 115ms -> 105ms for P = 32 -> 128.  Fitting that to
//
//      lag = deadtime + K/P
//
// gives K ~ 427 and deadtime ~ 102ms, i.e. only ~13ms of the lag was ever
// gain dependent.  Dead time cannot be removed by ANY feedback gain, so the
// remaining ~102ms has to be a rate limit or transport delay somewhere else.
//
// The two values this driver sends in every position command packet are prime
// suspects, because both were chosen from the manual rather than measured:
//
//   ServoAccel (servo reg 0x29) - documented only as "100 steps/s^2 per
//        count".  Default 0 was ASSUMED to mean "no limit".  If 0 instead
//        means slowest, the servo re-profiles every 1ms command from a crawl
//        and never reaches commanded speed - a velocity proportional lag that
//        is nearly independent of its P gain, which is what was measured.
//
//   ServoSpeed (servo reg 0x2E) - the servo's internal speed cap.  If catch-up
//        rate matters, lag should worsen as this approaches the test velocity.
//
// This sweeps both against measured lag.  Whichever moves the number is the
// real limit.  Run with the axis configured and enabled, and preferably in
// SERIAL_SERVO_MODE with FFVel = 0 so nothing masks the raw lag.

#include "KMotionDef.h"

#define AXIS    0
#define SERVO   0

#define TEST_VEL   1000.0    // counts/sec used for every measurement

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

// steady state following error / velocity = effective lag in seconds
double MeasureLag(void)
{
	double t, sum=0.0, start;
	int n=0;

	Move(AXIS, 500);
	while (!CheckDone(AXIS)) ;
	Delay_sec(0.5);

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

	t = (sum / n) / TEST_VEL;
	return t;
}

main()
{
	double lag, best;
	int i, bestaccel, bestspeed;
	int accels[6];
	int speeds[5];

	accels[0]=0; accels[1]=10; accels[2]=32; accels[3]=100; accels[4]=200; accels[5]=254;
	speeds[0]=1200; speeds[1]=1800; speeds[2]=2400; speeds[3]=3000; speeds[4]=3400;

	// ServoSpeed/ServoAccel are Feetech sync write fields - the Dynamixel
	// engine ignores them (its low lag mode uses profile vel/acc = 0)
	if (ServoProtocol)
	{
		printf("ServoProtocol=1 (Dynamixel) - this sweep is Feetech only\n");
		return;
	}

	if (!ServoPosValid[SERVO])
	{
		printf("serial servo %d has no position feedback\n", SERVO);
		return;
	}
	if (!chan[AXIS].Enable)
	{
		printf("axis %d is not enabled\n", AXIS);
		return;
	}

	if (chan[AXIS].FFVel != 0.0f || chan[AXIS].FFAccel != 0.0f)
		printf("NOTE: FFVel/FFAccel are nonzero - they mask the raw lag.  Zero them first.\n");

	// ---- sweep the acceleration sent in every position command ----
	//
	// If ServoAccel=0 really means "no limit", every row here is the same and
	// accel is not the culprit.  If lag drops sharply as accel rises, the
	// default of 0 was the bug.

	printf("commanded accel (servo reg 0x29, x100 steps/s^2):\n");

	ServoSpeed = 3400;
	best = 1e9; bestaccel = 0;

	for (i = 0; i < 6; i++)
	{
		ServoAccel = accels[i];
		Delay_sec(0.05);                 // let the round robin push it out
		lag = MeasureLag();
		printf("  accel=%3d  ->  lag %.1f ms  (%.0f counts at %.0f counts/sec)\n",
			accels[i], lag*1000.0, lag*TEST_VEL, TEST_VEL);

		if (lag < best) { best = lag; bestaccel = accels[i]; }
	}

	printf("  best accel=%d at %.1f ms\n", bestaccel, best*1000.0);

	// ---- sweep the servo's internal speed cap ----
	//
	// Lag should be flat while this stays well above TEST_VEL.  If it climbs as
	// the cap falls toward TEST_VEL, the servo has no catch-up headroom.

	printf("commanded speed cap (servo reg 0x2E, steps/sec):\n");

	ServoAccel = bestaccel;
	best = 1e9; bestspeed = 3400;

	for (i = 0; i < 5; i++)
	{
		ServoSpeed = speeds[i];
		Delay_sec(0.05);
		lag = MeasureLag();
		printf("  speed=%4d ->  lag %.1f ms\n", speeds[i], lag*1000.0);

		if (lag < best) { best = lag; bestspeed = speeds[i]; }
	}

	// ---- does the bus cycle pacing matter? ----
	//
	// Extra pacing spaces the command stream out.  Expect roughly 1ms of
	// added lag per 1ms of pacing; it bounds how much delay is ours.

	ServoSpeed = bestspeed;
	ServoAccel = bestaccel;

	ServoCycleTicks = 11;                // ~1ms extra between bus cycles
	Delay_sec(0.05);
	lag = MeasureLag();
	printf("cycle pacing 11 ticks -> lag %.1f ms\n", lag*1000.0);

	ServoCycleTicks = 0;                 // back to back cycles
	Delay_sec(0.05);
	lag = MeasureLag();
	printf("cycle pacing 0 ticks  -> lag %.1f ms\n", lag*1000.0);

	// restore the recommended operating values - the "best" accel above is
	// usually a noise-level tie and 0 (no limit) is the correct setting
	ServoAccel = 0;
	ServoSpeed = 3400;

	printf("\nbest: ServoAccel=%d ServoSpeed=%d -> %.1f ms\n",
		bestaccel, bestspeed, best*1000.0);
	printf("(ServoAccel restored to 0, ServoSpeed to 3400)\n");
	printf("set CmdLead to the lag (samples = ms/0.09), FFAccel for the accel term\n");
}
