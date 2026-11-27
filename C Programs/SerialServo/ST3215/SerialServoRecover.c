// SerialServoRecover.c
//
// Raw-bus diagnostic and recovery for a servo whose configuration looks
// corrupted or that appears to ignore EEPROM writes.
//
// HISTORY: this tool was written for the 2026-08 "stuck register" saga -
// reg 0x15 appeared stuck at 254 while writes through the DSP engine's aux
// mailbox acked cleanly.  The true cause turned out to be a TCC67 compiler
// bug (address of a function parameter - see TestParamAddr.c): the config
// program was transmitting garbage values, the servo faithfully stored
// them, and this tool "recovered" the cell simply by transmitting correct
// packets.  The servo was healthy all along.  The tool remains useful as an
// engine-independent way to exercise the bus and repair a genuinely
// corrupted configuration.
//
// FACTS (verified against Feetech's docs and SDKs):
//  - A write ACK confirms RECEIPT only, never application.  The error byte
//    in the ack carries hardware fault flags (voltage/sensor/temp/current/
//    angle/overload) - the protocol has NO "write rejected" indication.
//    READ-BACK IS THE ONLY PROOF a write applied.
//  - The lock register 0x37 gates PERSISTENCE, not acceptance: even locked,
//    a write changes the live value until power-off.
//  - The servo's internal flash commit can delay its write ack tens of ms
//    (measured >21ms) - allow generous timeouts around EEPROM writes.
//  - Factory restore is instruction 0x06 (current Feetech SDKs name it
//    INST_RECOVERY).  Do NOT use 0x0A - the SDKs call that one INST_RESET
//    but it only resets the multi-turn counter.  STS3215 factory baud is
//    1 Mbaud (our bus rate), so 0x06 cannot cost us contact via baud.
//    Whether the ID survives 0x06 is undocumented - it may revert to the
//    factory ID 1 - so THE SERVO MUST BE ALONE ON THE BUS (enforced below).
//
// Talks RAW through the FPGA UART (like SerialServoDiag.c): works without
// reflashing the DSP, gives full control of bus quiet time, and shows rx
// byte counts / framing errors / fault flags for every exchange.
//
// Sequence:
//   1  PING            - fault flags (context only - faults are not
//                        documented to block writes)
//   2  read key state  - P, D, lock, torque, mode, response level, pos corr
//   3  unlock 0x37=0   - and READ IT BACK to prove the unlock sticks
//   4  torque off 0x28=0 and read back
//   5  write P         - read back at 0.2s and 1s (the proof)
//   5b two byte write P+D - a different parse path in the servo firmware
//   6  control test    - write D to a CHANGED value, verify, restore it:
//                        distinguishes "only cell 0x15 damaged" from "all
//                        EEPROM writes silently ignored" (costs two extra
//                        EEPROM program cycles on a suspect servo)
//   7  optional factory restore (DO_FACTORY_RESET) - instruction 0x06,
//                        then our settings are re-applied
//
// If P still reads 254 after step 7: the parameter flash is beyond what the
// protocol can repair.  Remaining options: Feetech's Windows FD software
// (feetechrc.com/software.html) - its Programming tab can LOAD a parameter
// set saved from a healthy servo, and its Upgrade tab re-flashes the
// firmware (deepest repair; do not cut power mid-upgrade) - or replace the
// servo (~$15).
//
// Run INSTEAD of the axis init (it clears serial servo axis modes so the
// DSP engine releases the bus for raw access).

#include "KMotionDef.h"

#define SERVO       0      // servo ID (0-7)
#define USE_EXIO    0      // 0 = bus on IO0, 1 = bus on EX_IO_13
#define TARGET_P    128     // value to restore into reg 0x15 (factory default)

// LAST RESORT: 1 = if the direct rewrite fails, send instruction 0x06
// (factory restore).  Requires the servo to be ALONE on the bus (checked).
// Leave 0 for a diagnose-only run first.
#define DO_FACTORY_RESET  0

// settings re-applied after a factory restore
#define RESTORE_ID        0    // our chain ID (factory ships ID 1)
#define RESTORE_RETDELAY  0    // factory 250 (=500us) -> we run 0
#define RESTORE_POS_CORR  85   // reg 0x1F read +85 before the trouble.
                               // Factory initial value is documented as 0,
                               // so 85 was written post-factory (probably a
                               // middle-position calibration, 0x28=128) -
                               // restore it so the mechanical zero doesn't
                               // shift.  Signed; range -2047..2047.

