#include "KMotionDef.h"

// Configure KFLOP to service Konnect 32 Input 16 output IO board
// Board address is 0, 
// 16 Outputs are mapped to Virtual IO 48-63 (VirtualBits)
// 32 Inputs are mapped to Virtual IO 1024-1055 (VirtualBits[0])

main()
{
	InitAux();
	AddKonnect(0,&VirtualBits,VirtualBitsEx);
}
