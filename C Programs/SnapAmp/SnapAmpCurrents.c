#include "KMotionDef.h"

main()
{
	float Robot0, Robot1, Robot2, Robot3, Robot4;

	Robot0 = SNAP_CONVERT_ADC_TO_AMPS(ADC_BufferInSnap[0]);

	Robot1 = SNAP_CONVERT_ADC_TO_AMPS(ADC_BufferInSnap[1]);

	Robot2 = SNAP_CONVERT_ADC_TO_AMPS(ADC_BufferInSnap[2]);

	Robot3 = SNAP_CONVERT_ADC_TO_AMPS(ADC_BufferInSnap[3]);

	Robot4 = SNAP_CONVERT_ADC_TO_AMPS(ADC_BufferInSnap[8]);

	printf("Currents in Amps = %6.2f %6.2f %6.2f %6.2f %6.2f\n",Robot0, Robot1, Robot2, Robot3, Robot4);
}
