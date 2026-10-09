#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"

// Typically called from M6 to change G43 compensation to same tool as loaded tool.

int main()
{
    G43(40); // G43 H40
}
