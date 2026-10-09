// DxlSetup.c
//
// One-time setup of a Dynamixel XL430 (or other Protocol 2.0 X-series servo)
// for the Kogna serial servo bus.  Connect ONE new servo at a time.
//
// Factory servos talk 57600 baud with ID 1.  This program:
//   1  finds the servo (tries 1 Mbaud first, then 57600 via the FPGA's
//      baud select) and reports its model and firmware
//   2  sets Baud Rate = 1 Mbaud (EEPROM - torque must be off, which is the
//      power up state)
//   3  sets the ID to TO_ID, Return Delay = 0, Operating Mode = 4 (Extended
//      Position, +/-256 turns - matches the multi turn axis integration)
//   4  sets Profile Velocity = 0 and Profile Acceleration = 0: the LOW LAG
//      mode.  The servo's internal trajectory generator is disabled and
//      every Goal Position is acted on immediately, so the servo tracks the
//      Kogna's 1 kHz command stream directly.
//   5  verifies everything by reading back at 1 Mbaud
//
// Requires ServoProtocol = 1 (set here).  Run with no serial servo axes
// enabled.  IMPORTANT: verify the servo is an XL430 class device (6.5-12V).
// The XL330 looks identical in software but is damaged above 6V!

#include "KMotionDef.h"

#define TO_ID     2        // bus ID to assign (0-7 = axis channel number)
#define USE_EXIO  0        // 0 = bus on IO0, 1 = bus on EX_IO_13

// one aux transaction; size: 0=1 byte, 1=2 bytes, 2=4 bytes
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

int Rd(int id, int reg, int size)
{
	int v = 0;
	if (DxlReg(id, reg, &v, size, 1)) return -1;
	return v;
}

// write with readback verify; EEPROM quiet handled by the engine
int Wr(int id, int reg, int val, int size)
{
	int r, v;
	int local_val = val;

	r = DxlReg(id, reg, &local_val, size, 0);
	if (r) { printf("  WRITE reg %d FAILED (result %d)\n", reg, r); return 1; }
	if (ServoAuxErr > 0)
		printf("  reg %d write: servo error byte 0x%02X\n", reg, ServoAuxErr);
	Delay_sec(0.05);

	v = Rd(id, reg, size);
	if (v != local_val)
	{
		printf("  VERIFY reg %d failed (wrote %d read %d)\n", reg, local_val, v);
		return 1;
	}
	printf("  reg %d = %d OK\n", reg, local_val);
	return 0;
}

// look for any servo: returns its current ID or -1
int FindServo(void)
{
	int id, v;

	for (id = 0; id < 253; id++)
	{
		v = Rd(id, DXL_REG_MODEL, 1);
		if (v >= 0)
		{
			printf("  found ID %d: model %d firmware %d\n",
				id, v, Rd(id, DXL_REG_FIRMWARE, 0));
			return id;
		}
	}
	return -1;
}

main()
{
	int id, v, err = 0;

	ServoProtocol = 1;
	ServoBusEXIO = USE_EXIO;

	printf("scanning at 1 Mbaud...\n");
	ServoBaud576 = 0;
	Delay_sec(0.05);
	id = FindServo();

	if (id < 0)
	{
		printf("scanning at 57600 (factory rate)...\n");
		ServoBaud576 = 1;
		Delay_sec(0.05);
		id = FindServo();

		if (id < 0)
		{
			ServoBaud576 = 0;
			printf("no servo answers at either rate - check power/wiring\n");
			return;
		}

		// torque off first: EEPROM writes are rejected with torque on, and
		// torque stays latched on from any earlier axis session

		v = 0; DxlReg(id, DXL_REG_TORQUE, &v, 0, 0);
		Delay_sec(0.02);

		// move it to 1 Mbaud.  The servo applies a baud change IMMEDIATELY
		// after acking, so no readback verify is possible at the old rate -
		// send the raw write, switch our side, and verify at 1 Mbaud.

		printf("setting baud to 1 Mbaud...\n");
		v = 3;
		DxlReg(id, DXL_REG_BAUD, &v, 0, 0);   // result unchecked - the ack
		                                      // may already be unreadable
		Delay_sec(0.1);

		ServoBaud576 = 0;
		Delay_sec(0.05);

		if (Rd(id, DXL_REG_MODEL, 1) < 0 || Rd(id, DXL_REG_BAUD, 0) != 3)
		{
			printf("servo does not answer at 1 Mbaud after baud change\n");
			return;
		}
		printf("  servo answering at 1 Mbaud\n");
	}

	// torque off (RAM register, writable any time): the EEPROM writes below
	// are rejected with torque on, and torque stays latched on from any
	// earlier axis session

	v = 0; DxlReg(id, DXL_REG_TORQUE, &v, 0, 0);
	Delay_sec(0.02);

	if (id != TO_ID)
	{
		// the ID also applies immediately - write raw, verify at the NEW ID

		printf("setting ID %d -> %d...\n", id, TO_ID);
		v = TO_ID;
		DxlReg(id, DXL_REG_ID, &v, 0, 0);
		Delay_sec(0.1);
		id = TO_ID;

		if (Rd(id, DXL_REG_ID, 0) != TO_ID)
		{
			printf("servo does not answer at new ID %d\n", TO_ID);
			return;
		}
		printf("  ID %d verified\n", TO_ID);
	}

	printf("configuring...\n");
	err |= Wr(id, DXL_REG_RETDELAY, 0, 0);   // no response delay
	err |= Wr(id, DXL_REG_OPMODE,   4, 0);   // extended position (multi turn)
	err |= Wr(id, DXL_REG_PROF_ACC, 0, 2);   // no accel profile  - the
	err |= Wr(id, DXL_REG_PROF_VEL, 0, 2);   // no velocity profile - LOW LAG

	printf("\nmodel %d firmware %d at ID %d, 1 Mbaud\n",
		Rd(id, DXL_REG_MODEL, 1), Rd(id, DXL_REG_FIRMWARE, 0), id);
	printf("voltage %.1fV  temp %dC  hardware error 0x%02X\n",
		Rd(id, DXL_REG_PRES_VOLT, 1) * 0.1,
		Rd(id, DXL_REG_PRES_TEMP, 0),
		Rd(id, DXL_REG_HW_ERROR, 0));

	if (err) printf("** COMPLETED WITH ERRORS - review above **\n");
	else     printf("setup complete - torque is OFF, use DxlAxisInit.c to run an axis\n");
}
