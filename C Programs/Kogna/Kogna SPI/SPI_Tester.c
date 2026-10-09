#include "KMotionDef.h"

// includes for the TI DSP register definitions and driver functions
#include "TI_SOC_Lib\hw_types.h"
#include "TI_SOC_Lib\soc_C6748.h"
#include "TI_SOC_Lib\Kogna_edma.c"

#include "TI_SOC_Lib\hw_dspcache.h"

// TI SPI Register Definitions
#define EDMA_BASE SOC_EDMA30CC_0_REGS
#define SPI_BASE_ADDR SOC_SPI_1_REGS

#define SPIGCR0 0x0
#define SPIGCR1 0x4
#define SPIINT0 0x8
#define SPIFLG 0x10
#define SPIPC0 0x14
#define SPIPC1 0x18 
#define SPIDAT0 0x38
#define SPIDAT1 0x3C
#define SPIBUF 0x40
#define SPIEMU 0x44
#define SPIDELAY 0x48
#define SPIFMT0 0x50

// local function prototypes
void Init_SPI_Hardware();
void Print_SPI_Registers();
void SendSpiChar(unsigned char char2tx);		
void SPI_Txfer(unsigned char *tx_buff, unsigned char *rx_buff, int nBytes);
void SPI_Txfer_DMA(unsigned char *tx_buff, unsigned char *rx_buff, int nBytes);
void EdmaConfigTxSpi(unsigned char *Addr, int nBytes);
void EdmaConfigRxSPI(unsigned char *Addr, int nBytes);
void fill_buffs(unsigned char *tx_buff, unsigned char *rx_buff, int nBytes);
void print_buffs(unsigned char *tx_buff, unsigned char *rx_buff, int nBytes);

// number of bytes we want to test with
#define NUM_TEST_BYTES 32
	
// The buffers declared here doesn't work reliably with DMA and I don't understand why yet.
unsigned char TxBuff_global[NUM_TEST_BYTES];
unsigned char RxBuff_global[NUM_TEST_BYTES];

int main(void)
{
	int i;

    //The buffers declared here work reliably with DMA.
	unsigned char LocalTxBuff[NUM_TEST_BYTES];
	unsigned char LocalRxBuff[NUM_TEST_BYTES];

	// set pins 4 and 5 of group of 6 as I2C
	SPI_SetMode(0, 0);			// Set SPI mode as SPI or GPIO, 1=GPIO 0=SPI 2=I2C
	SPI_SetMode(1, 0);			// Set SPI mode as SPI or GPIO, 1=GPIO 0=SPI 2=I2C
	SPI_SetMode(3, 0);			// Set SPI mode as SPI or GPIO, 1=GPIO 0=SPI 2=I2C

	// Set up the DSP Registers for the SPI Hardware
	Init_SPI_Hardware();

    printf("SPI/DMA Test with LOCAL Buffer...\n");
    fill_buffs(LocalTxBuff, LocalRxBuff, NUM_TEST_BYTES);
    SPI_Txfer_DMA(LocalTxBuff, LocalRxBuff, NUM_TEST_BYTES);
    print_buffs(LocalTxBuff, LocalRxBuff, NUM_TEST_BYTES);

    printf("SPI/DMA Test with GLOBAL Buffer...\n");
    fill_buffs(TxBuff_global, RxBuff_global, NUM_TEST_BYTES);
    SPI_Txfer_DMA(TxBuff_global, RxBuff_global, NUM_TEST_BYTES);
    print_buffs(TxBuff_global, RxBuff_global, NUM_TEST_BYTES);

    printf("SPI/DMA Test with GATHER Buffer...\n");
    fill_buffs(&gather_buffer[0], &gather_buffer[NUM_TEST_BYTES>>3], NUM_TEST_BYTES);
    SPI_Txfer_DMA(&gather_buffer[0],  &gather_buffer[NUM_TEST_BYTES>>3], NUM_TEST_BYTES);
    print_buffs(&gather_buffer[0],  &gather_buffer[NUM_TEST_BYTES>>3], NUM_TEST_BYTES);

	// delay a bit
	Delay_sec(0.15);
	printf("Testing Non DMA SPI Transfers...\n");
    fill_buffs(TxBuff_global, RxBuff_global, NUM_TEST_BYTES);
    SPI_Txfer(TxBuff_global, RxBuff_global, NUM_TEST_BYTES);
    print_buffs(TxBuff_global, RxBuff_global, NUM_TEST_BYTES);
}

