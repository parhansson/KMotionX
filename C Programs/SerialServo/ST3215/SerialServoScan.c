// SerialServoScan.c
//
// Find every servo on the bus: probes all 254 IDs with a read of the model
// register and reports which IDs answer.  Use when a servo has been re-ID'd
// to something unknown (e.g. by an earlier buggy setup run) or to verify a
// chain.  Takes a few seconds.
//
// Also distinguishes the three failure classes SetID cannot:
//   "engine never serviced" - the DSP protocol engine is not running the
//                             request at all (firmware issue, report it)
//   silent ID               - nothing received (normal for an unused ID)
//   error result            - something answered but garbled (electrical /
//                             collision clue - reported per ID)

#include "KMotionDef.h"

#define STS_REG_MODEL 0x03

int ServoReg(int id, int reg, int *data, int two, int rd)
{
	int t;

	ServoAuxID = id;
	ServoAuxReg = reg;
	ServoAuxData = *data;
	ServoAuxTwo = two;
	ServoAuxRead = rd;
	ServoAuxGo = 1;

	for (t = 0; t < 400 && ServoAuxGo; t++) Delay_sec(0.0005);
	if (ServoAuxGo) { ServoAuxGo = 0; return -2; }

	if (rd) *data = ServoAuxData;
	return ServoAuxResult;
}

main()
{
	int id, v, r, found, garbled;

	// Feetech STS control table only - see the note in SerialServoAxisInit.c
	if (ServoProtocol)
	{
		printf("ServoProtocol=1 (Dynamixel) - use DxlSetup.c to find servos\n");
		return;
	}

	// first prove the engine services requests at all

	v = 0;
	r = ServoReg(0, STS_REG_MODEL, &v, 0, 1);

	if (r == -2)
	{
		printf("FAIL - the DSP protocol engine never serviced the request.\n");
		printf("The flashed firmware is missing the serial servo engine - rebuild\n");
		printf("the DSP from current C:\\KMotionIP-dev\\DSP_KOGNA sources.\n");
		return;
	}

	printf("engine OK - scanning IDs 0-253 (bus on %s)...\n",
		ServoBusEXIO ? "EX_IO_13" : "IO0");

	found = 0;
	garbled = 0;

	for (id = 0; id < 254; id++)
	{
		v = 0;
		r = ServoReg(id, STS_REG_MODEL, &v, 0, 1);

		if (r == 0)
		{
			printf("  ID %3d ANSWERS (model register = %d)\n", id, v);
			found++;
		}
		else if (r != 1)
		{
			// something came back but did not validate - worth knowing
			printf("  ID %3d garbled response (result %d)\n", id, r);
			garbled++;
		}
	}

	if (found == 0 && garbled == 0)
	{
		printf("no servo answered any ID.\n");
		printf("The request bursts should be visible on the scope (~8 bytes every\n");
		printf("few ms during the scan).  If bursts are present but nothing ever\n");
		printf("replies: servo power, ground, or signal wiring.  Run\n");
		printf("SerialServoDiag.c to test the raw FPGA path independently.\n");
	}
	else
	{
		printf("scan complete: %d servo(s) found, %d garbled.\n", found, garbled);
		printf("Use SerialServoSetID.c with FROM_ID = the ID found above.\n");
	}
}
