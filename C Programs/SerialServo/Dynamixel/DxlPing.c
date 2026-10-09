// DxlPing.c
//
// Bus level diagnostic for the Dynamixel engine.  Blinks each servo's LED
// (register 65) instead of reading a register, which answers a question the
// bus itself cannot: did the servo HEAR us?
//
// When a read returns "SILENCE, not one byte received" there are three
// indistinguishable causes - the servo never heard the request, it heard it
// and would not reply, or it replied and we never heard that.  The LED is
// outside the bus, so it separates them with your eyes:
//
//   LED blinks  -> the servo heard the packet.  Transmit path and the servo
//                  are both fine, so the problem is purely the REPLY path.
//   no blink    -> the servo never received it.  The problem is the transmit
//                  path: wiring, contacts, levels, or the wrong baud rate.
//
// NOTE: do not trust the printed result code here.  fast.c reports an
// addressed WRITE that draws no reply as SUCCESS by design, because a servo
// with Status Return Level 0 legitimately stays quiet.  The LED is the real
// answer - the same lesson as the torque register that "succeeded" while
// writing to the wrong address.
//
// Each ID is tried at 1 Mbaud and then at 57600, so a servo left at the
// factory baud rate (invisible to everything else at 1 Mbaud) still shows up.

#include "KMotionDef.h"

#define FIRST_ID  0        // range of bus IDs to ping
#define LAST_ID   3
#define BLINKS    4        // LED on/off cycles per servo
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

// blink one ID's LED, then report whether a read of that ID also answers
void PingID(int id)
{
	int i, v, r;

	for (i = 0; i < BLINKS; i++)
	{
		v = 1; DxlReg(id, DXL_REG_LED, &v, 0, 0);
		Delay_sec(0.2);
		v = 0; DxlReg(id, DXL_REG_LED, &v, 0, 0);
		Delay_sec(0.2);
	}

	// now see whether the same servo can be READ - this is the reply path
	v = 0;
	r = DxlReg(id, DXL_REG_MODEL, &v, 1, 1);

	if (r == 0)
		printf("  ID %d: read OK, model %d - fully working\n", id, v);
	else
		printf("  ID %d: read failed (result %d) - did its LED blink?\n", id, r);
}

main()
{
	int id;

	ServoProtocol = 1;
	ServoBusEXIO = USE_EXIO;

	printf("WATCH THE SERVO LEDs.  A blink means that servo received the\n");
	printf("packet even if it never replied.  Ignore write result codes.\n");

	printf("\n---- 1 Mbaud ----\n");
	ServoBaud576 = 0;
	Delay_sec(0.05);
	for (id = FIRST_ID; id <= LAST_ID; id++) PingID(id);

	printf("\n---- 57600 (factory baud) ----\n");
	ServoBaud576 = 1;
	Delay_sec(0.05);
	for (id = FIRST_ID; id <= LAST_ID; id++) PingID(id);

	ServoBaud576 = 0;                 // leave the bus at the normal rate
	Delay_sec(0.05);

	printf("\nblinked but never readable  -> reply path (contacts, turnaround)\n");
	printf("blinked only at 57600       -> that servo is at the factory baud\n");
	printf("never blinked at either     -> transmit path never reaches it\n");
}
