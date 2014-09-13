#include "KMotionDef.h"

// Home and set the commutation for two 3 phase brushless motors
//
// Assume index mark  


void main() 
{
    float k=0,A=10.0f;   // set coil current amplitude PWM units
	double p0;
	
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP0 ,SNAP_CONVERT_VOLTS_TO_ADC(80.0));
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP_ENA0,1);
	
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP1 ,SNAP_CONVERT_VOLTS_TO_ADC(80.0));
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP_ENA1,1);
	
	WriteSnapAmp(SNAP0+SNAP_PEAK_CUR_LIMIT0,9);  // current limit
	WriteSnapAmp(SNAP0+SNAP_PEAK_CUR_LIMIT1,9);  // current limit
	
	Delay_sec(1);  // wait for any fault to clear
   	
	// rotate until we find the index mark

	ch4->Enable=FALSE;
	ch4->OutputChan0=8;
	for (;;)
    {
		Delay_sec(0.001);  // wait a millisecond 
		
		Write3PH(ch4,A, ++k/1000.0);  // move the pole 

		if (ReadBit(68))  // check for index mark
		{
			p0=ch4->Position; // save position
			ch4->Position=0;  // set current position to Zero
			
			// set commutation offset to 1/4th of a cycle
			// encoder has 4000 counts/rev 
			// motor has 3 cycles per rev
			
			ch4->CommutationOffset = 3*8000/2.0/12.0 * 1.02;
			
			printf("Position = %f\n",p0);
			break;
		}
    }

	Write3PH(ch4,0,0);    // turn off the coil    

	// define the axis as 3 phase BRUSHLESS_3PH_MODE
	// and set low PID gains
	
	ch4->InputMode=ENCODER_MODE;
	ch4->OutputMode=BRUSHLESS_3PH_MODE;
	ch4->Vel=100000.000000;
	ch4->Accel=100000.000000;
	ch4->Jerk=100000000.000000;
	ch4->P=0.100000;
	ch4->I=0.000000;
	ch4->D=0.000000;
	ch4->FFAccel=0.000000;
	ch4->FFVel=0.000000;
	ch4->MaxI=200.000000;
	ch4->MaxErr=1000000.000000;
	ch4->MaxOutput=230.000000;
	ch4->DeadBandGain=1.000000;
	ch4->DeadBandRange=0.000000;
	ch4->InputChan0=4;
	ch4->InputChan1=4;
	ch4->OutputChan0=8;
	ch4->OutputChan1=9;
	ch4->LimitSwitchOptions=0x0;
	ch4->InputGain0=1.000000;
	ch4->InputGain1=1.000000;
	ch4->InputOffset0=0.000000;
	ch4->InputOffset1=0.000000;
	ch4->invDistPerCycle=2.0/8000.0; // CW (2 cycles/rev) / (8000 encoder cnts/rev)
	ch4->Lead=0.000000;
	ch4->MaxFollowingError=4000.000000;
	ch4->StepperAmplitude=20.000000;

	ch4->iir[0].B0=1.000000;
	ch4->iir[0].B1=0.000000;
	ch4->iir[0].B2=0.000000;
	ch4->iir[0].A1=0.000000;
	ch4->iir[0].A2=0.000000;

	ch4->iir[1].B0=1.000000;
	ch4->iir[1].B1=0.000000;
	ch4->iir[1].B2=0.000000;
	ch4->iir[1].A1=0.000000;
	ch4->iir[1].A2=0.000000;

	ch4->iir[2].B0=1.000000;
	ch4->iir[2].B1=0.000000;
	ch4->iir[2].B2=0.000000;
	ch4->iir[2].A1=0.000000;
	ch4->iir[2].A2=0.000000;

	EnableAxisDest(4,0.0f);  // Enable servo at destination of 0
	
	// rotate until we find the index mark

	ch5->Enable=FALSE;
	ch5->OutputChan0=10;
	for (;;)
    {
		Delay_sec(0.001);  // wait a millisecond 
		
		Write3PH(ch5,A, ++k/1000.0);  // move the pole 

		if (ReadBit(69))  // check for index mark
		{
			p0=ch5->Position; // save position
			ch5->Position=0;  // set current position to Zero
			
			// set commutation offset to 1/4th of a cycle
			// encoder has 4000 counts/rev 
			// motor has 3 cycles per rev
			
			ch5->CommutationOffset = 3*8000/2.0/12.0 * 1.02;
			
			printf("Position = %f\n",p0);
			break;
		}
    }

	Write3PH(ch5,0,0);    // turn off the coil    

	// define the axis as 3 phase BRUSHLESS_3PH_MODE
	// and set low PID gains
	
	ch5->InputMode=ENCODER_MODE;
	ch5->OutputMode=BRUSHLESS_3PH_MODE;
	ch5->Vel=100000.000000;
	ch5->Accel=100000.000000;
	ch5->Jerk=100000000.000000;
	ch5->P=0.100000;
	ch5->I=0.000000;
	ch5->D=0.000000;
	ch5->FFAccel=0.000000;
	ch5->FFVel=0.000000;
	ch5->MaxI=200.000000;
	ch5->MaxErr=1000000.000000;
	ch5->MaxOutput=230.000000;
	ch5->DeadBandGain=1.000000;
	ch5->DeadBandRange=0.000000;
	ch5->InputChan0=5;
	ch5->InputChan1=5;
	ch5->OutputChan0=10;
	ch5->OutputChan1=11;
	ch5->LimitSwitchOptions=0x0;
	ch5->InputGain0=1.000000;
	ch5->InputGain1=1.000000;
	ch5->InputOffset0=0.000000;
	ch5->InputOffset1=0.000000;
	ch5->invDistPerCycle=2.0/8000.0; // CW (2 cycles/rev) / (8000 encoder cnts/rev)
	ch5->Lead=0.000000;
	ch5->MaxFollowingError=4000.000000;
	ch5->StepperAmplitude=20.000000;

	ch5->iir[0].B0=1.000000;
	ch5->iir[0].B1=0.000000;
	ch5->iir[0].B2=0.000000;
	ch5->iir[0].A1=0.000000;
	ch5->iir[0].A2=0.000000;

	ch5->iir[1].B0=1.000000;
	ch5->iir[1].B1=0.000000;
	ch5->iir[1].B2=0.000000;
	ch5->iir[1].A1=0.000000;
	ch5->iir[1].A2=0.000000;

	ch5->iir[2].B0=1.000000;
	ch5->iir[2].B1=0.000000;
	ch5->iir[2].B2=0.000000;
	ch5->iir[2].A1=0.000000;
	ch5->iir[2].A2=0.000000;

	EnableAxisDest(5,0.0f);  // Enable servo at destination of 0
	DefineCoordSystem(4,5,-1,-1);
}