/* fill_buffs
*------------------------------------------------------------------------------------------
* fills the tx_buff with ascending numbers and initializes the Rx buffer elements to 0xFF
*------------------------------------------------------------------------------------------*/
void fill_buffs(unsigned char *tx_buff, unsigned char *rx_buff, int nBytes)
{
    int i;
    
    //initialize the buffers
    for(i=0; i<nBytes; i++)
	{
		tx_buff[i] = i;
		rx_buff[i] = 0xFF;
	}
}

/* SPI_Txfer
*------------------------------------------------------------------------------------------
* Will transfer nBytes over the SPI hardware (without dma).  the bytes in tx_buffer will be sent out
* MOSI and the bytes read in from MISO will fill the rx_buff  
*------------------------------------------------------------------------------------------*/
void SPI_Txfer(unsigned char *tx_buff, unsigned char *rx_buff, int nBytes)
{
	// disable the DMA servicing for the SPI data requests by setting the SPIINT0.DMAREQEN to 1.
    HWREG(SPI_BASE_ADDR + SPIINT0) &= (0xFFFFFFFF & 0<<16);
	
	int i=0;
	for(i=0; i<nBytes; i++)
	{
		SendSpiChar(tx_buff[i]);								// Tx a character
		while( (HWREG(SPI_BASE_ADDR + SPIFLG) & 1<<8)==0);		// wait for transaction to complete
        rx_buff[i]=HWREG(SPI_BASE_ADDR + SPIBUF);               // Read in the character from the Rx buffer
	}
}

/* SPI_Txfer_DMA
*------------------------------------------------------------------------------------------
* Will transfer nBytes over the SPI hardware using dma.  The bytes in tx_buffer will be sent out
* MOSI and the bytes read in from MISO will fill the rx_buff  
*------------------------------------------------------------------------------------------*/
void SPI_Txfer_DMA(unsigned char *tx_buff, unsigned char *rx_buff, int nBytes)
{
    // pointer to transfer paramter set the the SPI Rx DMA. 
    EDMA3CCPaRAMEntry *p2RxPaRAM;

	EdmaConfigTxSpi(tx_buff, nBytes);
    EdmaConfigRxSPI(rx_buff, nBytes);
    
    // get the pointer to the EDMA transfer paramaters for the SPI1_RX.
	p2RxPaRAM = (EDMA3CCPaRAMEntry *)(EDMA_BASE + EDMA3CC_OPT(EDMA3_CHA_SPI1_RX));

    // print out our DMA Transfer Counts before enabling
	printf("SPI Rx DMA Source[0x%x] Dest[0x%x] ", p2RxPaRAM->srcAddr, p2RxPaRAM->destAddr);
	printf("Counts: A[%d] B[%d] C[%d]\n", p2RxPaRAM->aCnt, p2RxPaRAM->bCnt, p2RxPaRAM->cCnt);


    // writeback and invalidate cache so DMA reads from memory have correct data written by DSP
    // note cache lines are 64 bytes
	HWREG(SOC_CACHE_0_REGS + DSPCACHE_L1DWIBAR) = tx_buff;  // address to write back invalidate
	HWREG(SOC_CACHE_0_REGS + DSPCACHE_L1DWIWC) = nBytes >> 2;  // no of words

    // writeback and invalidate cache so DSP reads after DMA transfer come directly from memory
    // note cache lines are 64 bytes
	HWREG(SOC_CACHE_0_REGS + DSPCACHE_L1DWIBAR) = rx_buff;  // address to write back invalidate
	HWREG(SOC_CACHE_0_REGS + DSPCACHE_L1DWIWC) = nBytes >> 2;  // no of words

    // enable the DMA servicing for the SPI data requests by setting the SPIINT0.DMAREQEN to 1.
    HWREG(SPI_BASE_ADDR + SPIINT0) |= 1<<16;

    // wait for transfer to complete
	while (p2RxPaRAM->cCnt){/* just wait */}

    // disable the DMA servicing for the SPI data requests by setting the SPIINT0.DMAREQEN to 0.
    HWREG(SPI_BASE_ADDR + SPIINT0) &= (0xFFFFFFFF & 0<<16);
}


