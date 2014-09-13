#include "KMotionDef.h"

// Setup to collect (gather) data on two PWM channels
//     and the commanded destination of axis 0
// Trigger the gather
// Perform a small Move
// Wait for data gather complete
// Print the data to the console (every tenth point)

main()
{
	int i,n_Samples = 0.5 / TIMEBASE;
	
	gather.Inject = FALSE;	// Don't inject any Data anywhere

	gather.list[0].type = GATHER_LASTPWM_TYPE;  // Gather PWM 0
	gather.list[0].addr = &LastPWM[0];

	gather.list[1].type = GATHER_LASTPWM_TYPE; // Gather PWM 1
	gather.list[1].addr = &LastPWM[1];

	gather.list[2].type = GATHER_DOUBLE_TYPE; // Gather Dest axis 0
	gather.list[2].addr = &chan[0].Dest;

	gather.list[3].type = GATHER_END_TYPE;  
	
    gather.bufptr = (double *)0xfffffffc;   // force more than endbuf
    gather.endptr = gather_buffer + 3 * n_Samples;

	TriggerGather();  // start capturing data
	
	MoveRel(0,20); // Start a motion
	
	while (!CheckDoneGather()) ; // what till all captured
	
	// print all captured data (every 50th sample)
	
	for (i=0; i<n_Samples; i+=10)
		printf("%d,%f,%f,%f\n", i,gather_buffer[i*3],
						   gather_buffer[i*3+1],
						   gather_buffer[i*3+2]); 

}

