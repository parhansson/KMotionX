// SerialServoLeadSchedule.c
//
// Velocity-scheduled command lead, evaluated at the COMMAND TAP.
//
// The servo's tracking lag grows with speed (measure the curve with
// SerialServoLagVsVel.c), so the lead should follow it.  The subtlety: the
// lead must be scheduled on the velocity of the trajectory point being
// COMMANDED (the tap), not the planner's current velocity - with a large
// CmdDelay the planner runs ~(CmdDelay-CmdLead) samples in the tap's future,
// so scheduling on chan[].last_vel over-leads during acceleration and
// under-leads during deceleration (position leads in accel / lags in decel -
// exactly the residual seen when scheduling on last_vel directly).
//
// This thread keeps a history of the planner velocity, indexed back by
// (CmdDelay - CmdLead) samples so the schedule sees the tap's velocity.
// It also slews CmdLead by at most 1 sample per tick, bounding the
// tap-slew distortion (a step change in lead momentarily scales the
// command velocity by the lead change rate).
//
// Set L0/SLOPE from the SerialServoLagVsVel.c table:
//   lead(v) = L0 + SLOPE * |v|      (samples; 1 sample = 90us)
//
// Run as its own thread alongside the axis (loops forever).

#include "KMotionDef.h"

#define AXIS   0

#define L0     250         // samples of lead at zero speed (~27ms)
#define SLOPE  0.751f      // samples per (count/sec) - hand fit, refine
                            // from the LagVsVel table
#define DSLOPE 0.25f       // EXTRA lead per count/s^2 while |v| is falling -
                            // tunes out the decel-only lag; 0 disables.
                            // (~500 samples at 2500 counts/s^2 measured need)

// User C threads run one time slice per 180us on Kogna - every SECOND 90us
// servo sample.  The history is therefore recorded at half the servo rate
// and all servo-sample distances must be divided by TICKS_PER_SLICE when
// indexing it.

#define TICKS_PER_SLICE 2   // 180us thread slice / 90us servo sample

#define VHIST  2048         // power of two; covers VHIST*TICKS_PER_SLICE samples

static float vhist[VHIST];

main()
{
	int idx = 0, back, want, lead;
	float v, vprev, decel;
	
	CmdDelay = 2500;

	for (;;)
	{
		WaitNextTimeSlice();

		// record planner velocity once per thread slice (every 2nd sample)
		vhist[idx & (VHIST-1)] = fast_fabs(chan[AXIS].last_vel);

		// velocity of the trajectory point currently being commanded:
		// the tap sits (CmdDelay - CmdLead) SERVO samples behind the
		// planner = that distance / TICKS_PER_SLICE history entries
		lead = chan[AXIS].CmdLead;
		back = (CmdDelay - lead) / TICKS_PER_SLICE;
		if (back < 0) back = 0;
		if (back > VHIST-1) back = VHIST-1;

		v = vhist[(idx - back) & (VHIST-1)];

		// the servo needs EXTRA lead while decelerating (measured ~40 counts
		// of decel-only lag that the constant velocity table cannot see).
		// decel = rate |v| is falling at the same schedule point, counts/s^2
		vprev = vhist[(idx - back - 1) & (VHIST-1)];
		decel = (vprev - v) * (1.0f/(TICKS_PER_SLICE*TIMEBASE));
		if (decel < 0.0f) decel = 0.0f;      // accelerating - no extra lead

		want = L0 + (int)(SLOPE * v + DSLOPE * decel);
		ch13-> Dest = want;
		if (want > CmdDelay) want = CmdDelay;
		if (want < 0) want = 0;

		// slew limit per SLICE = TICKS_PER_SLICE lead samples, keeping the
		// tap-slew distortion of command velocity within ~1 sample/sample.
		// (needed rate is SLOPE*accel*90us per sample - ~0.17 at 2500
		//  counts/s^2 - so this only guards startup jumps)
		if (want > lead + TICKS_PER_SLICE) lead += TICKS_PER_SLICE;
		else if (want < lead - TICKS_PER_SLICE) lead -= TICKS_PER_SLICE;
		else lead = want;

		chan[AXIS].CmdLead = lead;
		ch14-> Dest = want;
		ch15-> Dest = idx;
		idx++;
	}
}