static unsigned char T[32], R[64];
static int LastSt, LastCnt;    // FPGA status + rx count of the last transfer

// download T[0..n-1] to the FPGA, transmit, wait for the expected reply
// length (or timeout), upload whatever arrived into R.  Returns rx count.
int Xfer(int n, int expect, double tmax)
{
	int i, w, st;
	double t0;

	FPGAW(SERVO_FLUSH_ADD) = 1;                 // clear rx count + error flags

	for (i = 0; i < n; i += 2)
	{
		w = T[i];
		if (i+1 < n) w |= T[i+1] << 8;
		FPGAW(SERVO_TXBUF_ADD + i/2) = w;
	}
	FPGAW(SERVO_TXGO_ADD) = n;                  // start transmission

	t0 = Time_sec();
	for (;;)
	{
		st = FPGAW(SERVO_TXGO_ADD);
		if (!(st & SERVO_ST_BUSY) && (st & SERVO_ST_COUNT) >= expect) break;
		if (Time_sec() - t0 > tmax) break;
		Delay_sec(0.0002);
	}
	Delay_sec(0.001);                           // let a straggler finish

	st = FPGAW(SERVO_TXGO_ADD);
	LastSt = st;
	LastCnt = st & SERVO_ST_COUNT;
	if (LastCnt > 64) LastCnt = 64;
	for (i = 0; i < LastCnt; i += 2)
	{
		w = FPGAW(SERVO_RXBUF_ADD + i/2);
		R[i] = w & 0xff;
		if (i+1 < 64) R[i+1] = (w >> 8) & 0xff;
	}
	return LastCnt;
}

// assemble instruction packet FF FF id len instr par[0..np-1] chk and send
int Pkt(int id, int instr, unsigned char *par, int np, int expect)
{
	int i, sum;

	T[0] = 0xFF; T[1] = 0xFF;
	T[2] = id; T[3] = np + 2; T[4] = instr;
	for (i = 0; i < np; i++) T[5+i] = par[i];
	sum = 0;
	for (i = 2; i < 5+np; i++) sum += T[i];
	T[5+np] = (~sum) & 0xff;

	return Xfer(6+np, expect, 0.02);
}

// validate a status reply FF FF id len err [data] chk
// returns the servo's error byte (0 = no fault flags), or -1 no/bad reply
int ReplyErr(int id, int cnt, int datalen)
{
	int i, sum;

	if (cnt < 6+datalen) return -1;
	if (R[0]!=0xFF || R[1]!=0xFF || R[2]!=id || R[3]!=datalen+2) return -1;
	sum = 0;
	for (i = 2; i < 5+datalen; i++) sum += R[i];
	if (((~sum) & 0xff) != R[5+datalen]) return -1;
	return R[4];
}

// detail an exchange that returned no valid reply: rx count, framing/overflow
// flags, and a hex dump - separates dead bus / noise / response-level-0
void ShowBus(void)
{
	int i;

	printf("        (rx count %d, ferr %d, ovfl %d",
		LastCnt, (LastSt & SERVO_ST_FERR) ? 1:0, (LastSt & SERVO_ST_OVFL) ? 1:0);
	if (LastCnt)
	{
		printf(", bytes:");
		for (i = 0; i < LastCnt && i < 16; i++) printf(" %02X", R[i]);
	}
	printf(")\n");
}

// print an exchange result.  NOTE: for writes a clean ack proves RECEIPT
// only - the protocol has no write-rejected indication.
void PrintErr(char *what, int e)
{
	if (e < 0)
	{
		printf("  %s: NO/BAD REPLY%s\n", what,
			LastCnt == 0 && !(LastSt & (SERVO_ST_FERR|SERVO_ST_OVFL)) ?
			" (silence - response level 0?)" : "");
		ShowBus();
		return;
	}
	printf("  %s: error byte 0x%02X%s\n", what, e,
		e ? "" : " (no fault flags - receipt only, readback is the proof)");
	if (e & 0x01) printf("        bit0 VOLTAGE fault\n");
	if (e & 0x02) printf("        bit1 SENSOR fault\n");
	if (e & 0x04) printf("        bit2 OVERHEAT fault\n");
	if (e & 0x08) printf("        bit3 OVERCURRENT fault\n");
	if (e & 0x10) printf("        bit4 ANGLE fault\n");
	if (e & 0x20) printf("        bit5 OVERLOAD fault\n");
	if (e & 0xC0) printf("        bits 6/7 set - undefined in Feetech docs\n");
}

