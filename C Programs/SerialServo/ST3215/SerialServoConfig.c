// SerialServoConfig.c
//
// Read, display, and optionally set the STS3215's internal parameters.
//
// Run it as-is (all SET_ values -1) and it just DUMPS every relevant register
// of the servo with its meaning - a quick health/configuration report.  Set
// any SET_ define to a value >= 0 and that register is written (EEPROM
// registers are unlocked/relocked automatically, torque is turned off first
// since some firmwares reject EEPROM writes with torque enabled), then the
// dump is repeated to verify.
//
// Uses the DSP protocol engine's aux mailbox - no axis setup needed, works
// while axes are running (requests slot between cyclic transactions).
//
// CAUTIONS
//  - EEPROM registers (0x05-0x27) have limited write endurance.  Configure
//    once; never write them from a control loop.  SRAM registers (0x28+)
//    reset to their EEPROM-seeded defaults at servo power up.
//  - NEVER write the baud register (0x06): the FPGA engine is fixed at
//    1 Mbaud and the servo would go silent until reconfigured externally.
//  - A write ACK proves RECEIPT only (the protocol has no write-rejected
//    indication) - the readback verify below is the only real proof.  The
//    servo's internal flash commit can delay its ack tens of ms, so this
//    program pauses the engine's cyclic traffic (ServoCycleTicks) around
//    the writes, waits before verifying, and retries on mismatch.
//  - Values below are from the published register map plus what we measured
//    on a 12V ST3215-C018.  Voltage limit defaults differ between the 7.4V
//    C001 and 12V C018 - always read YOUR servo before changing protections.

#include "KMotionDef.h"

#define SERVO 0            // servo ID on the bus (0-7)

// ======================= values to write, -1 = leave =======================
//
// --------------------------- position loop (EEPROM) ------------------------
//
// The servo closes its own position loop from these.  Measured reality on
// the STS3215: the tracking lag is dominated by firmware dead time and is
// VELOCITY dependent (~41ms at 250 counts/s to ~189ms at 2500) - raising P
// barely reduces it (32->128 gained under 10%), so compensate lag with the
// Kogna CmdDelay/CmdLead preview instead and use these only for stiffness
// and damping AT the target.

#define SET_POS_P      128  // reg 0x15, default 32.  Proportional gain: how
                           // hard it pushes per count of internal error.
                           // Higher = stiffer holding, less load sag; too
                           // high = buzz/hunting at rest.  128 tested stable
                           // (with D=40) on the C018 with a small arm.

#define SET_POS_D      -1  // reg 0x16, default 32.  Derivative gain: damping.
                           // Raise together with P to suppress hunting.
                           // 40 tested with P=128.

#define SET_POS_I      -1  // reg 0x17, default 0.  Integral gain: removes
                           // steady-state error against constant load
                           // (gravity).  Leave 0 unless the axis must hold
                           // exact position under load with no KFLOP-side
                           // help - integral + torque limit = windup and
                           // slow oscillation if overdone.

// --------------------- motion feel / accuracy (EEPROM) ---------------------

#define SET_MIN_FORCE  -1  // reg 0x18 (2 bytes), default 16, 0-1000 = 0-100%.
                           // Minimum output used to BEGIN moving - a stiction
                           // breakaway kick.  Too low: sluggish start on
                           // small moves; too high: audible tick and
                           // overshoot on tiny corrections.

#define SET_CW_DEAD    -1  // reg 0x1A, default 1 (encoder counts)
#define SET_CCW_DEAD   -1  // reg 0x1B, default 1.  The servo stops correcting
                           // inside +/-deadband of its goal: our servo parks
                           // within ~2 counts of a commanded target.  0/0
                           // gives the tightest positioning but can buzz;
                           // widen if the servo sings at rest.

// ------------------------- torque / protections ----------------------------

#define SET_MAX_TORQUE -1  // reg 0x10 (2 bytes, EEPROM), default 1000
                           // (= 100%).  Hard ceiling on motor output; also
                           // seeds the SRAM torque limit (0x30) at power up.
                           // Lower it to make an axis compliant/safe around
                           // people or delicate mechanisms - at the cost of
                           // acceleration and load capacity.

#define SET_TORQUE_LIM -1  // reg 0x30 (2 bytes, SRAM).  The LIVE torque
                           // limit, 0-1000.  Resets to 0x10's value at power
                           // up - useful for temporary gentle modes without
                           // an EEPROM write.

#define SET_PROT_CURR  -1  // reg 0x1C (2 bytes, EEPROM), default 500,
                           // 6.5mA/count (500 = 3.25A).  Overcurrent
                           // protection threshold, with time window 0x26.

#define SET_OVL_THRESH -1  // reg 0x24, default 80 (% torque/load).  Load
                           // level that starts the overload timer.

