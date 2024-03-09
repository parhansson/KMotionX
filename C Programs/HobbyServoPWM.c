#include "KMotionDef.h"

void ServiceHobbyPWM(void);

int ncycles, v1, v2;

main()
{
	int LowClocks, scaler=150;
	float PWMPeriod, BaseFreq=16.6667e6, PulseGap=20e-3, SafetyTime=200e-6;
	// Set base PWM to be high for entire time less 200us
	
	LowClocks = SafetyTime * BaseFreq / scaler;
	PWMPeriod = 256.0*scaler/BaseFreq;
	ncycles = PulseGap/PWMPeriod;
	printf("PWM Period = %fus\n", PWMPeriod*1e6);
	printf("Low Clocks = %d\n", LowClocks);
	printf("Skip Cycles = %d\n", ncycles);
	
	SetBitDirection(26,1);  // define bit as an output
	ClearBit(26);
	SetBitDirection(27,1);  // define bit as an output
	ClearBit(27);
	SetBitDirection(28,1);  // define bit as an output
	ClearBit(28);
	
	FPGA(IO_PWMS_PRESCALE) = scaler;  	// divide clock by 256 (250Hz)
	FPGA(IO_PWMS) = 255-LowClocks;  	// Mostly high pulse
	FPGA(IO_PWMS+1) = 1;  			// Enable
	ClearBit(26);
	
	v1 =  800e-6 * BaseFreq / scaler;  // set Pulse time
	v2 = 1700e-6 * BaseFreq / scaler;  // set Pulse time

	printf("PWM settings v1 = %d v2 = %d\n", v1,v2);

	for (;;)
	{
		ServiceHobbyPWM();
	}
}


void ServiceHobbyPWM(void)
{
	static int i=0;
	static WaitLevel=0;
	
	if (!WaitLevel) // waiting for low?
	{
		if (!ReadBit(26)) // yes is it low?
		{
			WaitLevel=1;  // yes, change state
			if (++i == ncycles)  // count mostly high pulses, special cycle?
			{
				FPGA(IO_PWMS+2) = v1; 	// Pulse size
				FPGA(IO_PWMS+3) = 1;	// Enable
				FPGA(IO_PWMS+4) = v2; 	// Pulse size
				FPGA(IO_PWMS+5) = 1;	// Enable
				i=0;
			}
			else
			{
				FPGA(IO_PWMS+3) = 0;   // Disable
				FPGA(IO_PWMS+5) = 0;   // Disable
			}
		}
	}
	else
	{
		if (ReadBit(26)) // no, waiting for high, is it high?
		{
			WaitLevel=0; // yes, change state
		}
	}
}	