// read a register; *val filled in, returns error byte or -1
int RdReg(int id, int reg, int len, int *val)
{
	unsigned char par[2];
	int cnt, e;

	par[0] = reg; par[1] = len;
	cnt = Pkt(id, 0x02, par, 2, 6+len);
	e = ReplyErr(id, cnt, len);
	if (e < 0) return -1;
	*val = R[5];
	if (len == 2) *val |= R[6] << 8;
	return e;
}

// write a register (1 or 2 bytes); returns ack error byte or -1
int WrReg(int id, int reg, int val, int len)
{
	unsigned char par[3];
	int cnt;

	par[0] = reg; par[1] = val & 0xff; par[2] = (val >> 8) & 0xff;
	cnt = Pkt(id, 0x03, par, 1+len, 6);
	return ReplyErr(id, cnt, 0);
}

// read + print one register, returns its value (-1 unreadable)
int Show(char *name, int reg, int len)
{
	int v = -1, e;

	e = RdReg(SERVO, reg, len, &v);
	if (e < 0)
	{
		printf("  %-16s reg 0x%02X: NO/BAD REPLY\n", name, reg);
		ShowBus();
		return -1;
	}
	printf("  %-16s reg 0x%02X = %d%s\n", name, reg, v,
		e ? "  (FAULT FLAGS SET)" : "");
	if (e) PrintErr(name, e);
	return v;
}

// sign-magnitude (bit 11) decode for position correction
int DecodeCorr(int v)
{
	return (v & 0x800) ? -(v & 0x7FF) : (v & 0x7FF);
}

