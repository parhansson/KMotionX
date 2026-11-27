// DxlAxisInit3.c
//
// Configure THREE axes, each running a Dynamixel XL430 (Protocol 2.0) on the
// serial servo bus in the LOW LAG mode, with INDEPENDENT per axis settings.
// It also programs each servo's own position PID gains.
//
// The axis channel settings in ConfigAxes() are three flat blocks in the
// standard format, so the KMotion.exe Config Screen Import/Export works on
// this file.  Rules to keep it that way:
//   - Export updates values in place, keeping comments and extra lines such
//     as CmdLead where they are.  KMotion versions before 5.5.0 instead
//     regenerate the whole block from ch->InputMode through ch->iir[2].A2
//     (removing anything else inside it) and cannot find a block containing
//     comment lines at all - so keeping the blocks clean with CmdLead just
//     below each one remains the most compatible layout
//   - the servo's own register settings are not screen settings - they stay
//     in Settings()
//   - keep AxisCh[]/ServoID[] in Settings() consistent with the
//     InputChan0/OutputChan0 values in the blocks
//
// Every servo must already have been set up by DxlSetup.c: 1 Mbaud, its ID
// equal to the axis channel, Operating Mode 4 (Extended Position), and profile
// velocity/acceleration 0.
//
// The servo P/I/D gains live in RAM and RESET AT SERVO POWER UP, so they have
// to be rewritten every session - that is the main reason this program exists.
// Every write is read back and verified: a Dynamixel ack proves only that a
// well formed packet came back, not that the value was accepted.
//
// SAFETY
//   - The bus is QUIESCED first.  chan[] is persistent DSP state, so on a
//     re-run the axes may still be configured and enabled, in which case the
//     engine is still broadcasting SYNC WRITE goal positions every bus cycle.
//     Step 0 disables the axes, clears their modes, and waits for the engine
//     to release the bus before any register is touched.  Without this the
//     Goal Position preset below is overwritten within one bus cycle.
//   - Goal Position is preset to Present Position before torque comes on, so
//     enabling torque cannot snap a joint back to a stale goal.
//   - Torque is NOT released while writing settings (a gravity loaded joint
//     would drop).  The one EEPROM setting (PWM Limit) that requires torque
//     off is only written when the stored value differs, with torque off
//     just long enough for that single write.
//   - Nothing is enabled unless ALL three servos check out.  If any step fails
//     after torque is on, torque is turned back off on every servo and every
//     axis is disabled - and that back off is itself VERIFIED.
//
// Position units are servo counts (4096/rev), multi turn +/-256 revs.


#include "KMotionDef.h"


#define NAXES       3
#define USE_EXIO    0     // 0 = bus on IO0, 1 = bus on EX_IO_13
#define SET_COORD   1     // 1 = also DefineCoordSystem(axis0,axis1,axis2,-1)

#define SNAP_LIMIT  50    // counts of goal-vs-present mismatch tolerated
                          // before refusing to enable torque (4096 = 1 rev)

#define STALE_LIMIT 100   // ServoStale counts (90us servo interrupts) allowed
                          // before telemetry is considered dead.  With three
                          // servos sharing the bus each ID refreshes roughly
                          // every 25-30 interrupts, so this is ~3x margin.

#define DISABLE_TRIP_TIME 0.050  // secs an axis must stay disabled before the
                          // watch at the end turns everything off.  Brief
                          // disables (the Step Response Screen disables,
                          // downloads parameters, and re-enables) are ignored.

// =============== per axis SERVO REGISTER settings - EDIT HERE ==============
//
// These are the servo's OWN control table values (not axis channel settings -
// those are in ConfigAxes() below).  Filled in by Settings() rather than as
// initializers because TCC67 does not support aggregate initializers.  Index
// is the entry number, NOT the axis channel - AxisCh[] maps entry to channel.

int    AxisCh[NAXES];    // KFLOP/Kogna axis channel
int    ServoID[NAXES];   // Dynamixel ID on the bus (0-7)

int    SvP[NAXES];       // servo reg 84, position P gain  0 ~ 16,383
int    SvI[NAXES];       // servo reg 82, position I gain  0 ~ 16,383
int    SvD[NAXES];       // servo reg 80, position D gain  0 ~ 16,383
int    PWMLim[NAXES];    // servo reg 36, PWM Limit 885=100%

