#include "KMotionDef.h"

// Sets KStep's relay drivers on

main()
{
    SetBitDirection(0,1);  //set as output
    SetBitDirection(1,1);  //set as output

    SetBit(0);  // turn output on
    SetBit(1);  // turn output on
}
