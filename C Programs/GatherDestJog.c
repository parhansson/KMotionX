#include "KMotionDef.h"

// Setup to collect (gather) data on two PWM channels
//     and the commanded destination of axis 0
// Trigger the gather
// Perform a small Move
// Wait for data gather complete
// Print the data to the console (every tenth point)

main()
{
	int i,n_Samples = 0.005 / TIMEBASE;
	
	gather.Inject = FALSE;	// Don't inject any Data anywhere

	gather.list[0].type = GATHER_DOUBLE_TYPE; // Gather Dest
	gather.list[0].addr = &chan[4].Dest;

	gather.list[1].type = GATHER_DOUBLE_TYPE; // Gather Pos
	gather.list[1].addr = &chan[4].Position;

	gather.list[2].type = GATHER_FLOAT_TYPE ; // Gather Output
	gather.list[2].addr = &chan[4].Output;

	gather.list[3].type = GATHER_FLOAT_TYPE ; // Gather Output
	gather.list[3].addr = &chan[4].last_vel;

	gather.list[4].type = GATHER_END_TYPE;  
	
    gather.bufptr = (double *)0xfffffffc;   // force more than endbuf
    gather.endptr = gather_buffer + 4 * n_Samples;

	Jog(4,250000);
	Delay_sec(1);
	TriggerGather();  // start capturing data
	Delay_sec(0.002);
	Jog(4,-250000);
	while (!CheckDoneGather()) ; // what till all captured
	Jog(4,0);
	
	// print all captured data (every 50th sample)
	
	for (i=0; i<n_Samples; i+=1)
		printf("%d,%f,%f,%f,%f\n", i,gather_buffer[i*4],gather_buffer[i*4+1],
			gather_buffer[i*4+2],gather_buffer[i*4+3]); 

}