void Settings(void)
{
	AxisCh[0] = 0;    ServoID[0] = 0;
	SvP[0] = 2000;    SvI[0] = 0;    SvD[0] = 2000;    PWMLim[0] = 885;    //PID all 0 ~ 16,383

	AxisCh[1] = 1;    ServoID[1] = 1;
	SvP[1] = 1500;    SvI[1] = 0;    SvD[1] = 2000;    PWMLim[1] = 885;

	AxisCh[2] = 2;    ServoID[2] = 2;
	SvP[2] = 1000;    SvI[2] = 0;    SvD[2] = 2000;    PWMLim[2] = 885;
}

// ================== axis channel settings - EDIT HERE ======================
//
// *** P AND FFVel MUST BE MEASURED TOGETHER ***
// FFVel is not a gain here, it is SECONDS of constant servo lag to cancel, and
// that lag depends on the servo's P gain (SvP above).  The 0.0055 below was
// measured at servo P = 10000 (2026-08-05, XL430, no load, +/-4 counts at
// 1500 counts/s).  It is NOT the right figure for P = 1000, where the servo's
// own loop is much softer and the residual lag is several times larger.
// Under-compensating is harmless - it just leaves velocity-proportional
// following error - but re-run SerialServoLagVsVel.c at the P you actually
// ship and set FFVel to the measured lag before this goes on a machine.
//
// Other notes baked into the blocks:
//   - KFLOP/Kogna PID = 0: no PID assist, feed forward only (low lag mode)
//   - MaxOutput MUST be nonzero or FFVel is clamped away
//   - MaxFollowingError = 100: with 3 servos sharing the bus each one's
//     telemetry updates roughly every 2.5ms instead of ~0.8ms, so the
//     measured position is a coarser staircase - leave headroom
//   - iir filters are explicitly set to pass through - a stale iir from a
//     previous configuration with B0 = 0 would silently kill the servo output
//   - InputOffset0/OutputOffset are the per axis position offsets to set 0
//     (equal and opposite)

