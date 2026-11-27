#include "KMotionDef.h"
#define TMP 10 // which spare persist to use to transfer data
#include "KflopToKMotionCNCFunctions.c"


int main()
{
    float value;
    
    // Ask Operator desired rotation angle with drop down values specified
    if (!InputBox("Enter Angle in degrees;45.0;90.0;10.0",&value))  
    {
    	printf("value = %f\n", value);
    }
}

