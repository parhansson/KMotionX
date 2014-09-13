/******************************************************************************/
/* CLOAD.H                                                                    */
/******************************************************************************/

/*----------------------------------------------------------------------------*/
/* THIS MACRO IS USED TO FACILIATE ACCESS TO THE SECTION HEADERS              */
/*----------------------------------------------------------------------------*/
#define SECT_HDR(i) ((SCNHDR *)(sect_hdrs + (i) * SCNHSZ))

/*----------------------------------------------------------------------------*/
/* VARIABLES DEFINED BY THE LOADER.                                           */
/*----------------------------------------------------------------------------*/
extern int     verbose;              /* PRINT PROGRESS INFO                   */
extern int     need_symbols;         /* READ IN SYMBOL TABLE                  */
extern int     clear_bss;            /* CLEAR BSS SECTION                     */

extern FILE   *fin;                  /* COFF INPUT FILE                       */
extern FILHDR  file_hdr;             /* FILE HEADER STRUCTURE                 */
extern AOUTHDR o_filehdr;            /* OPTIONAL (A.OUT) FILE HEADER          */
extern T_ADDR  entry_point;          /* ENTRY POINT OF MODULE                 */
extern T_ADDR  reloc_amount[];       /* AMOUNT OF RELOCATION PER SECTION      */
extern char   *sect_hdrs;            /* ARRAY OF SECTION HEADERS              */
extern char   *str_buf;              /* STRING TABLE                          */
extern int     n_sections;           /* NUMBER OF SECTIONS IN THE FILE        */
extern int     big_e_target;         /* OBJECT DATA IS STORED MSB FIRST       */

/*----------------------------------------------------------------------------*/
/* LOADER FUNCTIONS.                                                          */
/*----------------------------------------------------------------------------*/
int           cload();               /* MAIN DRIVER FOR COFF LOADER           */
int           cload_headers();       /* LOAD FILE AND SECTION HEADERS         */
int           cload_data();          /* LOAD RAW DATA FROM FILE               */ 
int cload_sect_data(int s);		  	 /* LOAD DATA FROM ONE SECTION            */
int cload_cinit(					 /* PROCESS BUFFER OF C INITIALIZATION    */
   SCNHDR *sptr,                         /* CURRENT SECTION                   */
   int *packet_size,                     /* POINTER TO BUFFER SIZE            */
   int *excess);                         /* RETURNED NUMBER OF UNUSED BYTES   */
int           cload_symbols();       /* LOAD AND PROCESS COFF SYMBOL TABLE    */
									 /* READ, RELOCATE, AND SWAP A SYMBOL     */
int sym_read(unsigned int index, SYMENT *sym, AUXENT *aux);
char         *sym_name();            /* RETURN REAL NAME OF A SYMBOL          */
									 /* ADD SYMBOL TO RELOC SYMBOL TABLE      */
void reloc_add(unsigned int index,SYMENT *sym);
						             /* PERFORM A RELOCATION PATCH            */
int relocate(RELOC         *rp,      /* RELOCATION ENTRY                      */
			 unsigned char *data,    /* DATA BUFFER                           */
			 int            s);      /* INDEX OF CURRENT SECTION              */
int reloc_read(RELOC *rptr);         /* READ AND SWAP A RELOC ENTRY           */
int reloc_size(short type);          /* RETURN SIZE IN BYTES OF RELOC FIELD   */
int reloc_sym(int index) ;           /* LOOKUP A SYMBOL IN THE RELOC SYMTAB   */
									 /* EXTRACT RELOC FIELD FROM DATA         */
unsigned long unpack(unsigned char *data, int fieldsz, int wordsz,int bit_addr);                              /* BIT ADDRESS WITHIN BYTE      */

						             /* INSERT RELOCATED FIELD INTO DATA      */
void repack(unsigned long objval, unsigned char *data,
			int fieldsz, int wordsz, int bit_addr);

int           cload_lineno();        /* READ AND RELOCATE LINENO ENTRIES      */
void swap4byte(long *addr);          /* BYTE-SWAP A 32-BIT VALUE              */
void swap2byte(unsigned short *addr);/* BYTE-SWAP A 16-BIT VALUE              */


/*----------------------------------------------------------------------------*/
/* EXTERNAL FUNCTIONS (DEFINED BY APPLICATION).                               */
/*----------------------------------------------------------------------------*/
void          set_reloc_amount();    /* DETERMINE RELOCATION FOR ALL SECTIONS */
									 /* WRITE TO TARGET MEMORY                */
int mem_write(unsigned char *buffer, int nbytes, T_ADDR addr, int page);
									 /* LOOKUP AN UNDEFINED SYMBOL AND DEF IT */
void lookup_sym(SYMENT *sym, AUXENT *aux, short indx); 
int load_syms(int need_reloc);       /* BUILD THE SYMBOL TABLE                */
char		  *myalloc(int size);	 /* MEMORY ALLOCATION FUNCTIONS           */
char *mralloc(char *p, int size);
void load_msg(char *a, long b, long c, long d, long e, long f, long g);  /* WRITE OUT LOADER DEBUG INFO           */


int LoadCoff(CKMotionDLL *KMotionDLLtoUse, const char * Name, unsigned int *EntryPoint, int PackToFlash);
