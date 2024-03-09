#include "KMotionDef.h"

// Perform Encoder correction similar to lead screw compensation
//
// requires 2 spare "dummy" Axes configured as Encoder input (and disabled) 
// to read the magnetic encoder positions. 
//
// configure "real" motor Axes as "User Input" and 
// uses a User C Program to continuously: 
//    read the dummy Position, 
//    correct it, 
//    then write it to the real Position.

#define TableDelta 1000.0		// Table Value every this many encoder counts

double Table0[] = { 0.0, 5.0, -3.0, -4.0 };	// correction Table for Axis0
double Table1[] = { 0.0, -5.0, 2.0, -1.0 };	// correction Table for Axis1

void CorrectEncoder(int EncoderAxis, int MotorAxis, double *Table, int n);

void main()
{
	// Test the Correction Function
	ch5->Position = 2250.0;		// test value is quarter the way between last 2 table values
	CorrectEncoder(5, 3, Table0, sizeof(Table0) / sizeof(double));
	printf("Raw = %f Correction = %f Corrected = %f\n", ch5->Position,
		   ch3->Position - ch5->Position, ch3->Position);

	for (;;)					// loop forever
	{
		WaitNextTimeSlice();
		CorrectEncoder(5, 3, Table0, sizeof(Table0) / sizeof(double));
		CorrectEncoder(6, 4, Table1, sizeof(Table1) / sizeof(double));
	}
}

// Read raw encoder, interpolate into Table to get correction and apply for motor axis
// Parameters are Axis for Raw Encoder, Axis for Motor, Table, # Entries in table

void CorrectEncoder(int EncoderAxis, int MotorAxis, double *Table, int n)
{
	int i;
	double Encoder;

	Encoder = chan[EncoderAxis].Position;	// get raw encoder

	i = (int)(Encoder * (1.0 / TableDelta));	// determine Table region

	if (i < 0)					// limit to table
		i = 0;
	if (i > n - 2)
		i = n - 2;

	// Interpolate into Table
	chan[MotorAxis].Position = Encoder + Table[i] +
		(Encoder - i * TableDelta) * (1.0 / TableDelta) * (Table[i + 1] - Table[i]);
}
