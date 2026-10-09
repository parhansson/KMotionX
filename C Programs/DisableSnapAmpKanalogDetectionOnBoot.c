#include "KMotionDef.h"

// Disables autodetect of Kanalog and SnapAmp on boot
// (must be set and Flashed to User Data)

int main()
{
	DisableKanalogDetectOnBoot=TRUE;
	DisableSnapAmpDetectOnBoot=TRUE;
}
