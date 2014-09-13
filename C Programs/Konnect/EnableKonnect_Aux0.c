#include "KMotionDef.h"

// Configure KFLOP to service Konnect 32 Input 16 output IO board
// Board address is 0, 
// 16 Outputs are mapped to Virtual IO 48-63 (VirtualBits)
// 32 Inputs are mapped to Virtual IO 1024-1055 (VirtualBits[0])
//
// Attach Service to Aux0 Port (KFLOP JP4) instead of standard Aux1 Port (KFLOP JP6)

main()
{
	InitAux();
	AddKonnect_Aux0(0,&VirtualBits,VirtualBitsEx);
}