/* print_buffs
*------------------------------------------------------------------------------------------
* helper routing to print out the Tx and Rx buffers to the console
*------------------------------------------------------------------------------------------*/
void print_buffs(unsigned char *tx_buff, unsigned char *rx_buff, int nBytes)
{
    int i;
    for(i=0; i<nBytes; i++)
	{
        if(i%16==0 && i!=0) printf("\n");
		printf("%02x[%02x] ", tx_buff[i], rx_buff[i]);
	}
    printf("\n\n");
}


/* SendSpiChar
*------------------------------------------------------------------------------------------
* writes a character to the the SPI Transmit Data Register 0(SPIDAT0)
* IF the SPI is enabled, this will shift the bits out MOSI while shifting bits in from MISO
*------------------------------------------------------------------------------------------*/
void SendSpiChar(unsigned char char2tx)
{
	HWREG(SPI_BASE_ADDR + SPIDAT0) = char2tx; //test
}


/* InitializeSPI
*------------------------------------------------------------------------------------------
* Sets up the DSP hardware registers for the SPI1 periph.
* the procedure follows the listing in the TMS320C6748 Technical Ref Manual
*------------------------------------------------------------------------------------------*/
void Init_SPI_Hardware()
{
	// RESET SPI
	HWREG(SPI_BASE_ADDR + SPIGCR0) = 0;
	HWREG(SPI_BASE_ADDR + SPIGCR0) = 1;
	
	//printf("SPI Reset!\n"); Print_SPI_Registers(); printf("\n\n");

	// Config for Master Mode
	HWREG(SPI_BASE_ADDR + SPIGCR1) = 3;

	// Set to 3 pin mode (enable pin control functions for MOSI, MISO, CLK)
	HWREG(SPI_BASE_ADDR + SPIPC0) = 1<<11 | 1<<10 | 1<<9;

	// Data format Register Selection - Tell CPU which format register we want to use
	HWREG(SPI_BASE_ADDR + SPIDAT1) = 0<<24;

	// Data format Register Setting
	HWREG(SPI_BASE_ADDR + SPIFMT0) = 0
		| 0<<20		/* shift direciton	| 0=MSB Firts, 1=LSB First*/
		| 0<<17		/* clock polarity 	| 0=idle low, 1=idle high*/
		| 0<<16		/* clock phase		| 0= first data bit transmitted with first clock edge, 1st tx bit output before clock edge */
		| 20<<8		/* prescaler		| 2-255 => [ SPI clock frequency = SPI module clock/(PRESCALE + 1) ] */		
		| 8<<0;		/* character length	| 2-16, number of data bits */	

	//Configure Delays for Master (SPIDELAY)
	HWREG(SPI_BASE_ADDR + SPIDELAY) = 0;

	// Selection the error interrupt notifications (SPIINT0) and (SPILVL) could be done here. 
	
	// enable the SPI 
	HWREG(SPI_BASE_ADDR + SPIGCR1) |= 1<<24;

	//printf("SPI Configured!\n"); Print_SPI_Registers(); printf("\n\n");
}

