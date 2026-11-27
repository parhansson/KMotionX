#include "KMotionDef.h"

int main()
{
    FPGA32(LatchedDiffIn) = 0x000000;       // Clear all latched bits
    printf("%8X\n", FPGA32(LatchedDiffIn)); // display latched bits

    // Pulse Differential Output 0 looped back to Diff input 0 for ~ 400ns
    SetBit(250);
    ClearBit(250);

    for (;;)
    {
        printf("%8X\n", FPGA32(LatchedDiffIn)); // display latched bits
        Delay_sec(2);
    }

    return 0;
}
