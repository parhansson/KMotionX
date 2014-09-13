/**************************************************************
  
                 cload.c
  
                 staff
  
                 12-23-88
  
           (C) Texas Instruments Inc., 1992 
  
           Refer to the file 'license.txt' included with this 
           this package for usage and license information. 
  
*************************************************************/
/******************************************************************************/
/* CLOAD.C                                                                    */
/*                                                                            */
/*  This module contains functions to read in and load a COFF object file.    */
/*  The following routines are defined here:                                  */
/*                                                                            */
/*    cload()           - Main driver for COFF loader.                        */
/*    cload_headers()   - Read in the various headers of the COFF file.       */
/*    cload_data()      - Read in the raw data and load it into memory.       */
/*    cload_sect_data() - Read, relocate, and write out one section.          */
/*    cload_cinit()     - Process one buffer of C initialization records.     */
/*    cload_symbols()   - Read in the symbol table.                           */
/*    sym_read()        - Read and relocate a symbol and its aux entry.       */
/*    sym_name()        - Return a pointer to the name of a symbol.           */
/*    reloc_add()       - Add a symbol to the relocation symbol table.        */
/*    relocate()        - Perform a single relocation.                        */
/*    reloc_read()      - Read in and swap one relocation entry.              */
/*    reloc_size()      - Return the field size of a relocation type.         */
/*    reloc_sym()       - Search the relocation symbol table for a symbol.    */
/*    unpack()          - Extract a relocation field from object bytes.       */
/*    repack()          - Encode a relocated field into object bytes.         */
/*    cload_lineno()    - Read in & swap line number entries.                 */
/*    swap4byte()       - Swap the order of bytes in a long.                  */
/*    swap2byte()       - Swap the order of bytes in a short.                 */
/*                                                                            */
/*  The loader calls the following external functions to perform application  */
/*  specific tasks:                                                           */
/*                                                                            */
/*   set_reloc_amount() - Define relocation amounts for each section.         */
/*   mem_write()        - Load a buffer of data into memory.                  */
/*   lookup_sym()       - Look up a symbol in an external symbol table.       */
/*   load_syms()        - Build the symbol table for the application.         */
/*   myalloc()          - Application version of malloc().                    */
/*   mralloc()          - Application version of realloc().                   */
/*                                                                            */
/******************************************************************************/
#include "stdafx.h"

#define KMotionBd 1

#include "PARAMS.h"
#include "VERSION.h"
#include "COFF.h"
#include "CLOAD.h"


/*----------------------------------------------------------------------------*/
/* CONSTANTS, MACROS, VARIABLES, AND STRUCTURES FOR THE LOADER.               */
/*----------------------------------------------------------------------------*/
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MAXNSCNS 255                    /* MAXIMUM NUMBER OF SECTIONS         */

#define TRUE 1
#define FALSE 0

#define WORDSZ sizeof(T_DATA)           /* SIZE OF DATA UNITS IN OBJ FILE     */

FILE   *fin;                            /* INPUT FILE                         */
int     verbose      = FALSE;           /* PRINT PROGRESS INFO                */
int     need_symbols = TRUE;            /* APPLICATION NEEDS SYMBOL TABLE     */
int     clear_bss    = FALSE;           /* CLEAR BSS SECTION                  */

FILHDR  file_hdr;                       /* FILE HEADER STRUCTURE              */
AOUTHDR o_filehdr;                      /* OPTIONAL (A.OUT) FILE HEADER       */
T_ADDR  entry_point;                    /* ENTRY POINT OF MODULE              */
T_ADDR  reloc_amount[MAXNSCNS];         /* AMOUNT OF RELOCATION PER SECTION   */
char   *sect_hdrs=NULL;                 /* ARRAY OF SECTION HEADERS           */
char   *str_buf;                        /* STRING TABLE                       */
int     n_sections;                     /* NUMBER OF SECTIONS IN THE FILE     */
int     big_e_target;                   /* TARGET DATA IN BIG-ENDIAN FORMAT   */

static int     byte_swapped;            /* BYTE ORDERING OPPOSITE OF HOST     */
static T_SIZE  init_size = 0;           /* CURRENT SIZE OF C INITIALIZATION   */
static char    loadbuf[LOADBUFSIZE];    /* LOAD BUFFER                        */
static int     need_reloc = FALSE;      /* RELOCATION REQUIRED                */

/*----------------------------------------------------------------------------*/
/* THIS STRUCTURE IS USED TO STORE THE RELOCATION AMOUNTS FOR SYMBOLS.        */
/* EACH RELOCATABLE SYMBOL HAS A CORRESPONDING ENTRY IN THIS TABLE.           */
/* THE TABLE IS SORTED BY SYMBOL INDEX; LOOKUP USES A BINARY SEARCH.          */
/*----------------------------------------------------------------------------*/
typedef struct
{
   short         rt_index;              /* INDEX OF SYMBOL IN SYMBOL TABLE    */
   short         rt_scnum;              /* SECTION NUMBER SYMBOL DEFINED IN   */
   unsigned long rt_value;              /* RELOCATED VALUE OF SYMBOL          */
} RELOC_TAB;

/*----------------------------------------------------------------------------*/
/* THE RELOCATION SYMBOL TABLE IS ALLOCATED DYNAMICALLY, AND REALLOCATED      */
/* AS MORE SPACE IS NEEDED.                                                   */
/*----------------------------------------------------------------------------*/
#define RELOC_TAB_START 128             /* STARTING SIZE OF TABLE             */
#define RELOC_GROW_SIZE 128             /* REALLOC AMOUNT FOR TABLE           */

static RELOC_TAB *reloc_tab = NULL;     /* RELOCATION SYMBOL TABLE            */
static int        reloc_tab_size;       /* CURRENT ALLOCATED AMOUNT           */
static int        reloc_sym_index;      /* CURRENT SIZE OF TABLE              */