main()
{
	int i, e, v, p, d, cnt, id;

	// make the DSP protocol engine release the bus

	for (i = 0; i < N_CHANNELS_KOGNA; i++)
	{
		if (chan[i].InputMode == SERIAL_SERVO_INPUT_MODE) chan[i].InputMode = NO_INPUT_MODE;
		if (chan[i].OutputMode == SERIAL_SERVO_MODE ||
			chan[i].OutputMode == CL_SERIAL_SERVO_MODE)
		{
			DisableAxis(i);
			chan[i].OutputMode = NO_OUTPUT_MODE;
		}
	}
	ServoAuxGo = 0;
	Delay_sec(0.01);

	SetBitDirection(USE_EXIO ? SERVO_BUS_EXIO13_BIT : 0, 1);
	FPGAW(SERVO_BUS_ADD) = 1 | (USE_EXIO ? 2 : 0);

	// ---- 1: PING - shows fault flags (context; faults are not documented
	//         to block writes) ----

	printf("1: PING servo %d\n", SERVO);
	cnt = Pkt(SERVO, 0x01, 0, 0, 6);
	e = ReplyErr(SERVO, cnt, 0);
	PrintErr("PING", e);
	if (e < 0) { printf("  no reply - check power/wiring/ID\n"); return; }

	// ---- 2: key state ----

	printf("2: state before recovery\n");
	p = Show("position P",  0x15, 1);
	d = Show("position D",  0x16, 1);
	    Show("lock",        0x37, 1);
	    Show("torque en",   0x28, 1);
	    Show("mode",        0x21, 1);
	v = Show("resp level",  0x08, 1);
	if (v == 0)
		printf("  ** response level 0: writes are never acked - expect\n"
		       "     'silence' on every write below; readbacks still work **\n");
	v = -1;
	if (RdReg(SERVO, 0x1F, 2, &v) >= 0)
		printf("  %-16s reg 0x1F = %d (raw %d)\n", "pos corr", DecodeCorr(v), v);
	else
		printf("  %-16s reg 0x1F: NO/BAD REPLY\n", "pos corr");

	// ---- 3: unlock EEPROM and PROVE it took ----

	printf("3: unlock EEPROM (write 0x37 = 0)\n");
	e = WrReg(SERVO, 0x37, 0, 1);
	PrintErr("write ack", e);
	v = -1; RdReg(SERVO, 0x37, 1, &v);
	printf("  lock reads back %d %s\n", v, v == 0 ? "(unlocked)" : "** NOT 0 **");

	// ---- 4: torque off and verify ----

	printf("4: torque off (write 0x28 = 0)\n");
	e = WrReg(SERVO, 0x28, 0, 1);
	PrintErr("write ack", e);
	v = -1; RdReg(SERVO, 0x28, 1, &v);
	printf("  torque reads back %d %s\n", v, v == 0 ? "(off)" : "** NOT 0 **");

	// ---- 5: rewrite P; the readbacks are the real verdict ----

	printf("5: write P (0x15) = %d\n", TARGET_P);
	e = WrReg(SERVO, 0x15, TARGET_P, 1);
	PrintErr("write ack", e);

	Delay_sec(0.2);
	v = -1; e = RdReg(SERVO, 0x15, 1, &v);
	if (e < 0) printf("  P after 0.2s: NO/BAD REPLY\n");
	else       printf("  P after 0.2s = %d\n", v);
	Delay_sec(0.8);
	v = -1; e = RdReg(SERVO, 0x15, 1, &v);
	if (e < 0) printf("  P after 1s: NO/BAD REPLY\n");
	else printf("  P after 1s   = %d %s\n", v,
		v == TARGET_P ? "- RECOVERED" : "** STILL WRONG **");

	// 5b: if the single byte write is refused, try a TWO byte write spanning
	// P and D together (0x15+0x16 in one transaction) - a different parse
	// path in the servo firmware.  Needs a valid D value (never invent one).
	if (v != TARGET_P)
	{
		if (d < 0) RdReg(SERVO, 0x16, 1, &d);     // retry the step 2 read
		if (d < 0)
			printf("5b: skipped - D unreadable, won't guess a value to write\n");
		else
		{
			printf("5b: two byte write P+D (0x15) = %d,%d\n", TARGET_P, d);
			e = WrReg(SERVO, 0x15, TARGET_P | (d << 8), 2);
			PrintErr("write ack", e);
			Delay_sec(0.2);
			v = -1; e = RdReg(SERVO, 0x15, 1, &v);
			if (e < 0) printf("  P: NO/BAD REPLY\n");
			else printf("  P reads back %d %s\n", v,
				v == TARGET_P ? "- RECOVERED via 2 byte write" : "** STILL WRONG **");
		}
	}

	// ---- 6: control test - can the servo change ANY EEPROM register? ----
	//
	// Writing D with its own value would prove nothing (an ignored write
	// reads back unchanged too).  Write a CHANGED value, verify the change,
	// then restore.  Costs two extra EEPROM program cycles on a suspect
	// servo - accepted, this is a recovery tool.

	if (d >= 0)
	{
		int dt = (d > 0) ? d-1 : d+1;

		printf("6: control test - write D (0x16) = %d (was %d)\n", dt, d);
		e = WrReg(SERVO, 0x16, dt, 1);
		PrintErr("write ack", e);
		Delay_sec(0.2);
		v = -1; e = RdReg(SERVO, 0x16, 1, &v);
		if (e < 0) printf("  D: NO/BAD REPLY - can't judge\n");
		else if (v == dt)
		{
			printf("  D changed to %d - EEPROM writes WORK elsewhere:\n", v);
			printf("  only cell 0x15 is damaged\n");
			e = WrReg(SERVO, 0x16, d, 1);          // restore
			Delay_sec(0.2);
			v = -1; RdReg(SERVO, 0x16, 1, &v);
			printf("  D restored to %d %s\n", v, v == d ? "(ok)" : "** RESTORE FAILED **");
		}
		else
			printf("  D still reads %d - ALL EEPROM writes are being ignored\n", v);
	}
	else
		printf("6: skipped - D unreadable\n");

#if DO_FACTORY_RESET
	// ---- 7: factory restore (instruction 0x06) and re-apply settings ----
	//
	// 0x06 restores "specific data in the memory control table" to factory
	// values - the exact register set is UNDOCUMENTED for the STS3215, and
	// whether the ID survives is unknown (factory ID is 1).  So the servo
	// must be ALONE on the bus: if another chain member holds ID 1, the
	// restore writes below could capture and reconfigure it.

	printf("7: FACTORY RESTORE (instruction 0x06)\n");

	for (i = 0; i < 8; i++)                  // enforce isolation
	{
		if (i == SERVO) continue;
		cnt = Pkt(i, 0x01, 0, 0, 6);
		if (ReplyErr(i, cnt, 0) >= 0)
		{
			printf("  ABORT: servo ID %d also answers on this bus.\n", i);
			printf("  Disconnect all other servos and re-run.\n");
			return;
		}
	}

	WrReg(SERVO, 0x28, 0, 1);                // torque off + unlock: not
	Delay_sec(0.02);                         // documented as required for
	WrReg(SERVO, 0x37, 0, 1);                // 0x06, but harmless
	Delay_sec(0.02);

	cnt = Pkt(SERVO, 0x06, 0, 0, 6);
	e = ReplyErr(SERVO, cnt, 0);
	PrintErr("restore ack", e);
	Delay_sec(3.0);                          // undocumented duration - settle

	// find the servo again (ID may have reverted to factory 1)

	id = -1;
	cnt = Pkt(SERVO, 0x01, 0, 0, 6);
	if (ReplyErr(SERVO, cnt, 0) >= 0) id = SERVO;
	else
	{
		cnt = Pkt(1, 0x01, 0, 0, 6);
		if (ReplyErr(1, cnt, 0) >= 0) id = 1;
	}
	if (id < 0)
	{
		for (i = 0; i < 254 && id < 0; i++)  // full scan as a last resort
		{
			cnt = Pkt(i, 0x01, 0, 0, 6);
			if (ReplyErr(i, cnt, 0) >= 0) id = i;
		}
	}
	if (id < 0)
	{
		printf("  no servo answers on any ID after the restore.  Power cycle\n");
		printf("  the servo and run SerialServoScan.c; if still silent it\n");
		printf("  needs Feetech's FD software (Windows) to recover.\n");
		return;
	}
	printf("  servo answers on ID %d after restore\n", id);

	WrReg(id, 0x37, 0, 1);                   // unlock
	Delay_sec(0.02);

	if (id != RESTORE_ID)
	{
		e = WrReg(id, 0x05, RESTORE_ID, 1);
		PrintErr("write ID", e);
		Delay_sec(0.1);
		id = RESTORE_ID;
		WrReg(id, 0x37, 0, 1);               // unlock again on the new ID
		Delay_sec(0.02);
	}

	e = WrReg(id, 0x07, RESTORE_RETDELAY, 1);
	PrintErr("write return delay", e);
	Delay_sec(0.05);

	v = RESTORE_POS_CORR;
	if (v < -2047 || v > 2047) v = 0;        // range guard
	else if (v < 0) v = 0x800 | (-v);
	e = WrReg(id, 0x1F, v, 2);
	PrintErr("write pos corr", e);
	Delay_sec(0.05);

	WrReg(id, 0x37, 1, 1);                   // re-lock

	printf("  after restore:\n");
	p = -1; RdReg(id, 0x15, 1, &p); printf("    P            = %d (factory 32)\n", p);
	v = -1; RdReg(id, 0x16, 1, &v); printf("    D            = %d (factory 32)\n", v);
	v = -1; RdReg(id, 0x07, 1, &v); printf("    return delay = %d\n", v);
	v = -1;
	if (RdReg(id, 0x1F, 2, &v) >= 0) printf("    pos corr     = %d\n", DecodeCorr(v));
	else                             printf("    pos corr     : NO/BAD REPLY\n");
	printf("  the restore's register coverage is undocumented - run\n");
	printf("  SerialServoConfig.c (dump mode) and verify EVERY setting\n");
	printf("  you care about, then re-apply what's missing.\n");
	if (p != 32)
	{
		printf("  ** P reads %d, not factory 32 - the parameter flash is\n", p);
		printf("  beyond protocol repair.  Use Feetech FD software (Windows):\n");
		printf("  Programming tab LOAD from a healthy servo's saved set,\n");
		printf("  or Upgrade tab firmware re-flash - or replace (~$15).\n");
	}
#else
	printf("7: factory restore disabled (set DO_FACTORY_RESET 1 to enable;\n");
	printf("   the servo must be ALONE on the bus)\n");
#endif

	WrReg(SERVO, 0x37, 1, 1);                // re-lock EEPROM
	printf("done - torque is OFF; re-run the axis init before moving\n");
}
