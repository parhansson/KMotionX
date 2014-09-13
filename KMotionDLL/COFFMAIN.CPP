/**************************************************************
  
                 Coffmain.c
  
                 staff
  
                 12-23-88
  
           (C) Texas Instruments Inc., 1992 
  
           Refer to the file 'license.txt' included with this 
           this package for usage and license information. 
  
*************************************************************/
/************************************************************************/
/* C30 SDB Standalone COFF loader      (c) 1988 Texas Instruments Inc.  */
/************************************************************************/
/*                                                                      */
/* General:  This program loads and executes a TMS320C30 COFF output    */
/*           file on a Software Development Board (SDB).                */
/*                                                                      */
/*  Usage:   load30  <-options> filename                                */
/*  Options:     -b        clear .bss section                           */
/*               -r xxxx   relocate by xxxx                             */
/*               -a xxxx   use address xxxx for dual-port RAM           */
/*               -s        SRAM mapped over EPROM (don't reset)         */
/*               -q        quiet (no banner)                            */
/*               -h        load only (don't run)                        */
/*               -i        enable interrupts from SDB to PC             */
/*               -p        pause before running                         */
/*                                                                      */
/************************************************************************/
#include "stdafx.h"

#define KMotionBd 1

#include "PARAMS.h"
#include "VERSION.h"
#include "COFF.h"
#include "CLOAD.h"


#define MIN(a,b) ((a)<(b)?(a):(b))



/*--------------------------------------------------------------------------*/
/* SDB HARDWARE INTERFACE PARAMETERS                                        */
/*--------------------------------------------------------------------------*/
#define CONPORT     0x0339                 /* CONTROL REGISTER              */
#define MAPORT      0x0338                 /* MAP REGISTER                  */

#define XINTCLR     0x02                   /* IF 1, ENABLE INTERRUPTS TO PC */
#define DPSEL       0x04                   /* EANBLE DUAL-PORT RAM          */
#define SWRESET     0x08                   /* IF 0, HOLD C30 IN RESET       */

#define DEFPAGE     0xC9000                /* DEFAULT ADDRESS OF DP RAM     */

#define SETADDR     0xFF
#define SETEXE      0xFE
#define OKSDB       0x07
#define TEST        0x55
#define TESTRESP    0xAA

/*----------------------------------------------------------------------------*/
/* DEFINITION OF SEMAPHORES USED FOR HANDSHAKING DUAL-PORT RAM.  SEMAPHORES   */
/* ARE IMPLEMENTED IN HARDWARE AND ACCESSED VIA I/O PORTS FROM THE PC.        */
/* WRITING A ZERO TO A SEMAPHORE REQUESTS CONTROL OF THE FLAG.  READING BACK  */
/* A ZERO INDICATES A SUCCESSFUL REQUEST, READING BACK A 1 INDICATES FAILURE. */
/* TWO SEMAPHORES ARE USED FOR THE PC/SDB HANDSHAKING:                        */
/*     BUSY: CONTROLLING SIDE HAS ACCESS TO THE DP RAM.                       */
/*     IDLE: CONTROLLING SIDE IS WAITING FOR ACCESS.                          */
/*     RUN:  SDB IS RUNNING.                                                  */
/* AT EACH HANDHAKING JUNCTURE, THE SDB AND PC SWAP CONTROL OF BOTH FLAGS.    */
/*----------------------------------------------------------------------------*/
#define BUSY         0x330             /* CONTROLLING SIDE IS PROCESSING      */
#define IDLE         0x331             /* CONTROLLING SIDE IS WAITING         */
#define RUN          0x332             /* SDB IS RUNNING                      */

#define LATCH(s)     (outp(s,0))       /* LATCH IN A REQUEST FOR CONTROL      */
#define UNLOCK(s)    (outp(s,1))       /* FREE CONTROL OF THE SEMAPHORE       */
#define IS_LOCKED(s) (inp(s) != 0)     /* FREE CONTROL OF THE SEMAPHORE       */

#define TIMEOUT  2
unsigned long time();

/*--------------------------------------------------------------------------*/
/* LOADER DEFINITIONS                                                       */
/*--------------------------------------------------------------------------*/
FILE *fopen();
char *strrchr(), *malloc(), *realloc();

unsigned char far *pageloc = (unsigned char far *)DEFPAGE;

unsigned long reloc = 0;                 /* RELOCATION AMOUNT               */
int           quiet = 0;                 /* NO BANNER                       */
int           sflag = 0;                 /* SRAM MAPPED IN FOR EPROM       */
int           iflag = 0;                 /* ALLOW SDB TO INTERRUPT PC      */
int           pflag = 0;                 /* PAUSE BEFORE RUNNING           */
int           packtoflash= 0;            /* PACK to Flash Memory image     */
int           firstdata= 1;              /* flag so we can clear flash image*/

