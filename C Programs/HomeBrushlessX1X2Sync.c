#include "KMotionDef.h"

// Home and set the commutation for two 3 phase brushless motors
//
// Assume each has index mark  

void ConfigAll(void);
#define XX 0.85
void main() 
{
    float k=0,A=10.0f;   // set coil current amplitude PWM units
	double p0,p1;
	int found0,found1;
	
	ch0->Enable=FALSE;  // make sure the axes are disabled
	ch1->Enable=FALSE;
	ch4->OutputMode=0;
	ch5->OutputMode=0;

	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP0 ,SNAP_CONVERT_VOLTS_TO_ADC(47.0));
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP_ENA0,1);

	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP1 ,SNAP_CONVERT_VOLTS_TO_ADC(47.0));
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP_ENA1,1);
	
	WriteSnapAmp(SNAP0+SNAP_PEAK_CUR_LIMIT0,11);  // current limit
	WriteSnapAmp(SNAP0+SNAP_PEAK_CUR_LIMIT1,11);  // current limit
	
	ConfigAll(); // set all parameters

	k=-2000;
	Write3PH(ch0,A, k/20000.0);  // move the pole 
	Write3PH(ch1,A, k/20000.0+XX);  // move the pole 
	
	Delay_sec(2);  // wait for any fault to clear
   	
   	
	// rotate until we find the index mark

	found0 = found1 = FALSE;  // set both as not found yet
	
	while (!found0 || !found1)
    {
		Delay_sec(0.0005);  // wait a 1/2 millisecond 

		if (!found0)
		{
			Write3PH(ch0,A, -k/20000.0);  // move the pole 
	
			if (ReadBit(70))  // check for index mark
			{
				p0=ch0->Position; // save position
				ch0->Position=0;  // set current position to Zero
				// set commutation offset to 1/4th of a cycle
				ch0->CommutationOffset = 9.6*8000/2.0/12.0;
				found0=TRUE;
			}
		}
		
		if (!found1)
		{
			Write3PH(ch1,A, -k/20000.0+XX);  // move the pole 
	
			if (ReadBit(68))  // check for index mark
			{
				p1=ch1->Position; // save position
				ch1->Position=0;  // set current position to Zero
				// set commutation offset to 1/4th of a cycle
				ch1->CommutationOffset = -.835*8000/2.0/12.0;
				found1=TRUE;
			}
		}
		k++;
    }

	Write3PH(ch0,0,0);    // turn off the coil  
	Write3PH(ch1,0,0);    // turn off the coil  

	printf("Position X1 X2 = %f %f\n",p0,p1);

	EnableAxis(0);  // Enable servo where we are
	EnableAxis(1);  // Enable servo where we are
	
	Move(0,0);
	Move(1,0);
	DefineCoordSystem(0,1,-1,-1);
}

void ConfigAll(void)
{  
	// define the axis as 3 phase BRUSHLESS_3PH_MODE
	
	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=BRUSHLESS_3PH_MODE;
	ch0->Vel=200000.000000;
	ch0->Accel=1000000.000000;
	ch0->Jerk=10000000.000000;
	ch0->P=0.300000;
	ch0->I=0.000300;
	ch0->D=2.000000;
	ch0->FFAccel=0.000010;
	ch0->FFVel=0.001300;
	ch0->MaxI=200.000000;
	ch0->MaxErr=1000000.000000;
	ch0->MaxOutput=400.000000;
	ch0->DeadBandGain=1.000000;
	ch0->DeadBandRange=0.000000;
	ch0->InputChan0=4;
	ch0->InputChan1=4;
	ch0->OutputChan0=8;
	ch0->OutputChan1=9;
	ch0->LimitSwitchOptions=0x0;
	ch0->InputGain0=-1.000000;
	ch0->InputGain1=1.000000;
	ch0->InputOffset0=0.000000;
	ch0->InputOffset1=0.000000;
	ch0->invDistPerCycle=-0.000250;
	ch0->Lead=0.000000;
	ch0->MaxFollowingError=4000.000000;
	ch0->StepperAmplitude=20.000000;

	ch0->iir[0].B0=1.000000;
	ch0->iir[0].B1=0.000000;
	ch0->iir[0].B2=0.000000;
	ch0->iir[0].A1=0.000000;
	ch0->iir[0].A2=0.000000;

	ch0->iir[1].B0=1.000000;
	ch0->iir[1].B1=0.000000;
	ch0->iir[1].B2=0.000000;
	ch0->iir[1].A1=0.000000;
	ch0->iir[1].A2=0.000000;

	ch0->iir[2].B0=0.056605;
	ch0->iir[2].B1=0.113210;
	ch0->iir[2].B2=0.056605;
	ch0->iir[2].A1=1.228040;
	ch0->iir[2].A2=-0.454462;

	
	ch1->InputMode=ENCODER_MODE;
	ch1->OutputMode=BRUSHLESS_3PH_MODE;
	ch1->Vel=200000.000000;
	ch1->Accel=1000000.000000;
	ch1->Jerk=10000000.000000;
	ch1->P=0.300000;
	ch1->I=0.000300;
	ch1->D=2.000000;
	ch1->FFAccel=0.000010;
	ch1->FFVel=0.001300;
	ch1->MaxI=200.000000;
	ch1->MaxErr=1000000.000000;
	ch1->MaxOutput=400.000000;
	ch1->DeadBandGain=1.000000;
	ch1->DeadBandRange=0.000000;
	ch1->InputChan0=5;
	ch1->InputChan1=5;
	ch1->OutputChan0=10;
	ch1->OutputChan1=11;
	ch1->LimitSwitchOptions=0x0;
	ch1->InputGain0=1.000000;
	ch1->InputGain1=1.000000;
	ch1->InputOffset0=0.000000;
	ch1->InputOffset1=0.000000;
	ch1->invDistPerCycle=-0.000250;
	ch1->Lead=0.000000;
	ch1->MaxFollowingError=4000.000000;
	ch1->StepperAmplitude=20.000000;

	ch1->iir[0].B0=1.000000;
	ch1->iir[0].B1=0.000000;
	ch1->iir[0].B2=0.000000;
	ch1->iir[0].A1=0.000000;
	ch1->iir[0].A2=0.000000;

	ch1->iir[1].B0=1.000000;
	ch1->iir[1].B1=0.000000;
	ch1->iir[1].B2=0.000000;
	ch1->iir[1].A1=0.000000;
	ch1->iir[1].A2=0.000000;

	ch1->iir[2].B0=0.056605;
	ch1->iir[2].B1=0.113210;
	ch1->iir[2].B2=0.056605;
	ch1->iir[2].A1=1.228040;
	ch1->iir[2].A2=-0.454462;
}
