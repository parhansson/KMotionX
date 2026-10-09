// TestManyParams.c
//
// Verifies TCC67 support for functions with MORE THAN 10 PARAMETERS
// (added 2026-08-03; previously an error - and defining such a function
// silently corrupted compiler state).
//
// The TI C67 convention passes the first 10 args in registers A4,B4..B12.
// TCC67 now passes args 11..16 in stack slots (a TCC67-private convention;
// both caller and callee must be TCC67 compiled - no firmware function
// takes more than 10 args).  Limit is 16 args; 17+ is a clean compile
// error.  Also exercises address-of-parameter (see TestParamAddr.c) on
// overflow params, doubles spanning the register/stack boundary, writes
// to overflow params, nested many-arg calls, and calls with live values
// held across them.
//
// Run on any Kogna/KFLOP - no hardware needed.  Every line must PASS.

#include "KMotionDef.h"

int Sum11(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,int a9,
          int a10,int a11)
{
	return a1+a2+a3+a4+a5+a6+a7+a8+a9+a10+a11;
}

int Sum16(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,int a9,
          int a10,int a11,int a12,int a13,int a14,int a15,int a16)
{
	return a1+a2+a3+a4+a5+a6+a7+a8+a9+a10+a11+a12+a13+a14+a15+a16;
}

// doubles as the 10th (register) and 11th/13th (stack) args
double MixD(int a1,double d2,int a3,int a4,int a5,int a6,int a7,int a8,
            int a9,double d10,double d11,int a12,double d13)
{
	return d2 + d10 + d11 + d13 + (a1+a3+a4+a5+a6+a7+a8+a9+a12);
}

int ReadThru(int *p) { return *p; }

// modify overflow params + take an overflow param's address
int Modify13(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,
             int a9,int a10,int a11,int a12,int a13)
{
	a11 += 100;                       // store path to a stack param
	a13 = ReadThru(&a12) + a13;      // address of a stack param
	return a11 + a13 + a1 + a10;
}

int Live;

main()
{
	int r;
	double d;

	r = Sum11(1,2,3,4,5,6,7,8,9,10,11);
	printf("Sum11                : %s (%d)\n", r == 66 ? "PASS" : "FAIL", r);

	r = Sum16(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
	printf("Sum16                : %s (%d)\n", r == 136 ? "PASS" : "FAIL", r);

	d = MixD(1, 2.5, 3,4,5,6,7,8,9, 10.25, 11.125, 12, 13.0625);
	printf("MixD doubles         : %s (%f)\n",
		d == 2.5+10.25+11.125+13.0625+(1+3+4+5+6+7+8+9+12) ? "PASS" : "FAIL", d);

	r = Modify13(1,2,3,4,5,6,7,8,9,10,11,12,13);
	printf("Modify13 &param      : %s (%d)\n", r == (111 + 25 + 1 + 10) ? "PASS" : "FAIL", r);

	// nested: a 16 arg call as an argument of an 11 arg call
	r = Sum11(Sum16(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16),
	          2,3,4,5,6,7,8,9,10,11);
	printf("nested Sum11(Sum16)  : %s (%d)\n", r == 136+65 ? "PASS" : "FAIL", r);

	// live value held in a register across the call (preserve path)
	Live = 1000;
	r = Live + Sum11(1,2,3,4,5,6,7,8,9,10,11) + Live;
	printf("live value across    : %s (%d)\n", r == 2066 ? "PASS" : "FAIL", r);

	// printf (variable arguments, TI compiled) with 16 total args -
	// PASS if the numbers below read 1..12 and 2.5 4.25 6.125
	printf("printf 16 args       : %d %d %d %d %d %d %d %d %d %d %d %d %f %f %f\n",
		1,2,3,4,5,6,7,8,9,10,11,12, 2.5, 4.25, 6.125);
}