#define SET_PROT_TIME  -1  // reg 0x23, default 200 (10ms/count = 2s).  How
                           // long the overload must persist before
                           // protection trips.

#define SET_PROT_TORQ  -1  // reg 0x22, default 20 (%).  Output allowed AFTER
                           // an overload trip - the servo goes limp-ish
                           // instead of cooking itself in a stall.  The
                           // KFLOP following-error trip normally fires first
                           // (position freezes while the command moves on).

// ----------------------- supply / thermal limits ---------------------------

#define SET_MAX_VOLT   -1  // reg 0x0E, 0.1V/count.  Published C001 default
                           // 80 (8.0V) - a 12V C018 ships with a different
                           // value; READ YOURS FIRST.  Exceeding it sets the
                           // voltage fault bit (telemetry status).

#define SET_MIN_VOLT   -1  // reg 0x0F, 0.1V/count, C001 default 40 (4.0V).
                           // Brownout detector - useful to catch supply sag
                           // on multi-servo chains under acceleration.

#define SET_MAX_TEMP   -1  // reg 0x0D, default 70 (degrees C).  Thermal
                           // shutdown threshold.  Telemetry reports the
                           // live temperature continuously.

// -------------------------- travel / geometry ------------------------------

#define SET_MIN_ANGLE  -1  // reg 0x09 (2 bytes), default 0
#define SET_MAX_ANGLE  -1  // reg 0x0B (2 bytes), default 4095.  The servo
                           // refuses goals outside [min,max].  Narrow them
                           // as a servo-side travel limit, or set BOTH to 0
                           // to disable limits for continuous/multi-turn
                           // use (the Kogna side unwraps multi-turn
                           // position regardless, but the SERVO clips
                           // goals to these limits in normal mode).

#define SET_POS_CORR   -1  // reg 0x1F (2 bytes), default 0, +/-2047 with
                           // sign in bit 11 (give a SIGNED value here, the
                           // program encodes it).  Shifts the servo's zero -
                           // mechanical zero trim without re-mounting the
                           // horn.  (Alternative: torque register 0x28=128
                           // writes current position as center 2048.)

// ------------------------------ bus behavior -------------------------------

#define SET_RESP_LEVEL -1  // reg 0x08, default 1.  1 = servo acknowledges
                           // WRITEs with a status packet; 0 = responds only
                           // to READ/PING.  The Kogna engine works with
                           // either (broadcasts are never acknowledged and
                           // a clean ack timeout is treated as success).

#define SET_RET_DELAY  -1  // reg 0x07, default 0 after SerialServoSetID.c
                           // (factory 250!), 2us/count.  Wait inserted
                           // before every response - keep 0.

// ============================================================================

// registers not already defined in KMotionDef.h
#define R_RESP_LEVEL 0x08
#define R_MIN_ANGLE  0x09
#define R_MAX_ANGLE  0x0B
#define R_MAX_TEMP   0x0D
#define R_MAX_VOLT   0x0E
#define R_MIN_VOLT   0x0F
#define R_MAX_TORQUE 0x10
#define R_PROT_CURR  0x1C
#define R_POS_CORR   0x1F
#define R_PROT_TORQ  0x22
#define R_PROT_TIME  0x23
#define R_OVL_THRESH 0x24
#define R_TORQUE_LIM 0x30
#define R_PRES_VOLT  0x3E

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

	// the engine may hold an EEPROM write transaction open ~200ms waiting
	// out the servo's flash commit - poll well past that
	for (t = 0; t < 600 && ServoAuxGo; t++) Delay_sec(0.001);
	if (ServoAuxGo) { ServoAuxGo = 0; return -2; }

	if (rd) *data = ServoAuxData;
	return ServoAuxResult;
}

// read a register, -1 on failure
int Rd(int reg, int two)
{
	int v = 0;
	if (ServoReg(SERVO, reg, &v, two, 1)) return -1;
	return v;
}

// write with readback verify, retrying.  NOTE: the servo's ack proves only
// RECEIPT (the Feetech protocol has no write-rejected indication) - this
// readback is the only real verification.
//
// local_val: pass the address of a LOCAL, never of the parameter itself -
// TCC67 versions before 2026-08-03 miscompiled the address of a function
// parameter (C67 register args), which transmitted garbage values here.
int Wr(int reg, int val, int two)
{
	int r, v, tries;
//	int local_val = val;

	for (tries = 0; tries < 3; tries++)
	{
		r = ServoReg(SERVO, reg, &val, two, 0);
		if (r) { printf("  WRITE reg 0x%02X FAILED (result %d)\n", reg, r); return 1; }
		Delay_sec(0.25);   // allow for the servo's internal flash commit
		                   // (its ack can lag tens of ms) before verifying

		v = Rd(reg, two);
		if (v == val)
		{
			printf("  reg 0x%02X = %d OK%s\n", reg, val,
				tries ? " (after retry)" : "");
			return 0;
		}
		printf("  VERIFY reg 0x%02X failed (wrote %d read %d)%s\n",
			reg, val, v, tries < 2 ? " - retrying" : "");
		Delay_sec(1.0);
	}
	return 1;
}

