// TestParamAddr.c
//
// Verifies the TCC67 fix for taking the ADDRESS of a function parameter.
//
// The C67 calling convention passes the first 10 parameters in registers;
// TCC67's prolog homes them to stack slots and translates parameter loads
// and stores to those slots - but the address-of path historically did NOT
// translate, so &param pointed into the caller's frame where the parameter
// never existed.  (That single bug produced the entire "servo EEPROM
// corruption" saga of 2026-08: SerialServoConfig.c passed &val where val
// was a function parameter, and garbage got transmitted to the servo.)
//
// Run on any Kogna/KFLOP - no hardware needed.  Every line must PASS.

#include "KMotionDef.h"

int ReadThru(int *p)              { return *p; }
void WriteThru(int *p, int v)     { *p = v; }
double ReadThruD(double *p)       { return *p; }

// address of the FIRST parameter
int Test1(int val)
{
	int got = ReadThru(&val);          // read the param through its address
	WriteThru(&val, 777);              // write the param through its address
	return got == 111 && val == 777;   // both directions must work
}

// address of a LATER parameter (offset walk through the sizes)
int Test2(int a, int b, int c)
{
	int got = ReadThru(&c);
	WriteThru(&b, 555);
	return got == 33 && b == 555 && a == 11;
}

// address of a DOUBLE parameter (8 byte size in the translation walk)
int Test3(int a, double d, int b)
{
	double got = ReadThruD(&d);
	int gb = ReadThru(&b);
	return got == 2.5 && gb == 22 && a == 1;
}

// address of a parameter used in a LOOP mixed with direct references -
// pointer and direct access must alias the same storage
int Test4(int val)
{
	int i, *p = &val;

	for (i = 0; i < 5; i++) *p += 1;   // through the pointer
	val += 100;                        // direct
	return *p == 147 && val == 147;    // 42+5+100 either way
}

void Report(char *name, int pass)
{
	printf("%s: %s\n", name, pass ? "PASS" : "FAIL");
}

main()
{
	Report("Test1 addr of 1st param      ", Test1(111));
	Report("Test2 addr of 2nd/3rd params ", Test2(11, 22, 33));
	Report("Test3 addr of double param   ", Test3(1, 2.5, 22));
	Report("Test4 pointer/direct aliasing", Test4(42));
}
