#include "KMotionDef.h"
#include "MySpindleDefs.h"
#include "CSSJogwithSSV.c"
int main()
{
     for (;;)
     {
            WaitNextTimeSlice();
            ServiceCSS();
     }
}
