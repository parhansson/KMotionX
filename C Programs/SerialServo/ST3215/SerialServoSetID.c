// SerialServoSetID.c
//
// ONE-TIME setup: assign a servo its ID on the daisy chained bus.
//
// The axis modes use the servo ID (0-7) as the Input/Output Channel, so every
// servo on the chain needs a unique ID.  Factory servos ship as ID 1 - chain
// them one at a time, re-ID each new servo, then add the next.  Repeat this
// program with TO_ID = 0, 1, 2, ... as each servo is added.
//
// Uses the DSP protocol engine's aux mailbox, which runs whenever a bus pin
// is pending (bus on IO0, or EX_IO_13 if ServoBusEXIO=1) - no axis setup needed.
//
// Writes EEPROM - run once per servo, not in production code.

#include "KMotionDef.h"

#define FROM_ID  1        // the servo's current ID (factory = 1)
#define TO_ID    0        // the ID to assign (= its axis channel number)

// one servo control table access through the DSP protocol engine.
// returns 0 = OK, else the transaction result code, -2 = engine not running
int ServoReg(int id, int reg, int *data, int two, int rd)
{
	int t;

	ServoAuxID = id;
	ServoAuxReg = reg;
	ServoAuxData = *data;
	ServoAuxTwo = two;
	ServoAuxRead = rd;
	ServoAuxGo = 1;

	for (t = 0; t < 200 && ServoAuxGo; t++) Delay_sec(0.001);
	if (ServoAuxGo) { ServoAuxGo = 0; return -2; }

	if (rd) *data = ServoAuxData;
	return ServoAuxResult;
}

int ServoWrite(int id, int reg, int data) { return ServoReg(id, reg, &data, 0, 0); }

main()
{
	int v;

	// Feetech STS control table only.  On a Dynamixel these register numbers
	// land inside Homing Offset / Moving Threshold EEPROM - do not write them.
	if (ServoProtocol)
	{
		printf("ServoProtocol=1 (Dynamixel) - use DxlSetup.c instead\n");
		return;
	}

	printf("re-assigning servo ID %d -> %d\n", FROM_ID, TO_ID);

	// verify the source servo answers before touching EEPROM
	if (ServoReg(FROM_ID, STS_REG_ID, &v, 0, 1))
	{
		printf("no servo answers at ID %d - check wiring/power/baud\n", FROM_ID);
		return;
	}

	if (ServoWrite(FROM_ID, STS_REG_LOCK, 0))     { printf("unlock failed\n"); return; }
	Delay_sec(0.05);
	if (ServoWrite(FROM_ID, STS_REG_ID, TO_ID))   { printf("ID write failed\n"); return; }
	Delay_sec(0.05);

	// the servo answers on its new ID from here on
	if (ServoWrite(TO_ID, STS_REG_RETDELAY, 0))   { printf("return delay write failed\n"); return; }
	Delay_sec(0.05);
	if (ServoWrite(TO_ID, STS_REG_LOCK, 1))       { printf("lock failed\n"); return; }
	Delay_sec(0.05);

	// confirm by reading the ID register back from the new ID
	if (ServoReg(TO_ID, STS_REG_ID, &v, 0, 1) || v != TO_ID)
	{
		printf("verification failed (read %d) - power cycle and re-check\n", v);
		return;
	}

	printf("SUCCESS - servo now ID %d.  Power cycle the servo to confirm it stuck.\n", TO_ID);
}
