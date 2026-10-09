// Kogna I2C Functions

#include "Kogna_I2c_Lib\hw_i2c.h"
#include "Kogna_I2c_Lib\i2c.h"
#include "Kogna_I2c_Lib\psc.h"
#include "Kogna_I2c_Lib\edma.h"
#include "Kogna_I2c_Lib\edma_event.h"
#include "Kogna_I2c_Lib\soc_C6748.h"
#include "Kogna_I2c_Lib\hw_psc_C6748.h"
#include "Kogna_I2c_Lib\hw_types.h"
#include "Kogna_I2c_Lib\hw_edma3cc.h"

#include "Kogna_I2c_Lib\Kogna_edma.c"
#include "Kogna_I2c_Lib\i2c.c"


/*******************************************************************************
**                   FUNCTION PROTOTYPE
*******************************************************************************/
static void SetupI2C(int busclock, int SlaveAddr);


static void EdmaConfigTransmit(unsigned char *Addr, int nBytes);
static void EdmaConfigReceive(unsigned char *, int nBytes);

static void SetupI2CTransmit(int nBytes);
static void SetupI2CReceive(int nBytes);

static void I2C_Transmit(unsigned char *Addr, int nBytes);
static void I2C_Receive(unsigned char *Addr, int nBytes);


/*******************************************************************************
**                   INTERNAL VARIABLE DEFINITION
*******************************************************************************/
#define EDMA_BASE SOC_EDMA30CC_0_REGS


void I2C_Transmit(unsigned char *Addr, int nBytes)
{
	EdmaConfigTransmit(Addr, nBytes);
	SetupI2CTransmit(nBytes);
}

void I2C_Receive(unsigned char *Addr, int nBytes)
{
	EdmaConfigReceive(Addr, nBytes);
	SetupI2CReceive(nBytes);
}

/*
** Configures Edma to transmit nBytes2 bytes to
** i2c transmit register.
*/
static void EdmaConfigTransmit(unsigned char *Addr, int nBytes)
{
	volatile unsigned int evtQ = 0;	// highest priority queue
	EDMA3CCPaRAMEntry paramSet;

	/* Enabling the PSC for EDMA3CC_0. */
	PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_CC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

	/* Enabling the PSC for EDMA3TC_0. */
	PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_TC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

	/* Intialize the Edma */
	EDMA3Init(EDMA_BASE, evtQ);
	/* Request DMA Channel and TCC */
	EDMA3RequestChannel(EDMA_BASE, EDMA3_CHANNEL_TYPE_DMA,
		EDMA3_CHA_I2C0_TX, EDMA3_CHA_I2C0_TX, evtQ);

	// Enable the transfer
	EDMA3EnableTransfer(EDMA_BASE, EDMA3_CHA_I2C0_TX, EDMA3_TRIG_MODE_EVENT);


	/* on i2c reset both tx and rx events are enabled.
	 * Both rx and tx events are disabled by this API
	 */

	I2CDMATxRxEventDisable(SOC_I2C_0_REGS);

	paramSet.srcAddr = (int)Addr;
	paramSet.destAddr = (SOC_I2C_0_REGS + I2C_ICDXR);

	/*
	 ** I2C generates one EDMA event whenever I2CXSR is empty.  There is space
	 ** for only one byte of data in I2CXSR.  There is no fifo.  Hence per event
	 ** one bytes needs to be transfered. Thus EDMA is configured in ASYNC mode
	 ** with acount = 1, bcount = total_numbytes, ccount = 1.  BSRC index should
	 ** be 1 since memory pointer needs to incremented one after every byte 
	 ** transfer by EDMA.BDST index should be zero since the destination address
	 ** is in constant adrressing mode(hardware register).
	 **
	 */
	paramSet.srcBIdx = 0x01;
	paramSet.srcCIdx = 0x00;
	paramSet.destBIdx = 0x00;
	paramSet.destCIdx = 0x00;
	paramSet.aCnt = 0x01;
	paramSet.bCnt = nBytes;          
	paramSet.cCnt = 0x01;
	paramSet.bCntReload = 0x00;
	paramSet.linkAddr = 0xffff;
	paramSet.opt = 0;

	/* Program the TCC */
	paramSet.opt |= ((EDMA3_CHA_I2C0_TX << EDMA3CC_OPT_TCC_SHIFT) & EDMA3CC_OPT_TCC);

	/* configure PaRAM Set */
	EDMA3SetPaRAM(EDMA_BASE, EDMA3_CHA_I2C0_TX, &paramSet);
}

