#include "KMotionDef.h"

main()
{
	int Adc;
	float Voltage;
	int i;
	
	i=0;
	for (;;)
	{
		Adc = ReadSnapAmp(SNAP0 + SNAP_SUPPLY_VOLT0);
		Voltage = SNAP_CONVERT_ADC_TO_VOLTS(Adc);
		printf("%d Supply = %4.1f Volts\n",i++,Voltage);
		Delay_sec(1);
	}
}
