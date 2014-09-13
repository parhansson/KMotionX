#include "KMotionDef.h"

main()
{
	// Mux encoder inputs from KFLOP JP7 & JP5 to JP4 aand JP6
	FPGAW(ENC_NOISE_FILTER_ADD) = ENC_0_3_JP4 + ENC_4_7_JP6 + ENC_NOISE_FILTER_DEFAULT_VAL; 
}
