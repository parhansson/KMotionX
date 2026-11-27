#include "KMotionDef.h"

// Capture the commanded Destination AND measured Position of axis
// channels 0-2 on every servo sample into the gather buffer, then write
// them to a CSV file on the PC.  Works on KFLOP and Kogna.  Open the
// file in MotionLogPlotter: the Position strip plots Dest and Position
// together and the bottom strip plots their difference (Dest - Pos),
// for any of the three axes.
//
// Launch from a spare thread (or an M-code) just before the motion to
// diagnose.  Each sample is 7 doubles: the gather buffer holds
// 1,000,000 doubles on KFLOP (142,000 samples) and 7,000,000 on Kogna
// (1,000,000 samples).
//
// Two ways to get the file (FAST_FILE below):
//   1  the DSP writes it with fputs + FormatFixed (~5000 lines/sec).  The
//      old fprintf("%12.4f"...) version managed ~1500: TI's printf costs
//      ~20us of DSP time per converted value.
//   0  the DSP only captures; the PC uploads the raw gather buffer at
//      several MB/s and formats it.  After "Capture done" run, from the
//      KMotion\Release folder:
//        GatherToCSV C:\Temp\DestPosLog.csv 7 60000 0 6 --header time,Pos0,Pos1,Pos2,Dest0,Dest1,Dest2
//      (7 columns, N rows; adjust N to match below)
//
// File format (accepted directly by MotionLogPlotter):
//   time, Pos0, Pos1, Pos2, Dest0, Dest1, Dest2

#define N 60000     // samples to capture (10.8 s at DECIM=1)
#define DECIM 1     // servo samples per capture (1 = every 180us)
#define FAST_FILE 1 // 1 = DSP writes the CSV, 0 = capture only, upload with GatherToCSV

int main()
{
	int i,k,n;
	double T0,*p=gather_buffer;
	char line[256];

	T0 = Time_sec();

	// Capture Data

	for (i=0; i<N; i++)
	{
		for (k=0; k<DECIM; k++) WaitNextTimeSlice();

		*p++ = Time_sec() - T0;
		*p++ = ch0->Position;
		*p++ = ch1->Position;
		*p++ = ch2->Position;
		*p++ = ch0->Dest;
		*p++ = ch1->Dest;
		*p++ = ch2->Dest;
	}

	// round times to nearest servo tick
	p=gather_buffer;
	for (i=0; i<N; i++, p+=7)
		p[0] = ((int)(p[0]/TIMEBASE + 0.5))*TIMEBASE;

#if FAST_FILE
	printf("Capture done - writing C:\\Temp\\DestPosLog.csv\n");

	p=gather_buffer;
	FILE *f=fopen("C:\\Temp\\DestPosLog.csv","wt");
	for (i=0; i<N; i++)
	{
		n  = FormatFixed(line, p[0], 6);
		for (k=1; k<7; k++)
		{
			line[n++] = ',';
			n += FormatFixed(line+n, p[k], 4);
		}
		line[n++] = '\n';
		line[n] = 0;
		fputs(line, f);
		p += 7;
	}
	fclose(f);
	printf("DestPosLog.csv written (%d samples)\n", N);
#else
	printf("Capture done - %d samples x 7 in the gather buffer.  Upload with:\n", N);
	printf("GatherToCSV C:\\Temp\\DestPosLog.csv 7 %d 0 6 --header time,Pos0,Pos1,Pos2,Dest0,Dest1,Dest2\n", N);
#endif
}
