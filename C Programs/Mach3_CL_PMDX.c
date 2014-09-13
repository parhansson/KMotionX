#include "KMotionDef.h"

#define FEEDHOLDBIT 7
#define ESTOPBIT 14
#define SIMESTOPBIT 46
#define MANUALBIT 47

#define X 0
#define Y 1
#define Z 2

#define Z_DISABLE_VAR 99
 
void ServiceEStop(void);

int flast=0,flastsolid=-1,fcount=0;
int elast=0,elastsolid=-1,ecount=0;
int mlast=0,mlastsolid=-1,mcount=0;

int Estop,Manual;
	
int Debounce(int n, int *cnt, int *last, int *lastsolid);

main()
{
	int result;

	Estop = ReadBit(ESTOPBIT);
	Manual = !ReadBit(MANUALBIT);

	ch0->InputMode=ENCODER_MODE;
	ch0->OutputMode=CL_STEP_DIR_MODE;
	ch0->Vel=33333.000000;
	ch0->Accel=130000.000000;
	ch0->Jerk=1000000.000000;
	ch0->P=0.000000;
	ch0->I=0.001200;
	ch0->D=0.000000;
	ch0->FFAccel=0.000000;
	ch0->FFVel=0.000000;
	ch0->MaxI=1000.000000;
	ch0->MaxErr=250.000000;
	ch0->MaxOutput=1000.000000;
	ch0->DeadBandGain=0.000000;
	ch0->DeadBandRange=4.400000;
	ch0->InputChan0=0;
	ch0->InputChan1=0;
	ch0->OutputChan0=8;
	ch0->OutputChan1=0;
	ch0->LimitSwitchOptions=0x0;
	ch0->InputGain0=7.874020;
	ch0->InputGain1=1.000000;
	ch0->InputOffset0=0.000000;
	ch0->InputOffset1=0.000000;
	ch0->invDistPerCycle=1.000000;
	ch0->Lead=0.000000;
	ch0->MaxFollowingError=215.000000;
	ch0->StepperAmplitude=100.000000;

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

	ch0->iir[2].B0=0.002960;
	ch0->iir[2].B1=0.005921;
	ch0->iir[2].B2=0.002960;
	ch0->iir[2].A1=1.841730;
	ch0->iir[2].A2=-0.853569;



	ch1->InputMode=ENCODER_MODE;
	ch1->OutputMode=CL_STEP_DIR_MODE;
	ch1->Vel=33333.000000;
	ch1->Accel=130000.000000;
	ch1->Jerk=1000000.000000;
	ch1->P=0.000000;
	ch1->I=0.001200;
	ch1->D=0.000000;
	ch1->FFAccel=0.000000;
	ch1->FFVel=0.000000;
	ch1->MaxI=1000.000000;
	ch1->MaxErr=250.000000;
	ch1->MaxOutput=1000.000000;
	ch1->DeadBandGain=0.000000;
	ch1->DeadBandRange=4.000000;
	ch1->InputChan0=1;
	ch1->InputChan1=0;
	ch1->OutputChan0=9;
	ch1->OutputChan1=0;
	ch1->LimitSwitchOptions=0x0;
	ch1->InputGain0=-7.874020;
	ch1->InputGain1=1.000000;
	ch1->InputOffset0=0.000000;
	ch1->InputOffset1=0.000000;
	ch1->invDistPerCycle=1.000000;
	ch1->Lead=0.000000;
	ch1->MaxFollowingError=215.000000;
	ch1->StepperAmplitude=100.000000;

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

	ch1->iir[2].B0=0.002960;
	ch1->iir[2].B1=0.005921;
	ch1->iir[2].B2=0.002960;
	ch1->iir[2].A1=1.841730;
	ch1->iir[2].A2=-0.853569;


	ch2->InputMode=ENCODER_MODE;
	ch2->OutputMode=CL_STEP_DIR_MODE;
	ch2->Vel=33333.000000;
	ch2->Accel=130000.000000;
	ch2->Jerk=1000000.000000;
	ch2->P=0.000000;
	ch2->I=0.001200;
	ch2->D=0.000000;
	ch2->FFAccel=0.000000;
	ch2->FFVel=0.000000;
	ch2->MaxI=1000.000000;
	ch2->MaxErr=250.000000;
	ch2->MaxOutput=1000.000000;
	ch2->DeadBandGain=0.000000;
	ch2->DeadBandRange=5.000000;
	ch2->InputChan0=2;
	ch2->InputChan1=0;
	ch2->OutputChan0=10;
	ch2->OutputChan1=0;
	ch2->LimitSwitchOptions=0x0;
	ch2->InputGain0=10;
	ch2->InputGain1=1.000000;
	ch2->InputOffset0=0.000000;
	ch2->InputOffset1=0.000000;
	ch2->invDistPerCycle=1.000000;
	ch2->Lead=0.000000;
	ch2->MaxFollowingError=215.000000;
	ch2->StepperAmplitude=100.000000;

	ch2->iir[0].B0=1.000000;
	ch2->iir[0].B1=0.000000;
	ch2->iir[0].B2=0.000000;
	ch2->iir[0].A1=0.000000;
	ch2->iir[0].A2=0.000000;

	ch2->iir[1].B0=1.000000;
	ch2->iir[1].B1=0.000000;
	ch2->iir[1].B2=0.000000;
	ch2->iir[1].A1=0.000000;
	ch2->iir[1].A2=0.000000;

	ch2->iir[2].B0=0.002960;
	ch2->iir[2].B1=0.005921;
	ch2->iir[2].B2=0.002960;
	ch2->iir[2].A1=1.841730;
	ch2->iir[2].A2=-0.853569;
	
	ch0->OutputGain = 1;
	ch1->OutputGain = -1;
	ch2->OutputGain = -1;
	
	// check if the User wants to disable Z
	if (persist.UserData[Z_DISABLE_VAR])
		chan[Z].OutputMode = STEP_DIR_MODE;
	else
		chan[Z].OutputMode = CL_STEP_DIR_MODE;


	// check if the User wants Manual Mode
	if (Manual)
	{
		chan[X].OutputMode = STEP_DIR_MODE;
		chan[Y].OutputMode = STEP_DIR_MODE;
	}
	else
	{
		chan[X].OutputMode = CL_STEP_DIR_MODE;
		chan[Y].OutputMode = CL_STEP_DIR_MODE;
	}


	EnableAxisDest(0,ch0->Position);
	EnableAxisDest(1,ch1->Position);
	EnableAxisDest(2,ch2->Position);

	DefineCoordSystem(0,1,2,-1);
	
	for (;;)
	{
		if (Debounce(ReadBit(FEEDHOLDBIT),&fcount,&flast,&flastsolid) == 0)
		{
			if (CS0_StoppingState == 0)
			{
				StopCoordinatedMotion();
				printf("Feed Hold on\n");
			}
		}
		
		// Because of the way the PMDX-131 board works 
		// (Manual mode forces Estop also)
		// Simulate a new Estop that is only active
		// when NOT in Manual mode  

		Estop = ReadBit(ESTOPBIT);
		Manual = !ReadBit(MANUALBIT);
		
		result = Debounce(Estop && !Manual,&ecount,&elast,&elastsolid);
		
		if (result == 1)
		{
			SetBit(SIMESTOPBIT);
			printf("Simulated EStop on\n");
		}

		if (result == 0)
		{
			ClearBit(SIMESTOPBIT);
			printf("Simulated EStop off\n");
		}
		

		// handle switching into or out of Manual Mode
		result = Debounce(Manual,&mcount,&mlast,&mlastsolid);
		
		if (result == 1)
		{
			// just entered manual mode
			
			DisableAxis(X);
			DisableAxis(Y);
			DisableAxis(Z);
			chan[X].OutputMode = STEP_DIR_MODE;
			chan[Y].OutputMode = STEP_DIR_MODE;
			chan[Z].OutputMode = STEP_DIR_MODE;
			EnableAxisDest(X,chan[X].Position);
			EnableAxisDest(Y,chan[Y].Position);
			EnableAxisDest(Z,chan[Z].Position);

			printf("Simulated Manual Mode on\n");
		}
		
		if (result == 0)
		{
			// just entered auto mode
			
			DisableAxis(X);
			DisableAxis(Y);
			DisableAxis(Z);
			chan[X].OutputMode = CL_STEP_DIR_MODE;
			chan[Y].OutputMode = CL_STEP_DIR_MODE;

			if (persist.UserData[Z_DISABLE_VAR])
			{
				chan[Z].OutputMode = STEP_DIR_MODE;
				printf("Z Disabled\n");
			}
			else
			{
				chan[Z].OutputMode = CL_STEP_DIR_MODE;
				printf("Z Enabled\n");
			}

			EnableAxisDest(X,chan[X].Position);
			EnableAxisDest(Y,chan[Y].Position);
			EnableAxisDest(Z,chan[Z].Position);
			printf("Simulated Manual Mode off\n");
			
			printf("Pulse Estop\n");
			SetBit(SIMESTOPBIT);
			Delay_sec(1);
			ClearBit(SIMESTOPBIT);
		}

		WaitNextTimeSlice();
	}

	return 0;
}



// Debounce a bit
//
// return 1 one time when first debounced high 
// return 0 one time when first debounced low 
// return -1 otherwise 
#define DBTIME 300

int Debounce(int n, int *cnt, int *last, int *lastsolid)
{
	int v = -1;
	
	if (n == *last)  // same as last time?
	{
		if (*cnt == DBTIME-1)
		{
			if (n != *lastsolid)
			{
				v = *lastsolid = n;  // return debounced value
			}
		}
		if (*cnt < DBTIME)	(*cnt)++;
	}
	else
	{
		*cnt = 0;  // reset count
	}
	*last = n;
	return v;
}

