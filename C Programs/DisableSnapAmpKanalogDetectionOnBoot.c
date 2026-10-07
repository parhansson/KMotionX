#include "KMotionDef.h"

// Disables autodetect of Kanalog and SnapAmp on boot
// (must be set and Flashed to User Data)

main()
{
	DisableKanalogDetectOnBoot=TRUE;
	DisableSnapAmpDetectOnBoot=TRUE;
}
