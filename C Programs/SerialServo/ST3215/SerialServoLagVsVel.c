// SerialServoLagVsVel.c
//
// Measure the servo's tracking lag at SEVERAL constant velocities.
//
// Every previous lag measurement ran at exactly 1000 counts/sec, so the
// "80ms" figure was a single point.  Velocity-scheduled lead experiments
// suggest the lag actually GROWS with speed (roughly tau ~ 27ms + 84us per
// count/s from a hand fit).  This measures tau(v) properly - constant
// velocity segments only, no acceleration confounds - giving the ground
// truth for a lead-vs-velocity schedule.
//
// Run with the axis configured and enabled, CmdDelay active or not (the
// measurement is against DelayedDest, the official reference).  CmdLead is
// forced to 0 during the test so the raw plant lag is measured, and restored
// afterward.  FFVel/FFAccel should be zero.

#include "KMotionDef.h"

#define AXIS    0
#define SERVO   0

double MeasureLagAtV(double vel)
{
	double t, sum=0.0, start;
	int n=0;

	Move(AXIS, 500);
	while (!CheckDone(AXIS)) ;
	Delay_sec(0.5);

	Jog(AXIS, vel);
	Delay_sec(1.0);              // ramp + settle

	start = Time_sec();
	while (Time_sec() - start < 0.3)
	{
		sum += chan[AXIS].DelayedDest - chan[AXIS].Position;
		n++;
		Delay_sec(0.002);
	}

	Jog(AXIS, 0.0);
	Delay_sec(0.5);

	if (n == 0) return 0.0;
	return (sum / n) / vel;       // steady error / velocity = lag, seconds
}

main()
{
	int i, savedlead;
	double v, lag;
	double vels[7];

	vels[0]=250; vels[1]=500; vels[2]=750; vels[3]=1000;
	vels[4]=1500; vels[5]=2000; vels[6]=2500;

	if (!ServoPosValid[SERVO] || !chan[AXIS].Enable)
	{
		printf("axis %d / servo %d not ready\n", AXIS, SERVO);
		return;
	}

	if (chan[AXIS].FFVel != 0.0f || chan[AXIS].FFAccel != 0.0f)
		printf("NOTE: FFVel/FFAccel nonzero - they distort the measurement\n");

	savedlead = chan[AXIS].CmdLead;
	chan[AXIS].CmdLead = 0;       // measure the raw plant
	Delay_sec(0.05);

	printf("lag vs velocity (CmdLead forced 0, error vs official reference):\n");
	printf("  counts/s     lag ms    lead samples (ms/0.09)\n");


	chan[AXIS].Accel = 5000;

	for (i = 0; i < 7; i++)
	{
		v = vels[i];
		lag = MeasureLagAtV(v);
		printf("  %6.0f     %7.1f     %5.0f\n", v, lag*1000.0, lag/90e-6);
	}

	chan[AXIS].CmdLead = savedlead;

	printf("fit lead(v) = L0 + slope*|v| from the table above and schedule it\n");
	printf("on the velocity AT THE TAP - see SerialServoLeadSchedule.c\n");
}
