/*
    Simple test program, run on kflop, used with test_exec.py.
*/

#include "KMotionDef.h"

void main()
{
    int i;
    
    persist.UserData[0] = 0x12345678;
    
    for (i = 0; i < 4; ++i) {
        printf("Hello world!\n");
        SetBit(46);
        Delay_sec(0.1);
        ClearBit(46);
        Delay_sec(0.1);
    }
    printf("Kflop Done!\n");
}

