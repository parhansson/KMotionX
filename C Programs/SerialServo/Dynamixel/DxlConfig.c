// DxlConfig.c
//
// Read, display, and optionally set a Dynamixel XL430's parameters
// (Protocol 2.0, ServoProtocol = 1).  Run as-is (all SET_ values -1) for a
// dump; set any SET_ define >= 0 to write that register.
//
// EEPROM registers (address below 64) can only be written with TORQUE OFF -
// this program turns torque off before writing and LEAVES IT OFF, so re-run
// DxlAxisInit.c afterwards.  RAM registers (64 and up) reset at power up.
//
// The status packet error byte is checked on every write - Dynamixels
// actually report rejected writes (data range/limit/access errors), unlike
// the Feetech servos.

#include "KMotionDef.h"

#define SERVO 0            // servo ID on the bus (0-7)

// ======================= values to write, -1 = leave =======================

#define SET_POS_P      -1  // reg 84 (2 bytes), default 640.  Position loop P.
                           // With the profile generator off (low lag mode)
                           // this is the main tracking stiffness.
#define SET_POS_I      -1  // reg 82 (2 bytes), default 0
#define SET_POS_D      -1  // reg 80 (2 bytes), default 3600
#define SET_FF1_VEL    -1  // reg 90 (2 bytes), default 0.  Velocity
                           // feedforward - worth trying for tracking
#define SET_FF2_ACC    -1  // reg 88 (2 bytes), default 0.  Accel feedforward

#define SET_PROF_VEL   -1  // reg 112 (4 bytes, RAM).  0 = no velocity
                           // profile = LOW LAG (DxlSetup default)
#define SET_PROF_ACC   -1  // reg 108 (4 bytes, RAM).  0 = no accel profile

#define SET_RETDELAY   -1  // reg 9, 2us units - keep 0
#define SET_OPMODE     -1  // reg 11: 3 = position (1 turn), 4 = extended
                           // position +/-256 turns (the axis integration
                           // expects 4)
#define SET_DRIVEMODE  -1  // reg 10: bit0 = reverse direction
#define SET_HOMING     ((int)0x80000000)  // reg 20 (4 bytes SIGNED), added to
                           // Present Position - mechanical zero trim.
                           // 0x80000000 = leave (any other value written)
#define SET_TEMP_LIM   -1  // reg 31, deg C, default 72
#define SET_PWM_LIM    -1  // reg 36 (2 bytes), default 885 = 100%.  Lower
                           // for a compliant/safe axis
#define SET_SHUTDOWN   -1  // reg 63, fault mask, default 0x35

// ============================================================================

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

int Rd(int reg, int size)
{
	int v = 0;
	if (DxlReg(SERVO, reg, &v, size, 1)) return -1;
	return v;
}

// write with readback verify; prints the servo's error byte on rejection
int Wr(int reg, int val, int size)
{
	int r, v;
	int local_val = val;

	r = DxlReg(SERVO, reg, &local_val, size, 0);
	if (r) { printf("  WRITE reg %d FAILED (result %d)\n", reg, r); return 1; }

	if (ServoAuxErr > 0)
		printf("  reg %d: servo reports error 0x%02X%s\n", reg, ServoAuxErr,
			(ServoAuxErr & 0x7f) == 4 ? " (data range)" :
			(ServoAuxErr & 0x7f) == 6 ? " (data limit)" :
			(ServoAuxErr & 0x7f) == 7 ? " (access - EEPROM needs torque off)" : "");

	Delay_sec(0.05);

	v = Rd(reg, size);
	if (v != local_val)
	{
		printf("  VERIFY reg %d failed (wrote %d read %d)\n", reg, local_val, v);
		return 1;
	}
	printf("  reg %d = %d OK\n", reg, local_val);
	return 0;
}

void Dump(void)
{
	printf("\n---- Dynamixel %d configuration ----\n", SERVO);
	printf("model             %d\n", Rd(DXL_REG_MODEL, 1));
	printf("firmware          %d\n", Rd(DXL_REG_FIRMWARE, 0));
	printf("ID                %d\n", Rd(DXL_REG_ID, 0));
	printf("baud              %d (3 = 1 Mbaud)\n", Rd(DXL_REG_BAUD, 0));
	printf("return delay      %d (x2us)\n", Rd(DXL_REG_RETDELAY, 0));
	printf("drive mode        %d\n", Rd(DXL_REG_DRIVEMODE, 0));
	printf("operating mode    %d (4 = extended position)\n", Rd(DXL_REG_OPMODE, 0));
	printf("homing offset     %d\n", Rd(DXL_REG_HOMING, 2));
	printf("temp limit        %d C\n", Rd(DXL_REG_TEMP_LIM, 0));
	printf("volt max/min      %d/%d (x0.1V)\n",
		Rd(DXL_REG_VOLT_MAX, 1), Rd(DXL_REG_VOLT_MIN, 1));
	printf("PWM limit         %d (885 = 100%%)\n", Rd(DXL_REG_PWM_LIM, 1));
	printf("shutdown mask     0x%02X\n", Rd(DXL_REG_SHUTDOWN, 0));
	printf("torque            %d\n", Rd(DXL_REG_TORQUE, 0));
	printf("hardware error    0x%02X\n", Rd(DXL_REG_HW_ERROR, 0));
	printf("position P/I/D    %d / %d / %d\n",
		Rd(DXL_REG_POS_P, 1), Rd(DXL_REG_POS_I, 1), Rd(DXL_REG_POS_D, 1));
	printf("FF1(vel)/FF2(acc) %d / %d\n", Rd(DXL_REG_FF1, 1), Rd(DXL_REG_FF2, 1));
	printf("profile vel/acc   %d / %d (0/0 = LOW LAG)\n",
		Rd(DXL_REG_PROF_VEL, 2), Rd(DXL_REG_PROF_ACC, 2));
	printf("present pos       %d\n", Rd(DXL_REG_PRES_POS, 2));
	printf("present volt/temp %.1fV / %dC\n",
		Rd(DXL_REG_PRES_VOLT, 1) * 0.1, Rd(DXL_REG_PRES_TEMP, 0));
	printf("------------------------------------\n\n");
}