/******************************************************************************/
/*                                                                            */
/* CLOAD() - Main driver for COFF loader.                                     */
/*                                                                            */
/******************************************************************************/
int cload()
{
	int result;

	reloc_tab=NULL;
	sect_hdrs=NULL;

	result = !cload_headers() || !cload_symbols() || !cload_data();
		
	if (reloc_tab) free(reloc_tab);
	if (sect_hdrs) free(sect_hdrs);

	if (result)	return FALSE;

	return TRUE;
}

/******************************************************************************/
/*                                                                            */
/* CLOAD_HEADERS() - Read in the various headers of the COFF file.            */
/*                                                                            */
/******************************************************************************/
int cload_headers()
{
   int i;

   byte_swapped = FALSE;

   if (!fread(&file_hdr, FILHSZ, 1, fin)) return FALSE;

   /*-------------------------------------------------------------------------*/
   /* MAKE SURE THIS IS REALLY A COFF FILE. CHECK FOR SWAPPED FILES.          */
   /* DETERMINE BYTE ORDERING OF OBJECT DATA.                                 */
   /*-------------------------------------------------------------------------*/
   if (file_hdr.f_magic != MAGIC)
   {
       swap2byte(&file_hdr.f_magic);

       if (file_hdr.f_magic != MAGIC) return FALSE;

       byte_swapped = TRUE;

       swap2byte(&file_hdr.f_nscns);  swap4byte(&file_hdr.f_timdat);
       swap4byte(&file_hdr.f_symptr); swap4byte(&file_hdr.f_nsyms);
       swap2byte(&file_hdr.f_opthdr); swap2byte(&file_hdr.f_flags);
   }
   big_e_target = (file_hdr.f_flags & F_BIG);

   /*-------------------------------------------------------------------------*/
   /* READ IN OPTIONAL HEADER, IF THERE IS ONE, AND SWAP IF NEEDED.           */
   /*-------------------------------------------------------------------------*/
   if (file_hdr.f_opthdr == AOUTSZ)
   {
      if (fread(&o_filehdr, file_hdr.f_opthdr, 1, fin) != 1) return FALSE;

      if (byte_swapped)
      {
	  swap2byte((unsigned short *)&o_filehdr.magic);      swap2byte((unsigned short *)&o_filehdr.vstamp);
	  swap4byte(&o_filehdr.tsize);      swap4byte(&o_filehdr.dsize);
	  swap4byte(&o_filehdr.bsize);      swap4byte(&o_filehdr.entrypt);
	  swap4byte(&o_filehdr.text_start); swap4byte(&o_filehdr.data_start);
      }
      entry_point = o_filehdr.entrypt;
   }
   else if (file_hdr.f_opthdr && fseek(fin, (long)file_hdr.f_opthdr, 1) == -1)
      return FALSE;

   /*-------------------------------------------------------------------------*/
   /* READ IN SECTION HEADERS.                                                */
   /*-------------------------------------------------------------------------*/
   if (!(sect_hdrs = myalloc((n_sections = file_hdr.f_nscns) * SCNHSZ)) || 
       fread(sect_hdrs, SCNHSZ, n_sections, fin) != (unsigned)n_sections)
      return FALSE;

   /*-------------------------------------------------------------------------*/
   /* SWAP SECTION HEADERS IF REQUIRED.                                       */
   /*-------------------------------------------------------------------------*/
   if (byte_swapped)
   {
       for (i = 0; i < n_sections; i++)
       {
           SCNHDR *sptr = SECT_HDR(i);

           swap4byte(&sptr->s_paddr);  swap4byte(&sptr->s_vaddr);
           swap4byte(&sptr->s_size);   swap4byte(&sptr->s_scnptr);
           swap4byte(&sptr->s_relptr); swap4byte(&sptr->s_lnnoptr);
           swap4byte((long *)&sptr->s_nreloc); swap4byte((long *)&sptr->s_nlnno);
           swap4byte((long *)&sptr->s_flags);
       }
   }

   /*-------------------------------------------------------------------------*/
   /* CALL AN EXTERNAL ROUTINE TO DETERMINE THE RELOCATION AMOUNTS FOR        */
   /* EACH SECTION.                                                           */
   /*-------------------------------------------------------------------------*/
   for (i = 0; i < n_sections; i++) reloc_amount[i] = 0;
   set_reloc_amount();
   for (i = 0; i < n_sections; i++) need_reloc |= (reloc_amount[i] != 0);


   return TRUE;
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_DATA() - Read in the raw data and load it into memory.               */
/*                                                                            */
/******************************************************************************/
int cload_data()
{
   int s;
   int ok = TRUE;

   /*-------------------------------------------------------------------------*/
   /* LOOP THROUGH THE SECTIONS AND LOAD THEM ONE AT A TIME.                  */
   /*-------------------------------------------------------------------------*/
   for (s = 0; s < n_sections && ok; s++)
   {
      SCNHDR *sptr = SECT_HDR(s);                        
      int     bss_flag;

      /*----------------------------------------------------------------------*/
      /* IF THIS IS THE TEXT SECTION, RELOCATE THE ENTRY POINT.               */
      /*----------------------------------------------------------------------*/
      if ((sptr->s_flags & STYP_TEXT) && !strcmp(sptr->s_name, ".text"))
	 entry_point += reloc_amount[s];

      /*----------------------------------------------------------------------*/
      /* SET A FLAG INDICATING A BSS SECTION THAT MUST BE LOADED.             */
      /*----------------------------------------------------------------------*/
      bss_flag = (clear_bss && 
		 (sptr->s_flags & STYP_BSS) && !strcmp(sptr->s_name, ".bss"));

      /*----------------------------------------------------------------------*/
      /* IGNORE EMPTY SECTIONS OR SECTIONS WHOSE FLAGS INDICATE THE           */
      /* SECTION IS NOT TO BE LOADED.  NOTE THAT THE CINIT SECTION,           */
      /* ALTHOUGH IT HAS THE STYP_COPY FLAG SET, MUST BE LOADED.              */
      /*----------------------------------------------------------------------*/
      if (!sptr->s_scnptr && !bss_flag                                ||
	  !sptr->s_size                                               || 
          (sptr->s_flags & STYP_DSECT)                                ||
          (sptr->s_flags & STYP_COPY) && strcmp(sptr->s_name, CINIT)  ||
	  (sptr->s_flags & STYP_NOLOAD))
	 continue;

      /*----------------------------------------------------------------------*/
      /* LOAD ALL OTHER SECTIONS.                                             */
      /*----------------------------------------------------------------------*/

      ok = cload_sect_data(s);

   }
   return ok;
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_SECT_DATA() - Read, relocate, and write out the data for one section.*/
/*                                                                            */
/******************************************************************************/
int cload_sect_data(int s)  /* INDEX OF CURRENT SECTION         */
{
   SCNHDR       *sptr    = SECT_HDR(s);                       
   T_ADDR        addr    = sptr->s_vaddr; /* CURRENT ADDRESS IN SECTION       */
   unsigned long nbytes;                  /* BYTE COUNT WITHIN SECTION        */
   int           packet_size;             /* SIZE OF CURRENT DATA BUFFER      */
   int           excess  = 0;             /* BYTES LEFT FROM PREVIOUS BUFFER  */
   unsigned int  n_reloc = 0;             /* COUNTER FOR RELOCATION ENTRIES   */
   RELOC         reloc;                   /* RELOCATION ENTRY                 */
   int           bss_flag;

   /*-------------------------------------------------------------------------*/
   /* READ THE FIRST RELOCATION ENTRY, IF THERE ARE ANY.                      */
   /* IF THIS IS A BSS SECTION, CLEAR THE LOAD BUFFER.                        */
   /*-------------------------------------------------------------------------*/
   if (need_reloc && sptr->s_nreloc &&
       (fseek(fin, sptr->s_relptr, 0) == -1 || !reloc_read(&reloc)))
      return FALSE;

   if (bss_flag = (sptr->s_flags & STYP_BSS) && !strcmp(sptr->s_name, ".bss"))
      for (nbytes = 0; nbytes < LOADBUFSIZE; ++nbytes) loadbuf[nbytes] = 0;

   /*-------------------------------------------------------------------------*/
   /* COPY ALL THE DATA IN THE SECTION.                                       */
   /*-------------------------------------------------------------------------*/
   for (nbytes = 0; nbytes < (unsigned)(LOCTOBYTE(sptr->s_size)); nbytes += packet_size)
   {
      int j, ok;
      /*----------------------------------------------------------------------*/
      /* READ IN A BUFFER OF DATA.  IF THE PREVIOUS RELOCATION SPANNED        */
      /* ACCROSS THE END OF THE LAST BUFFER, COPY THE LEFTOVER BYTES INTO     */
      /* THE BEGINNING OF THE NEW BUFFER.                                     */
      /*----------------------------------------------------------------------*/
      for (j = 0; j < excess; ++j) loadbuf[j] = loadbuf[packet_size + j];

      packet_size = MIN(LOCTOBYTE(sptr->s_size) - nbytes, LOADBUFSIZE);

      if (!bss_flag &&
	  (fseek(fin, sptr->s_scnptr + nbytes + excess, 0) == -1 ||
	   fread(loadbuf + excess, packet_size - excess, 1, fin) != 1))
	 return FALSE;
      excess = 0;

      /*----------------------------------------------------------------------*/
      /* READ AND PROCESS ALL THE RELOCATION ENTRIES THAT AFFECT DATA         */
      /* CURRENTLY IN THE BUFFER.                                             */
      /*----------------------------------------------------------------------*/
      if (need_reloc)
	 while (n_reloc < sptr->s_nreloc && 
		(unsigned long)reloc.r_vaddr < addr + BYTETOLOC(packet_size))
	 {
	    int i = LOCTOBYTE(reloc.r_vaddr - addr);   /* BYTE INDEX OF FIELD */

	    /*----------------------------------------------------------------*/
	    /* IF THIS RELOCATION SPANS PAST THE END OF THE BUFFER,           */
	    /* SET 'EXCESS' TO THE NUMBER OF REMAINING BYTES AND FLUSH THE    */
	    /* BUFFER.  AT THE NEXT FILL, THESE BYTES WILL BE COPIED FROM     */
	    /* THE END OF THE BUFFER TO THE BEGINNING AND THEN RELOCATED.     */
	    /*----------------------------------------------------------------*/
	    if (i + (int)MAX(WORDSZ, reloc_size(reloc.r_type)) > packet_size)
	    { 
	       i          -= i % LOADWORDSIZE;   /* DON'T BREAK WITHIN A WORD */
	       excess      = packet_size - i; 
	       packet_size = i;
	       break;
	    }

	    /*----------------------------------------------------------------*/
	    /* PERFORM THE RELOCATION AND READ IN THE NEXT RELOCATION ENTRY.  */
	    /*----------------------------------------------------------------*/
	    relocate(&reloc, (unsigned char *)(loadbuf + i), s);

	    if (n_reloc++ < sptr->s_nreloc                                    &&
	       (fseek(fin, sptr->s_relptr + ((long)n_reloc * RELSZ), 0) == -1 ||
		 !reloc_read(&reloc))) return FALSE;
	 }

      /*----------------------------------------------------------------------*/
      /* WRITE OUT THE RELOCATED DATA TO THE TARGET DEVICE.  IF THIS IS A     */
      /* CINIT SECTION, CALL A SPECIAL FUNCTION TO HANDLE IT.                 */
      /*----------------------------------------------------------------------*/
      ok = (sptr->s_flags & STYP_COPY) && !strcmp(sptr->s_name, CINIT) ?
         cload_cinit(sptr, &packet_size, &excess) :
         mem_write((unsigned char *)loadbuf, packet_size, addr + reloc_amount[s], sptr->s_page);

      if (!ok) return FALSE;

      /*----------------------------------------------------------------------*/
      /* KEEP TRACK OF THE ADDRESS WITHIN THE SECTION.                        */
      /*----------------------------------------------------------------------*/
      addr += BYTETOLOC(packet_size);

   }
   return TRUE;
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_CINIT() - Process one buffer of C initialization records.            */
/*                                                                            */
/******************************************************************************/
int cload_cinit(
				SCNHDR *sptr,                         /* CURRENT SECTION                   */
				int *packet_size,                     /* POINTER TO BUFFER SIZE            */
				int *excess)                          /* RETURNED NUMBER OF UNUSED BYTES   */
{
	int           i;                      /* BYTE COUNTER                      */
	int           init_packet_size;       /* SIZE OF CURRENT INITIALIZATION    */
	static T_ADDR init_addr;              /* ADDRESS OF CURRENT INITIALIZATION */
	
	/*-------------------------------------------------------------------------*/
	/*  PROCESS ALL THE INITIALIZATION RECORDS IN THE BUFFER.                  */
	/*-------------------------------------------------------------------------*/
	for (i = 0; i < *packet_size; i += init_packet_size)
	{
		/*----------------------------------------------------------------------*/
		/* IF STARTING A NEW INITIALIZATION, READ THE SIZE AND ADDRESS FROM     */
		/* THE TABLE.                                                           */
		/*----------------------------------------------------------------------*/
		if (init_size == 0)
		{
			T_SIZE temp;
			
			/*-------------------------------------------------------------------*/
			/* IF THE SIZE AND ADDRESS ARE NOT FULLY CONTAINED IN THIS BUFFER,   */
			/* STOP HERE.  SET THE 'EXCESS' COUNTER TO THE NUMBER OF UNPROCESSED */
			/* BYTES - THESE WILL BE COPIED TO THE HEAD OF THE NEXT BUFFER.      */
			/*-------------------------------------------------------------------*/
			if (i + (int)sizeof(T_SIZE) > *packet_size)
			{ *excess += *packet_size - i;  *packet_size = i;  break; }
			
			/*-------------------------------------------------------------------*/
			/* IF THE NEXT SIZE FIELD IS ZERO, BREAK.                            */
			/*-------------------------------------------------------------------*/
			temp = (T_SIZE)unpack((unsigned char *)(loadbuf + i), sizeof(T_SIZE), sizeof(T_SIZE), 0);
		
			if (temp == 0) // tktk skip 0 length pads
			{
				i += sizeof(T_SIZE);
				init_packet_size=0;
			}
			else
			{

				/*-------------------------------------------------------------------*/
				/* READ THE ADDRESS FIELD, IF IT'S ALL HERE.                         */
				/*-------------------------------------------------------------------*/
				if (i + (signed)sizeof(T_SIZE) + (signed)sizeof(T_ADDR) > *packet_size)
				{ *excess += *packet_size - i;  *packet_size = i;  break; }
				
				i         += sizeof(T_SIZE);
				init_size  = temp;
				init_addr  = unpack((unsigned char *)(loadbuf + i), sizeof(T_ADDR), sizeof(T_ADDR), 0);
				i         += sizeof(T_ADDR);
			}
		}
		
		if (init_size>0)  // tktk skip 0 length pads
		{
			/*----------------------------------------------------------------------*/
			/* WRITE OUT THE CURRENT PACKET, UP TO THE END OF THE BUFFER.           */
			/*----------------------------------------------------------------------*/
			if (init_packet_size = MIN(*packet_size - i, (int)(init_size * WORDSZ)))
			{
				if (!mem_write((unsigned char *)(loadbuf + i), init_packet_size, init_addr, sptr->s_page))
					return FALSE;
				
				init_addr += BYTETOLOC(init_packet_size);
				init_size -= init_packet_size / WORDSZ;
			}
		}
	}
	return TRUE;
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_SYMBOLS() - Read in the symbol table.                                */
/*                                                                            */
/******************************************************************************/
int cload_symbols()
{
   long str_size;
   int first, next;
   SYMENT sym;
   AUXENT aux;

   if (!need_symbols && !need_reloc) return TRUE;

   /*------------------------------------------------------------------------*/
   /* ALLOCATE THE RELOCATION SYMBOL TABLE.                                  */
   /*------------------------------------------------------------------------*/
   if (need_reloc)
   {
      reloc_tab_size = MIN(RELOC_TAB_START, file_hdr.f_nsyms);
      reloc_tab = (RELOC_TAB *)(myalloc(reloc_tab_size * sizeof(RELOC_TAB)));
      reloc_sym_index = 0;
   }

   /*------------------------------------------------------------------------*/
   /* SEEK TO THE END OF THE SYMBOL TABLE AND READ IN THE SIZE OF THE STRING */
   /* TABLE. THEN, ALLOCATE AND READ THE STRING TABLE.                       */
   /*------------------------------------------------------------------------*/
   if (fseek(fin, file_hdr.f_symptr + (file_hdr.f_nsyms * SYMESZ), 0) == -1 ||
       fread(&str_size, sizeof(long), 1, fin) != 1)
       str_size = 0;
   else 
   {
       if (byte_swapped) swap4byte(&str_size);
       str_buf = myalloc((int)str_size + 4);

       if (fread(str_buf + 4, (int)str_size - 4, 1, fin) != 1) return FALSE;
   }


   /*------------------------------------------------------------------------*/
   /* IF THE APPLICATION NEEDS THE SYMBOL TABLE, LET IT READ IT IN.          */
   /* PASS NEED_RELOC TO THE APPLICATION SO THAT IT CAN CALL RELOC_ADD()     */
   /*------------------------------------------------------------------------*/
   if (need_symbols) return load_syms(need_reloc);

   /*------------------------------------------------------------------------*/
   /*  READ THE SYMBOL TABLE AND BUILD THE RELOCATION SYMBOL TABLE           */
   /*  FOR SYMBOLS THAT CAN BE USED IN RELCOATION, STORE THEM IN A           */
   /*  SPECIAL SYMBOL TABLE THAT CAN BE SEARCHED QUICKLY DURING              */
   /*  RELOCATION.                                                           */
   /*------------------------------------------------------------------------*/
   for (first = 0; first < file_hdr.f_nsyms; first = next)
   {
       if (!(next = sym_read(first, &sym, &aux))) return FALSE;

       if (sym.n_sclass == C_EXT   || sym.n_sclass == C_EXTDEF ||
           sym.n_sclass == C_STAT  || sym.n_sclass == C_LABEL  ||
           sym.n_sclass == C_BLOCK || sym.n_sclass == C_FCN) 
	  reloc_add(first, &sym);
   }
   return TRUE;
}


/******************************************************************************/
/*                                                                            */
/* SYM_READ() - Read and relocate a symbol and its aux entry.  Return the     */
/*              index of the next symbol.                                     */
/*                                                                            */
/******************************************************************************/
int sym_read(unsigned int index, SYMENT *sym, AUXENT *aux)
{
    /*------------------------------------------------------------------------*/
    /* READ IN A SYMBOL AND ITS AUX ENTRY.                                    */
    /*------------------------------------------------------------------------*/
    if (fseek(fin, file_hdr.f_symptr + ((long)index * SYMESZ), 0) == -1 ||
        fread(sym, SYMESZ, 1, fin) != 1                                 ||
        (sym->n_numaux && fread(aux, SYMESZ, 1, fin) != 1)) return FALSE;

    if (byte_swapped)
    {
        /*--------------------------------------------------------------------*/
	/* SWAP THE SYMBOL TABLE ENTRY.                                       */
        /*--------------------------------------------------------------------*/
        if (sym->n_zeroes == 0) swap4byte((long *)(&sym->n_nptr));
        swap4byte(&sym->n_value);
        swap2byte((unsigned short *)&sym->n_scnum);
        swap2byte((unsigned short *)&sym->n_type);

        /*--------------------------------------------------------------------*/
	/* SWAP THE AUX ENTRY, BASED ON THE STORAGE CLASS.                    */
        /*--------------------------------------------------------------------*/
        if (sym->n_numaux) switch(sym->n_sclass)
        {
          case C_FILE    : break;

          case C_STRTAG  :
          case C_UNTAG   :
          case C_ENTAG   : swap2byte(&aux->x_sym.x_misc.x_lnsz.x_size);
                           swap4byte(&aux->x_sym.x_fcnary.x_fcn.x_endndx);
                           break;

          case C_FCN     :
          case C_BLOCK   : swap2byte(&aux->x_sym.x_misc.x_lnsz.x_lnno);
                           swap4byte(&aux->x_sym.x_fcnary.x_fcn.x_endndx);
                           break;

          case C_EOS     : swap2byte(&aux->x_sym.x_misc.x_lnsz.x_size);
                           swap4byte(&aux->x_sym.x_tagndx);
                           break;

          default        : /*-------------------------------------------------*/
                           /* HANDLE FUNCTION DEFINITION SYMBOL               */
                           /*-------------------------------------------------*/
                           if (((sym->n_type >> 4) & 3) == DT_FCN)
                           {
                               swap4byte(&aux->x_sym.x_tagndx);
                               swap4byte(&aux->x_sym.x_misc.x_fsize);
                               swap4byte(&aux->x_sym.x_fcnary.x_fcn.x_lnnoptr);
                               swap4byte(&aux->x_sym.x_fcnary.x_fcn.x_endndx);
                               swap2byte(&aux->x_sym.x_regcount);
                           }

                           /*-------------------------------------------------*/
                           /* HANDLE ARRAYS.                                  */
                           /*-------------------------------------------------*/
                           else if (((sym->n_type >> 4) & 3) == DT_ARY)
                           {
                               swap4byte(&aux->x_sym.x_tagndx);
                               swap2byte(&aux->x_sym.x_misc.x_lnsz.x_lnno);
                               swap2byte(&aux->x_sym.x_misc.x_lnsz.x_size);
                               swap2byte(&aux->x_sym.x_fcnary.x_ary.x_dimen[0]);
                               swap2byte(&aux->x_sym.x_fcnary.x_ary.x_dimen[1]);
                               swap2byte(&aux->x_sym.x_fcnary.x_ary.x_dimen[2]);
                               swap2byte(&aux->x_sym.x_fcnary.x_ary.x_dimen[3]);
                           }

                           /*-------------------------------------------------*/
                           /* HANDLE SECTION DEFINITIONS                      */
                           /*-------------------------------------------------*/
                           else if (sym->n_type == 0)
                           {
                               swap4byte(&aux->x_scn.x_scnlen);
                               swap2byte(&aux->x_scn.x_nreloc);
                               swap2byte(&aux->x_scn.x_nlinno);
                           }

                           /*-------------------------------------------------*/
                           /* HANDLE MISC SYMBOL RECORD                       */
                           /*-------------------------------------------------*/
                           else
                           {
                               swap2byte(&aux->x_sym.x_misc.x_lnsz.x_size);
                               swap4byte(&aux->x_sym.x_tagndx);
                           }
        }
    }

    /*------------------------------------------------------------------------*/
    /* RELOCATE THE SYMBOL, BASED ON ITS STORAGE CLASS.                       */
    /*------------------------------------------------------------------------*/
    switch(sym->n_sclass)
    {
       case C_EXT     :
       case C_EXTDEF  :
       case C_STAT    :
       case C_LABEL   :
       case C_BLOCK   :
       case C_FCN     : 
	  /*------------------------------------------------------------------*/
	  /* IF THE SYMBOL IS UNDEFINED, CALL AN APPLICATION ROUTINE TO LOOK  */
	  /* IT UP IN AN EXTERNAL SYMBOL TABLE.  IF THE SYMBOL IS DEFINED,    */
	  /* RELOCATE IT ACCORDING TO THE SECTION IT IS DEFINED IN.           */
	  /*------------------------------------------------------------------*/
          if (sym->n_scnum == 0) 
	     lookup_sym((struct syment *)index, (union auxent *)sym, (short)aux);  // tktk ??
          else if (sym->n_scnum > 0) 
	     sym->n_value += reloc_amount[sym->n_scnum - 1];
    }
    return (index + sym->n_numaux + 1);
}


/******************************************************************************/
/*                                                                            */
/* SYM_NAME() - Return a pointer to the name of a symbol in either the symbol */
/*              entry or the string table.                                    */
/*                                                                            */
/******************************************************************************/
char *sym_name(SYMENT *symptr)
{
    static char temp[9];

    if (symptr->n_zeroes == 0) return(str_buf + symptr->n_offset);

    strncpy(temp, symptr->n_name, 8);
    temp[8] = 0;
    return temp;
}


/******************************************************************************/
/*                                                                            */
/* RELOC_ADD() - Add an entry to the relocation symbol table.  This table     */
/*               stores relocation information for each relocatable symbol.   */
/*                                                                            */
/******************************************************************************/
void reloc_add(unsigned int index,SYMENT *sym)
{
   if (!need_reloc) return;

   if (reloc_sym_index >= reloc_tab_size)
   {
      reloc_tab_size += RELOC_GROW_SIZE;
      reloc_tab = (RELOC_TAB * )mralloc((char *)reloc_tab, reloc_tab_size *
                                                   sizeof(RELOC_TAB));
   }
   reloc_tab[reloc_sym_index  ].rt_index = index;
   reloc_tab[reloc_sym_index  ].rt_scnum = sym->n_scnum;
   reloc_tab[reloc_sym_index++].rt_value = sym->n_value;
}


/******************************************************************************/
/*                                                                            */
/* RELOCATE() - Perform a single relocation by patching the raw data.         */
/*                                                                            */
/******************************************************************************/
int relocate(RELOC         *rp,         /* RELOCATION ENTRY                   */
			 unsigned char *data,       /* DATA BUFFER                        */
			 int            s)          /* INDEX OF CURRENT SECTION           */
{
   int fieldsz = reloc_size(rp->r_type);     /* SIZE OF ACTUAL PATCH VALUE    */
   int wordsz  = MAX(fieldsz, WORDSZ);       /* SIZE OF CONTAINING FIELD      */
   long objval;                              /* FIELD TO BE PATCHED           */
   long reloc_amt;                           /* AMOUNT OF RELOCATION          */

   /*-------------------------------------------------------------------------*/
   /* LOOK UP THE SYMBOL BEING REFERENCED IN THE RELOCATION SYMBOL TABLE.     */
   /* USE THE SYMBOL VALUE TO CALCULATE THE RELOCATION AMOUNT:                */
   /* 1) IF THE SYM INDEX IS -1 (INTERNAL RELOCATION) USE THE RELOCATION      */
   /*    AMOUNT OF THE CURRENT SECTION.                                       */
   /* 2) IF THE SYMBOL WAS UNDEFINED (DEFINED IN SECTION 0), USE THE          */
   /*    SYMBOL'S VALUE.                                                      */
   /* 3) IF THE SYMBOL HAS A POSITIVE SECTION NUMBER, USE THE RELOCATION      */
   /*    AMOUNT FOR THE SECTION IN WHICH THE SYMBOL IS DEFINED.               */
   /* 4) OTHERWISE, THE SYMBOL IS ABSOLUTE, SO THE RELOCATION AMOUNT IS 0.    */
   /*-------------------------------------------------------------------------*/
   if (rp->r_symndx == -1) reloc_amt = reloc_amount[s];                         
   else 
   {
      int rt_index = reloc_sym(rp->r_symndx);       /* INDEX IN RELOC TABLE   */
      int sect_ref = reloc_tab[rt_index].rt_scnum;  /* SECTION WHERE DEFINED  */
      reloc_amt    = sect_ref == 0 ? reloc_tab[rt_index].rt_value 
		   : sect_ref >  0 ? reloc_amount[sect_ref - 1]
				   : 0;
   }

   /*-------------------------------------------------------------------------*/
   /* EXTRACT THE RELOCATABLE FIELD FROM THE OBJECT DATA.                     */
   /*-------------------------------------------------------------------------*/
   objval = unpack(data, fieldsz, wordsz, BIT_OFFSET(rp->r_vaddr));

   /*-------------------------------------------------------------------------*/
   /* MODIFY THE FIELD BASED ON THE RELOCATION TYPE.                          */
   /*-------------------------------------------------------------------------*/
   switch (rp->r_type)
   {
      /*----------------------------------------------------------------------*/
      /* NORMAL RELOCATIONS: ADD IN THE RELOCATION AMOUNT.                    */
      /*----------------------------------------------------------------------*/
      case R_RELBYTE:
      case R_RELWORD:
      case R_REL24:
      case R_RELLONG:
      case R_PARTLS16:
	 objval += reloc_amt;
	 break;

      /*----------------------------------------------------------------------*/
      /* 34010 ONE'S COMPLEMENT RELOCATION.  SUBTRACT INSTEAD OF ADD.         */
      /*----------------------------------------------------------------------*/
      case R_OCRLONG:
	 objval -= reloc_amt;
	 break;

      /*----------------------------------------------------------------------*/
      /* 34020 WORD-SWAPPED RELOCATION.  SWAP BEFORE RELOCATING.              */
      /*----------------------------------------------------------------------*/
      case R_GSPOPR32:
          objval  = ((objval >> 16) & 0xFFFF) | (objval << 16); 
          objval += reloc_amt;
          objval  = ((objval >> 16) & 0xFFFF) | (objval << 16);
          break; 

      /*----------------------------------------------------------------------*/
      /* PC-RELATIVE RELOCATIONS.  IN THIS CASE THE RELOCATION AMOUNT         */
      /* IS ADJUSTED BY THE PC DIFFERENCE.   IF THIS IS AN INTERNAL           */
      /* RELOCATION TO THE CURRENT SECTION, NO ADJUSTMENT IS NEEDED.          */
      /*----------------------------------------------------------------------*/
      case R_PCRBYTE:
      case R_PCRWORD:
      case R_GSPPCR16:
      case R_PCRLONG:
	 if (rp->r_symndx != -1)                /* IF NOT INTERNAL RELOCATION */
	 {
            int shift  = 8 * (4 - fieldsz);
	    objval     = (long)(objval << shift) >> shift;   /* SIGN EXTEND   */
	    reloc_amt -= reloc_amount[s];            

            if (rp->r_type == R_GSPPCR16) reloc_amt >>= 4;   /* BITS TO WORDS */

	    objval += reloc_amt;
	 }
         break;

      /*----------------------------------------------------------------------*/
      /* 320C30 PAGE-ADDRESSING RELOCATION.  CALCULATE THE ADDRESS FROM       */
      /* THE 8-BIT PAGE VALUE IN THE FIELD, THE 16-BIT OFFSET IN THE RELOC    */
      /* ENTRY, AND THE RELOCATION AMOUNT.  THEN, STORE THE 8-BIT PAGE        */
      /* VALUE OF THE RESULT BACK IN THE FIELD.                               */
      /*----------------------------------------------------------------------*/
      case R_PARTMS8:
	  objval = ((objval << 16) + rp->r_disp + reloc_amt) >> 16;
	  break;

      /*----------------------------------------------------------------------*/
      /* DSP(320) PAGE-ADDRESSING.  CALCULATE ADDRESS FROM THE 16-BIT         */
      /* VALUE IN THE RELOCATION FIELD PLUS THE RELOCATION AMOUNT.  OR THE    */
      /* TOP 9 BITS OF THIS RESULT INTO THE RELOCATION FIELD.                 */
      /*----------------------------------------------------------------------*/
      case R_PARTMS9:
	  objval = (objval & 0xFE00) | 
		   (((rp->r_disp + reloc_amt) >> 7) & 0x1FF);
          break;

      /*----------------------------------------------------------------------*/
      /* DSP(320) PAGE-ADDRESSING.  CALCULATE ADDRESS AS ABOVE, AND OR THE    */
      /* 7-BIT DISPLACEMENT INTO THE FIELD.                                   */
      /*----------------------------------------------------------------------*/
      case R_PARTLS7:
	  objval = (objval & 0x80) | ((rp->r_disp + reloc_amt) & 0x7F);
	  break;

      /*----------------------------------------------------------------------*/
      /* DSP(320) 13-BIT CONSTANT.  RELOCATE ONLY THE LOWER 13 BITS OF THE    */
      /* FIELD.                                                               */
      /*----------------------------------------------------------------------*/
      case R_REL13:
          objval = (objval & 0xE000) | ((objval + reloc_amt) & 0x1FFF);
          break;

   }

   /*-------------------------------------------------------------------------*/
   /* PACK THE RELOCATED FIELD BACK INTO THE OBJECT DATA.                     */
   /*-------------------------------------------------------------------------*/
   repack(objval, data, fieldsz, wordsz, 0);
   
   return TRUE;
} 


/******************************************************************************/
/*                                                                            */
/* RELOC_READ() - Read a single relocation entry.                             */
/*                                                                            */
/******************************************************************************/
int reloc_read(RELOC *rptr)
{

   if (fread(rptr, RELSZ, 1, fin) != 1) return FALSE;

   if (byte_swapped)
   {
      swap4byte(&rptr->r_vaddr);  swap2byte((unsigned short *)&rptr->r_symndx);
      swap2byte(&rptr->r_disp);   swap2byte(&rptr->r_type);  
   }
   return TRUE;
}



/******************************************************************************/
/*                                                                            */
/* RELOC_SIZE() - Return the field size in bytes of a relocation type.        */
/*                                                                            */
/******************************************************************************/
int reloc_size(short type)
{
   switch (type)
   {
       case R_RELBYTE:
       case R_PCRBYTE:
       case R_PARTMS8:
       case R_PARTLS7:      return 1;

       case R_RELWORD:
       case R_PCRWORD:
       case R_GSPPCR16:
       case R_PARTLS16:   
       case R_PARTMS9:
       case R_REL13:        return 2;

       case R_REL24:        return 3;             

       case R_GSPOPR32:
       case R_RELLONG:
       case R_PCRLONG:
       case R_OCRLONG:      return 4;           

       default:             return 0;
   }
}


/******************************************************************************/
/*                                                                            */
/* RELOC_SYM() - Search the relocation symbol table for a symbol with the     */
/*               specified index.                                             */
/*                                                                            */
/******************************************************************************/
int reloc_sym(int index)
{
   int i = 0,
       j = reloc_sym_index - 1;

   /*-------------------------------------------------------------------------*/
   /* THIS IS A SIMPLE BINARY SEARCH (THE RELOC TABLE IS ALWAYS SORTED).      */
   /*-------------------------------------------------------------------------*/
   while (i <= j)
   {
      int m = (i + j) / 2;
      if      (reloc_tab[m].rt_index < index) i = m + 1;
      else if (reloc_tab[m].rt_index > index) j = m - 1;
      else return m; 
   }

   ASSERT(FALSE);

   return 0;
}


/******************************************************************************/
/*                                                                            */
/*  UNPACK() - Extract a relocation field from object bytes and convert into  */
/*             a long so it can be relocated.                                 */
/*                                                                            */
/******************************************************************************/
unsigned long unpack(unsigned char *data, int fieldsz, int wordsz,int bit_addr)                              /* BIT ADDRESS WITHIN BYTE      */
{
   register int i;
   int r = bit_addr;                          /* RIGHT SHIFT VALUE            */
   int l = 8 - r;                             /* LEFT SHIFT VALUE             */
   unsigned long objval = 0;

   if (!big_e_target)
   {
       /*---------------------------------------------------------------------*/
       /* TAKE THE FIRST 'fieldsz' BYTES AND CONVERT                          */
       /*---------------------------------------------------------------------*/
       if (r == 0)
          for (i = fieldsz - 1; i >= 0; --i) 
	     objval = (objval << 8) | data[i];
       else
       {
          for (i = fieldsz; i > 0; --i)
	     objval = (objval << 8) | (data[i] << l) | (data[i-1] >> r);
    
          data[0]       = (data[0]       << l) >> l;
          data[fieldsz] = (data[fieldsz] >> r) << r;
       }
   }
   else      /* MSBFIRST */
   {
       /*---------------------------------------------------------------------*/
       /* TAKE THE LAST 'fieldsz' BYTES.                                      */
       /*---------------------------------------------------------------------*/
       if (r == 0)
          for (i = wordsz - fieldsz; i < wordsz; ++i) 
	     objval = (objval << 8) | data[i];
       else
       {
          int firstbyte = wordsz - fieldsz;

          for (i = firstbyte; i < wordsz; ++i)
	     objval = (objval << 8) | (data[i] << r) | (data[i+1] >> l);

          data[firstbyte] = (data[firstbyte] >> l) << l;
          data[wordsz]    = (data[wordsz]    << r) >> r;
       }
   }
   return objval;
}


/******************************************************************************/
/*                                                                            */
/* REPACK() - Encode a binary relocated field back into the object data.      */
/*                                                                            */
/******************************************************************************/
void repack(unsigned long objval, unsigned char *data,
			int fieldsz, int wordsz, int bit_addr)  
{
   register int i;
   int r = bit_addr;                          /* RIGHT SHIFT VALUE            */
   int l = 8 - r;                             /* LEFT SHIFT VALUE             */

   if (!big_e_target)
   {
       /*---------------------------------------------------------------------*/
       /* ENCODE LSB OF VALUE FIRST                                           */
       /*---------------------------------------------------------------------*/
       if (r == 0)
          for (i = 0; i < fieldsz; objval >>= 8) data[i++] = (unsigned char)objval;
       else
       {
          data[0] |= (objval << r);
          objval  >>= l;
          for (i = 1; i < fieldsz; objval >>= 8)  data[i++] = (unsigned char)objval;
          data[fieldsz] |= objval;
       }
   }
   else   /* MSBFIRST */
   {
       /*---------------------------------------------------------------------*/
       /* ENCODE MSB OF VALUE FIRST                                           */
       /*---------------------------------------------------------------------*/
       if (r == 0)                       
          for (i = wordsz - 1; i >= wordsz - fieldsz; objval >>= 8)
	     data[i--] = (unsigned char)objval;               
       else 
       {
          int firstbyte = wordsz - fieldsz;

          data[wordsz] |= (unsigned char)(objval << l); 
          objval >>= r;
          for (i = wordsz - 1; i > firstbyte; objval >>= 8) data[i--] = (unsigned char)(unsigned char)objval;
          data[firstbyte] |= objval; 
       }
   }
}


/******************************************************************************/
/*                                                                            */
/* CLOAD_LINENO() - Read in, swap, and relocate line number entries.  This    */
/*                  function is not called directly by the loader, but by the */
/*                  application when it needs to read in line number entries. */
/*                                                                            */
/******************************************************************************/
int cload_lineno(
   long    filptr,                     /* WHERE TO READ FROM                  */
   int     count,                      /* HOW MANY TO READ                    */
   LINENO *lptr,                       /* WHERE TO PUT THEM                   */
   int     scnum)                      /* SECTION NUMBER OF THESE ENTRIES     */
{
    int i;

    /*------------------------------------------------------------------------*/
    /* READ IN THE REQUESTED NUMBER OF LINE NUMBER ENTRIES AS A BLOCK.        */
    /*------------------------------------------------------------------------*/
    if (fseek(fin, filptr, 0) == -1 ||
        fread(lptr, count, LINESZ, fin) != LINESZ) return FALSE;

    /*------------------------------------------------------------------------*/
    /* SWAP AND RELOCATE EACH ENTRY, AS NECESSARY.                            */
    /*------------------------------------------------------------------------*/
    if (byte_swapped || reloc_amount[scnum - 1])
       for (i = 0; i < count; i++)
       {
	  if (byte_swapped)
	  {
	      swap2byte(&lptr->l_lnno);
	      swap4byte(&lptr->l_addr.l_paddr);
	  }

	  if (lptr->l_lnno != 0) 
	     lptr->l_addr.l_paddr += reloc_amount[scnum - 1];

	  lptr = (LINENO *) (((char *)lptr) + LINESZ);
       }
    
    return TRUE;
}


/******************************************************************************/
/*                                                                            */
/*  SWAP4BYTE() - Swap the order of bytes in a long.                          */
/*                                                                            */
/******************************************************************************/
void swap4byte(long *addr)
{
    long temp1, temp2, temp3, temp4;

    temp1 = (*addr)       & 0xFF;
    temp2 = (*addr >> 8)  & 0xFF;
    temp3 = (*addr >> 16) & 0xFF;
    temp4 = (*addr >> 24) & 0xFF;

    *addr = (temp1 << 24) | (temp2 << 16) | (temp3 << 8) | temp4;
}


/******************************************************************************/
/*                                                                            */
/*  SWAP2BYTE() - Swap the order of bytes in a short.                         */
/*                                                                            */
/******************************************************************************/
void swap2byte(unsigned short *addr)
{
   unsigned short temp1,temp2;

   temp1 = temp2 = *addr;
   *addr = ((temp2 & 0xFF) << 8) | ((temp1 >> 8) & 0xFF);
}
