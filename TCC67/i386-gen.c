/*
 *  X86 code generator for TCC
 * 
 *  Copyright (c) 2001, 2002 Fabrice Bellard
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define DO_C67 1


#if (DO_C67)
/* number of available registers */
#define NB_REGS            24

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#define RC_INT     0x0001 /* generic integer register */
#define RC_FLOAT   0x0002 /* generic float register */
#define RC_EAX     0x0004
#define RC_ST0     0x0008 
#define RC_ECX     0x0010
#define RC_EDX     0x0020
#define RC_INT_BSIDE  0x00000040 /* generic integer register  on b side */
#define RC_C67_A4     0x00000100
#define RC_C67_A5     0x00000200
#define RC_C67_B4     0x00000400
#define RC_C67_B5     0x00000800
#define RC_C67_A6     0x00001000
#define RC_C67_A7     0x00002000
#define RC_C67_B6     0x00004000
#define RC_C67_B7     0x00008000
#define RC_C67_A8     0x00010000
#define RC_C67_A9     0x00020000
#define RC_C67_B8     0x00040000
#define RC_C67_B9     0x00080000
#define RC_C67_A10    0x00100000
#define RC_C67_A11    0x00200000
#define RC_C67_B10    0x00400000
#define RC_C67_B11    0x00800000
#define RC_C67_A12    0x01000000
#define RC_C67_A13    0x02000000
#define RC_C67_B12    0x04000000
#define RC_C67_B13    0x08000000
#define RC_IRET    RC_C67_A4 /* function return: integer register */
#define RC_LRET    RC_C67_A5 /* function return: second integer register */
#define RC_FRET    RC_C67_A4 /* function return: float register */

/* pretty names for the registers */
enum {
    TREG_EAX = 0,  // really A2
    TREG_ECX,      // really A3
    TREG_EDX,      // really B0
    TREG_ST0,      // really B1
	TREG_C67_A4,
	TREG_C67_A5,
	TREG_C67_B4,
	TREG_C67_B5,
	TREG_C67_A6,
	TREG_C67_A7,
	TREG_C67_B6,
	TREG_C67_B7,
	TREG_C67_A8,
	TREG_C67_A9,
	TREG_C67_B8,
	TREG_C67_B9,
	TREG_C67_A10,
	TREG_C67_A11,
	TREG_C67_B10,
	TREG_C67_B11,
	TREG_C67_A12,
	TREG_C67_A13,
	TREG_C67_B12,
	TREG_C67_B13,
};

int reg_classes[NB_REGS] = {
    /* eax  */ RC_INT |                RC_FLOAT | RC_EAX,  // only allow even regs for floats (allow for doubles)
    /* ecx  */ RC_INT |                           RC_ECX,
    /* edx  */ RC_INT | RC_INT_BSIDE | RC_FLOAT | RC_EDX,  // only allow even regs for floats (allow for doubles)
    /* st0  */ RC_INT | RC_INT_BSIDE |            RC_ST0,
    /* A4   */ RC_C67_A4,
    /* A5   */ RC_C67_A5,
    /* B4   */ RC_C67_B4,
    /* B5   */ RC_C67_B5,
    /* A6   */ RC_C67_A6,
    /* A7   */ RC_C67_A7,
    /* B6   */ RC_C67_B6,
    /* B7   */ RC_C67_B7,
    /* A8   */ RC_C67_A8,
    /* A9   */ RC_C67_A9,
    /* B8   */ RC_C67_B8,
    /* B9   */ RC_C67_B9,
    /* A10  */ RC_C67_A10,
    /* A11  */ RC_C67_A11,
    /* B10  */ RC_C67_B10,
    /* B11  */ RC_C67_B11,
    /* A12  */ RC_C67_A12,
    /* A13  */ RC_C67_A13,
    /* B12  */ RC_C67_B12,
    /* B13  */ RC_C67_B13
};

/* return registers for function */
#define REG_IRET TREG_C67_A4 /* single word int return register */
#define REG_LRET TREG_C67_A5 /* second word return register (for long long) */
#define REG_FRET TREG_C67_A4 /* float return register */





// although tcc thinks it is passing parameters on the stack,
// the C67 really passes up to the first 10 params in special
// regs or regs pairs (for 64 bit params).  So keep track of
// the stack offsets so we can translate to the appropriate 
// reg (pair)


#define NoCallArgsPassedOnStack 10
int NoOfCurFuncArgs;
int TranslateStackToReg[NoCallArgsPassedOnStack];
int ParamLocOnStack[NoCallArgsPassedOnStack];
int TotalBytesPushedOnStack;




// intrinsic functions

TokenSym *ts_divi;
TokenSym *ts_divu;
TokenSym *ts_divf;
TokenSym *ts_divd;
TokenSym *ts_remi;
TokenSym *ts_remu;




#else

/* number of available registers */
#define NB_REGS             4

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#define RC_INT     0x0001 /* generic integer register */
#define RC_FLOAT   0x0002 /* generic float register */
#define RC_EAX     0x0004
#define RC_ST0     0x0008 
#define RC_ECX     0x0010
#define RC_EDX     0x0020
#define RC_IRET    RC_EAX /* function return: integer register */
#define RC_LRET    RC_EDX /* function return: second integer register */
#define RC_FRET    RC_ST0 /* function return: float register */

/* pretty names for the registers */
enum {
    TREG_EAX = 0,
    TREG_ECX,
    TREG_EDX,
    TREG_ST0,
};

int reg_classes[NB_REGS] = {
    /* eax */ RC_INT | RC_EAX,
    /* ecx */ RC_INT | RC_ECX,
    /* edx */ RC_INT | RC_EDX,
    /* st0 */ RC_FLOAT | RC_ST0,
};

/* return registers for function */
#define REG_IRET TREG_EAX /* single word int return register */
#define REG_LRET TREG_EDX /* second word return register (for long long) */
#define REG_FRET TREG_ST0 /* float return register */



#endif


/* defined if function parameters must be evaluated in reverse order */

#if (!DO_C67)
	#define INVERT_FUNC_PARAMS
#endif

/* defined if structures are passed as pointers. Otherwise structures
   are directly pushed on stack. */
//#define FUNC_STRUCT_PARAM_AS_PTR

/* pointer size, in bytes */
#define PTR_SIZE 4

/* long double size and alignment, in bytes */
#define LDOUBLE_SIZE  8
#define LDOUBLE_ALIGN 4
/* maximum alignment (for aligned attribute support) */
#define MAX_ALIGN     8

/* relocation type for 32 bit data relocation */
#define R_DATA_32 R_386_32

/* function call context */
typedef struct GFuncContext {
    int args_size;
    int args_count;
    int func_call; /* func call type (FUNC_STDCALL or FUNC_CDECL) */
#if (DO_C67)
    int func_type; /* parameter type (FUNC_NEW or FUNC_OLD or FUNC_ELLIPSIS ... */
	int args_sizes[NoCallArgsPassedOnStack];
#endif
} GFuncContext;

/******************************************************/

static unsigned long func_sub_sp_offset;
static unsigned long func_bound_offset;
static int func_ret_sub;


static BOOL	C67_invert_test;
static int C67_compare_reg;


/* XXX: make it faster ? */
void g(int c)
{
#if (DO_C67)
	ALWAYS_ASSERT(FALSE);
#endif

    int ind1;
    ind1 = ind + 1;
    if (ind1 > (int)cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind] = c;
    ind = ind1;
}

#ifdef ASSEMBLY_LISTING_C67
FILE *f=NULL;
#endif


void C67_g(int c)
{
    int ind1;

#ifdef ASSEMBLY_LISTING_C67
	fprintf(f," %08X",c);
#endif

#if (!DO_C67)
	ALWAYS_ASSERT(FALSE);
#endif

    
	ind1 = ind + 4;
    if (ind1 > (int)cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind]   =      c & 0xff;
    cur_text_section->data[ind+1] =  (c>>8) & 0xff;
    cur_text_section->data[ind+2] = (c>>16) & 0xff;
    cur_text_section->data[ind+3] = (c>>24) & 0xff;
    ind = ind1;
}


void o(int c)
{
    while (c) {
        g(c);
        c = c / 256;
    }
}

void gen_le32(int c)
{
    g(c);
    g(c >> 8);
    g(c >> 16);
    g(c >> 24);
}

/* output a symbol and patch all calls to it */
void gsym_addr(int t, int a)
{
    int n, *ptr;
    while (t) {
        ptr = (int *)(cur_text_section->data + t);

		#if (DO_C67)
		{
			Sym *sym;

			// extract 32 bit address from MVKH/MVKL
			n =  ((*ptr     >> 7) & 0xffff);
			n |= ((*(ptr+1) >> 7) & 0xffff)<<16;

			// define a label that will be relocated

			sym = get_sym_ref(&char_pointer_type, cur_text_section, a, 0);
	        greloc(cur_text_section, sym, t,   R_C60LO16);
	        greloc(cur_text_section, sym, t+4, R_C60HI16);

			// clear out where the pointer was

			*ptr     &= ~(0xffff << 7);
			*(ptr+1) &= ~(0xffff << 7);
		}
		#else
			n = *ptr; /* next value */
			*ptr = a - t - 4;
		#endif
        t = n;
    }
}

void gsym(int t)
{
    gsym_addr(t, ind);
}

/* psym is used to put an instruction with a data field which is a
   reference to a symbol. It is in fact the same as oad ! */
#define psym oad