void Dump(void)
{
	int v;

	printf("\n---- servo %d configuration ----\n", SERVO);
	v=Rd(0x00,0);          printf("firmware          %d.", v);
	v=Rd(0x01,0);          printf("%d\n", v);
	v=Rd(STS_REG_ID,0);    printf("ID                %d\n", v);
	v=Rd(STS_REG_RETDELAY,0); printf("return delay      %d (x2us)\n", v);
	v=Rd(R_RESP_LEVEL,0);  printf("response level    %d (1=ack writes)\n", v);
	v=Rd(STS_REG_MODE,0);  printf("operating mode    %d (0=position)\n", v);

	v=Rd(STS_REG_POS_P,0); printf("position P        %d (default 32)\n", v);
	v=Rd(STS_REG_POS_D,0); printf("position D        %d (default 32)\n", v);
	v=Rd(STS_REG_POS_I,0); printf("position I        %d (default 0)\n", v);
	v=Rd(STS_REG_MIN_FORCE,1); printf("min start force   %d /1000\n", v);
	v=Rd(STS_REG_CW_DEAD,0);   printf("deadband CW/CCW   %d/", v);
	v=Rd(STS_REG_CCW_DEAD,0);  printf("%d counts\n", v);

	v=Rd(R_MAX_TORQUE,1);  printf("max torque        %d /1000 (EEPROM)\n", v);
	v=Rd(R_TORQUE_LIM,1);  printf("torque limit      %d /1000 (SRAM, live)\n", v);
	v=Rd(R_PROT_CURR,1);   printf("protect current   %d (x6.5mA = %dmA)\n", v, v*13/2);
	v=Rd(R_OVL_THRESH,0);  printf("overload thresh   %d %%\n", v);
	v=Rd(R_PROT_TIME,0);   printf("protect time      %d (x10ms)\n", v);
	v=Rd(R_PROT_TORQ,0);   printf("protect torque    %d %% after trip\n", v);

	v=Rd(R_MAX_TEMP,0);    printf("max temperature   %d C\n", v);
	v=Rd(R_MAX_VOLT,0);    printf("max voltage       %d (x0.1V)\n", v);
	v=Rd(R_MIN_VOLT,0);    printf("min voltage       %d (x0.1V)\n", v);
	v=Rd(R_PRES_VOLT,0);   printf("PRESENT voltage   %d (x0.1V)\n", v);

	v=Rd(R_MIN_ANGLE,1);   printf("min angle         %d\n", v);
	v=Rd(R_MAX_ANGLE,1);   printf("max angle         %d (0/0 = multi-turn)\n", v);
	v=Rd(R_POS_CORR,1);
	if (v >= 0) printf("position corr     %d\n", (v & 0x800) ? -(v & 0x7FF) : (v & 0x7FF));
	printf("--------------------------------\n\n");
}