CKMotionDLL *KMotionDLL;

/****************************************************************************/
/*                                                                          */
/* MAIN()                                                                   */
/*                                                                          */
/****************************************************************************/
int LoadCoff(CKMotionDLL *KMotionDLLtoUse, const char *Name, unsigned int *EntryPoint, int PackToFlash)
{
	int  files   = 0;
	int  nostart = 0;
	
	need_symbols = 0;

	KMotionDLL = KMotionDLLtoUse;  // save globally so memwrite can use it

	/*----------------------------------------------------------------------*/
	/* PROCESS COMMAND LINE ARGUMENTS                                       */
	/*----------------------------------------------------------------------*/
	
    clear_bss = 1;  
    quiet     = 1;  
    nostart   = 1;  
    sflag     = 1;  
    iflag     = 1;  
    pflag     = 1;  
    ++verbose;      
    reloc   = 0x00000000;
    pageloc = 0x00000000;
	
	packtoflash = PackToFlash;
    firstdata= 1;              /* flag so we can clear flash image*/

	
	fin = fopen(Name, "rb");

	if (!fin)
	{ 
		char s2[300]="can't open file : ";
		strcat(s2,Name);
		AfxMessageBox(s2,MB_OK|MB_SYSTEMMODAL);
		return 1;
	}
	
	if (!cload()) 
	{ 
		AfxMessageBox("error loading file",MB_OK|MB_SYSTEMMODAL); 
		fclose(fin);
		return 1;
	}

	fclose(fin);

	*EntryPoint = entry_point;

	return 0;
}


#define ARGBUFSIZE 256

int BytesPerLine=N_BYTES_PER_LINE;


/****************************************************************************/
/*                                                                          */
/* MEM_WRITE() - Write a 4K buffer of data to the SDB.                      */
/*                                                                          */
/****************************************************************************/
int mem_write(unsigned char *buffer, int nbytes, T_ADDR addr, int page)
{
	char s[256],x[256];
	int i,k;
	
	if (nbytes<1) return 1;

	if (packtoflash!=0)
	{
		if (firstdata && packtoflash!=2)
		{
			if (KMotionDLL->FirmwareVersion()==1)
				BytesPerLine=16;

			if (KMotionDLL->WriteLine("CLEARFLASHIMAGE")) return 0;
			firstdata=0;
		}


		if (BytesPerLine==16)
		{
			// SCRAMBLE DATA to Make compatible OLD VERSION of FLASH Layout

			if (addr >= 0x80000000 && addr < 0x80000000+13*0x400)
			{
				addr += 3*0x400 - 0x80000000;
			}
			else if (addr >= 3*0x400 && addr < 64*0x400)
			{
				addr += 13*0x400;

				if (addr>64*0x400)
					addr += 0x80000000 - 64*0x400;
			}
		}


		sprintf(s,"LOADFLASH %X %X",addr,nbytes);
	}
	else
	{
		sprintf(s,"LOADDATA %X %X",addr,nbytes);
	}
	
	if (KMotionDLL->WaitToken()) return 0;
	if (KMotionDLL->WriteLine(s))
	{
		KMotionDLL->ReleaseToken();
		return 0;
	}



	for (i=0; i<nbytes; i+=BytesPerLine)
	{
		s[0]=0;

		for (k=0; k<BytesPerLine && i+k<nbytes; k++)
		{
			if (k+1<BytesPerLine && i+k+1<nbytes)
				sprintf(x,"%02X ",buffer[i+k]);
			else
				sprintf(x,"%02X",buffer[i+k]);

			strcat(s,x);
		}
		if (KMotionDLL->WriteLine(s))
		{
			KMotionDLL->ReleaseToken();
			return 0;
		}
	}

	KMotionDLL->ReleaseToken();
	return 1;
}

/****************************************************************************/
/* OTHER LOADER INTERFACE FUNCTIONS.                                        */
/****************************************************************************/
void set_reloc_amount()
{
  int i;

  for(i=0;i<n_sections;++i) reloc_amount[i] = reloc;
}

void lookup_sym(SYMENT *sym, AUXENT *aux, short indx)
{}

int load_syms(int need_reloc)
{
   return 1;
}

char *myalloc(int size)
{
   char *p = (char *)malloc(size);
   if (p) return p;

   AfxMessageBox("out of memory",MB_OK|MB_SYSTEMMODAL);
   exit(1);
}

char *mralloc(char *p, int size)
{
   p = (char *)realloc(p, size);
   if (p) return p;

   AfxMessageBox("out of memory",MB_OK|MB_SYSTEMMODAL);
   exit(1);
}