void ConfigAxes(void)
{
	ch0->InputMode=SERIAL_SERVO_INPUT_MODE;
	ch0->OutputMode=CL_SERIAL_SERVO_MODE;
	ch0->Vel=400;
	ch0->Accel=8000;
	ch0->Jerk=80000;
	ch0->P=0;
	ch0->I=0.001;
	ch0->D=0;
	ch0->FFAccel=0;
	ch0->FFVel=0.02;
	ch0->MaxI=200;
	ch0->MaxErr=1e+06;
	ch0->MaxOutput=200;
	ch0->DeadBandGain=1;
	ch0->DeadBandRange=0;
	ch0->InputChan0=0;
	ch0->InputChan1=0;
	ch0->OutputChan0=0;
	ch0->OutputChan1=0;
	ch0->MasterAxis=-1;
	ch0->LimitSwitchOptions=0x100;
	ch0->LimitSwitchNegBit=0;
	ch0->LimitSwitchPosBit=0;
	ch0->SoftLimitPos=1e+09;
	ch0->SoftLimitNeg=-1e+09;
	ch0->InputGain0=1;
	ch0->InputGain1=1;
	ch0->InputOffset0=-1009;
	ch0->InputOffset1=0;
	ch0->OutputGain=1;
	ch0->OutputOffset=1009;
	ch0->SlaveGain=1;
	ch0->BacklashMode=BACKLASH_OFF;
	ch0->BacklashAmount=0;
	ch0->BacklashRate=0;
	ch0->invDistPerCycle=1;
	ch0->Lead=0;
	ch0->MaxFollowingError=20;
	ch0->StepperAmplitude=20;

	ch0->iir[0].B0=1;
	ch0->iir[0].B1=0;
	ch0->iir[0].B2=0;
	ch0->iir[0].A1=0;
	ch0->iir[0].A2=0;

	ch0->iir[1].B0=1;
	ch0->iir[1].B1=0;
	ch0->iir[1].B2=0;
	ch0->iir[1].A1=0;
	ch0->iir[1].A2=0;

	ch0->iir[2].B0=0.000769;
	ch0->iir[2].B1=0.001538;
	ch0->iir[2].B2=0.000769;
	ch0->iir[2].A1=1.92076;
	ch0->iir[2].A2=-0.923833;

	ch0->CmdLead=0;               // low lag mode - no command preview needed

	ch1->InputMode=SERIAL_SERVO_INPUT_MODE;
	ch1->OutputMode=CL_SERIAL_SERVO_MODE;
	ch1->Vel=200;
	ch1->Accel=8000;
	ch1->Jerk=80000;
	ch1->P=0;
	ch1->I=0.001;   // test comment 1
	/* test 2 */
	ch1->D=0;
	ch1->FFAccel=0;
	ch1->FFVel=0.03;
	ch1->MaxI=200;
	ch1->MaxErr=1e+06;
	ch1->MaxOutput=200;
	ch1->DeadBandGain=1;
	ch1->DeadBandRange=0;
	ch1->InputChan0=1;
	ch1->InputChan1=0;
	ch1->OutputChan0=1;
	ch1->OutputChan1=0;
	ch1->MasterAxis=-1;
	ch1->LimitSwitchOptions=0x100;
	ch1->LimitSwitchNegBit=0;
	ch1->LimitSwitchPosBit=0;
	ch1->SoftLimitPos=1e+09;
	ch1->SoftLimitNeg=-1e+09;
	ch1->InputGain0=1;
	ch1->InputGain1=1;
	ch1->InputOffset0=1041;
	ch1->InputOffset1=0;
	ch1->OutputGain=1;
	ch1->OutputOffset=-1041;
	ch1->SlaveGain=1;
	ch1->BacklashMode=BACKLASH_OFF;
	ch1->BacklashAmount=0;
	ch1->BacklashRate=0;
	ch1->invDistPerCycle=1;
	ch1->Lead=0;
	ch1->MaxFollowingError=20;
	ch1->StepperAmplitude=20;

	ch1->iir[0].B0=1;
	ch1->iir[0].B1=0;
	ch1->iir[0].B2=0;
	ch1->iir[0].A1=0;
	ch1->iir[0].A2=0;

	ch1->iir[1].B0=1;
	ch1->iir[1].B1=0;
	ch1->iir[1].B2=0;
	ch1->iir[1].A1=0;
	ch1->iir[1].A2=0;

	ch1->iir[2].B0=0.000769;
	ch1->iir[2].B1=0.001538;
	ch1->iir[2].B2=0.000769;
	ch1->iir[2].A1=1.92081;
	ch1->iir[2].A2=-0.923885;

	ch1->CmdLead=0;               // low lag mode - no command preview needed


	ch2->InputMode=SERIAL_SERVO_INPUT_MODE;  // test
	ch2->OutputMode=CL_SERIAL_SERVO_MODE;
	ch2->Vel=200;
	ch2->Accel=8000;
	ch2->Jerk=80000;
	ch2->P=1;
	ch2->I=0.001;
	ch2->D=200;
	ch2->FFAccel=0;
	ch2->FFVel=0.03;
	ch2->MaxI=200;
	ch2->MaxErr=1e+06;
	ch2->MaxOutput=200;
	ch2->DeadBandGain=1;
	ch2->DeadBandRange=0;
	ch2->InputChan0=2;
	ch2->InputChan1=0;
	ch2->OutputChan0=2;
	ch2->OutputChan1=0;
	ch2->MasterAxis=-1;
	ch2->LimitSwitchOptions=0x100;
	ch2->LimitSwitchNegBit=0;
	ch2->LimitSwitchPosBit=0;
	ch2->SoftLimitPos=1e+09;
	ch2->SoftLimitNeg=-1e+09;
	ch2->InputGain0=-1;
	ch2->InputGain1=1;
	ch2->InputOffset0=3055;
	ch2->InputOffset1=0;
	ch2->OutputGain=-1;
	ch2->OutputOffset=3055;
	ch2->SlaveGain=1;
	ch2->BacklashMode=BACKLASH_OFF;
	ch2->BacklashAmount=0;
	ch2->BacklashRate=0;
	ch2->invDistPerCycle=1;
	ch2->Lead=0;
	ch2->MaxFollowingError=50;
	ch2->StepperAmplitude=20;

	ch2->iir[0].B0=1;
	ch2->iir[0].B1=0;
	ch2->iir[0].B2=0;
	ch2->iir[0].A1=0;
	ch2->iir[0].A2=0;

	ch2->iir[1].B0=1;
	ch2->iir[1].B1=0;
	ch2->iir[1].B2=0;
	ch2->iir[1].A1=0;
	ch2->iir[1].A2=0;

	ch2->iir[2].B0=0.000126;
	ch2->iir[2].B1=0.000252;
	ch2->iir[2].B2=0.000126;
	ch2->iir[2].A1=1.96833;
	ch2->iir[2].A2=-0.968829;

	ch2->CmdLead=0;               // low lag mode - no command preview needed
}

