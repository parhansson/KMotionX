// SerialServoDiag.c
//
// Layered diagnostic for the daisy chained serial servo bus.  Tests each
// layer independently and reports exactly where the path breaks:
//
//   1  FPGA identity  - is the NEW byte stream bitstream actually loaded?
//                       (writes a pattern to the TX buffer and reads it back;
//                        the old per-pin bitstream has no memory there)
//   2  raw PING       - hand assembled 6 byte PING to ID 0 through the FPGA
//                       UART directly; dumps status and response bytes
//   3  raw telemetry  - hand assembled 21 byte telemetry read, checksum
//                       verified in C, position printed.  Response read back
//                       BOTH with single FPGAW reads and a FPGA64 burst and
//                       compared - a mismatch means a bus read timing issue
//   4  engine test    - configures axis 0 modes so the DSP protocol engine
//                       claims the bus, then reports ServoPosValid/Result/
//                       Errors so engine level failures are visible
//
// Run this INSTEAD of the axis init (it clears any serial servo axis modes
// first so the DSP engine releases the bus for the raw tests).

#include "KMotionDef.h"

#define SERVO 0          // servo ID to test (0-7)
#define USE_EXIO 0       // 0 = bus on IO0, 1 = bus on EX_IO_13

int WaitCount(int expect, double tmax)
{
	int st;
	double t0 = Time_sec();

	for (;;)
	{
		st = FPGAW(SERVO_TXGO_ADD);
		if ((st & SERVO_ST_COUNT) >= expect && !(st & SERVO_ST_BUSY)) return st;
		if (Time_sec() - t0 > tmax) return st;
		Delay_sec(0.0002);
	}
}

void DumpRx(int n)
{
	int i, w;

	for (i = 0; i < n; i += 2)
	{
		w = FPGAW(SERVO_RXBUF_ADD + i/2);
		printf(" %02X", w & 0xff);
		if (i+1 < n) printf(" %02X", (w>>8) & 0xff);
	}
	printf("\n");
}

