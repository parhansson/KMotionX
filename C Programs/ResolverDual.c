#include "KMotionDef.h"

// Two Axis Resolver Program  1/6/08
//
// outputs square wave to both resolvers using a PWM
//
// then samples output coils near positive and negative peaks
// takes the difference to compute magnitudes
//

// these ratios are used to match the amplitudes of sine:cosine
#define RATIO0 (978.0f/768.0f)  // size j/size k
#define RATIO1 (950.0f/709.0f)  // size n/size m

main()
{
	int i=0;
	int k0,j0,k1,j1;
	int m0,n0,m1,n1;
	
	SetBit(28);  //+/-15V on
	SetBitDirection(30,1);  		// configure AUX switch as output
	DefineCoordSystem(0,1,-1,-1);	// Define 2 axis system
	
	Delay_sec(0.1);					// wait for +/- 15V to be stable

	for (;;)					// repeat forever
	{
		WaitNextTimeSlice();	// wait a few servo cycles
		WaitNextTimeSlice();
		Delay_sec(10e-6); 		// wait for ADC conversion to complete
		k0=ADC(0);				// Sample all the ADCs
		j0=ADC(1);
		m0=ADC(2);
		n0=ADC(3);
		SetBit(30);				// Switch the resolver excitation
		
		//compute & track position based on measured magnitudes
		DoResolverInput2(ch0,(k1-k0)*RATIO0,j1-j0);  
		DoResolverInput2(ch1,(m1-m0)*RATIO1,n1-n0);
		
		WaitNextTimeSlice(); 	// wait a few servo cycles
		WaitNextTimeSlice();
		Delay_sec(10e-6); 		// wait for ADC conversion to complete
		k1=ADC(0);				// Sample all the ADCs
		j1=ADC(1);
		m1=ADC(2);
		n1=ADC(3);
		ClearBit(30);			// Switch the resolver excitation
		
		//compute & track position based on measured magnitudes
		DoResolverInput2(ch0,(k1-k0)*RATIO0,j1-j0);
		DoResolverInput2(ch1,(m1-m0)*RATIO1,n1-n0);

#if 0	// enable this to print the position occasionally
		if (++i==1000)
		{
			i=0;
			printf("%5.0f %5d\n",(k1-k0)*1.0,j1-j0);
		}
#endif
	}
}