// ===========================================================================

// one servo control table access through the DSP protocol engine
// size: 0 = 1 byte, 1 = 2 bytes, 2 = 4 bytes
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

// read a register into *val.  Returns 0 on success - the value itself is never
// used as the status, so registers that can legitimately read -1 (4 byte
// signed positions) work correctly.
int RdChk(int id, int reg, int size, int *val)
{
	int v = 0;
	int r = DxlReg(id, reg, &v, size, 1);
	if (!r) *val = v;
	return r;
}

// write with readback verify.  Returns 0 on success.
//
// NOTE the local copy of val: DxlReg takes the ADDRESS of the value, and
// taking the address of a function PARAMETER used to miscompile under TCC67
// (parameters arrive in registers).  The compiler fix is in but has not been
// hardware validated yet - see TestParamAddr.c - so copy to a local first.
int Wr(int id, int reg, int val, int size)
{
	int local = val;
	int r, v = 0;

	r = DxlReg(id, reg, &local, size, 0);
	if (r)
	{
		printf("  servo %d reg %d: write failed, result %d\n", id, reg, r);
		return 1;
	}

	if (ServoAuxErr > 0)
		printf("  servo %d reg %d: servo error byte 0x%02X%s\n", id, reg, ServoAuxErr,
			(ServoAuxErr & 0x7f) == 4 ? " (data range)" :
			(ServoAuxErr & 0x7f) == 6 ? " (data limit)" :
			(ServoAuxErr & 0x7f) == 7 ? " (access - EEPROM needs torque off)" : "");

	if ((r = RdChk(id, reg, size, &v)))
	{
		// result codes: -2 engine never completed, 1 no/short response,
		// 2 wrong length, 3 framing error, 4 header/ID, 5 CRC, 6 servo
		// error status - the code discriminates line noise from silence
		printf("  servo %d reg %d: cannot read back, result %d\n", id, reg, r);
		return 1;
	}

	if (v != local)
	{
		printf("  servo %d reg %d: VERIFY failed, wrote %d read %d\n",
			id, reg, local, v);
		return 1;
	}
	return 0;
}

// Torque off everywhere, axes disabled, bus released.  The torque off is
// VERIFIED and retried - reporting "backed off" without checking would be
// worse than not reporting at all.  Returns 1 if every servo confirmed.
int AllOff(void)
{
	int i, k, ok = 1;

	for (i = 0; i < NAXES; i++)
	{
		DisableAxis(AxisCh[i]);
		chan[AxisCh[i]].InputMode  = NO_INPUT_MODE;
		chan[AxisCh[i]].OutputMode = NO_OUTPUT_MODE;
	}

	Delay_sec(0.05);              // let the engine release the bus

	for (i = 0; i < NAXES; i++)
	{
		for (k = 0; k < 3; k++)
			if (!Wr(ServoID[i], DXL_REG_TORQUE, 0, 0)) break;

		if (k == 3)
		{
			printf("*** servo %d: TORQUE OFF NOT CONFIRMED -"
				" JOINT MAY STILL BE LIVE ***\n", ServoID[i]);
			ok = 0;
		}
	}
	return ok;
}

// is this servo reporting FRESH telemetry right now?  ServoPosValid alone is
// sticky - it only means "has reported at some point" - so it can still be set
// from a previous run.
int Live(int id)
{
	return ServoPosValid[id] && ServoStale[id] < STALE_LIMIT && ServoResult[id] == 0;
}


