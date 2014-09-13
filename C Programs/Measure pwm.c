#include "KMotionDef.h"

// Test and Measure Digital I/O bit PWM feature

main()
{
	int i,n=1000;
	double t0,t1;

	SetBitDirection(0,1);  // define bit as an output

	FPGA[IO_PWMS_PRESCALE] = 255;  	// divide clock by 256 (slowest)
	FPGA[IO_PWMS] = 128;  			// square wave
	FPGA[IO_PWMS+1] = 0;  			// I/O bit number
	
	t0 = Time_sec();
	
	for (i=0;i<n;i++)  // count n cycles
	{
		while (ReadBit(0)) ;  // wait for high
		while (!ReadBit(0)) ; // wait for low
	}
	t1 = Time_sec();

	printf("Time period = %8.2f us\n",(t1-t0)*1e6/n);
	printf("FPGA Clock Frequency =%6.2f MHz\n",1.0/((t1-t0)/n)*256*256*1e-6);
}
