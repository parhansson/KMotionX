/* Link template for TI Linker.  Tags are replaced by KMotion to create Link.cmd used in linking */

-q -c -x -e ___user_init
{OBJECTFILE} 
"{DSP_KFLOP_PATH}user_boot.obj"
--output_file={OUTPUTFILE}
-m{MAPFILE}

-heap  0x100
-stack 0x100 
--diag_suppress=16002
--diag_suppress=10063
"{DSP_KFLOP_PATH}DSPKFLOP.sym"  /* include KFLOP Symbols */

MEMORY
{
	IRAM:     o = {IRAMSTART}	l = {IRAMLENGTH}	/*  for FAST User C Programs use small leftover toward end of IRAM */                           
	USER_SA:  o = {THREADSTART}	l = {THREADLENGTH}	/*  THREAD1 Starts at 0x80050000 */                        
}

SECTIONS
{
	GROUP
	{
		/* MUST PUT A NON ZERO WORD IN NORMAL 0x8006000 THREAD2 SPACE OR KFLOP WILL REFUSE TO RUN THREAD2 */
		.mytext: START(__start_.text), palign(8), fill = 0xAAAAAAAA {. += 4;}
    
		.text:  END(__stop_.text)
		.stack
		.bss: START( __start_.bss) END(__stop_.bss)
		.const: START( __start_.data)  
		.data: 
		.far 
		.switch 
		.cio
		.cinit: END(__stop_.data)
	}  >  USER_SA
	
	GROUP
	{
		.IRAM
		.IRAMP
		.IRAMD	
	}  >  IRAM
}                             


 