main()
{
	int i, id, v, model, pos, goal, bad = 0, n;
	int pwm, torque, wr;
	double t0;

	Settings();

	// ---- 0. quiesce the bus.  chan[] survives between runs, so the engine may
	//         still own the bus and be broadcasting SYNC WRITE goal positions
	//         from a previous session.  Release our axes, then confirm nobody
	//         else is holding it - we must not stomp on a channel we don't own.

	for (i = 0; i < NAXES; i++)
	{
		DisableAxis(AxisCh[i]);
		chan[AxisCh[i]].InputMode  = NO_INPUT_MODE;
		chan[AxisCh[i]].OutputMode = NO_OUTPUT_MODE;
	}

	t0 = Time_sec();
	while (ServoActiveMask && Time_sec() - t0 < 0.5) Delay_sec(0.001);

	if (ServoActiveMask)
	{
		printf("serial servo bus still claimed, ServoActiveMask 0x%02X\n",
			ServoActiveMask);
		printf("another axis owns it - disable that axis first.  Nothing done.\n");
		return;
	}

	// ServoActiveMask drops the tick after the modes are cleared, and the
	// engine then abandons whatever bus cycle was in progress - a servo may
	// still be answering it.  Starting the first request below right away
	// collided with that reply ("axis 0: no Dynamixel answers at ID 0" on a
	// re-run).  Let the wire go quiet first, as AllOff() does.
	Delay_sec(0.05);

	// safe to set the sticky globals now: every axis is disabled and stationary

	ServoProtocol   = 1;          // Dynamixel Protocol 2.0 engine
	ServoBusEXIO    = USE_EXIO;
	ServoBaud576    = 0;          // 1 Mbaud
	ServoCycleTicks = 0;          // bus cycles back to back
	CmdDelay        = 0;          // low lag: no global command preview

	// ---- 1. prove every servo answers, is in the expected mode, and is not
	//         sitting on a latched fault.  The bus really is quiet here.

	for (i = 0; i < NAXES; i++)
	{
		id = ServoID[i];

		if (RdChk(id, DXL_REG_MODEL, 1, &model))
		{
			printf("axis %d: no Dynamixel answers at ID %d\n", AxisCh[i], id);
			bad = 1;
			continue;
		}

		if (RdChk(id, DXL_REG_OPMODE, 0, &v)) { bad = 1; continue; }

		if (v != 4)
		{
			printf("servo %d: operating mode %d, expected 4 (extended position)"
				" - run DxlSetup.c\n", id, v);
			bad = 1;
			continue;
		}

		if (RdChk(id, DXL_REG_HW_ERROR, 0, &v))
		{
			printf("servo %d: hardware error status unreadable\n", id);
			bad = 1;
			continue;
		}

		if (v)
		{
			printf("servo %d: LATCHED HARDWARE ERROR 0x%02X"
				" - power cycle the servo to clear\n", id, v);
			bad = 1;
			continue;
		}

		printf("axis %d: servo %d model %d OK\n", AxisCh[i], id, model);
	}

	if (bad) { printf("*** setup incomplete - nothing enabled ***\n"); return; }

	// ---- 2. servo side gains
	//
	//	The P/I/D gains and profiles are RAM registers - safe to write with
	//	torque on.  The PWM Limit however is EEPROM, which the servo REFUSES
	//	to write while torque is on (error 0x07) - and torque may still be
	//	latched on from a previous session, holding the robot up.  Turning
	//	torque off lets a gravity loaded joint drop, so: the PWM Limit is
	//	only written when the stored value actually differs (EEPROM is
	//	persistent, so normally it already matches), and when it must be
	//	written torque goes off just long enough for the one write and then
	//	RIGHT back on.  Re-enabling cannot snap the joint - the XL430
	//	latches Goal Position = Present Position when torque comes on.

	for (i = 0; i < NAXES; i++)
	{
		id = ServoID[i];

		// RAM registers - always safe with torque on

		bad |= Wr(id, DXL_REG_POS_P, SvP[i], 1);
		bad |= Wr(id, DXL_REG_POS_I, SvI[i], 1);
		bad |= Wr(id, DXL_REG_POS_D, SvD[i], 1);

		// keep the internal trajectory generator off.  This is what LOW LAG
		// means, and the FFVel numbers above are only valid with it off.
		bad |= Wr(id, DXL_REG_PROF_ACC, 0, 2);
		bad |= Wr(id, DXL_REG_PROF_VEL, 0, 2);

		// PWM Limit (EEPROM): write only if it differs from what is stored

		if (RdChk(id, DXL_REG_PWM_LIM, 1, &pwm)) { bad = 1; continue; }

		if (pwm != PWMLim[i])
		{
			if (RdChk(id, DXL_REG_TORQUE, 0, &torque)) { bad = 1; continue; }

			if (torque && Wr(id, DXL_REG_TORQUE, 0, 0))
			{
				printf("servo %d: cannot release torque for PWM Limit write\n", id);
				bad = 1;
				continue;
			}

			wr = Wr(id, DXL_REG_PWM_LIM, PWMLim[i], 1);

			// torque back on IMMEDIATELY (before even checking the write)
			// so the joint drops as briefly as possible

			if (torque && Wr(id, DXL_REG_TORQUE, 1, 0))
			{
				printf("*** servo %d: TORQUE RE-ENABLE FAILED after PWM Limit"
					" write - JOINT IS FREE ***\n", id);
				bad = 1;
			}

			bad |= wr;
		}

		printf("servo %d: P=%d I=%d D=%d, PWM=%d, profiles off\n",
			id, SvP[i], SvI[i], SvD[i], PWMLim[i]);
	}

	if (bad) { printf("*** gain setup failed - nothing enabled ***\n"); return; }

	// ---- 3. make every servo hold WHERE IT IS.  Goal Position can still hold
	//         a value from an earlier session, and if the joint has been moved
	//         by hand since, enabling torque would snap it back.  Preset
	//         Goal = Present, then confirm it from the servo itself.

	for (i = 0; i < NAXES; i++)
	{
		id = ServoID[i];

		if (RdChk(id, DXL_REG_PRES_POS, 2, &pos))
		{
			printf("servo %d: cannot read present position\n", id);
			bad = 1;
			continue;
		}

		// Preset WITHOUT the exact-equality verify: with torque off the
		// XL430 mirrors Goal = Present continuously, so a read-back can
		// legitimately differ by a count of encoder jitter (observed:
		// wrote 867 read 868 -> false abort).  The snap-limit check just
		// below is the REAL validation, with the tolerance that register
		// deserves.
		{
			int local = pos;
			if (DxlReg(id, DXL_REG_GOAL_POS, &local, 2, 0))
			{
				printf("servo %d: could not preset goal position\n", id);
				bad = 1;
				continue;
			}
		}

		// independent confirmation - do not trust the write alone
		if (RdChk(id, DXL_REG_GOAL_POS, 2, &goal))
		{
			printf("servo %d: cannot read goal position\n", id);
			bad = 1;
			continue;
		}

		if (goal - pos > SNAP_LIMIT || pos - goal > SNAP_LIMIT)
		{
			printf("*** servo %d: goal %d vs present %d - torque would snap,"
				" REFUSING\n", id, goal, pos);
			bad = 1;
		}
	}

	if (bad) { printf("*** unsafe goal positions - nothing enabled ***\n"); return; }

	// ---- 4. axis configuration.  This is what makes the DSP protocol engine
	//         claim the bus and begin polling telemetry.

	ConfigAxes();

	// ---- 5. torque on.  REQUIRED: position goals are silently ignored with
	//         torque off.  Wr() reads register 64 back, so a refused enable is
	//         caught here rather than showing up as a dead axis later.

#if 1
	for (i = 0; i < NAXES; i++)
		if (Wr(ServoID[i], DXL_REG_TORQUE, 1, 0))
		{
			printf("servo %d: TORQUE ENABLE FAILED\n", ServoID[i]);
			bad = 1;
		}
#endif


	if (bad)
	{
		if (AllOff()) printf("*** torque enable failed - all servos confirmed"
			" off, nothing enabled ***\n");
		else          printf("*** torque enable failed AND BACK OFF INCOMPLETE"
			" - see above ***\n");
		return;
	}

	// ---- 6. wait for FRESH telemetry from every servo (the enable interlock)

	t0 = Time_sec();
	for (;;)
	{
		n = 0;
		for (i = 0; i < NAXES; i++) if (Live(ServoID[i])) n++;
		if (n == NAXES) break;
		if (Time_sec() - t0 > 2.0) break;
		Delay_sec(0.001);
	}

	for (i = 0; i < NAXES; i++)
		if (!Live(ServoID[i]))
		{
			printf("servo %d: no fresh telemetry (valid %d stale %d result %d)\n",
				ServoID[i], ServoPosValid[ServoID[i]],
				ServoStale[ServoID[i]], ServoResult[ServoID[i]]);
			bad = 1;
		}

	if (bad)
	{
		if (AllOff()) printf("*** no feedback - all servos confirmed off,"
			" nothing enabled ***\n");
		else          printf("*** no feedback AND BACK OFF INCOMPLETE"
			" - see above ***\n");
		return;
	}

	// ---- 7. enable at the measured position, so enabling never causes motion.
	//         Re-check freshness immediately before each enable.

	for (i = 0; i < NAXES; i++)
	{
		if (!Live(ServoID[i]))
		{
			printf("servo %d: telemetry went stale while enabling\n", ServoID[i]);
			AllOff();
			printf("*** aborted mid enable - all axes disabled ***\n");
			return;
		}
		printf("Enabling Axis %d at Dest %f\n", AxisCh[i], chan[AxisCh[i]].Position);
		EnableAxisDest(AxisCh[i], chan[AxisCh[i]].Position);
	}

	if (SET_COORD) DefineCoordSystem(AxisCh[0], AxisCh[1], AxisCh[2], -1);

	// ---- 8. report

	printf("\n");
	for (i = 0; i < NAXES; i++)
	{
		id = ServoID[i];

		if (RdChk(id, DXL_REG_HW_ERROR, 0, &v))
			printf("axis %d  servo %d  pos %d  hwerr UNREADABLE\n",
				AxisCh[i], id, ServoPosition[id]);
		else
			printf("axis %d  servo %d  pos %d  hwerr 0x%02X\n",
				AxisCh[i], id, ServoPosition[id], v);

		printf("         P=%d I=%d D=%d  FFVel=%.4f sec\n",
			SvP[i], SvI[i], SvD[i], chan[AxisCh[i]].FFVel);
		printf("         %.1fV  %dC\n",
			SERVO_CONVERT_TO_VOLTS(ServoVolts[id]), ServoTemp[id]);
	}

	printf("\n%d axes enabled", NAXES);
	if (SET_COORD) printf(" as coordinate system X=%d Y=%d Z=%d",
		AxisCh[0], AxisCh[1], AxisCh[2]);
	printf("\n");
	printf("note: 'Current' telemetry reports Present Load (0.1%% units) -\n");
	printf("the XL430 has no current sensor\n");

#if 1
	// if any axis stays disabled for more than DISABLE_TRIP_TIME then
	// disable all axes and turn all torque off.  The time window is
	// necessary because the Step Response Screen briefly disables an axis,
	// downloads parameters, and re-enables it - a short disable like that
	// must not drop the whole robot.
	{
		double DisTime[NAXES];
		int i, k;

		for (i = 0; i < NAXES; i++) DisTime[i] = 0;

		for (;;)
		{
			for (i = 0; i < NAXES; i++)
			{
				if (!chan[AxisCh[i]].Enable) // Axis disabled?
				{
					if (DisTime[i] == 0)
					{
						DisTime[i] = Time_sec();  // just now - start timing it
					}
					else if (Time_sec() - DisTime[i] > DISABLE_TRIP_TIME)
					{
						// disabled too long to be a parameter download -
						// treat as a real trip/disable: everything off

						for (k = 0; k < NAXES; k++)
						{
							printf("Disable Axis %d\n",AxisCh[k]);
							DisableAxis(AxisCh[k]);  // disable all axes
							Wr(ServoID[k], DXL_REG_TORQUE, 0, 0); // torque off
						}

						return;
					}
				}
				else
				{
					DisTime[i] = 0;  // enabled (or re-enabled) - clear its timer
				}
			}
		}
	}
#endif
}