/*
** Configures Edma to receive nBytes bytes to
** i2c transmit register.
*/
static void EdmaConfigReceive(unsigned char *Addr, int nBytes)
{
	volatile unsigned int evtQ = 0;	// highest priority queue
	EDMA3CCPaRAMEntry paramSet;

	/* Enabling the PSC for EDMA3CC_0. */
	PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_CC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

	/* Enabling the PSC for EDMA3TC_0. */
	PSCModuleControl(SOC_PSC_0_REGS, HW_PSC_TC0, PSC_POWERDOMAIN_ALWAYS_ON, PSC_MDCTL_NEXT_ENABLE);

	/* Intialize the Edma */
	EDMA3Init(EDMA_BASE, evtQ);
	/* Request DMA Channel and TCC */
	EDMA3RequestChannel(EDMA_BASE, EDMA3_CHANNEL_TYPE_DMA,
		EDMA3_CHA_I2C0_RX, EDMA3_CHA_I2C0_RX, evtQ);

	// Enable the transfer
	EDMA3EnableTransfer(EDMA_BASE, EDMA3_CHA_I2C0_RX, EDMA3_TRIG_MODE_EVENT);


	/* on i2c reset both tx and rx events are enabled.
	 * Both rx and tx events are disabled by this API
	 */

	I2CDMATxRxEventDisable(SOC_I2C_0_REGS);

	paramSet.srcAddr = (SOC_I2C_0_REGS + I2C_ICDRR);
	paramSet.destAddr = (int)Addr;

	/*
	 ** I2C generates one EDMA event whenever I2CXSR is empty.  There is space
	 ** for only one byte of data in I2CXSR.  There is no fifo.  Hence per event
	 ** one bytes needs to be transfered.Thus EDMA is configured in ASYNC mode
	 ** with acount = 1, bcount = total_numbytes, ccount = 1.  BSRC index should
	 ** be 1 since memory pointer needs to incremented one after every byte 
	 ** transfer by EDMA.BDST index should be zero since the destination address
	 ** is in constant adrressing mode(hardware register).
	 **
	 */
	paramSet.srcBIdx = 0x00;
	paramSet.srcCIdx = 0x00;
	paramSet.destBIdx = 0x01;
	paramSet.destCIdx = 0x00;
	paramSet.aCnt = 0x01;
	paramSet.bCnt = nBytes;
	paramSet.cCnt = 0x01;
	paramSet.bCntReload = 0x00;
	paramSet.linkAddr = 0xffff;
	paramSet.opt = 0;

	/* Program the TCC */
	paramSet.opt |= ((EDMA3_CHA_I2C0_RX << EDMA3CC_OPT_TCC_SHIFT) & EDMA3CC_OPT_TCC);

	/* configure PaRAM Set */
	EDMA3SetPaRAM(EDMA_BASE, EDMA3_CHA_I2C0_RX, &paramSet);
}



/*
** Configures I2C to communicate with I/0 Expander
** at 100kbps
*/
static void SetupI2C(int busclock, int SlaveAddr)
{
	// set pins 4 and 5 of group of 6 as I2C
	SPI_SetMode(4, 2);			// Set SPI mode as SPI or GPIO, 1=GPIO 0=SPI 2=I2C
	SPI_SetMode(5, 2);			// Set SPI mode as SPI or GPIO, 1=GPIO 0=SPI 2=I2C

	/* Put i2c in reset/disabled state */
	I2CMasterDisable(SOC_I2C_0_REGS);

	/* Configure i2c bus speed to 100khz */
	I2CMasterInitExpClk(SOC_I2C_0_REGS, 24000000, 8000000, busclock);

	/* Set i2c slave address */
	I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, SlaveAddr);

	/* Bring i2c out of reset */
	I2CMasterEnable(SOC_I2C_0_REGS);
}

/*
** Enables Transmit Event to be generated by the I2C,generates
** start condition on i2c bus and waits still stop generated.
*/
static void SetupI2CTransmit(int nBytes)
{
	/* configures number of transfer carried out 
	 ** before generating stop condition
	 */
	I2CSetDataCount(SOC_I2C_0_REGS, nBytes);

	/* 
	 ** Configure i2c has master-transmitter and to generate stop condition
	 ** when value in internal data count register count down to zero
	 */
	I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_TX | I2C_CFG_STOP);

	/* Enable transmit event of I2C */
	I2CDMATxEventEnable(SOC_I2C_0_REGS);

	I2CMasterStart(SOC_I2C_0_REGS);

	Delay_sec(10e-6);  // make sure it started and busy before returning
}


/*
** Enables Receive Event to be generated by the I2C,generates
** start condition on i2c bus and waits still stop generated.
*/
static void SetupI2CReceive(int nBytes)
{
	/* configures number of transfer carried out 
	 ** before generating stop condition
	 */
	I2CSetDataCount(SOC_I2C_0_REGS, nBytes);
	
	/* 
	 ** Configure i2c has master-transmitter and to generate stop condition
	 ** when value in internal data count register count down to zero
	 */
	I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_RX | I2C_CFG_STOP);

	/* Enable receive event of I2C */
	I2CDMARxEventEnable(SOC_I2C_0_REGS);

	I2CMasterStart(SOC_I2C_0_REGS);

	Delay_sec(10e-6);  // make sure it started and busy before returning
}