main()
{
	int v, wrote = 0, err = 0, savedticks;

	// Feetech STS control table only - see the note in SerialServoAxisInit.c
	if (ServoProtocol)
	{
		printf("ServoProtocol=1 (Dynamixel) - use DxlConfig.c instead\n");
		return;
	}

	// prove the servo answers before doing anything
	v = 0;
	if (ServoReg(SERVO, STS_REG_ID, &v, 0, 1))
	{
		printf("no servo answers at ID %d\n", SERVO);
		return;
	}

	Dump();

	// collect whether anything is to be written
	if (SET_POS_P<0 && SET_POS_D<0 && SET_POS_I<0 && SET_MIN_FORCE<0 &&
		SET_CW_DEAD<0 && SET_CCW_DEAD<0 && SET_MAX_TORQUE<0 && SET_TORQUE_LIM<0 &&
		SET_PROT_CURR<0 && SET_OVL_THRESH<0 && SET_PROT_TIME<0 && SET_PROT_TORQ<0 &&
		SET_MAX_VOLT<0 && SET_MIN_VOLT<0 && SET_MAX_TEMP<0 && SET_MIN_ANGLE<0 &&
		SET_MAX_ANGLE<0 && SET_POS_CORR==-1 && SET_RESP_LEVEL<0 && SET_RET_DELAY<0)
	{
		printf("(all SET_ values are -1 - dump only, nothing written)\n");
		return;
	}

	// CRITICAL: quiet the bus for the EEPROM writes.  A large ServoCycleTicks
	// spaces the engine's cyclic SYNC_WRITE/telemetry cycles ~2s apart while
	// still letting these aux requests through immediately, so each write
	// gets a silent window for its internal EEPROM programming.
	savedticks = ServoCycleTicks;
	ServoCycleTicks = 20000;
	Delay_sec(0.1);            // let the in-progress bus cycle finish

	// EEPROM writes can be rejected while torque is enabled
	printf("writing (torque off, EEPROM unlocked, bus cycles paused)...\n");
	v = 0; err |= ServoReg(SERVO, STS_REG_TORQUE, &v, 0, 0);
	Delay_sec(0.02);
	v = 0; err |= ServoReg(SERVO, STS_REG_LOCK, &v, 0, 0);
	Delay_sec(0.02);
	if (err)
	{
		ServoCycleTicks = savedticks;
		printf("torque off / unlock failed - aborting\n");
		return;
	}

	if (SET_POS_P>=0)      { err |= Wr(STS_REG_POS_P, SET_POS_P, 0); wrote++; }
	if (SET_POS_D>=0)      { err |= Wr(STS_REG_POS_D, SET_POS_D, 0); wrote++; }
	if (SET_POS_I>=0)      { err |= Wr(STS_REG_POS_I, SET_POS_I, 0); wrote++; }
	if (SET_MIN_FORCE>=0)  { err |= Wr(STS_REG_MIN_FORCE, SET_MIN_FORCE, 1); wrote++; }
	if (SET_CW_DEAD>=0)    { err |= Wr(STS_REG_CW_DEAD, SET_CW_DEAD, 0); wrote++; }
	if (SET_CCW_DEAD>=0)   { err |= Wr(STS_REG_CCW_DEAD, SET_CCW_DEAD, 0); wrote++; }
	if (SET_MAX_TORQUE>=0) { err |= Wr(R_MAX_TORQUE, SET_MAX_TORQUE, 1); wrote++; }
	if (SET_PROT_CURR>=0)  { err |= Wr(R_PROT_CURR, SET_PROT_CURR, 1); wrote++; }
	if (SET_OVL_THRESH>=0) { err |= Wr(R_OVL_THRESH, SET_OVL_THRESH, 0); wrote++; }
	if (SET_PROT_TIME>=0)  { err |= Wr(R_PROT_TIME, SET_PROT_TIME, 0); wrote++; }
	if (SET_PROT_TORQ>=0)  { err |= Wr(R_PROT_TORQ, SET_PROT_TORQ, 0); wrote++; }
	if (SET_MAX_VOLT>=0)   { err |= Wr(R_MAX_VOLT, SET_MAX_VOLT, 0); wrote++; }
	if (SET_MIN_VOLT>=0)   { err |= Wr(R_MIN_VOLT, SET_MIN_VOLT, 0); wrote++; }
	if (SET_MAX_TEMP>=0)   { err |= Wr(R_MAX_TEMP, SET_MAX_TEMP, 0); wrote++; }
	if (SET_MIN_ANGLE>=0)  { err |= Wr(R_MIN_ANGLE, SET_MIN_ANGLE, 1); wrote++; }
	if (SET_MAX_ANGLE>=0)  { err |= Wr(R_MAX_ANGLE, SET_MAX_ANGLE, 1); wrote++; }
	if (SET_RESP_LEVEL>=0) { err |= Wr(R_RESP_LEVEL, SET_RESP_LEVEL, 0); wrote++; }
	if (SET_RET_DELAY>=0)  { err |= Wr(STS_REG_RETDELAY, SET_RET_DELAY, 0); wrote++; }

	// position correction is signed, sign in bit 11
	if (SET_POS_CORR != -1)
	{
		v = SET_POS_CORR;
		if (v < 0) v = 0x800 | (-v & 0x7FF);
		else v = v & 0x7FF;
		err |= Wr(R_POS_CORR, v, 1); wrote++;
	}

	// SRAM live torque limit (not EEPROM, but harmless inside the window)
	if (SET_TORQUE_LIM>=0) { err |= Wr(R_TORQUE_LIM, SET_TORQUE_LIM, 1); wrote++; }

	v = 1; ServoReg(SERVO, STS_REG_LOCK, &v, 0, 0);   // re-lock EEPROM
	Delay_sec(0.02);

	ServoCycleTicks = savedticks;   // resume normal bus cycling

	printf("%d register(s) written%s\n", wrote, err ? " WITH ERRORS" : "");

	Dump();

	printf("NOTE: torque was turned OFF for the EEPROM writes - re-run the\n");
	printf("axis init (or write reg 0x28 = 1) before commanding motion.\n");
	printf("EEPROM values apply immediately but verify after a power cycle.\n");
}