/* Print_SPI_Registers
*------------------------------------------------------------------------------------------
* helper routine to print out the SPI hardware registers for debugging
*------------------------------------------------------------------------------------------*/
void Print_SPI_Registers()
{
	printf("Config Reg => %x\n", HWREG(SPI_BASE_ADDR + SPIGCR1));
	printf("SPIPC0 => %x\n", HWREG(SPI_BASE_ADDR + SPIPC0));
	printf("SPIDAT1 => %x\n", HWREG(SPI_BASE_ADDR + SPIDAT1));
	printf("SPIFMT0 => %x\n", HWREG(SPI_BASE_ADDR + SPIFMT0));
	printf("FLAGS => %x\n", HWREG(SPI_BASE_ADDR + SPIFLG));
	printf("RX Buffer => %x\n", HWREG(SPI_BASE_ADDR + SPIBUF));
}

void EdmaConfigTxSpi(unsigned char *Addr, int nBytes)
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
	EDMA3RequestChannel(EDMA_BASE, EDMA3_CHANNEL_TYPE_DMA, EDMA3_CHA_SPI1_TX, EDMA3_CHA_SPI1_TX, evtQ);

	// Enable the transfer
	EDMA3EnableTransfer(EDMA_BASE, EDMA3_CHA_SPI1_TX, EDMA3_TRIG_MODE_EVENT);


	paramSet.srcAddr = (unsigned int)Addr;
	paramSet.destAddr = (SPI_BASE_ADDR + SPIDAT0);

	/*
	 ** SPI generates event whenever TX is empty.  We set SPI length to 1 byte and there is no fifo.
	 ** Hence per event one bytes needs to be transfered. Thus EDMA is configured in ASYNC mode
	 ** with acount = 1, bcount = total_numbytes, ccount = 1.  BSRC index should be 1 since memory pointer needs to incremented one after every byte 
	 ** transfer by EDMA. BDST index should be zero since the destination address is in constant adrressing mode(hardware register).
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
	paramSet.opt |= ((EDMA3_CHA_SPI1_TX << EDMA3CC_OPT_TCC_SHIFT) & EDMA3CC_OPT_TCC);

	/* configure PaRAM Set */
	EDMA3SetPaRAM(EDMA_BASE, EDMA3_CHA_SPI1_TX, &paramSet);
}

/*
** Configures Edma to receive nBytes bytes from the SPI1 
*/
void EdmaConfigRxSPI(unsigned char *Addr, int nBytes)
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
	EDMA3RequestChannel(EDMA_BASE, EDMA3_CHANNEL_TYPE_DMA, EDMA3_CHA_SPI1_RX, EDMA3_CHA_SPI1_RX, evtQ);

	// Enable the transfer
	EDMA3EnableTransfer(EDMA_BASE, EDMA3_CHA_SPI1_RX, EDMA3_TRIG_MODE_EVENT);

	paramSet.srcAddr = (SPI_BASE_ADDR + SPIBUF);
	paramSet.destAddr = Addr;

	/*
	 ** SPI generates one EDMA event whenever XXXX is empty.  There is space for only one byte of data in SPIBUF.  There is no fifo.  
     ** Hence per event, one bytes needs to be transfered. Thus EDMA is configured in ASYNC mode with acount = 1, bcount = total_numbytes, ccount = 1.  
     ** BSRC index should be 1 since memory pointer needs to incremented one after every byte transfer by EDMA.
     ** BDST index should be zero since the destination address is in constant adrressing mode(hardware register).
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
	paramSet.opt |= ((EDMA3_CHA_SPI1_RX << EDMA3CC_OPT_TCC_SHIFT) & EDMA3CC_OPT_TCC);

	/* configure PaRAM Set */
	EDMA3SetPaRAM(EDMA_BASE, EDMA3_CHA_SPI1_RX, &paramSet);
}

