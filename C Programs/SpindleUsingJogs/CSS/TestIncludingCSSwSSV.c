#include "KMotionDef.h"
#include "MySpindleDefs.h"
#include "CSSJogwithSSV.c"
main()
{
     for (;;)
     {
            WaitNextTimeSlice();
            ServiceCSS();
     }
}
