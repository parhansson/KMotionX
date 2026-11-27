// Kogna I2C Master Transmit/Receive Example 

#include "KMotionDef.h"
#include "Kogna_I2c_Lib\Kogna_I2C.c"

#define I2C_SLAVE_ADDR       (0x20u)	/* I2C address of PCF8575 expander  */

// PCF8575 I2C Quasi-bidirectional 16-bit IO Expander
// IO pins have weak pullups.  Setting Output low drives
// low as an output as open collector output.  Setting Output 
// high allows to be driven low externally as an input.
//
// Tested with Adafruit PCF8575 I2C 16 GPIO Expander Breakout
// https://learn.adafruit.com/adafruit-pcf8575


int main(void)
{
	unsigned char WriteBuff[] = { 0xff, 0xff };  // all bits high = Inputs
	unsigned char ReadBuff[] = { 0, 0 };

//	HWREG(SOC_I2C_0_REGS + I2C_ICEMDR) = 2;	// set to ignore NACKs

	SetupI2C(400000, I2C_SLAVE_ADDR);  // set bitrate and Slave Address

	for (;;)
	{
		WriteBuff[0] ^= (1<<4);  // Toggle bit 4 low/high to blink LED connected to Output 4

		//Transmit - Outputs
		I2C_Transmit(WriteBuff, sizeof(WriteBuff));	// Transmit as Master

		while (I2CMasterBusBusy(SOC_I2C_0_REGS) != 0) ;	// wait till no longer busy

		//Receive - Inputs
		I2C_Receive(ReadBuff, sizeof(ReadBuff));  // receive as Master
		
		while (I2CMasterBusBusy(SOC_I2C_0_REGS) != 0);	// wait till no longer busy
		
		// if no inputs low read should match wwrite
		if (ReadBuff[0] != WriteBuff[0] || ReadBuff[1] != WriteBuff[1])
		{
			printf("Outputs LSB %02X MSB %02X Inputs LSB %02X MSB %02X\n", 
				WriteBuff[0], WriteBuff[1], ReadBuff[0], ReadBuff[1]);
			break;
		}
		
		Delay_sec(0.25);  // delay so blink visible
	}
}
