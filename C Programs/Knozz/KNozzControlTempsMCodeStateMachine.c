#include "KMotionDef.h"

// This program services KNozz to read the two KNozz ADCs and
// control two heaters based on a pair of Persist Variable Setpoints
// The consecutive Persist variables correspond to the Nozzle and Bed Setpoints
//
// A KMotionCNC MCode can be configured to set the temperature setpoints 
// using P and Q parameters. ie.
// M100 P100 Q50 (Set Nozzle and Bed temperature setpoints)

#define AUX 1					// 0=Aux0 1=Aux1  defines which Aux connector to use.

// define pair of persist variables (as floats) as heater setpoints
#define NOZ_VAR 30
#define BED_VAR (NOZ_VAR+1)

//SPI and Heater IO definitions
#define CS (23 + AUX*10)
#define DATAIN (24 + AUX*10)	// with respect to KFLOP
#define CLK (25 + AUX*10)
#define DOUT (22 + AUX*10)		// with respect to KFLOP
#define NOZ_HEAT (21 + AUX*10)
#define BED_HEAT (20 + AUX*10)

int SPI_IN(int send_data);		// function to read serial SPI dual ADC
float TempToADC(float T);		// function to convert Temp C to ADC counts
float ADCtoTemp(float At);		// Solve inverse function numerically using guesses and linear interpolate
void ServiceKNozz(void);		// Service KNozz Temperature controls
BOOL ServiceKNozzSPI(void);		// Service KNozz SPI

float *NozSetPoint = (float *)&persist.UserData[NOZ_VAR];	// define convienient pointers to Persist floats
float *BedSetPoint = (float *)&persist.UserData[BED_VAR];

main()
{

	SetBitDirection(CS, 1);
	SetBitDirection(CLK, 1);
	SetBitDirection(NOZ_HEAT, 1);
	SetBitDirection(DOUT, 1);
	SetBitDirection(BED_HEAT, 1);

	*NozSetPoint = 0;			// start with heaters off
	*BedSetPoint = 0;

	for (;;)
	{
		Delay_sec(0.001);		// loop ~every millisecond
		ServiceKNozz();
	}
}

int raw_counts_bed, raw_counts_noz;  // global KNozz ADC readings

// Service KNozz Temperature controls
void ServiceKNozz(void)
{
	static int JobWasActive = FALSE;
	static int i = 0;

	if (!ServiceKNozzSPI()) //Service KNozz SPI and check if values updated
		return;					

	if (i++ >= 100)				// diagnostic printout ~ every 1 sec
	{
		i = 0;
		printf
			("Nozz:Setpt %6.1fC %6.1fcnts Actual %6.1fC %4dcnts   Bed:Setpt %6.1fC %6.1fcnts Actual %6.1fC %4dcnts\n",
			 *NozSetPoint, TempToADC(*NozSetPoint), ADCtoTemp(raw_counts_noz), raw_counts_noz,
			 *BedSetPoint, TempToADC(*BedSetPoint), ADCtoTemp(raw_counts_bed), raw_counts_bed);
	}

	if (raw_counts_noz < TempToADC(*NozSetPoint))
		SetBit(NOZ_HEAT);
	else
		ClearBit(NOZ_HEAT);

	if (raw_counts_bed < TempToADC(*BedSetPoint))
		SetBit(BED_HEAT);
	else
		ClearBit(BED_HEAT);


	if (JobWasActive && !JOB_ACTIVE)	// Job Stopped?  
	{
		//          *NozSetPoint=0;  // yes, turn off heater?
		//          *BedSetPoint=0;
	}

	JobWasActive = JOB_ACTIVE;
}

typedef enum
{								// states
	CS_HIGH,
	CLK_LOW,
	CS_LOW,
	DOING_BITS_CLK,
	DOING_BITS_RW,
	DOING_LAST_BIT
} SPI_STATES;


// Service SPI port to KNozz without any delays before returning
//
// State machine keeps track of what to do next
//
// Must delay > 5us between calls
//
// Return TRUE when values have been updated

BOOL ServiceKNozzSPI(void)
{
	static SPI_STATES State = CS_HIGH;
	static int dataIn;
	static int channel = 0;
	static int bit;

	BOOL updated = FALSE;		// assume not updated

	switch (State++)			// by default advance to next state
	{
	case CS_HIGH:
		SetBit(CS);				//CS high
		dataIn = bit = 0;
		break;

	case CLK_LOW:
		ClearBit(CLK);			//CLK low
		break;

	case CS_LOW:
		ClearBit(CS);			//CS low
		SetBit(DOUT);			//first send bit is always high
		break;

	case DOING_BITS_CLK:
		SetBit(CLK);			//CLK high
		break;

	case DOING_BITS_RW:
		dataIn = (dataIn << 1) | ReadBit(DATAIN);	// read the bit
		ClearBit(CLK);			//CLK low
		SetStateBit(DOUT, ((channel ? 0xd000 : 0xf000) >> (14 - bit)) & 1);	// send bit
		if (++bit < 16)
			State = DOING_BITS_CLK;	// loop back until finished al bits
		break;

	case DOING_LAST_BIT:
		dataIn = (dataIn << 1) | ReadBit(DATAIN);	// read the bit
		SetBit(CS);				//CS high
		if (channel)			// Save channel globally
			raw_counts_noz = dataIn;
		else
			raw_counts_bed = dataIn;

		updated = channel;		// if doing channel 1 then both have been updated
		channel = 1 - channel;	// toggle channels;
		State = CS_HIGH;		// loop back to beginning
		break;
	}

	return updated;
}

// Convert Temperature in C to equivalent ADC Counts
// based on 3rd order data fit to this measured data
//
// Temp C, ADC
// 16.0 199
// 35.0 400
// 49.1 600
// 54.7 700
// 60.7 800
// 66.9 900
// 73.6 1000
// 78.4 1100
// 85.2 1200
// 93.8 1300
//107.5 1482
//115.5 1570
//125.0 1688
//132.0 1722

// function to convert Temp C to ADC counts
float TempToADC(float T)
{
	return ((-0.000837 * T + 0.170202) * T + 4.565045) * T + 78.452228;	// 3rd order polynomial
}


// Solve inverse function numerically using guesses and linear interpolate
float ADCtoTemp(float At)
{
	int i;
	float A, T, T0 = 0.0, T1 = 100.0;	// initial guess 0 snd 1

	float A0 = TempToADC(T0);	// see how well they did
	float A1 = TempToADC(T1);


	for (i = 0; i < 10; i++)
	{
		// linearly interpolate
		T = T0 + (T1 - T0) * (At - A0) / (A1 - A0);

		A = TempToADC(T);		// check how well it works
//      printf("Desired ADC %f guess Temp %f ADC %f\n",At,T,A);

		if (fast_fabs(A - At) < 0.1f)
			break;				// good result exit

		// replace furthest away guess with new result
		if (fast_fabs(A - A0) > fast_fabs(A - A1))
		{
			T0 = T;				// replace guess #0
			A0 = A;
		}
		else
		{
			T1 = T;				// replace guess #1
			A1 = A;
		}
	}
	return T;
}