main()
{
	int v, r, wrote = 0, err = 0, savedticks, any_eeprom;

	ServoProtocol = 1;
	ServoBaud576 = 0;

	// report WHY the probe failed.  On a multi drop bus silence and garbage
	// have completely different causes: silence means nobody transmitted (the
	// servos never heard us, or are not running), while any received-but-bad
	// reply means something IS driving the wire - typically two servos
	// answering at once.  Result codes are set in fast.c SVP_WAIT/SVP_READBACK.

	v = 0;
	r = DxlReg(SERVO, DXL_REG_MODEL, &v, 1, 1);
	if (r)
	{
		printf("no answer at ID %d - result %d: %s\n", SERVO, r,
			r == 1 ? "SILENCE, not one byte received" :
			r == 2 ? "PARTIAL reply - collision or marginal signal" :
			r == 3 ? "FRAMING error - signal integrity or wrong baud" :
			r == 4 ? "reply had the wrong ID or header" :
			r == 5 ? "CRC error - two servos answering, or corruption" :
			r == 6 ? "short reply - the servo rejected the read" :
			         "the DSP engine never serviced the request");
		return;
	}

	Dump();

	if (SET_POS_P<0 && SET_POS_I<0 && SET_POS_D<0 && SET_FF1_VEL<0 &&
		SET_FF2_ACC<0 && SET_PROF_VEL<0 && SET_PROF_ACC<0 && SET_RETDELAY<0 &&
		SET_OPMODE<0 && SET_DRIVEMODE<0 && SET_HOMING==(int)0x80000000 &&
		SET_TEMP_LIM<0 && SET_PWM_LIM<0 && SET_SHUTDOWN<0)
	{
		printf("(all SET_ values are -1 - dump only, nothing written)\n");
		return;
	}

	// EEPROM registers (address < 64) are rejected with torque on, so only
	// EEPROM writes need the torque-off + quiet-bus window.  RAM-only
	// tuning (PID/FF/profiles) can run on a live, enabled axis.

	any_eeprom = (SET_RETDELAY>=0 || SET_DRIVEMODE>=0 || SET_OPMODE>=0 ||
		SET_HOMING != (int)0x80000000 || SET_TEMP_LIM>=0 || SET_PWM_LIM>=0 ||
		SET_SHUTDOWN>=0);

	savedticks = ServoCycleTicks;

	if (any_eeprom)
	{
		ServoCycleTicks = 20000;
		Delay_sec(0.1);

		printf("writing (torque off, bus cycles paused)...\n");
		v = 0; err |= DxlReg(SERVO, DXL_REG_TORQUE, &v, 0, 0);
		Delay_sec(0.02);
	}
	else
		printf("writing (RAM registers only - axis may stay enabled)...\n");

	if (SET_RETDELAY>=0)  { err |= Wr(DXL_REG_RETDELAY, SET_RETDELAY, 0); wrote++; }
	if (SET_DRIVEMODE>=0) { err |= Wr(DXL_REG_DRIVEMODE, SET_DRIVEMODE, 0); wrote++; }
	if (SET_OPMODE>=0)    { err |= Wr(DXL_REG_OPMODE, SET_OPMODE, 0); wrote++; }
	if (SET_HOMING != (int)0x80000000)
	                      { err |= Wr(DXL_REG_HOMING, SET_HOMING, 2); wrote++; }
	if (SET_TEMP_LIM>=0)  { err |= Wr(DXL_REG_TEMP_LIM, SET_TEMP_LIM, 0); wrote++; }
	if (SET_PWM_LIM>=0)   { err |= Wr(DXL_REG_PWM_LIM, SET_PWM_LIM, 1); wrote++; }
	if (SET_SHUTDOWN>=0)  { err |= Wr(DXL_REG_SHUTDOWN, SET_SHUTDOWN, 0); wrote++; }

	// RAM registers (no torque restriction, but harmless in the window)

	if (SET_POS_P>=0)     { err |= Wr(DXL_REG_POS_P, SET_POS_P, 1); wrote++; }
	if (SET_POS_I>=0)     { err |= Wr(DXL_REG_POS_I, SET_POS_I, 1); wrote++; }
	if (SET_POS_D>=0)     { err |= Wr(DXL_REG_POS_D, SET_POS_D, 1); wrote++; }
	if (SET_FF1_VEL>=0)   { err |= Wr(DXL_REG_FF1, SET_FF1_VEL, 1); wrote++; }
	if (SET_FF2_ACC>=0)   { err |= Wr(DXL_REG_FF2, SET_FF2_ACC, 1); wrote++; }
	if (SET_PROF_VEL>=0)  { err |= Wr(DXL_REG_PROF_VEL, SET_PROF_VEL, 2); wrote++; }
	if (SET_PROF_ACC>=0)  { err |= Wr(DXL_REG_PROF_ACC, SET_PROF_ACC, 2); wrote++; }

	ServoCycleTicks = savedticks;

	printf("%d register(s) written%s\n", wrote, err ? " WITH ERRORS" : "");

	Dump();

	if (any_eeprom)
		printf("NOTE: torque was turned OFF - re-run DxlAxisInit.c before moving.\n");
	printf("RAM registers (PID, FF, profiles) reset at servo power up.\n");
}