/* instruction + 4 bytes data. Return the address of the data */
static int oad(int c, int s)
{
    int ind1;

    o(c);
    ind1 = ind + 4;
    if (ind1 > (int)cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    *(int *)(cur_text_section->data + ind) = s;
    s = ind;
    ind = ind1;
    return s;
}

/* output constant with relocation if 'r & VT_SYM' is true */
static void gen_addr32(int r, Sym *sym, int c)
{
    if (r & VT_SYM)
        greloc(cur_text_section, sym, ind, R_386_32);
    gen_le32(c);
}

/* generate a modrm reference. 'op_reg' contains the addtionnal 3
   opcode bits */
static void gen_modrm(int op_reg, int r, Sym *sym, int c)
{
    op_reg = op_reg << 3;
    if ((r & VT_VALMASK) == VT_CONST) {
        /* constant memory reference */
        o(0x05 | op_reg);
        gen_addr32(r, sym, c);
    } else if ((r & VT_VALMASK) == VT_LOCAL) {
        /* currently, we use only ebp as base */
        if (c == (char)c) {
            /* short reference */
            o(0x45 | op_reg);
            g(c);
        } else {
            oad(0x85 | op_reg, c);
        }
    } else {
        g(0x00 | op_reg | (r & VT_VALMASK));
    }
}

// these are regs that tcc doesn't really know about, 
// but asign them unique values so the mapping routines
// can distinquish them

#define C67_A0 105
#define C67_SP 106
#define C67_B3 107
#define C67_FP 108
#define C67_B2 109
#define C67_CREG_ZERO -1 // Special code for no condition reg test


int ConvertRegToRegClass(int r)
{
	// only works for A4-B13

	return RC_C67_A4 << (r-TREG_C67_A4);
}


// map TCC reg to C67 reg number

int C67_map_regn(int r)
{
	if (r==0)        // normal tcc regs
		return 0x2;  // A2
	else if (r==1)   // normal tcc regs
		return 3;    // A3
	else if (r==2)   // normal tcc regs
		return 0;    // B0
	else if (r==3)   // normal tcc regs
		return 1;    // B1
	else if (r>=TREG_C67_A4 && r<=TREG_C67_B13)  // these form a pattern of alt pairs
		return (((r & 0xfffffffc) >> 1) | (r & 1)) + 2;  
	else if (r==C67_A0)
		return 0; // set to A0 (offset reg)
	else if (r==C67_B2)
		return 2; // set to B2 (offset reg)
	else if (r==C67_B3)
		return 3; // set to B3 (return address reg)
	else if (r==C67_SP)
		return 15; // set to SP (B15) (offset reg)
	else if (r==C67_FP)
		return 15; // set to FP (A15) (offset reg)
	else if (r==C67_CREG_ZERO)
		return 0; // Special code for no condition reg test
	else
		ALWAYS_ASSERT(FALSE);

	return 0;
}

// mapping from tcc reg number to 
// C67 register to condition code field
//
// valid condition code regs are:
//
// tcc reg 2 ->B0 -> 1
// tcc reg 3 ->B1 -> 2
// tcc reg 0 -> A2 -> 5
// tcc reg 1 -> A3 -> X
// tcc reg      B2 -> 3

int C67_map_regc(int r)
{
	if (r==0)   // normal tcc regs
		return 0x5;      
	else if (r==2)   // normal tcc regs
		return 0x1;  
	else if (r==3)   // normal tcc regs
		return 0x2;  
	else if (r==C67_B2)   // normal tcc regs
		return 0x3;  
	else if (r==C67_CREG_ZERO)
		return 0; // Special code for no condition reg test
	else
		ALWAYS_ASSERT(FALSE);

	return 0;
}


// map TCC reg to C67 reg side A or B

int C67_map_regs(int r)
{
	if (r==0)   // normal tcc regs
		return 0x0;  
	else if (r==1)   // normal tcc regs
		return 0x0;  
	else if (r==2)   // normal tcc regs
		return 0x1;  
	else if (r==3)   // normal tcc regs
		return 0x1;  
	else if (r>=TREG_C67_A4 && r<=TREG_C67_B13)  // these form a pattern of alt pairs
		return (r & 2) >> 1;  
	else if (r==C67_A0)
		return 0; // set to A side 
	else if (r==C67_B2)
		return 1; // set to B side 
	else if (r==C67_B3)
		return 1; // set to B side
	else if (r==C67_SP)
		return 0x1; // set to SP (B15) B side 
	else if (r==C67_FP)
		return 0x0; // set to FP (A15) A side 
	else
		ALWAYS_ASSERT(FALSE);

	return 0;
}

int C67_map_S12(char *s)
{
	if (strstr(s,".S1")!=NULL)
		return 0;
	else if (strcmp(s,".S2"))
		return 1;
	else
		ALWAYS_ASSERT(FALSE);
	
	return 0;
}

int C67_map_D12(char *s)
{
	if (strstr(s,".D1")!=NULL)
		return 0;
	else if (strcmp(s,".D2"))
		return 1;
	else
		ALWAYS_ASSERT(FALSE);
	
	return 0;
}



void C67_asm(char *s,int a, int b, int c)
{
	BOOL xpath;

#ifdef ASSEMBLY_LISTING_C67
	if (!f)
	{
		f=fopen("TCC67_out.txt","wt");
	}
	fprintf(f,"%04X ",ind);
#endif

	if (strstr(s,"MVKL")==s)
	{
		C67_g((C67_map_regn(b)<<23)|
			  ((a & 0xffff)   << 7) |
			  (0x0a           << 2) |
			  (C67_map_regs(b)<< 1));
	}
	else if (strstr(s,"MVKH")==s)
	{
		C67_g((C67_map_regn(b) <<23) |
			  (((a>>16)&0xffff)<< 7) |
		      (0x1a            << 2) |
			  (C67_map_regs(b) << 1));
	}
	else if (strstr(s,"STW.D SP POST DEC")==s)
	{
		C67_g((C67_map_regn(a) <<23) |    //src
			  (15              <<18) |    //SP B15
		      (2               <<13) |    //ucst5 (must keep 8 byte boundary !!)
		      (0xa			   << 9) |    //mode a = post dec ucst
		      (0               << 8) |    //r (LDDW bit 0)
		      (1               << 7) |    //y D1/D2 use B side
		      (7               << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1               << 2) |    //opcode
		      (C67_map_regs(a) << 1) |    //side of src
		      (0               << 0));    //parallel
	}
	else if (strstr(s,"STB.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //src
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2 A side
		      (3              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of src
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"STH.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //src
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2 A side
		      (5              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of src
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"STB.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //src
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2 A side
		      (3              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of src
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"STH.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //src
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2 A side
		      (5              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of src
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"STW.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //src
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2 A side
		      (7              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of src
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"STW.D *")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //src
			  (C67_map_regn(b)<<18) |    //base reg A0
		      (0              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(b)<< 7) |    //y D1/D2 base reg side
		      (7              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of src
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"STH.D *")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //src
			  (C67_map_regn(b)<<18) |    //base reg A0
		      (0              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(b)<< 7) |    //y D1/D2 base reg side
		      (5              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of src
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"STB.D *")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //src
			  (C67_map_regn(b)<<18) |    //base reg A0
		      (0              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(b)<< 7) |    //y D1/D2 base reg side
		      (3              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of src
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"STW.D +*")==s)
	{
		ALWAYS_ASSERT(c<32);
		C67_g((C67_map_regn(a)<<23) |    //src
			  (C67_map_regn(b)<<18) |    //base reg A0
		      (c              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(b)<< 7) |    //y D1/D2 base reg side
		      (7              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of src
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDW.D SP PRE INC")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //dst
			  (15             <<18) |    //base reg B15
		      (2              <<13) |    //ucst5 (must keep 8 byte boundary)
		      (9			  << 9) |    //mode 9 = pre inc ucst5
		      (0              << 8) |    //r (LDDW bit 0)
		      (1              << 7) |    //y D1/D2  B side
		      (6              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDDW.D SP PRE INC")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //dst
			  (15             <<18) |    //base reg B15
		      (1              <<13) |    //ucst5 (must keep 8 byte boundary)
		      (9			  << 9) |    //mode 9 = pre inc ucst5
		      (1              << 8) |    //r (LDDW bit 1)
		      (1              << 7) |    //y D1/D2  B side
		      (6              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDW.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //dst
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2  A side
		      (6              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDDW.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //dst
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (1              << 8) |    //r (LDDW bit 1)
		      (0              << 7) |    //y D1/D2  A side
		      (6              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDH.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //dst
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2  A side
		      (4              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDB.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //dst
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2  A side
		      (2              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDHU.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //dst
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2  A side
		      (0              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDBU.D *+SP[A0]")==s)
	{
		C67_g((C67_map_regn(a)<<23) |    //dst
			  (15             <<18) |    //base reg A15
		      (0              <<13) |    //offset reg A0
		      (5			  << 9) |    //mode 5 = pos offset, base reg + off reg
		      (0              << 8) |    //r (LDDW bit 0)
		      (0              << 7) |    //y D1/D2  A side
		      (1              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(a)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDW.D *")==s)
	{
		C67_g((C67_map_regn(b)<<23) |    //dst
			  (C67_map_regn(a)<<18) |    //base reg A15
		      (0              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(a)<< 7) |    //y D1/D2  src side
		      (6              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(b)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDDW.D *")==s)
	{
		C67_g((C67_map_regn(b)<<23) |    //dst
			  (C67_map_regn(a)<<18) |    //base reg A15
		      (0              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (1              << 8) |    //r (LDDW bit 1)
		      (C67_map_regs(a)<< 7) |    //y D1/D2  src side
		      (6              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(b)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDH.D *")==s)
	{
		C67_g((C67_map_regn(b)<<23) |    //dst
			  (C67_map_regn(a)<<18) |    //base reg A15
		      (0              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(a)<< 7) |    //y D1/D2  src side
		      (4              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(b)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDB.D *")==s)
	{
		C67_g((C67_map_regn(b)<<23) |    //dst
			  (C67_map_regn(a)<<18) |    //base reg A15
		      (0              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(a)<< 7) |    //y D1/D2  src side
		      (2              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(b)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDHU.D *")==s)
	{
		C67_g((C67_map_regn(b)<<23) |    //dst
			  (C67_map_regn(a)<<18) |    //base reg A15
		      (0              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(a)<< 7) |    //y D1/D2  src side
		      (0              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(b)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDBU.D *")==s)
	{
		C67_g((C67_map_regn(b)<<23) |    //dst
			  (C67_map_regn(a)<<18) |    //base reg A15
		      (0              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(a)<< 7) |    //y D1/D2  src side
		      (1              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
		      (1              << 2) |    //opcode
		      (C67_map_regs(b)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"LDW.D +*")==s)
	{
		C67_g((C67_map_regn(b)<<23) |    //dst
			  (C67_map_regn(a)<<18) |    //base reg A15
		      (1              <<13) |    //cst5
		      (1			  << 9) |    //mode 1 = pos cst offset
		      (0              << 8) |    //r (LDDW bit 0)
		      (C67_map_regs(a)<< 7) |    //y D1/D2  src side
		      (6              << 4) |    //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
		      (1              << 2) |    //opcode
		      (C67_map_regs(b)<< 1) |    //side of dst
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPLTSP")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x3a           << 6) |    //opcode
		      (0x8            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPGTSP")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x39           << 6) |    //opcode
		      (0x8            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPEQSP")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x38           << 6) |    //opcode
		      (0x8            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}

	else if (strstr(s,"CMPLTDP")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x2a           << 6) |    //opcode
		      (0x8            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPGTDP")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x29           << 6) |    //opcode
		      (0x8            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPEQDP")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x28           << 6) |    //opcode
		      (0x8            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPLT")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x57           << 5) |    //opcode
		      (0x6            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPGT")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x47           << 5) |    //opcode
		      (0x6            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPEQ")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x53           << 5) |    //opcode
		      (0x6            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPLTU")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x5f           << 5) |    //opcode
		      (0x6            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"CMPGTU")==s)
	{
		xpath = C67_map_regs(a) ^ C67_map_regs(b);
		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g((C67_map_regn(c)<<23) |    //dst
			  (C67_map_regn(b)<<18) |    //src2
		      (C67_map_regn(a)<<13) |    //src1
		      (xpath		  <<12) |    //x use cross path for src2
		      (0x4f           << 5) |    //opcode
		      (0x6            << 2) |    //opcode fixed
		      (C67_map_regs(c)<< 1) |    //side for reg c
		      (0              << 0));    //parallel
	}
	else if (strstr(s,"B DISP")==s)
	{
		C67_g(
			  (0				 <<29) |    //creg
			  (0                 <<28) |    //z
			  (a                 << 7) |    //cnst
		      (0x4               << 2) |    //opcode fixed
		      (0				 << 1) |	//S0/S1
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"B.")==s)
	{
		xpath = C67_map_regs(c)^1;

		C67_g(
			  (C67_map_regc(b)   <<29) |    //creg
			  (a                 <<28) |    //inv
			  (0                 <<23) |    //dst
		      (C67_map_regn(c)   <<18) |    //src2
		      (0     		     <<13) |    //
		      (xpath			 <<12) |    //x cross path if !B side
		      (0xd               << 6) |    //opcode
		      (0x8               << 2) |    //opcode fixed
		      (1				 << 1) |	//must be S2
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"MV.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2
		      (0     		     <<13) |    //src1 (cst5)
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x2               << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"SPTRUNC.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2
		      (0     		     <<13) |    //src1 NA
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0xb               << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"DPTRUNC.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      ((C67_map_regn(b)+1)<<18)|    //src2   WEIRD CPU must specify odd reg for some reason
		      (0     		     <<13) |    //src1 NA
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x1               << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"INTSP.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2   
		      (0     		     <<13) |    //src1 NA
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x4a              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"INTSPU.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2  
		      (0     		     <<13) |    //src1 NA
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x49              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"INTDP.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2  
		      (0     		     <<13) |    //src1 NA
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x39              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"INTDPU.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      ((C67_map_regn(b)+1)<<18)|    //src2   WEIRD CPU must specify odd reg for some reason
		      (0     		     <<13) |    //src1 NA
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x3b              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"SPDP.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2
		      (0     		     <<13) |    //src1 NA
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x2               << 6) |    //opcode
		      (0x8               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"DPSP.L")==s)
	{
		ALWAYS_ASSERT(C67_map_regs(b) == C67_map_regs(c)); 
		
		C67_g(
			  (0                  <<29) |    //creg
			  (0                  <<28) |    //inv
			  (C67_map_regn(c)    <<23) |    //dst
		      ((C67_map_regn(b)+1)<<18) |    //src2 WEIRD CPU must specify odd reg for some reason
		      (0     		      <<13) |    //src1 NA
		      (0   			      <<12) |    //x cross path if opposite sides
		      (0x9                << 5) |    //opcode
		      (0x6                << 2) |    //opcode fixed
		      (C67_map_regs(c)    << 1) |	//side of dest
		      (0                  << 0));    //parallel
	}
	else if (strstr(s,"ADD.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x3               << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"SUB.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x7               << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"OR.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x7f              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"AND.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x7b              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"XOR.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x6f              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"ADDSP.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x10              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"ADDDP.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x18              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"SUBSP.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x11              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"SUBDP.L")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x19              << 5) |    //opcode
		      (0x6               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"MPYSP.M")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x1c              << 7) |    //opcode
		      (0x0               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"MPYDP.M")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2 (possible x path)
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x0e              << 7) |    //opcode
		      (0x0               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"MPYI.M")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2
		      (C67_map_regn(a)   <<13) |    //src1 (cst5)
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x4               << 7) |    //opcode
		      (0x0               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"SHR.S")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x37              << 6) |    //opcode
		      (0x8               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"SHRU.S")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x27              << 6) |    //opcode
		      (0x8               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"SHL.S")==s)
	{
		xpath = C67_map_regs(b)^C67_map_regs(c);  

		ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(c)   <<23) |    //dst
		      (C67_map_regn(b)   <<18) |    //src2
		      (C67_map_regn(a)   <<13) |    //src1 
		      (xpath			 <<12) |    //x cross path if opposite sides
		      (0x33              << 6) |    //opcode
		      (0x8               << 2) |    //opcode fixed
		      (C67_map_regs(c)   << 1) |	//side of dest
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"||ADDK")==s)
	{
		xpath = 0;  // no xpath required just use the side of the src/dst

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(b)   <<23) |    //dst
		      ((a&0xffff)        <<07) |    //scst16
		      (0x14              << 2) |    //opcode fixed
		      (C67_map_regs(b)	 << 1) |	//side of dst
		      (1                 << 0));    //parallel
	}
	else if (strstr(s,"ADDK")==s)
	{
		xpath = 0;  // no xpath required just use the side of the src/dst

		C67_g(
			  (0                 <<29) |    //creg
			  (0                 <<28) |    //inv
			  (C67_map_regn(b)   <<23) |    //dst
		      ((a&0xffff)        <<07) |    //scst16
		      (0x14              << 2) |    //opcode fixed
		      (C67_map_regs(b)	 << 1) |	//side of dst
		      (0                 << 0));    //parallel
	}
	else if (strstr(s,"NOP")==s)
	{
		C67_g(
			  ((a-1)             <<13) |    //no of cycles
		      (0                 << 0));    //parallel
	}
	else
		ALWAYS_ASSERT(FALSE);

#ifdef ASSEMBLY_LISTING_C67
	fprintf(f," %s %d %d %d\n",s,a,b,c);
#endif

}
//r=reg to load, fr=from reg, symbol for relocation, constant

void C67_MVKL(int r, int fc)
{
	C67_asm("MVKL.",fc,r,0);
}

void C67_MVKH(int r, int fc)
{
	C67_asm("MVKH.",fc,r,0);
}

void C67_STB_SP_A0(int r)
{
	C67_asm("STB.D *+SP[A0]",r,0,0);     // STB  r,*+SP[A0]
}

void C67_STH_SP_A0(int r)
{
	C67_asm("STH.D *+SP[A0]",r,0,0);     // STH  r,*+SP[A0]
}

void C67_STW_SP_A0(int r)
{
	C67_asm("STW.D *+SP[A0]",r,0,0);     // STW  r,*+SP[A0]
}

void C67_STB_PTR(int r, int r2)
{
	C67_asm("STB.D *",r,r2,0);      // STB  r, *r2
}

void C67_STH_PTR(int r, int r2)
{
	C67_asm("STH.D *",r,r2,0);      // STH  r, *r2
}

void C67_STW_PTR(int r, int r2)
{
	C67_asm("STW.D *",r,r2,0);      // STW  r, *r2
}

void C67_STW_PTR_PRE_INC(int r, int r2, int n)
{
	C67_asm("STW.D +*",r,r2,n);      // STW  r, *+r2
}

void C67_PUSH(int r)
{
	C67_asm("STW.D SP POST DEC",r,0,0);     // STW  r,*SP--
}

void C67_LDW_SP_A0(int r)
{
	C67_asm("LDW.D *+SP[A0]",r,0,0);     // LDW  *+SP[A0],r
}

void C67_LDDW_SP_A0(int r)
{
	C67_asm("LDDW.D *+SP[A0]",r,0,0);     // LDDW  *+SP[A0],r
}

void C67_LDH_SP_A0(int r)
{
	C67_asm("LDH.D *+SP[A0]",r,0,0);     // LDH  *+SP[A0],r
}

void C67_LDB_SP_A0(int r)
{
	C67_asm("LDB.D *+SP[A0]",r,0,0);     // LDB  *+SP[A0],r
}

void C67_LDHU_SP_A0(int r)
{
	C67_asm("LDHU.D *+SP[A0]",r,0,0);     // LDHU  *+SP[A0],r
}

void C67_LDBU_SP_A0(int r)
{
	C67_asm("LDBU.D *+SP[A0]",r,0,0);     // LDBU  *+SP[A0],r
}

void C67_LDW_PTR(int r, int r2)
{
	C67_asm("LDW.D *",r,r2,0);     // LDW  *r,r2
}

void C67_LDDW_PTR(int r, int r2)
{
	C67_asm("LDDW.D *",r,r2,0);     // LDDW  *r,r2
}

void C67_LDH_PTR(int r, int r2)
{
	C67_asm("LDH.D *",r,r2,0);     // LDH  *r,r2
}

void C67_LDB_PTR(int r, int r2)
{
	C67_asm("LDB.D *",r,r2,0);     // LDB  *r,r2
}

void C67_LDHU_PTR(int r, int r2)
{
	C67_asm("LDHU.D *",r,r2,0);     // LDHU  *r,r2
}

void C67_LDBU_PTR(int r, int r2)
{
	C67_asm("LDBU.D *",r,r2,0);     // LDBU  *r,r2
}

void C67_LDW_PTR_PRE_INC(int r, int r2)
{
	C67_asm("LDW.D +*",r,r2,0);     // LDW  *+r,r2
}

void C67_POP(int r)
{
	C67_asm("LDW.D SP PRE INC",r,0,0);     // LDW  *++SP,r
}

void C67_POP_DW(int r)
{
	C67_asm("LDDW.D SP PRE INC",r,0,0);     // LDDW  *++SP,r
}

void C67_CMPLT(int s1, int s2, int dst)
{
	C67_asm("CMPLT.L1",s1,s2,dst);     
}

void C67_CMPGT(int s1, int s2, int dst)
{
	C67_asm("CMPGT.L1",s1,s2,dst);     
}

void C67_CMPEQ(int s1, int s2, int dst)
{
	C67_asm("CMPEQ.L1",s1,s2,dst);     
}

void C67_CMPLTU(int s1, int s2, int dst)
{
	C67_asm("CMPLTU.L1",s1,s2,dst);     
}

void C67_CMPGTU(int s1, int s2, int dst)
{
	C67_asm("CMPGTU.L1",s1,s2,dst);     
}


void C67_CMPLTSP(int s1, int s2, int dst)
{
	C67_asm("CMPLTSP.S1",s1,s2,dst);     
}

void C67_CMPGTSP(int s1, int s2, int dst)
{
	C67_asm("CMPGTSP.S1",s1,s2,dst);     
}

void C67_CMPEQSP(int s1, int s2, int dst)
{
	C67_asm("CMPEQSP.S1",s1,s2,dst);     
}

void C67_CMPLTDP(int s1, int s2, int dst)
{
	C67_asm("CMPLTDP.S1",s1,s2,dst);     
}

void C67_CMPGTDP(int s1, int s2, int dst)
{
	C67_asm("CMPGTDP.S1",s1,s2,dst);     
}

void C67_CMPEQDP(int s1, int s2, int dst)
{
	C67_asm("CMPEQDP.S1",s1,s2,dst);     
}


void C67_IREG_B_REG(int inv, int r1,int r2)    // [!R] B  r2
{
	C67_asm("B.S2",inv,r1,r2);     
}


// call with how many 32 bit words to skip
// (0 would branch to the branch instruction)

void C67_B_DISP(int disp)    //  B  +2  Branch with constant displacement
{
	// Branch point is relative to the 8 word fetch packet
	//
	// we will assume the text section always starts on an 8 word (32 byte boundary)
	//
	// so add in how many words into the fetch packet the branch is


	C67_asm("B DISP",disp + ((ind&31)>>2),0,0);     
}

void C67_NOP(int n)
{
	C67_asm("NOP",n,0,0);     
}

void C67_ADDK(int n, int r)
{
	ALWAYS_ASSERT(abs(n) < 32767);

	C67_asm("ADDK",n,r,0);     
}

void C67_ADDK_PARALLEL(int n, int r)
{
	ALWAYS_ASSERT(abs(n) < 32767);

	C67_asm("||ADDK",n,r,0);     
}

void C67_Adjust_ADDK(int *inst, int n)
{
	ALWAYS_ASSERT(abs(n) < 32767);

	*inst = (*inst & (~(0xffff << 7)) ) | ((n & 0xffff) << 7);
}

void C67_MV(int r, int v)
{
	C67_asm("MV.L",0,r,v);     
}


void C67_DPTRUNC(int r, int v)
{
	C67_asm("DPTRUNC.L",0,r,v);     
}

void C67_SPTRUNC(int r, int v)
{
	C67_asm("SPTRUNC.L",0,r,v);     
}

void C67_INTSP(int r, int v)
{
	C67_asm("INTSP.L",0,r,v);     
}

void C67_INTDP(int r, int v)
{
	C67_asm("INTDP.L",0,r,v);     
}

void C67_INTSPU(int r, int v)
{
	C67_asm("INTSPU.L",0,r,v);     
}

void C67_INTDPU(int r, int v)
{
	C67_asm("INTDPU.L",0,r,v);     
}

void C67_SPDP(int r, int v) 
{
	C67_asm("SPDP.L",0,r,v);     
}

void C67_DPSP(int r, int v)  // note regs must be on the same side
{
	C67_asm("DPSP.L",0,r,v);     
}

void C67_ADD(int r, int v)
{
	C67_asm("ADD.L",v,r,v);     
}

void C67_SUB(int r, int v)
{
	C67_asm("SUB.L",v,r,v);     
}

void C67_AND(int r, int v)
{
	C67_asm("AND.L",v,r,v);     
}

void C67_OR(int r, int v)
{
	C67_asm("OR.L",v,r,v);     
}

void C67_XOR(int r, int v)
{
	C67_asm("XOR.L",v,r,v);     
}

void C67_ADDSP(int r, int v)
{
	C67_asm("ADDSP.L",v,r,v);     
}

void C67_SUBSP(int r, int v)
{
	C67_asm("SUBSP.L",v,r,v);     
}

void C67_MPYSP(int r, int v)
{
	C67_asm("MPYSP.M",v,r,v);     
}

void C67_ADDDP(int r, int v)
{
	C67_asm("ADDDP.L",v,r,v);     
}

void C67_SUBDP(int r, int v)
{
	C67_asm("SUBDP.L",v,r,v);     
}

void C67_MPYDP(int r, int v)
{
	C67_asm("MPYDP.M",v,r,v);     
}

void C67_MPYI(int r, int v)
{
	C67_asm("MPYI.M",v,r,v);     
}

void C67_SHL(int r, int v)
{
	C67_asm("SHL.S",r,v,v);     
}

void C67_SHRU(int r, int v)
{
	C67_asm("SHRU.S",r,v,v);     
}

void C67_SHR(int r, int v)
{
	C67_asm("SHR.S",r,v,v);     
}



/* load 'r' from value 'sv' */
void load(int r, SValue *sv)
{
#if (DO_C67)

    int v, t, ft, fc, fr, size, element;
	BOOL Unsigned=false;
    SValue v1;

    fr = sv->r;
    ft = sv->type.t;
    fc = sv->c.ul;

    v = fr & VT_VALMASK;
    if (fr & VT_LVAL) 
	{
        if (v == VT_LLOCAL) 
		{
            v1.type.t = VT_INT;
            v1.r = VT_LOCAL | VT_LVAL;
            v1.c.ul = fc;
            load(r, &v1);
            fr = r;
			v = r;  // not too sure about this tktk
        } 
		
		if ((ft & VT_BTYPE) == VT_LDOUBLE) 
		{
			error("long double not supported");
        }
		else if ((ft & VT_TYPE) == VT_BYTE) 
		{
            size=1;
        } 
		else if ((ft & VT_TYPE) == (VT_BYTE | VT_UNSIGNED)) 
		{
            size=1;
			Unsigned=TRUE;
        } 
		else if ((ft & VT_TYPE) == VT_SHORT) 
		{
            size=2;
        } 
		else if ((ft & VT_TYPE) == (VT_SHORT | VT_UNSIGNED)) 
		{
            size=2;
			Unsigned=TRUE;
        } 
		else if ((ft & VT_BTYPE) == VT_DOUBLE) 
		{
            size=8;
		}
		else
		{
			size=4;
		}

		// check if fc is a positive reference on the stack, 
		// if it is tcc is referencing what it thinks is a parameter
		// on the stack, so check if it is really in a register.


		if (v == VT_LOCAL && fc > 0)
		{
			int stack_pos=8;

			for (t=0; t<NoCallArgsPassedOnStack; t++)
			{
				if (fc==stack_pos) break;

				stack_pos += TranslateStackToReg[t];
			}

			// param has been pushed on stack, get it like a local var

			fc = ParamLocOnStack[t]-8;
		}

		
		if ((fr & VT_VALMASK) < VT_CONST)  // check for pure indirect of register
		{
			if (size == 1)
			{
				if (Unsigned)
					C67_LDBU_PTR(v,r); // LDBU  *v,r
				else
					C67_LDB_PTR(v,r);  // LDB  *v,r
			}
			else if (size == 2)
			{
				if (Unsigned)
					C67_LDHU_PTR(v,r);  // LDHU  *v,r
				else
					C67_LDH_PTR(v,r);   // LDH  *v,r
			}
			else if (size == 4)
			{
				C67_LDW_PTR(v,r);       // LDW  *v,r
			}
			else if (size==8)
			{
				C67_LDDW_PTR(v,r);       // LDDW  *v,r
			}

			if (size==8)
				C67_NOP(5);		 	  // NOP 5
			else
				C67_NOP(4);		 	  // NOP 4

			return;
		}
		else if (fr & VT_SYM)
		{
			greloc(cur_text_section, sv->sym, ind,   R_C60LO16);  // rem the inst need to be patched
			greloc(cur_text_section, sv->sym, ind+4, R_C60HI16);

			
			C67_MVKL(C67_A0,fc);   //r=reg to load,  constant
			C67_MVKH(C67_A0,fc);   //r=reg to load,  constant

			
			if (size == 1)
			{
				if (Unsigned)
					C67_LDBU_PTR(C67_A0,r); // LDBU  *A0,r
				else
					C67_LDB_PTR(C67_A0,r);  // LDB  *A0,r
			}
			else if (size == 2)
			{
				if (Unsigned)
					C67_LDHU_PTR(C67_A0,r);  // LDHU  *A0,r
				else
					C67_LDH_PTR(C67_A0,r);   // LDH  *A0,r
			}
			else if (size == 4)
			{
				C67_LDW_PTR(C67_A0,r);       // LDW  *A0,r
			}
			else if (size==8)
			{
				C67_LDDW_PTR(C67_A0,r);       // LDDW  *A0,r
			}

			if (size==8)
				C67_NOP(5);		 	  // NOP 5
			else
				C67_NOP(4);		 	  // NOP 4

			return;
		}
		else if ((fr & VT_VALMASK) == VT_CONST)  // check for pure indirect of constant
		{
			C67_MVKL(C67_A0,fc);   //r=reg to load,  constant
			C67_MVKH(C67_A0,fc);   //r=reg to load,  constant

			if (size == 1)
			{
				if (Unsigned)
					C67_LDBU_PTR(C67_A0,r); // LDBU  *v,r
				else
					C67_LDB_PTR(C67_A0,r);  // LDB  *v,r
			}
			else if (size == 2)
			{
				if (Unsigned)
					C67_LDHU_PTR(C67_A0,r);  // LDHU  *v,r
				else
					C67_LDH_PTR(C67_A0,r);   // LDH  *v,r
			}
			else if (size == 4)
			{
				C67_LDW_PTR(C67_A0,r);       // LDW  *v,r
			}
			else if (size==8)
			{
				C67_LDDW_PTR(C67_A0,r);       // LDDW  *v,r
			}

			if (size==8)
				C67_NOP(5);		 	  // NOP 5
			else
				C67_NOP(4);		 	  // NOP 4

			return;
		}
		else
		{
			element = size;

			// divide offset in bytes to create element index
			C67_MVKL(C67_A0,(fc/element)+8/element);   //r=reg to load,  constant
			C67_MVKH(C67_A0,(fc/element)+8/element);   //r=reg to load,  constant
			
			if (size == 1)
			{
				if (Unsigned)
					C67_LDBU_SP_A0(r);            // LDBU  r, SP[A0]
				else
					C67_LDB_SP_A0(r);             // LDB  r, SP[A0]
			}
			else if (size == 2)
			{
				if (Unsigned)
					C67_LDHU_SP_A0(r);            // LDHU  r, SP[A0]
				else
					C67_LDH_SP_A0(r);             // LDH  r, SP[A0]
			}
			else if (size == 4)
			{
				C67_LDW_SP_A0(r);				  // LDW  r, SP[A0]
			}
			else if (size==8)
			{
				C67_LDDW_SP_A0(r);				  // LDDW  r, SP[A0]
			}
			
			if (size==8)
				C67_NOP(5);		 	  // NOP 5
			else
				C67_NOP(4);		 	  // NOP 4

			return;
		}
    } else {
        if (v == VT_CONST) 
		{
			if (fr & VT_SYM)
			{
				greloc(cur_text_section, sv->sym, ind,   R_C60LO16);  // rem the inst need to be patched
				greloc(cur_text_section, sv->sym, ind+4, R_C60HI16);
			}
			C67_MVKL(r,fc);   //r=reg to load, constant
			C67_MVKH(r,fc);   //r=reg to load, constant
        } 
		else if (v == VT_LOCAL) 
		{
			C67_MVKL(r,fc+8);   //r=reg to load, constant C67 stack points to next free
			C67_MVKH(r,fc+8);   //r=reg to load, constant
			C67_ADD(C67_FP,r);   // MV v,r   v -> r
        } 
		else if (v == VT_CMP) 
		{
			C67_MV(C67_compare_reg,r);   // MV v,r   v -> r
        } 
		else if (v == VT_JMP || v == VT_JMPI) 
		{
            t = v & 1;
			C67_B_DISP(4);   //  Branch with constant displacement, skip over this branch, load, nop, load
			C67_MVKL(r,t);   //  r=reg to load, 0 or 1 (do this while branching)
			C67_NOP(4);		 //  NOP 4
            gsym(fc);        //  modifies other branches to branch here
			C67_MVKL(r,t^1);   //  r=reg to load, 0 or 1
        }
		else if (v != r) 
		{
			C67_MV(v,r);   // MV v,r   v -> r

			if ((ft & VT_BTYPE) == VT_DOUBLE)
				C67_MV(v+1,r+1);   // MV v,r   v -> r
        }
    }
}

#else

    int v, t, ft, fc, fr;
    SValue v1;

    fr = sv->r;
    ft = sv->type.t;
    fc = sv->c.ul;

    v = fr & VT_VALMASK;
    if (fr & VT_LVAL) {
        if (v == VT_LLOCAL) {
            v1.type.t = VT_INT;
            v1.r = VT_LOCAL | VT_LVAL;
            v1.c.ul = fc;
            load(r, &v1);
            fr = r;
        }
        if ((ft & VT_BTYPE) == VT_FLOAT) {
            o(0xd9); /* flds */
            r = 0;
        } else if ((ft & VT_BTYPE) == VT_DOUBLE) {
            o(0xdd); /* fldl */
            r = 0;
        } else if ((ft & VT_BTYPE) == VT_LDOUBLE) {
            o(0xdb); /* fldt */
            r = 5;
        } else if ((ft & VT_TYPE) == VT_BYTE) {
            o(0xbe0f);   /* movsbl */
        } else if ((ft & VT_TYPE) == (VT_BYTE | VT_UNSIGNED)) {
            o(0xb60f);   /* movzbl */
        } else if ((ft & VT_TYPE) == VT_SHORT) {
            o(0xbf0f);   /* movswl */
        } else if ((ft & VT_TYPE) == (VT_SHORT | VT_UNSIGNED)) {
            o(0xb70f);   /* movzwl */
        } else {
            o(0x8b);     /* movl */
        }
        gen_modrm(r, fr, sv->sym, fc);
    } else {
        if (v == VT_CONST) {
            o(0xb8 + r); /* mov $xx, r */
            gen_addr32(fr, sv->sym, fc);
        } else if (v == VT_LOCAL) {
            o(0x8d); /* lea xxx(%ebp), r */
            gen_modrm(r, VT_LOCAL, sv->sym, fc);
        } else if (v == VT_CMP) {
            oad(0xb8 + r, 0); /* mov $0, r */
            o(0x0f); /* setxx %br */
            o(fc);
            o(0xc0 + r);
        } else if (v == VT_JMP || v == VT_JMPI) {
            t = v & 1;
            oad(0xb8 + r, t); /* mov $1, r */
            o(0x05eb); /* jmp after */
            gsym(fc);
            oad(0xb8 + r, t ^ 1); /* mov $0, r */
        } else if (v != r) {
            o(0x89);
            o(0xc0 + r + v * 8); /* mov v, r */
        }
    }
}

#endif


/* store register 'r' in lvalue 'v' */
void store(int r, SValue *v)
{
#if (DO_C67)

    int fr, bt, ft, fc, size, t, element;

    ft = v->type.t;
    fc = v->c.ul;
    fr = v->r & VT_VALMASK;
    bt = ft & VT_BTYPE;
    /* XXX: incorrect if float reg to reg */

    if (bt == VT_LDOUBLE) 
	{
		error("long double not supported");
    } 
	else 
	{
        if (bt == VT_SHORT)
			size=2;
        else if (bt == VT_BYTE)
			size=1;
		else if (bt == VT_DOUBLE)
			size=8;
		else
			size=4;

		if ((v->r & VT_VALMASK) == VT_CONST) 
		{
			/* constant memory reference */
			
			if (v->r & VT_SYM)
			{
				greloc(cur_text_section, v->sym, ind,   R_C60LO16);  // rem the inst need to be patched
				greloc(cur_text_section, v->sym, ind+4, R_C60HI16);
			}
			C67_MVKL(C67_A0,fc);    //r=reg to load,  constant
			C67_MVKH(C67_A0,fc);    //r=reg to load,  constant
			
			if (size==1)
				C67_STB_PTR(r,C67_A0); // STB  r, *A0
			else if (size==2)
				C67_STH_PTR(r,C67_A0); // STH  r, *A0
			else if (size==4 || size==8)
				C67_STW_PTR(r,C67_A0); // STW  r, *A0
			
			if (size==8)
				C67_STW_PTR_PRE_INC(r+1,C67_A0,1); // STW  r, *+A0[1]
			return;
		} 
		else if ((v->r & VT_VALMASK) == VT_LOCAL) 
		{
			// check case of storing to passed argument that
			// tcc thinks is on the stack but for C67 is
			// passed as a reg.  However it may have been
			// saved to the stack, if that reg was required
			// for a call to a child function

			if (fc > 0)  // argument ??
			{
				// walk through sizes and figure which param

				int stack_pos=8;

				for (t=0; t<NoCallArgsPassedOnStack; t++)
				{
					if (fc==stack_pos) break;

					stack_pos += TranslateStackToReg[t];
				}

				// param has been pushed on stack, get it like a local var
				fc=ParamLocOnStack[t]-8;
			}

			if (size==8)
				element = 4;
			else
				element = size;

			// divide offset in bytes to create word index
			C67_MVKL(C67_A0,(fc/element)+8/element);   //r=reg to load,  constant
			C67_MVKH(C67_A0,(fc/element)+8/element);   //r=reg to load,  constant

			
			
			if (size==1)
				C67_STB_SP_A0(r);             // STB  r, SP[A0]
			else if (size==2)
				C67_STH_SP_A0(r);             // STH  r, SP[A0]
			else if (size==4 || size==8)
				C67_STW_SP_A0(r);             // STW  r, SP[A0]
			
			if (size==8)
			{
				C67_ADDK(1,C67_A0);  //  ADDK 1,A0
				C67_STW_SP_A0(r+1);  //  STW  r, SP[A0]
			}
			return;
		} 
		else 
		{
			if (size==1)
				C67_STB_PTR(r,fr);                      // STB  r, *fr
			else if (size==2)
				C67_STH_PTR(r,fr);                      // STH  r, *fr
			else if (size==4 || size==8)
				C67_STW_PTR(r,fr);                      // STW  r, *fr
	
			if (size==8)
			{
				C67_STW_PTR_PRE_INC(r+1,fr,1); // STW  r, *+fr[1]
			}
			return;
		}
    }
    if (fr == VT_CONST ||
        fr == VT_LOCAL ||
        (v->r & VT_LVAL)) {
        gen_modrm(r, v->r, v->sym, fc);
    } else if (fr != r) {
        o(0xc0 + fr + r * 8); /* mov r, fr */
    }
}
	
#else



    int fr, bt, ft, fc;

    ft = v->type.t;
    fc = v->c.ul;
    fr = v->r & VT_VALMASK;
    bt = ft & VT_BTYPE;
    /* XXX: incorrect if float reg to reg */

    if (bt == VT_FLOAT) {
        o(0xd9); /* fsts */
        r = 2;
    } 
	else if (bt == VT_DOUBLE) {
        o(0xdd); /* fstpl */
        r = 2;
    } else if (bt == VT_LDOUBLE) {
        o(0xc0d9); /* fld %st(0) */
        o(0xdb); /* fstpt */
        r = 7;
    } else {
        if (bt == VT_SHORT)
            o(0x66);
        if (bt == VT_BYTE)
            o(0x88);
        else
        {
			o(0x89);
		}
    }
    if (fr == VT_CONST ||
        fr == VT_LOCAL ||
        (v->r & VT_LVAL)) {
        gen_modrm(r, v->r, v->sym, fc);
    } else if (fr != r) {
        o(0xc0 + fr + r * 8); /* mov r, fr */
    }
}
#endif



/* start function call and return function call context */
void gfunc_start(GFuncContext *c, int func_call)
{
    c->args_size = 0;
    c->args_count = 0;
    c->func_call = func_call;
}

/* push function parameter which is in (vtop->t, vtop->c). Stack entry
   is then popped. */
void gfunc_param(GFuncContext *c)
{
#if (DO_C67)
    int size, r;

    if ((vtop->type.t & VT_BTYPE) == VT_STRUCT) {
		error("passing struct not allowed, consider passing by ref");
	} else {
        /* simple type (currently always same size) */
        /* XXX: implicit cast ? */
		


        if ((vtop->type.t & VT_BTYPE) == VT_LLONG) 
		{
			error("long long not supported");
		}
		else if ((vtop->type.t & VT_BTYPE) == VT_LDOUBLE) 
		{
			error("long double not supported");
		}
		else if ((vtop->type.t & VT_BTYPE) == VT_DOUBLE) 
		{
            size = 8;
        } 
		else 
		{
            size = 4;
        }


        
		// put the parameter into the corresponding reg (pair)

		if (c->args_count >= 0 && c->args_count < NoCallArgsPassedOnStack)
		{
			r = gv(RC_C67_A4 << (2 * c->args_count));

			// must put on stack because with 1 pass compiler , no way to tell
			// if an up coming nested call might overwrite these regs

			C67_PUSH(r);  

			if (size == 8)
			{
				C67_STW_PTR_PRE_INC(r+1, C67_SP, 3);  // STW  r, *+SP[3] (go back and put the other)
			}
		}
		else
			error("more than 10 function parameters not allowed");
		
		
		
		c->args_size += size;
		c->args_sizes[c->args_count] = size;
		c->args_count++;
    }
    vtop--;
#else
    int size, align, r;

    if ((vtop->type.t & VT_BTYPE) == VT_STRUCT) {
        size = type_size(&vtop->type, &align);
        /* align to stack align size */
        size = (size + 3) & ~3;
        /* allocate the necessary size on stack */
        oad(0xec81, size); /* sub $xxx, %esp */
        /* generate structure store */
        r = get_reg(RC_INT);
        o(0x89); /* mov %esp, r */
        o(0xe0 + r);
        vset(&vtop->type, r | VT_LVAL, 0);
        vswap();
        vstore();
        c->args_size += size;
    } else if (is_float(vtop->type.t)) {
        gv(RC_FLOAT); /* only one float register */
        if ((vtop->type.t & VT_BTYPE) == VT_FLOAT)
            size = 4;
        else if ((vtop->type.t & VT_BTYPE) == VT_DOUBLE)
            size = 8;
        else
            size = 12;
        oad(0xec81, size); /* sub $xxx, %esp */
        if (size == 12)
            o(0x7cdb);
        else
            o(0x5cd9 + size - 4); /* fstp[s|l] 0(%esp) */
        g(0x24);
        g(0x00);
        c->args_size += size;
    } else {
        /* simple type (currently always same size) */
        /* XXX: implicit cast ? */
        r = gv(RC_INT);
        if ((vtop->type.t & VT_BTYPE) == VT_LLONG) {
            size = 8;
            o(0x50 + vtop->r2); /* push r */
        } else {
            size = 4;
        }
        o(0x50 + r); /* push r */
        c->args_size += size;
    }
    vtop--;
#endif
}

static void gadd_sp(int val)
{
    if (val == (char)val) {
        o(0xc483);
        g(val);
    } else {
        oad(0xc481, val); /* add $xxx, %esp */
    }
}

/* 'is_jmp' is '1' if it is a jump */
static void gcall_or_jmp(int is_jmp)
{
#if (DO_C67)
    int r;
	Sym *sym;

    if ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        /* constant case */
        if (vtop->r & VT_SYM) {
            /* relocation case */

            // get add into A0, then start the jump B3

	        greloc(cur_text_section, vtop->sym, ind,   R_C60LO16);  // rem the inst need to be patched
	        greloc(cur_text_section, vtop->sym, ind+4, R_C60HI16);

			C67_MVKL(C67_A0,0);   //r=reg to load, constant
			C67_MVKH(C67_A0,0);   //r=reg to load, constant
			C67_IREG_B_REG(0,C67_CREG_ZERO,C67_A0);    //  B.S2x  A0

			if (is_jmp)
			{
				C67_NOP(5);  // simple jump, just put NOP
			}
			else
			{
				// Call, must load return address into B3 during delay slots

				sym = get_sym_ref(&char_pointer_type, cur_text_section, ind+12, 0);  // symbol for return address
				greloc(cur_text_section, sym, ind,   R_C60LO16);  // rem the inst need to be patched
				greloc(cur_text_section, sym, ind+4, R_C60HI16);
				C67_MVKL(C67_B3,0);   //r=reg to load, constant
				C67_MVKH(C67_B3,0);   //r=reg to load, constant
				C67_NOP(3);           // put remaining NOPs
			}
        } else {
            /* put an empty PC32 relocation */
			ALWAYS_ASSERT(FALSE);
        }
    } else {
        /* otherwise, indirect call */
        r = gv(RC_INT);
		C67_IREG_B_REG(0,C67_CREG_ZERO,r);    //  B.S2x  r

		if (is_jmp)
		{
			C67_NOP(5);  // simple jump, just put NOP
		}
		else
		{
			// Call, must load return address into B3 during delay slots

			sym = get_sym_ref(&char_pointer_type, cur_text_section, ind+12, 0);  // symbol for return address
			greloc(cur_text_section, sym, ind,   R_C60LO16);  // rem the inst need to be patched
			greloc(cur_text_section, sym, ind+4, R_C60HI16);
			C67_MVKL(C67_B3,0);   //r=reg to load, constant
			C67_MVKH(C67_B3,0);   //r=reg to load, constant
			C67_NOP(3);           // put remaining NOPs
		}
    }
#else
    int r;
    if ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        /* constant case */
        if (vtop->r & VT_SYM) {
            /* relocation case */
            greloc(cur_text_section, vtop->sym, 
                   ind + 1, R_386_PC32);
        } else {
            /* put an empty PC32 relocation */
            put_elf_reloc(symtab_section, cur_text_section, 
                          ind + 1, R_386_PC32, 0);
        }
        oad(0xe8 + is_jmp, vtop->c.ul - 4); /* call/jmp im */
    } else {
        /* otherwise, indirect call */
        r = gv(RC_INT);
        o(0xff); /* call/jmp *r */
        o(0xd0 + r + (is_jmp << 4));
    }
#endif
}



#if (DO_C67)  


// Checks if a specific register is in use

int CheckInUse(int r)
{
    SValue *p;

    for(p=vstack;p<=vtop;p++) 
	{
        if ((p->r  & VT_VALMASK) == r ||
            (p->r2 & VT_VALMASK) == r)
		{
			return TRUE;
		}
	}
	return FALSE;
}

#endif


// checks if a particular register is in use anywhere in the 
// value stack.  TI Compiler spec says that registers
// A0-A9 and B0-B9 must be preserved by caller.
// Since we use A2,A3,B0,and B1 to hold values,
// check which if any are in use and save them  


void CheckPreserveRegs(int *A2_inuse ,int *A3_inuse,int *B0_inuse,int *B1_inuse)
{
	*A2_inuse = CheckInUse(TREG_EAX);   // really A2
	*A3_inuse = CheckInUse(TREG_ECX);   // really A3
	*B0_inuse = CheckInUse(TREG_EDX);   // really B0
	*B1_inuse = CheckInUse(TREG_ST0);   // really B1

	
	if (*A2_inuse && *A3_inuse)
	{
		C67_PUSH(TREG_EAX);// really A2
		C67_STW_PTR_PRE_INC(TREG_ECX, C67_SP, 3);  // STW  r, *+SP[1] (go back and put the other)
	}
	else if (*A2_inuse)
	{
		C67_PUSH(TREG_EAX);// really A2
	}
	else if (*A3_inuse)
	{
		C67_PUSH(TREG_ECX);// really A3
	}

	if (*B0_inuse && *B1_inuse)
	{
		C67_PUSH(TREG_EDX);// really B0
		C67_STW_PTR_PRE_INC(TREG_ST0, C67_SP, 3);  // STW  r, *+SP[1] (go back and put the other)
	}
	else if (*B0_inuse)
	{
		C67_PUSH(TREG_EDX);// really B0
	}
	else if (*B1_inuse)
	{
		C67_PUSH(TREG_ST0);// really B1
	}
}


// Now reload any values that we saved before the call 

void CheckRestoreRegs(int A2_inuse, int A3_inuse, int B0_inuse, int B1_inuse)
{
	if (A2_inuse && A3_inuse)
	{
		C67_POP_DW(TREG_EAX);
	}
	else if (A2_inuse)
	{
		C67_POP(TREG_EAX);// really A2
	}
	else if (A3_inuse)
	{
		C67_POP(TREG_ECX);// really A3
	}

	if (B0_inuse && B1_inuse)
	{
		C67_POP_DW(TREG_EDX);// really B0
	}
	else if (B0_inuse)
	{
		C67_POP(TREG_EDX);// really B0
	}
	else if (B1_inuse)
	{
		C67_POP(TREG_ST0);// really B1
	}


	if (A2_inuse || A3_inuse || B0_inuse || B1_inuse)
	{
		C67_NOP(4);		//  if we popped anything wait for loads to happen
	}
}



/* generate function call with address in (vtop->t, vtop->c) and free function
   context. Stack entry is popped */
void gfunc_call(GFuncContext *c)
{
#if (DO_C67)   
	int A2_inuse=0,A3_inuse=0,B0_inuse=0,B1_inuse=0;

	int r, i, proto_count, size, loc, stackused=0;;
	Sym *sym;

	if (c->args_size && c->func_call == FUNC_CDECL)
    {
		if (c->args_count > NoCallArgsPassedOnStack)
		{
			error("more than 10 function params not currently supported");
			// handle more than 10, put some on the stack
			gadd_sp(c->args_size);
		}
		else
		{


			// POP all the params on the stack into registers for the
			// immediate call (in reverse order)

			for (i=c->args_count-1; i>=0; i--)
			{
				if (c->args_sizes[i] == 8)
					C67_POP_DW(TREG_C67_A4 + i*2);
				else
					C67_POP(TREG_C67_A4 + i*2);
			}


			CheckPreserveRegs(&A2_inuse,&A3_inuse,&B0_inuse,&B1_inuse);



			// Special case for variable arguments

			if (vtop->type.ref->c == FUNC_ELLIPSIS)
			{
				// yes there are variable arguments

				// determine how many are in the function prototype

				proto_count=0;
				sym=vtop->type.ref;
				while (sym->next)
				{
					sym=sym->next;
					proto_count++;
				}

				// rule is that last formal param plus var
				// params are pushed on stack

				// figure the size of the stack needed
				// considering alignment

				loc=0;
				for (i=proto_count-1; i<c->args_count; i++)
				{
					// align to argument size
					
					size = c->args_sizes[i];
			        loc = (loc+size-1) & ~(size-1);
					loc += size;
				}

				// finally align to Double word
				size = 8;
			    loc = (loc+size-1) & ~(size-1);

				stackused=loc;

				C67_ADDK(-stackused,C67_SP);    //  ADDK.L2 -loc,SP
				C67_NOP(3);						//  NOP wait for loads to happen

				
				//now re-save the required params (registers) onto the stack 

				loc=4;
				for (i=proto_count-1; i<c->args_count; i++)
				{
					// align to argument size
					
					size = c->args_sizes[i];
			        loc = (loc+size-1) & ~(size-1);

					r = TREG_C67_A4+(2 * i);

					// must put on stack because with 1 pass compiler , no way to tell
					// if an up coming nested call might overwrite these regs

					C67_STW_PTR_PRE_INC(r, C67_SP, loc/4);  // STW  r, *+SP[3] (go back and put the other)

					if (size == 8)
					{
						C67_STW_PTR_PRE_INC(r+1, C67_SP, loc/4+1);  // STW  r, *+SP[3] (go back and put the other)
					}
					loc += size;

				}
			}
		}
	}
	else
	{
		CheckPreserveRegs(&A2_inuse,&A3_inuse,&B0_inuse,&B1_inuse);
	}

    gcall_or_jmp(0);

	if (stackused)
	{
		// recover stack

		C67_ADDK(stackused,C67_SP);    //  ADDK.L2 loc,SP
	}

	// Now reload any values that we saved before the call 
	
	CheckRestoreRegs(A2_inuse,A3_inuse,B0_inuse,B1_inuse);


#else

    gcall_or_jmp(0);

#endif

    vtop--;
}




// to be compatible with Code Composer for the C67
// the first 10 parameters must be passed in registers
// (pairs for 64 bits) starting wit; A4:A5, then B4:B5 and
// ending with B12:B13.
//
// When a call is made, if the caller has its parameters
// in regs A4-B13 these must be saved before/as the call 
// parameters are loaded and restored upon return (or if/when needed).

/* generate function prolog of type 't' */
void gfunc_prolog(CType *func_type)
{
    int addr, align, size, func_call;
    Sym *sym;
    CType *type;

    sym = func_type->ref;
    func_call = sym->r;
    addr = 8;
    /* if the function returns a structure, then add an
       implicit pointer parameter */
    func_vt = sym->type;
    if ((func_vt.t & VT_BTYPE) == VT_STRUCT) {
        func_vc = addr;
        addr += 4;
    }

	NoOfCurFuncArgs=0;

    /* define parameters */
    while ((sym = sym->next) != NULL) {
        type = &sym->type;
        sym_push(sym->v & ~SYM_FIELD, type,
                 VT_LOCAL | VT_LVAL, addr);
        size = type_size(type, &align);
        size = (size + 3) & ~3;

#if (DO_C67)
		// keep track of size of arguments so
		// we can translate where tcc thinks they
		// are on the stack into the appropriate reg

		TranslateStackToReg[NoOfCurFuncArgs] = size;
		NoOfCurFuncArgs++;
#endif

#ifdef FUNC_STRUCT_PARAM_AS_PTR
        /* structs are passed as pointer */
        if ((type->t & VT_BTYPE) == VT_STRUCT) {
            size = 4;
        }
#endif
        addr += size;
    }
    func_ret_sub = 0;
    /* pascal type call ? */
    if (func_call == FUNC_STDCALL)
        func_ret_sub = addr - 8;

#if (DO_C67)
	C67_MV(C67_FP,C67_A0);    //  move FP -> A0
	C67_MV(C67_SP,C67_FP);    //  move SP -> FP

	// place all the args passed in regs onto the stack

	loc=0;
	for (int i=0; i<NoOfCurFuncArgs; i++)
	{

		ParamLocOnStack[i] = loc;        // remember where the param is
		loc += -8;

		C67_PUSH(TREG_C67_A4 + i*2);

		if (TranslateStackToReg[i] == 8)
		{
			C67_STW_PTR_PRE_INC(TREG_C67_A4 + i*2 + 1, C67_SP, 3);  // STW  r, *+SP[1] (go back and put the other)
		}
	}

	TotalBytesPushedOnStack=-loc;

    func_sub_sp_offset = ind; // remember where we put the stack instruction 
	C67_ADDK(0,C67_SP);       //  ADDK.L2 loc,SP  (just put zero temporarily)

    C67_PUSH(C67_A0);
    C67_PUSH(C67_B3);

#else

    o(0xe58955); /* push   %ebp, mov    %esp, %ebp */
    func_sub_sp_offset = oad(0xec81, 0); /* sub $xxx, %esp */

#endif
    /* leave some room for bound checking code */
    if (do_bounds_check) {
        oad(0xb8, 0); /* lbound section pointer */
        oad(0xb8, 0); /* call to function */
        func_bound_offset = lbounds_section->data_offset;
    }
}

/* generate function epilog */
void gfunc_epilog(void)
{
#ifdef CONFIG_TCC_BCHECK
    if (do_bounds_check && func_bound_offset != lbounds_section->data_offset) {
        int saved_ind;
        int *bounds_ptr;
        Sym *sym, *sym_data;
        /* add end of table info */
        bounds_ptr = section_ptr_add(lbounds_section, sizeof(int));
        *bounds_ptr = 0;
        /* generate bound local allocation */
        saved_ind = ind;
        ind = func_sub_sp_offset + 4;
        sym_data = get_sym_ref(&char_pointer_type, lbounds_section, 
                               func_bound_offset, lbounds_section->data_offset);
        greloc(cur_text_section, sym_data,
               ind + 1, R_386_32);
        oad(0xb8, 0); /* mov %eax, xxx */
        sym = external_global_sym(TOK___bound_local_new, &func_old_type, 0);
        greloc(cur_text_section, sym, 
               ind + 1, R_386_PC32);
        oad(0xe8, -4);
        ind = saved_ind;
        /* generate bound check local freeing */
        o(0x5250); /* save returned value, if any */
        greloc(cur_text_section, sym_data,
               ind + 1, R_386_32);
        oad(0xb8, 0); /* mov %eax, xxx */
        sym = external_global_sym(TOK___bound_local_delete, &func_old_type, 0);
        greloc(cur_text_section, sym, 
               ind + 1, R_386_PC32);
        oad(0xe8, -4);
        o(0x585a); /* restore returned value, if any */
    }
#endif

#if (DO_C67)
	{
		int local = (-loc + 7) & -8;          // stack must stay aligned to 8 bytes for LDDW instr
		C67_POP(C67_B3);
		C67_NOP(4);									// NOP wait for load
		C67_IREG_B_REG(0,C67_CREG_ZERO,C67_B3);    //  B.S2  B3
		C67_POP(C67_FP);
		C67_ADDK(local,C67_SP);    //  ADDK.L2 loc,SP  
		C67_Adjust_ADDK((int *)(cur_text_section->data + func_sub_sp_offset),-local+TotalBytesPushedOnStack);
		C67_NOP(3);			      // NOP 
	}
#else

    o(0xc9); /* leave */
    if (func_ret_sub == 0) {
        o(0xc3); /* ret */
    } else {
        o(0xc2); /* ret n */
        g(func_ret_sub);
        g(func_ret_sub >> 8);
    }

    /* align local size to word & save local variables */
    *(int *)(cur_text_section->data + func_sub_sp_offset) = (-loc + 3) & -4; 
#endif
}

/* generate a jump to a label */
int gjmp(int t)
{
#if (DO_C67)
	int ind1=ind;

	C67_MVKL(C67_A0,t);   //r=reg to load,  constant
	C67_MVKH(C67_A0,t);   //r=reg to load,  constant
	C67_IREG_B_REG(0,C67_CREG_ZERO,C67_A0);    // [!R] B.S2x  A0
	C67_NOP(5);
	return ind1;

#else
    return psym(0xe9, t);
#endif
}

/* generate a jump to a fixed address */
void gjmp_addr(int a)
{
#if (DO_C67)
	Sym *sym;
	// I guess this routine is used for relative short
	// local jumps, for now just handle it as the general
	// case
	
	// define a label that will be relocated

	sym = get_sym_ref(&char_pointer_type, cur_text_section, a, 0);
	greloc(cur_text_section, sym, ind,   R_C60LO16);
	greloc(cur_text_section, sym, ind+4, R_C60HI16);

	gjmp(0);  // place a zero there later the symbol will be added to it
#else
    int r;
    r = a - ind - 2;
    if (r == (char)r) {
        g(0xeb);
        g(r);
    } else {
        oad(0xe9, a - ind - 5);
    }
#endif
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
int gtst(int inv, int t)
{
#if (DO_C67)
	int ind1, n;
#endif
    int v, *p;

    v = vtop->r & VT_VALMASK;
    if (v == VT_CMP) {
        /* fast case : can jump directly since flags are set */
#if (DO_C67)
		// C67 uses B2 sort of as flags register
		ind1=ind;                      
		C67_MVKL(C67_A0,t);   //r=reg to load, constant
		C67_MVKH(C67_A0,t);   //r=reg to load, constant

		if (C67_compare_reg != TREG_EAX &&  // check if not already in a conditional test reg
			C67_compare_reg != TREG_EDX &&
			C67_compare_reg != TREG_ST0 && 
			C67_compare_reg != C67_B2)
		{
			C67_MV(C67_compare_reg,C67_B2);
			C67_compare_reg = C67_B2;
		}
		
		C67_IREG_B_REG(C67_invert_test^inv,C67_compare_reg,C67_A0);    // [!R] B.S2x  A0
		C67_NOP(5);
		t=ind1;                     //return where we need to patch

#else
        g(0x0f);
        t = psym((vtop->c.i - 16) ^ inv, t);
#endif
    } else if (v == VT_JMP || v == VT_JMPI) {
        /* && or || optimization */
        if ((v & 1) == inv) 
		{
            /* insert vtop->c jump list in t */
            p = &vtop->c.i;
        
#if (DO_C67)
			// I guess the idea is to traverse to the
			// null at the end of the list and store t
			// there
			
			n = *p;
			while (n != 0)
            {
				p = (int *)(cur_text_section->data + n);

				// extract 32 bit address from MVKH/MVKL
				n =  ((*p     >> 7) & 0xffff);
				n |= ((*(p+1) >> 7) & 0xffff)<<16;
			}
            *p     |= (   t    & 0xffff) << 7;
            *(p+1) |= ((t>>16) & 0xffff) << 7;
#else
			while (*p != 0)
                p = (int *)(cur_text_section->data + *p);
            *p = t;
#endif
            t = vtop->c.i;

        } 
		else 
		{
            t = gjmp(t);
            gsym(vtop->c.i);
        }
    } else {
        if (is_float(vtop->type.t)) {
            vpushi(0);
            gen_op(TOK_NE);
        }
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* constant jmp optimization */
            if ((vtop->c.i != 0) != inv) 
                t = gjmp(t);
        } else {
#if (DO_C67)
			// I think we need to get the value on the stack
			// into a register, test it, and generate a branch
			// return the address of the branch, so it can be
			// later patched

            v = gv(RC_INT);   // get value into a reg 
			ind1=ind;                      
			C67_MVKL(C67_A0,t);   //r=reg to load, constant
			C67_MVKH(C67_A0,t);   //r=reg to load, constant

			if (v != TREG_EAX &&  // check if not already in a conditional test reg
				v != TREG_EDX &&
				v != TREG_ST0 && 
				v != C67_B2)
			{
				C67_MV(v,C67_B2);
				v = C67_B2;
			}
			
			C67_IREG_B_REG(inv,v,C67_A0);    // [!R] B.S2x  A0
			C67_NOP(5);
			t=ind1;                     //return where we need to patch
			ind1=ind;                      
#else
            v = gv(RC_INT);
            o(0x85);
            o(0xc0 + v * 9);
            g(0x0f);
            t = psym(0x85 ^ inv, t);
#endif
        }
    }
    vtop--;
    return t;
}

/* generate an integer binary operation */
void gen_opi(int op)
{
#if (DO_C67)

	
	int r, fr, opc;

    switch(op) {
    case '+':
    case TOK_ADDC1: /* add with carry generation */
        opc = 0;
    gen_op8:


// C67 can't do const compares, must load into a reg
// so just go to gv2 directly - tktk



        if (op >= TOK_ULT && op <= TOK_GT) 
			gv2(RC_INT_BSIDE, RC_INT);  // make sure r (src1) is on the B Side of CPU
		else
			gv2(RC_INT, RC_INT);  

        r = vtop[-1].r;
        fr = vtop[0].r;

		C67_compare_reg = C67_B2;


		if (op == TOK_LT)             
		{
			C67_CMPLT(r,fr,C67_B2);      
			C67_invert_test = false;
		}
		else if (op == TOK_GE)
		{
			C67_CMPLT(r,fr,C67_B2);      
			C67_invert_test = true;
		}
		else if (op == TOK_GT)
		{
			C67_CMPGT(r,fr,C67_B2);      
			C67_invert_test = false;
		}
		else if (op == TOK_LE)
		{
			C67_CMPGT(r,fr,C67_B2);      
			C67_invert_test = true;
		}
		else if (op == TOK_EQ)
		{
			C67_CMPEQ(r,fr,C67_B2);      
			C67_invert_test = false;
		}
		else if (op == TOK_NE)
		{
			C67_CMPEQ(r,fr,C67_B2);      
			C67_invert_test = true;
		}
		else if (op == TOK_ULT)
		{
			C67_CMPLTU(r,fr,C67_B2);      
			C67_invert_test = false;
		}
		else if (op == TOK_UGE)
		{
			C67_CMPLTU(r,fr,C67_B2);      
			C67_invert_test = true;
		}
		else if (op == TOK_UGT)
		{
			C67_CMPGTU(r,fr,C67_B2);      
			C67_invert_test = false;
		}
		else if (op == TOK_ULE)
		{
			C67_CMPGTU(r,fr,C67_B2);      
			C67_invert_test = true;
		}
		else if (op == '+')
			C67_ADD(fr,r);         // ADD  r,fr,r
		else if (op == '-')
			C67_SUB(fr,r);         // SUB  r,fr,r
		else if (op == '&')
			C67_AND(fr,r);         // AND  r,fr,r
		else if (op == '|')
			C67_OR(fr,r);          // OR  r,fr,r
		else if (op == '^')
			C67_XOR(fr,r);         // XOR  r,fr,r
		else
			ALWAYS_ASSERT(FALSE);

        vtop--;
        if (op >= TOK_ULT && op <= TOK_GT) {
            vtop->r = VT_CMP;
            vtop->c.i = op;
        }
        break;
    case '-':
    case TOK_SUBC1: /* sub with carry generation */
        opc = 5;
        goto gen_op8;
    case TOK_ADDC2: /* add with carry use */
        opc = 2;
        goto gen_op8;
    case TOK_SUBC2: /* sub with carry use */
        opc = 3;
        goto gen_op8;
    case '&':
        opc = 4;
        goto gen_op8;
    case '^':
        opc = 6;
        goto gen_op8;
    case '|':
        opc = 1;
        goto gen_op8;
    case '*':
    case TOK_UMULL:
        gv2(RC_INT, RC_INT);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
		C67_MPYI(fr,r);         // 32 bit bultiply  fr,r,fr
		C67_NOP(8);			    // NOP 8 for worst case
        break;
    case TOK_SHL:
        gv2(RC_INT_BSIDE, RC_INT_BSIDE);  // shift amount must be on same side as dst
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
		C67_SHL(fr,r);          // arithmetic/logical shift
        break;

    case TOK_SHR:
        gv2(RC_INT_BSIDE, RC_INT_BSIDE);   // shift amount must be on same side as dst
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
		C67_SHRU(fr,r);          // logical shift
        break;

    case TOK_SAR:
        gv2(RC_INT_BSIDE, RC_INT_BSIDE);   // shift amount must be on same side as dst
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
		C67_SHR(fr,r);          // arithmetic shift
        break;

	case '/':
		{
		    GFuncContext gf;

			vswap();
			/* call generic idiv function */
			gfunc_start(&gf, FUNC_CDECL);
			gfunc_param(&gf);
			gfunc_param(&gf);
			vpush_global_sym(&func_old_type, ts_divi->tok);
			gfunc_call(&gf);
			vpushi(0);
			vtop->r = REG_IRET;
			vtop->r2 = VT_CONST;
		}
		break;


    case TOK_UDIV:
    case TOK_PDIV:
		{
		    GFuncContext gf;

			vswap();
			/* call generic udiv function */
			gfunc_start(&gf, FUNC_CDECL);
			gfunc_param(&gf);
			gfunc_param(&gf);
			vpush_global_sym(&func_old_type, ts_divu->tok);
			gfunc_call(&gf);
			vpushi(0);
			vtop->r = REG_IRET;
			vtop->r2 = VT_CONST;
		}
		break;


    case '%':
		{
		    GFuncContext gf;

			vswap();
			/* call generic mod function */
			gfunc_start(&gf, FUNC_CDECL);
			gfunc_param(&gf);
			gfunc_param(&gf);
			vpush_global_sym(&func_old_type, ts_remi->tok);
			gfunc_call(&gf);
			vpushi(0);
			vtop->r = REG_IRET;
			vtop->r2 = VT_CONST;
		}
		break;

	
	case TOK_UMOD:
		{
		    GFuncContext gf;

			vswap();
			/* call generic mod function */
			gfunc_start(&gf, FUNC_CDECL);
			gfunc_param(&gf);
			gfunc_param(&gf);
			vpush_global_sym(&func_old_type, ts_remu->tok);
			gfunc_call(&gf);
			vpushi(0);
			vtop->r = REG_IRET;
			vtop->r2 = VT_CONST;
		}
        break;

    default:
        opc = 7;
        goto gen_op8;
    }
}
    
	
#else
	
	int r, fr, opc;
	int c;

    switch(op) {
    case '+':
    case TOK_ADDC1: /* add with carry generation */
        opc = 0;
    gen_op8:
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* constant case */
            vswap();
            r = gv(RC_INT);
            vswap();
            c = vtop->c.i;
            if (c == (char)c) {
                /* XXX: generate inc and dec for smaller code ? */
                o(0x83);
                o(0xc0 | (opc << 3) | r);
                g(c);
            } else {
                o(0x81);
                oad(0xc0 | (opc << 3) | r, c);
            }
        } else {

            gv2(RC_INT, RC_INT);
            r = vtop[-1].r;
            fr = vtop[0].r;
            o((opc << 3) | 0x01);
            o(0xc0 + r + fr * 8); 
        }
        vtop--;
        if (op >= TOK_ULT && op <= TOK_GT) {
            vtop->r = VT_CMP;
            vtop->c.i = op;
        }
        break;
    case '-':
    case TOK_SUBC1: /* sub with carry generation */
        opc = 5;
        goto gen_op8;
    case TOK_ADDC2: /* add with carry use */
        opc = 2;
        goto gen_op8;
    case TOK_SUBC2: /* sub with carry use */
        opc = 3;
        goto gen_op8;
    case '&':
        opc = 4;
        goto gen_op8;
    case '^':
        opc = 6;
        goto gen_op8;
    case '|':
        opc = 1;
        goto gen_op8;
    case '*':
        gv2(RC_INT, RC_INT);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        o(0xaf0f); /* imul fr, r */
        o(0xc0 + fr + r * 8);
        break;
    case TOK_SHL:
        opc = 4;
        goto gen_shift;
    case TOK_SHR:
        opc = 5;
        goto gen_shift;
    case TOK_SAR:
        opc = 7;
    gen_shift:
        opc = 0xc0 | (opc << 3);
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* constant case */
            vswap();
            r = gv(RC_INT);
            vswap();
            c = vtop->c.i & 0x1f;
            o(0xc1); /* shl/shr/sar $xxx, r */
            o(opc | r);
            g(c);
        } else {
            /* we generate the shift in ecx */
            gv2(RC_INT, RC_ECX);
            r = vtop[-1].r;
            o(0xd3); /* shl/shr/sar %cl, r */
            o(opc | r);
        }
        vtop--;
        break;
	
	case '/':
    case TOK_UDIV:
    case TOK_PDIV:
    case '%':
    case TOK_UMOD:
    case TOK_UMULL:
        /* first operand must be in eax */
        /* XXX: need better constraint for second operand */
        gv2(RC_EAX, RC_ECX);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        save_reg(TREG_EDX);
        if (op == TOK_UMULL) {
            o(0xf7); /* mul fr */
            o(0xe0 + fr);
            vtop->r2 = TREG_EDX;
            r = TREG_EAX;
        } else {
            if (op == TOK_UDIV || op == TOK_UMOD) {
                o(0xf7d231); /* xor %edx, %edx, div fr, %eax */
                o(0xf0 + fr);
            } else {
                o(0xf799); /* cltd, idiv fr, %eax */
                o(0xf8 + fr);
            }
            if (op == '%' || op == TOK_UMOD)
                r = TREG_EDX;
            else
                r = TREG_EAX;
        }
        vtop->r = r;
        break;
    default:
        opc = 7;
        goto gen_op8;
    }
}

#endif

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranted to have the same floating point type */
/* XXX: need to use ST1 too */
void gen_opf(int op)
{
#if (DO_C67)
    int ft, fc, fr,r;

    if (op >= TOK_ULT && op <= TOK_GT) 
		gv2(RC_EDX,RC_EAX);  // make sure src2 is on b side
	else
		gv2(RC_FLOAT,RC_FLOAT);  // make sure src2 is on b side
	
    ft = vtop->type.t;
    fc = vtop->c.ul;
    r = vtop->r;
    fr = vtop[-1].r;


    if ((ft & VT_BTYPE) == VT_LDOUBLE) 
		error("long doubles not supported");

    if (op >= TOK_ULT && op <= TOK_GT) 
	{

        r = vtop[-1].r;
        fr = vtop[0].r;

		C67_compare_reg = C67_B2;

		if (op == TOK_LT)             
		{
		    if ((ft & VT_BTYPE) == VT_DOUBLE)
			{
				C67_CMPLTDP(r,fr,C67_B2);      
				C67_NOP(1);
			}
			else
			{
				C67_CMPLTSP(r,fr,C67_B2);      
			}

			C67_invert_test = false;
	        vtop--;
		}
		else if (op == TOK_GE)
		{
		    if ((ft & VT_BTYPE) == VT_DOUBLE) 
			{
				C67_CMPLTDP(r,fr,C67_B2);      
				C67_NOP(1);
			}
			else
			{
				C67_CMPLTSP(r,fr,C67_B2);      
			}
			C67_invert_test = true;
	        vtop--;
		}
		else if (op == TOK_GT)
		{
		    if ((ft & VT_BTYPE) == VT_DOUBLE) 
			{
				C67_CMPGTDP(r,fr,C67_B2);      
				C67_NOP(1);
			}
			else
			{
				C67_CMPGTSP(r,fr,C67_B2);      
			}

			C67_invert_test = false;
	        vtop--;
		}
		else if (op == TOK_LE)
		{
		    if ((ft & VT_BTYPE) == VT_DOUBLE) 
			{
				C67_CMPGTDP(r,fr,C67_B2);      
				C67_NOP(1);
			}
			else
			{
				C67_CMPGTSP(r,fr,C67_B2);      
			}

			C67_invert_test = true;
	        vtop--;
		}
		else if (op == TOK_EQ)
		{
		    if ((ft & VT_BTYPE) == VT_DOUBLE) 
			{
				C67_CMPEQDP(r,fr,C67_B2);      
				C67_NOP(1);
			}
			else
			{
				C67_CMPEQSP(r,fr,C67_B2);      
			}

			C67_invert_test = false;
	        vtop--;
		}
		else if (op == TOK_NE)
		{
		    if ((ft & VT_BTYPE) == VT_DOUBLE) 
			{
				C67_CMPEQDP(r,fr,C67_B2);      
				C67_NOP(1);
			}
			else
			{
				C67_CMPEQSP(r,fr,C67_B2);      
			}

			C67_invert_test = true;
	        vtop--;
		}
		else 
		{
			ALWAYS_ASSERT(FALSE);
		}
        vtop->r = VT_CMP;  // tell TCC that result is in "flags" actually B2
	}
	else
	{
		if (op=='+')
		{
			if ((ft & VT_BTYPE) == VT_DOUBLE)
			{
 				C67_ADDDP(r,fr);         // ADD  fr,r,fr
				C67_NOP(6);
			}
			else
			{
				C67_ADDSP(r,fr);         // ADD  fr,r,fr
				C67_NOP(3);
			}
	        vtop--;
		}
		else if (op=='-')
		{
			if ((ft & VT_BTYPE) == VT_DOUBLE)
			{
				C67_SUBDP(r,fr);         // SUB  fr,r,fr
				C67_NOP(6);
			}
			else
			{
				C67_SUBSP(r,fr);         // SUB  fr,r,fr
				C67_NOP(3);
			}
	        vtop--;
		}
		else if (op=='*')
		{
			if ((ft & VT_BTYPE) == VT_DOUBLE)
			{
				C67_MPYDP(r,fr);         // MPY  fr,r,fr
				C67_NOP(9);
			}
			else
			{
				C67_MPYSP(r,fr);         // MPY  fr,r,fr
				C67_NOP(3);
			}
	        vtop--;
		}
		else if (op=='/')
		{
			if ((ft & VT_BTYPE) == VT_DOUBLE)
			{
				// must call intrinsic DP floating point divide
				GFuncContext gf;

				vswap();
				/* call generic idiv function */
				gfunc_start(&gf, FUNC_CDECL);
				gfunc_param(&gf);
				gfunc_param(&gf);
				vpush_global_sym(&func_old_type, ts_divd->tok);
				gfunc_call(&gf);
				vpushi(0);
				vtop->r = REG_FRET;
				vtop->r2 = REG_LRET;

			}
			else
			{
				// must call intrinsic SP floating point divide
				GFuncContext gf;

				vswap();
				/* call generic idiv function */
				gfunc_start(&gf, FUNC_CDECL);
				gfunc_param(&gf);
				gfunc_param(&gf);
				vpush_global_sym(&func_old_type, ts_divf->tok);
				gfunc_call(&gf);
				vpushi(0);
				vtop->r = REG_FRET;
				vtop->r2 = VT_CONST;
			}
		}
		else
			ALWAYS_ASSERT(FALSE);
    }
}

#else

    int a, ft, fc, swapped, r;

    /* convert constants to memory references */
    if ((vtop[-1].r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        vswap();
        gv(RC_FLOAT);
        vswap();
    }
    if ((vtop[0].r & (VT_VALMASK | VT_LVAL)) == VT_CONST)
        gv(RC_FLOAT);

    /* must put at least one value in the floating point register */
    if ((vtop[-1].r & VT_LVAL) &&
        (vtop[0].r & VT_LVAL)) {
        vswap();
        gv(RC_FLOAT);
        vswap();
    }
    swapped = 0;
    /* swap the stack if needed so that t1 is the register and t2 is
       the memory reference */
    if (vtop[-1].r & VT_LVAL) {
        vswap();
        swapped = 1;
    }
    if (op >= TOK_ULT && op <= TOK_GT) {
        /* load on stack second operand */
        load(TREG_ST0, vtop);
        save_reg(TREG_EAX); /* eax is used by FP comparison code */
        if (op == TOK_GE || op == TOK_GT)
            swapped = !swapped;
        else if (op == TOK_EQ || op == TOK_NE)
            swapped = 0;
        if (swapped)
            o(0xc9d9); /* fxch %st(1) */
        o(0xe9da); /* fucompp */
        o(0xe0df); /* fnstsw %ax */
        if (op == TOK_EQ) {
            o(0x45e480); /* and $0x45, %ah */
            o(0x40fC80); /* cmp $0x40, %ah */
        } else if (op == TOK_NE) {
            o(0x45e480); /* and $0x45, %ah */
            o(0x40f480); /* xor $0x40, %ah */
            op = TOK_NE;
        } else if (op == TOK_GE || op == TOK_LE) {
            o(0x05c4f6); /* test $0x05, %ah */
            op = TOK_EQ;
        } else {
            o(0x45c4f6); /* test $0x45, %ah */
            op = TOK_EQ;
        }
        vtop--;
        vtop->r = VT_CMP;
        vtop->c.i = op;
    } else {
        /* no memory reference possible for long double operations */
        if ((vtop->type.t & VT_BTYPE) == VT_LDOUBLE) {
            load(TREG_ST0, vtop);
            swapped = !swapped;
        }
        
        switch(op) {
        default:
        case '+':
            a = 0;
            break;
        case '-':
            a = 4;
            if (swapped)
                a++;
            break;
        case '*':
            a = 1;
            break;
        case '/':
            a = 6;
            if (swapped)
                a++;
            break;
        }
        ft = vtop->type.t;
        fc = vtop->c.ul;
        if ((ft & VT_BTYPE) == VT_LDOUBLE) {
            o(0xde); /* fxxxp %st, %st(1) */
            o(0xc1 + (a << 3));
        } else {
            /* if saved lvalue, then we must reload it */
            r = vtop->r;
            if ((r & VT_VALMASK) == VT_LLOCAL) {
                SValue v1;
                r = get_reg(RC_INT);
                v1.type.t = VT_INT;
                v1.r = VT_LOCAL | VT_LVAL;
                v1.c.ul = fc;
                load(r, &v1);
                fc = 0;
            }

            if ((ft & VT_BTYPE) == VT_DOUBLE)
                o(0xdc);
            else
                o(0xd8);
            gen_modrm(a, r, vtop->sym, fc);
        }
        vtop--;
    }
}
#endif

/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
void gen_cvt_itof(int t)
{
#if (DO_C67)
	int r;

    gv(RC_FLOAT);  // C67 needs reg pairs to handle doubles
	r=vtop->r;

	if ((t & VT_BTYPE) == VT_DOUBLE)
	{
		if (t & VT_UNSIGNED)
			C67_INTDPU(r, r);
		else
			C67_INTDP(r, r);

		C67_NOP(4);
		vtop->type.t = VT_DOUBLE;
	}
	else
	{
		if (t & VT_UNSIGNED)
			C67_INTSPU(r, r);
		else
			C67_INTSP(r, r);
		C67_NOP(3);
		vtop->type.t = VT_FLOAT;
	}

#else
    save_reg(TREG_ST0);
    gv(RC_INT);
    if ((vtop->type.t & VT_BTYPE) == VT_LLONG) {
        /* signed long long to float/double/long double (unsigned case
           is handled generically) */
        o(0x50 + vtop->r2); /* push r2 */
        o(0x50 + (vtop->r & VT_VALMASK)); /* push r */
        o(0x242cdf); /* fildll (%esp) */
        o(0x08c483); /* add $8, %esp */
    } else if ((vtop->type.t & (VT_BTYPE | VT_UNSIGNED)) == 
               (VT_INT | VT_UNSIGNED)) {
        /* unsigned int to float/double/long double */
        o(0x6a); /* push $0 */
        g(0x00);
        o(0x50 + (vtop->r & VT_VALMASK)); /* push r */
        o(0x242cdf); /* fildll (%esp) */
        o(0x08c483); /* add $8, %esp */
    } else {
        /* int to float/double/long double */
        o(0x50 + (vtop->r & VT_VALMASK)); /* push r */
        o(0x2404db); /* fildl (%esp) */
        o(0x04c483); /* add $4, %esp */
    }
    vtop->r = TREG_ST0;
#endif
}

/* convert fp to int 't' type */
/* XXX: handle long long case */
void gen_cvt_ftoi(int t)
{
#if (DO_C67)
	int r;

    gv(RC_FLOAT);
	r=vtop->r;

    if (t != VT_INT)
        error("long long not supported");
    else 
    {
		if ((vtop->type.t & VT_BTYPE) == VT_DOUBLE)
		{
			C67_DPTRUNC(r, r);
			C67_NOP(3);
		}
		else
		{
			C67_SPTRUNC(r, r);
			C67_NOP(3);
		}

		vtop->type.t = VT_INT;

	}
#else

    int r, r2, size;
    Sym *sym;
    CType ushort_type;

    ushort_type.t = VT_SHORT | VT_UNSIGNED;

    gv(RC_FLOAT);
    if (t != VT_INT)
        size = 8;
    else 
        size = 4;
    
    o(0x2dd9); /* ldcw xxx */
    sym = external_global_sym(TOK___tcc_int_fpu_control, 
                              &ushort_type, VT_LVAL);
    greloc(cur_text_section, sym, 
           ind, R_386_32);
    gen_le32(0);
    
    oad(0xec81, size); /* sub $xxx, %esp */
    if (size == 4)
        o(0x1cdb); /* fistpl */
    else
        o(0x3cdf); /* fistpll */
    o(0x24);
    o(0x2dd9); /* ldcw xxx */
    sym = external_global_sym(TOK___tcc_fpu_control, 
                              &ushort_type, VT_LVAL);
    greloc(cur_text_section, sym, 
           ind, R_386_32);
    gen_le32(0);

    r = get_reg(RC_INT);
    o(0x58 + r); /* pop r */
    if (size == 8) {
        if (t == VT_LLONG) {
            vtop->r = r; /* mark reg as used */
            r2 = get_reg(RC_INT);
            o(0x58 + r2); /* pop r2 */
            vtop->r2 = r2;
        } else {
            o(0x04c483); /* add $4, %esp */
        }
    }
    vtop->r = r;
#endif
}

/* convert from one floating point type to another */
void gen_cvt_ftof(int t)
{
#if (DO_C67)
	int r,r2;

	if ((vtop->type.t & VT_BTYPE) == VT_DOUBLE &&
		(t & VT_BTYPE)            == VT_FLOAT)
	{
		// convert double to float

	    gv(RC_FLOAT);  // get it in a register pair
		
		r = vtop->r;

		C67_DPSP(r,r);  // convert it to SP same register
		C67_NOP(3);

		vtop->type.t = VT_FLOAT;
		vtop->r2 = VT_CONST;      // set this as unused
	}
	else if ((vtop->type.t & VT_BTYPE) == VT_FLOAT &&
		     (t & VT_BTYPE)            == VT_DOUBLE)
	{
		// convert float to double

	    gv(RC_FLOAT);  // get it in a register

		r = vtop->r;

		if (r==TREG_EAX)         // make sure the paired reg is avail
			r2 = get_reg(RC_ECX);
		else if (r==TREG_EDX)
			r2 = get_reg(RC_ST0);
		else
			ALWAYS_ASSERT(FALSE);

		C67_SPDP(r,r);  // convert it to DP same register
		C67_NOP(1);

		vtop->type.t = VT_DOUBLE;
		vtop->r2 = r2;      // set this as unused
	}
	else
	{
		ALWAYS_ASSERT(FALSE);
	}

#else
    /* all we have to do on i386 is to put the float in a register */
    gv(RC_FLOAT);
#endif
}

/* computed goto support */
void ggoto(void)
{
    gcall_or_jmp(1);
    vtop--;
}

/* bound check support functions */
#ifdef CONFIG_TCC_BCHECK

/* generate a bounded pointer addition */
void gen_bounded_ptr_add(void)
{
    Sym *sym;

    /* prepare fast i386 function call (args in eax and edx) */
    gv2(RC_EAX, RC_EDX);
    /* save all temporary registers */
    vtop -= 2;
    save_regs(0);
    /* do a fast function call */
    sym = external_global_sym(TOK___bound_ptr_add, &func_old_type, 0);
    greloc(cur_text_section, sym, 
           ind + 1, R_386_PC32);
    oad(0xe8, -4);
    /* returned pointer is in eax */
    vtop++;
    vtop->r = TREG_EAX | VT_BOUNDED;
    /* address of bounding function call point */
    vtop->c.ul = (cur_text_section->reloc->data_offset - sizeof(Elf32_Rel)); 
}

/* patch pointer addition in vtop so that pointer dereferencing is
   also tested */
void gen_bounded_ptr_deref(void)
{
    int func;
    int size, align;
    Elf32_Rel *rel;
    Sym *sym;

    size = 0;
    /* XXX: put that code in generic part of tcc */
    if (!is_float(vtop->type.t)) {
        if (vtop->r & VT_LVAL_BYTE)
            size = 1;
        else if (vtop->r & VT_LVAL_SHORT)
            size = 2;
    }
    if (!size)
        size = type_size(&vtop->type, &align);
    switch(size) {
    case  1: func = TOK___bound_ptr_indir1; break;
    case  2: func = TOK___bound_ptr_indir2; break;
    case  4: func = TOK___bound_ptr_indir4; break;
    case  8: func = TOK___bound_ptr_indir8; break;
    case 12: func = TOK___bound_ptr_indir12; break;
    case 16: func = TOK___bound_ptr_indir16; break;
    default:
        error("unhandled size when derefencing bounded pointer");
        func = 0;
        break;
    }

    /* patch relocation */
    /* XXX: find a better solution ? */
    rel = (Elf32_Rel *)(cur_text_section->reloc->data + vtop->c.ul);
    sym = external_global_sym(func, &func_old_type, 0);
    if (!sym->c)
        put_extern_sym(sym, NULL, 0, 0);
    rel->r_info = ELF32_R_INFO(sym->c, ELF32_R_TYPE(rel->r_info));
}
#endif

/* end of X86 code generator */
/*************************************************************/
