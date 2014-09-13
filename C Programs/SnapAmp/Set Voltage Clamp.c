#include "KMotionDef.h"

main()
{
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP0 ,SNAP_CONVERT_VOLTS_TO_ADC(80.0));
	WriteSnapAmp(SNAP0+SNAP_SUPPLY_CLAMP_ENA0,1);
}
