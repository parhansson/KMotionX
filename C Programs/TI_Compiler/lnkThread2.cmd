-c
-heap  15700000
-stack 0x800 
/*-lrts6701.lib */

/* LINK CMD FILE TO PUT USER PROGRAM IN SMALL SPACE AT END OF INTERNAL DSP RAM !!!!!!!!!!!!!!!!!  */ 

/* Hard Code KFLOP Addresses */
_SetBit=0x10011028;
_ClearBit=0x10010e20;
  
MEMORY
{
    IRAM_BOOT: 	o = 10000000h   l = 00001000h                           
    ENTRYPT:    o = 10001000h   l = 00000040h                           
/*    IRAM:       o = 10001040h   l = 0001efc0h        */

                   
    IRAM:       o = 1001c000h   l = 00004000h   /* for FAST User C Programs use small leftover toward end of IRAM !!!!!!!!!!!!*/                           


    SDRAM_BOOT: o = 80000000h   l = 0000f000h  /* 60K Permanent boot loader */  
    SDRAM_SA:   o = 8000f000h   l = 00034c00h  
    
	USER_SA:    o = 80060000h   l = 00010000h  /* these blocks are put in flash  THREAD2 Starts at 0x80060000 !!!!!!!!!!!!!!*/                        
	
	
	SDRAM:      o = 80100000h   l = 00f00000h  /* ~ 15 MBytes leftover*/                        
}

SECTIONS
{
	/* MUST PUT A NON ZERO WORD IN NORMAL 0x8006000 THREAD2 SPACE OR KFLOP WILL REFUSE TO RUN THREAD2 !!!!!!!!!!!!!!*/
	.mytext: palign(8), fill = 0xAAAAAAAA {. += 4;} > USER_SA 


    "vectors"   : load = 0x10000000
    .boot_bss	>    	IRAM_BOOT
    .boot_iram	>  		IRAM_BOOT
	.AltProg    >       IRAM
	.bstart     >       ENTRYPT
	.boot       : load = 0x80000000
	.bootf		>       SDRAM_BOOT
    .big        >       SDRAM_SA  /* FPGA data */
    .cinit      >       SDRAM_SA
    
    
    .text       >       IRAM      /* PUT FAST USER CODE HERE IN IRAM !!!!!!!!!!!!!!!!   */
    
        
    .sdramsa    >       SDRAM_SA
    .vec     	>       SDRAM_SA  /* this isn't used, map here */
    .stack      >       IRAM
    .bss        >       IRAM
    .const      >       IRAM
    .data       >       IRAM
    .far        >       IRAM
    .switch     >       IRAM
    .sdram      >       SDRAM
    .sysmem     >       SDRAM
    .cio        >       IRAM
}                             

 
