-c
-heap  15700000
-stack 0x800 

/* LINK CMD FILE TO PUT USER PROGRAM IN SMALL SPACE AT END OF INTERNAL DSP RAM !!!!!!!!!!!!!!!!!  */ 

/* Hard Code KFLOP Addresses */
"..\..\DSP_KFLOP\DSPKFLOP.sym"
  
MEMORY
{
    IRAM:       o = 1001d000h   l = 00003000h  /*  for FAST User C Programs use small leftover toward end of IRAM !!!!!!!!!!!!*/                           
	USER_SA:    o = 80060000h   l = 00010000h  /* these blocks are put in flash  THREAD2 Starts at 0x80060000 !!!!!!!!!!!!!!*/                        
	SDRAM:      o = 80100000h   l = 00f00000h  /* ~ 15 MBytes leftover*/                        
}

SECTIONS
{
	/* MUST PUT A NON ZERO WORD IN NORMAL 0x8006000 THREAD2 SPACE OR KFLOP WILL REFUSE TO RUN THREAD2 !!!!!!!!!!!!!!*/
	.mytext: palign(8), fill = 0xAAAAAAAA {. += 4;} > USER_SA 
    
	GROUP
	{
		.text
		.stack
		.bss 
		.const 
		.data 
		.far 
		.switch 
		.cio
	}  >  IRAM
}                             

 