main()
{
	int i, st, w, sum, pos, bad;
	union { double d[4]; unsigned char b[32]; } burst;

	// make sure the DSP protocol engine releases the bus

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

	// ---- 1: FPGA identity - TX buffer memory readback ----
	//
	// four reads distinguish three cases:
	//   A5C3 3C5A A5C3 3C5A  - good
	//   xxxx A5C3 3C5A A5C3  - RAM present but reads lag the address by one
	//                          access: the pwms.vhd buffer read path fix is
	//                          not in this FPGA build
	//   anything else        - new bitstream not loaded

	{
		int r1, r2, r3, r4;

		FPGAW(SERVO_TXBUF_ADD)   = 0xA5C3;
		FPGAW(SERVO_TXBUF_ADD+1) = 0x3C5A;
		r1 = FPGAW(SERVO_TXBUF_ADD)   & 0xffff;
		r2 = FPGAW(SERVO_TXBUF_ADD+1) & 0xffff;
		r3 = FPGAW(SERVO_TXBUF_ADD)   & 0xffff;
		r4 = FPGAW(SERVO_TXBUF_ADD+1) & 0xffff;

		printf("1: TX buffer readback %04X %04X %04X %04X  (expect A5C3 3C5A A5C3 3C5A)\n",
			r1, r2, r3, r4);

		// judge on reads 2-4: the very first read after a write may return
		// the write-latched RAM output (a first access artifact the engine
		// never encounters, since it never reads a region it just wrote)

		if (r2 == 0x3C5A && r3 == 0xA5C3 && r4 == 0x3C5A)
		{
			// good
		}
		else if (r2 == 0xA5C3 && r3 == 0x3C5A && r4 == 0xA5C3)
		{
			printf("   FAIL - buffer RAM present but reads return the PREVIOUS\n");
			printf("   address's data.  Rebuild the FPGA from the current pwms.vhd\n");
			printf("   (it contains the buffer read path fix) and retry.\n");
			return;
		}
		else
		{
			printf("   FAIL - the new serial servo FPGA bitstream is NOT loaded.\n");
			printf("   Rebuild/flash the FPGA (sts_uart.vhd design) and retry.\n");
			return;
		}
	}

	printf("   status = %04X (expect 0000)\n", FPGAW(SERVO_TXGO_ADD) & 0xffff);

	// ---- 2: raw PING to the servo ----

	// FF FF ID 02 01 CHK   with CHK = ~(ID+02+01)
	FPGAW(SERVO_TXBUF_ADD)   = 0xFFFF;
	FPGAW(SERVO_TXBUF_ADD+1) = 0x0200 | SERVO;
	FPGAW(SERVO_TXBUF_ADD+2) = ((~(SERVO+2+1) & 0xff) << 8) | 0x01;
	FPGAW(SERVO_TXGO_ADD) = 6;

	st = WaitCount(6, 0.02);
	printf("2: PING  status=%04X count=%d busy=%d ferr=%d\n",
		st & 0xffff, st & SERVO_ST_COUNT, (st & SERVO_ST_BUSY) ? 1:0,
		(st & SERVO_ST_FERR) ? 1:0);

	if ((st & SERVO_ST_COUNT) < 6)
	{
		printf("   FAIL - no PING reply.  The request should be visible on %s\n",
			USE_EXIO ? "EX_IO_13" : "IO0");
		printf("   (6 bytes, ~60us burst).  If the burst is there but no reply:\n");
		printf("   servo power / ID / wiring.  If no burst: FPGA pin path.\n");
		if (st & SERVO_ST_COUNT) { printf("   partial rx:"); DumpRx(st & SERVO_ST_COUNT); }
		return;
	}

	printf("   reply:"); DumpRx(6);

	// ---- 3: raw telemetry read, dual readback ----

	FPGAW(SERVO_TXBUF_ADD)   = 0xFFFF;
	FPGAW(SERVO_TXBUF_ADD+1) = 0x0400 | SERVO;
	FPGAW(SERVO_TXBUF_ADD+2) = 0x3802;
	FPGAW(SERVO_TXBUF_ADD+3) = ((~(SERVO+4+2+0x38+15) & 0xff) << 8) | 0x0F;
	FPGAW(SERVO_TXGO_ADD) = 8;

	st = WaitCount(21, 0.02);
	printf("3: TELEM status=%04X count=%d\n", st & 0xffff, st & SERVO_ST_COUNT);

	if ((st & SERVO_ST_COUNT) < 21)
	{
		printf("   FAIL - PING works but the 21 byte telemetry reply did not arrive\n");
		if (st & SERVO_ST_COUNT) { printf("   partial rx:"); DumpRx(st & SERVO_ST_COUNT); }
		return;
	}

	printf("   reply:"); DumpRx(21);

	// checksum in C from single word reads
	sum = 0; bad = 0;
	for (i = 2; i < 20; i++)
	{
		w = FPGAW(SERVO_RXBUF_ADD + i/2);
		sum += (i & 1) ? (w>>8) & 0xff : w & 0xff;
	}
	w = FPGAW(SERVO_RXBUF_ADD + 10);   // byte 20 = checksum
	if (((~sum) & 0xff) != (w & 0xff))
	{
		printf("   FAIL - checksum bad (calc %02X got %02X)\n", (~sum)&0xff, w&0xff);
		bad = 1;
	}

	// compare a FPGA64 burst readback against the single word reads
	for (i = 0; i < 3; i++) burst.d[i] = FPGA64(SERVO_RXBUF_ADD + i*4);
	for (i = 0; i < 21; i++)
	{
		w = FPGAW(SERVO_RXBUF_ADD + i/2);
		w = (i & 1) ? (w>>8) & 0xff : w & 0xff;
		if (burst.b[i] != w)
		{
			printf("   FAIL - FPGA64 burst read differs at byte %d (%02X vs %02X)\n",
				i, burst.b[i], w);
			printf("   -> bus burst read timing issue in the FPGA buffer read path\n");
			bad = 1;
			break;
		}
	}

	if (!bad)
	{
		w = FPGAW(SERVO_RXBUF_ADD + 2);          // bytes 5,4 -> posL in high? byte5 posL
		pos = ((FPGAW(SERVO_RXBUF_ADD + 3) & 0xff) << 8) | ((w >> 8) & 0xff);
		if (pos & 0x8000) pos = -(pos & 0x7fff);
		printf("   raw path GOOD - servo position %d counts\n", pos);
	}

	// ---- 4: DSP protocol engine test ----

	printf("4: engine test - claiming the bus via axis 0 modes\n");

	ch0->InputMode  = SERIAL_SERVO_INPUT_MODE;
	ch0->InputChan0 = SERVO;
	ch0->InputGain0 = 1.0;

	Delay_sec(0.2);

	printf("   PosValid=%d Position=%d result=%d errors=%d stale=%d\n",
		ServoPosValid[SERVO], ServoPosition[SERVO], ServoResult[SERVO],
		ServoErrors[SERVO] & 0xff, ServoStale[SERVO]);
	printf("   volts=%.1f temp=%dC status=%04X\n",
		SERVO_CONVERT_TO_VOLTS(ServoVolts[SERVO]), ServoTemp[SERVO],
		ServoStatus[SERVO] & 0xffff);

	if (ServoPosValid[SERVO])
		printf("   engine GOOD - everything works, axis init should succeed now\n");
	else
		printf("   engine FAIL with raw path good - report result code above\n");

	ch0->InputMode = NO_INPUT_MODE;   // release again
}
