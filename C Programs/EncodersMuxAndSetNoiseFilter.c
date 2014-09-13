#include "KMotionDef.h"

main()
{
	// addr to r/w encoder noise rejection filter value (0..255), 
	// Bit8 switches Ch4-7 from JP5 to JP6
	FPGAW(ENC_NOISE_FILTER_ADD)=0x107;
}
