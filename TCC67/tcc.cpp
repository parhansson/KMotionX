/*
 *  TCC - Tiny C Compiler
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


#define _CRT_SECURE_NO_DEPRECATE
#define WINVER 0x0600 
#define _WIN32_IE 0x0600 

#include <afxwin.h>
#ifdef WIN32
#define _GNU_SOURCE
#else
#define _close close
#define _read read
#define _getcwd getcwd
#define _lseek lseek
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <signal.h>
#include <fcntl.h>
#include <setjmp.h>
#ifdef WIN32
#include <sys/timeb.h>
#endif
#ifndef WIN32
#include <sys/time.h>
#include <sys/ucontext.h>
#endif
#if defined __LP64__ && defined __APPLE__
// PH must be included before elf.h on OS X
#include <vector>
#endif
#include "elf.h"
#include "stab.h"
#ifndef CONFIG_TCC_STATIC
#include <dlfcn.h>
#endif

#ifdef WIN32
#include <direct.h> 
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <sys/locking.h>
#include <share.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <io.h>
#endif


#ifdef _DEBUG
#define ASSEMBLY_LISTING_C67 
#endif

#include "tcc.h"
#include "libtcc.h"
#include "WriteCoff.h"

/* parser debug */
//#define PARSE_DEBUG
/* preprocessor debug */
//#define PP_DEBUG
/* include file debug */
//#define INC_DEBUG

//#define MEM_DEBUG

/* assembler debug */
//#define ASM_DEBUG

/* target selection */
//#define TCC_TARGET_I386   /* i386 code generator */

/* default target is I386 */

/* parser */
static struct BufferedFile *file;
static int ch, tok;
static CValue tokc;
static CTString tokcstr; /* current parsed string, if any */
/* additionnal informations about token */
static int tok_flags;
#define TOK_FLAG_BOL   0x0001 /* beginning of line before */
#define TOK_FLAG_BOF   0x0002 /* beginning of file before */
#define TOK_FLAG_ENDIF 0x0004 /* a endif was found matching starting #ifdef */

static int *macro_ptr, *macro_ptr_allocated;
static int *unget_saved_macro_ptr;
static int unget_saved_buffer[TOK_MAX_SIZE + 1];
static int unget_buffer_enabled;
static int parse_flags;
#define PARSE_FLAG_PREPROCESS 0x0001 /* activate preprocessing */
#define PARSE_FLAG_TOK_NUM    0x0002 /* return numbers instead of TOK_PPNUM */
#define PARSE_FLAG_LINEFEED   0x0004 /* line feed is returned as a
                                        token. line feed is also
                                        returned at eof */
 
TCCState TheTCCState;


static Section *text_section, *data_section, *bss_section; /* predefined sections */
static Section *cur_text_section; /* current section where function code is
                              generated */
/* bound check related sections */
static Section *bounds_section; /* contains global data bound description */
static Section *lbounds_section; /* contains local data bound description */
/* symbol sections */
Section *symtab_section, *strtab_section;

/* debug sections */
Section *stab_section, *stabstr_section;

/* loc : local variable index
   ind : output code index
   rsym: return symbol
   anon_sym: anonymous symbol index
*/
static int rsym, anon_sym, ind, loc;
/* expression generation modifiers */
static int const_wanted; /* true if constant wanted */
static int nocode_wanted; /* true if no code generation wanted for an expression */
static int global_expr;  /* true if compound literals must be allocated
                            globally (used during initializers parsing */
static CType func_vt; /* current function return type (used by return
                         instruction) */
static int func_vc;
static int last_line_num, last_ind, func_ind; /* debug last line number and pc */
static int tok_ident;
static TokenSym **table_ident;
static TokenSym *hash_ident[TOK_HASH_SIZE];
static char token_buf[STRING_MAX_SIZE + 1];
static const char *funcname;
static Sym *global_stack, *local_stack;
static Sym *define_stack;
static Sym *global_label_stack, *local_label_stack;

static SValue vstack[VSTACK_SIZE], *vtop;
/* some predefined types */
static CType char_pointer_type, func_old_type, int_type;
/* true if isid(c) || isnum(c) */
static unsigned char isidnum_table[256];

/* compile with debug symbol (and use them if error during execution) */
int do_debug = 0;

/* compile with built-in memory and bounds checker */
static int do_bounds_check = 0;

/* display benchmark infos */
static int do_bench = 0;
static int total_lines;
static int total_bytes;

/* use GNU C extensions */
static int gnu_ext = 1;

/* use Tiny C extensions */
static int tcc_ext = 1;

/* max number of callers shown if error */
static int num_callers = 6;

/* XXX: suppress that ASAP */
static TCCState *tcc_state;

/* give the path of the tcc libraries */
static const char *tcc_lib_path = CONFIG_TCC_PREFIX "/lib/tcc";


/* token values */

/* warning: the following compare tokens depend on i386 asm code */
#define TOK_ULT 0x92
#define TOK_UGE 0x93
#define TOK_EQ  0x94
#define TOK_NE  0x95
#define TOK_ULE 0x96
#define TOK_UGT 0x97
#define TOK_LT  0x9c
#define TOK_GE  0x9d
#define TOK_LE  0x9e
#define TOK_GT  0x9f

#define TOK_LAND  0xa0
#define TOK_LOR   0xa1

#define TOK_DEC   0xa2
#define TOK_MID   0xa3 /* inc/dec, to void constant */
#define TOK_INC   0xa4
#define TOK_UDIV  0xb0 /* unsigned division */
#define TOK_UMOD  0xb1 /* unsigned modulo */
#define TOK_PDIV  0xb2 /* fast division with undefined rounding for pointers */
#define TOK_CINT   0xb3 /* number in tokc */
#define TOK_CCHAR 0xb4 /* char constant in tokc */
#define TOK_STR   0xb5 /* pointer to string in tokc */
#define TOK_TWOSHARPS 0xb6 /* ## preprocessing token */
#define TOK_LCHAR    0xb7
#define TOK_LSTR     0xb8
#define TOK_CFLOAT   0xb9 /* float constant */
#define TOK_LINENUM  0xba /* line number info */
#define TOK_CDOUBLE  0xc0 /* double constant */
#define TOK_CLDOUBLE 0xc1 /* long double constant */
#define TOK_UMULL    0xc2 /* unsigned 32x32 -> 64 mul */
#define TOK_ADDC1    0xc3 /* add with carry generation */
#define TOK_ADDC2    0xc4 /* add with carry use */
#define TOK_SUBC1    0xc5 /* add with carry generation */
#define TOK_SUBC2    0xc6 /* add with carry use */
#define TOK_CUINT    0xc8 /* unsigned int constant */
#define TOK_CLLONG   0xc9 /* long long constant */
#define TOK_CULLONG  0xca /* unsigned long long constant */
#define TOK_ARROW    0xcb
#define TOK_DOTS     0xcc /* three dots */
#define TOK_SHR      0xcd /* unsigned shift right */
#define TOK_PPNUM    0xce /* preprocessor number */

#define TOK_SHL   0x01 /* shift left */
#define TOK_SAR   0x02 /* signed shift right */
  
/* assignement operators : normal operator or 0x80 */
#define TOK_A_MOD 0xa5
#define TOK_A_AND 0xa6
#define TOK_A_MUL 0xaa
#define TOK_A_ADD 0xab
#define TOK_A_SUB 0xad
#define TOK_A_DIV 0xaf
#define TOK_A_XOR 0xde
#define TOK_A_OR  0xfc
#define TOK_A_SHL 0x81
#define TOK_A_SAR 0x82

/* WARNING: the content of this string encodes token numbers */
static char tok_two_chars[] = "<=\236>=\235!=\225&&\240||\241++\244--\242==\224<<\1>>\2+=\253-=\255*=\252/=\257%=\245&=\246^=\336|=\374->\313..\250##\266";

#define TOK_EOF       (-1)  /* end of file */
#define TOK_LINEFEED  10    /* line feed */

/* all identificators and strings have token above that */
#define TOK_IDENT 256

/* only used for i386 asm opcodes definitions */
#define DEF_ASM(x) DEF(TOK_ASM_ ## x, #x)

#define DEF_BWL(x) \
 DEF(TOK_ASM_ ## x ## b, #x "b") \
 DEF(TOK_ASM_ ## x ## w, #x "w") \
 DEF(TOK_ASM_ ## x ## l, #x "l") \
 DEF(TOK_ASM_ ## x, #x)

#define DEF_WL(x) \
 DEF(TOK_ASM_ ## x ## w, #x "w") \
 DEF(TOK_ASM_ ## x ## l, #x "l") \
 DEF(TOK_ASM_ ## x, #x)

#define DEF_FP1(x) \
 DEF(TOK_ASM_ ## f ## x ## s, "f" #x "s") \
 DEF(TOK_ASM_ ## fi ## x ## l, "fi" #x "l") \
 DEF(TOK_ASM_ ## f ## x ## l, "f" #x "l") \
 DEF(TOK_ASM_ ## fi ## x ## s, "fi" #x "s")

#define DEF_FP(x) \
 DEF(TOK_ASM_ ## f ## x, "f" #x ) \
 DEF(TOK_ASM_ ## f ## x ## p, "f" #x "p") \
 DEF_FP1(x)

#define DEF_ASMTEST(x) \
 DEF_ASM(x ## o) \
 DEF_ASM(x ## no) \
 DEF_ASM(x ## b) \
 DEF_ASM(x ## c) \
 DEF_ASM(x ## nae) \
 DEF_ASM(x ## nb) \
 DEF_ASM(x ## nc) \
 DEF_ASM(x ## ae) \
 DEF_ASM(x ## e) \
 DEF_ASM(x ## z) \
 DEF_ASM(x ## ne) \
 DEF_ASM(x ## nz) \
 DEF_ASM(x ## be) \
 DEF_ASM(x ## na) \
 DEF_ASM(x ## nbe) \
 DEF_ASM(x ## a) \
 DEF_ASM(x ## s) \
 DEF_ASM(x ## ns) \
 DEF_ASM(x ## p) \
 DEF_ASM(x ## pe) \
 DEF_ASM(x ## np) \
 DEF_ASM(x ## po) \
 DEF_ASM(x ## l) \
 DEF_ASM(x ## nge) \
 DEF_ASM(x ## nl) \
 DEF_ASM(x ## ge) \
 DEF_ASM(x ## le) \
 DEF_ASM(x ## ng) \
 DEF_ASM(x ## nle) \
 DEF_ASM(x ## g)

#define TOK_ASM_int TOK_INT

enum {
    TOK_LAST = TOK_IDENT - 1,
#define DEF(id, str) id,
#include "tcctok.h"
#undef DEF
};

static const char tcc_keywords[] = 
#define DEF(id, str) str "\0"
#include "tcctok.h"
#undef DEF
;

#define TOK_UIDENT TOK_DEFINE

#ifdef WIN32
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#ifdef __LP64__
/* SJH - there are many cases where pointers are assumed to be same size as int.
That doesn't work for 64-bit hosts.  So we do a quick hack to fix that...
A vector is maintained which maps int to ptr.  This makes pointers take up
96 bits instead of 64, but we have no choice when the code assumes 32 bit pointers
for both host and target.  Note: code should use macros INT2PTR() and PTR2INT().
*/
#include <vector>
static std::vector<void *> _vptab(1, NULL);   // Init with 1st entry reserved for NULL.
int _ptr2int(void * p)
{
    if (!p)
        return 0;
    int i = _vptab.size();
    _vptab.push_back(p);
    return i;
}
void * _int2ptr(int i)
{
    if (!i)
        return NULL;
    ALWAYS_ASSERT((unsigned)i < _vptab.size());
    return _vptab[i];
}
#endif

#if defined(WIN32) || defined(TCC_UCLIBC) || defined(__FreeBSD__)
/* currently incorrect */
long double strtold(const char *nptr, char **endptr)
{
    return (long double)strtod(nptr, endptr);
}
float strtof(const char *nptr, char **endptr)
{
    return (float)strtod(nptr, endptr);
}
#else
/* XXX: need to define this to use them in non ISOC99 context */
extern float strtof (const char *__nptr, char **__endptr);
extern long double strtold (const char *__nptr, char **__endptr);
#endif

char *pstrcpy(char *buf, int buf_size, const char *s);
static char *pstrcat(char *buf, int buf_size, const char *s);

static void next(void);
static void next_nomacro(void);
static void parse_expr_type(CType *type);
static void expr_type(CType *type);
static void unary_type(CType *type);
static void block(int *bsym, int *csym, int *case_sym, int *def_sym, 
                  int case_reg, int is_expr);
static int expr_const(void);
static void expr_eq(void);
static void gexpr(void);
static void decl(int l);
static void decl_initializer(CType *type, Section *sec, unsigned long c, 
                             int first, int size_only);
static void decl_initializer_alloc(CType *type, AttributeDef *ad, int r, 
                                   int has_init, int v, int scope);
int gv(int rc);
void gv2(int rc1, int rc2);
void move_reg(int r, int s);
void save_regs(int n);
void save_reg(int r);
void vpop(void);
void vswap(void);
void vdup(void);
int get_reg(int rc);
int check_r2_free(int r);

static void macro_subst(TokenString *tok_str, 
                        Sym **nested_list, const int *macro_str);
int save_reg_forced(int r);
void gen_op(int op);
void force_charshort_cast(int t);
static void gen_cast(CType *type);
void vstore(void);
static Sym *sym_find(int v);
static Sym *sym_push(int v, CType *type, int r, int c);

/* type handling */
static int type_size(CType *type, int *a);
static inline CType *pointed_type(CType *type);
static int pointed_size(CType *type);
static int lvalue_type(int t);
static int is_compatible_types(CType *type1, CType *type2);
static int parse_btype(CType *type, AttributeDef *ad);
static void type_decl(CType *type, AttributeDef *ad, int *v, int td);

void error(const char *fmt, ...);
void vpushi(int v);
void vset(CType *type, int r, int v);
void type_to_str(char *buf, int buf_size, 
                 CType *type, const char *varstr);
char *get_tok_str(int v, CValue *cv);
static Sym *get_sym_ref(CType *type, Section *sec, 
                        unsigned long offset, unsigned long size);
static Sym *external_global_sym(int v, CType *type, int r);

/* section generation */
static void section_realloc(Section *sec, unsigned long new_size);
static void *section_ptr_add(Section *sec, unsigned long size);
static void put_extern_sym(Sym *sym, Section *section, 
                           unsigned long value, unsigned long size);
static void greloc(Section *s, Sym *sym, unsigned long addr, int type);
static int put_elf_str(Section *s, const char *sym);
static int put_elf_sym(Section *s, 
                       unsigned long value, unsigned long size,
                       int info, int other, int shndx, const char *name);
static int add_elf_sym(Section *s, unsigned long value, unsigned long size,
                       int info, int sh_num, const char *name, int other);
static void put_elf_reloc(Section *symtab, Section *s, unsigned long offset,
                          int type, int symbol);
static void put_stabs(const char *str, int type, int other, int desc, 
                      unsigned long value);
static void put_stabs_r(const char *str, int type, int other, int desc, 
                        unsigned long value, Section *sec, int sym_index);
static void put_stabn(int type, int other, int desc, int value);
static void put_stabd(int type, int other, int desc);
static int tcc_add_dll(TCCState *s, const char *filename, int flags);

#define AFF_PRINT_ERROR     0x0001 /* print error if file not found */
#define AFF_REFERENCED_DLL  0x0002 /* load a referenced dll from another dll */
static int tcc_add_file_internal(TCCState *s, const char *filename, int flags);


/* tccasm.c */

#ifdef CONFIG_TCC_ASM

typedef struct ExprValue {
    uint32_t v;
    Sym *sym;
} ExprValue;

#define MAX_ASM_OPERANDS 30

typedef struct ASMOperand {
    int id; /* GCC 3 optionnal identifier (0 if number only supported */
    char *constraint;
    char asm_str[16]; /* computed asm string for operand */
    SValue *vt; /* C value of the expression */
    int ref_index; /* if >= 0, gives reference to a output constraint */
    int priority; /* priority, used to assign registers */
    int reg; /* if >= 0, register number used for this operand */
} ASMOperand;

static void asm_expr(TCCState *s1, ExprValue *pe);
static int asm_int_expr(TCCState *s1);
static int find_constraint(ASMOperand *operands, int nb_operands, 
                           const char *name, const char **pp);

static int tcc_assemble(TCCState *s1, int do_preprocess);

#endif

static void asm_instr(void);


/* true if float/double/long double type */
static inline int is_float(int t)
{
    int bt;
    bt = t & VT_BTYPE;
    return bt == VT_LDOUBLE || bt == VT_DOUBLE || bt == VT_FLOAT;
}

#ifdef TCC_TARGET_I386
#include "i386-gen.c"
#endif

// true if register class may need register pair
static inline int is_pair(int rc)
{
	return (rc == RC_EAX || rc == RC_EDX || rc == RC_FLOAT);
}




#if (DO_C67)
unsigned int text_sect_addr=0x400;
unsigned int data_sect_addr=0xffffffff;
unsigned int  bss_sect_addr=0xffffffff;
#endif

#if !DO_C67
static const char **rt_bound_error_msg;
#endif


#ifdef CONFIG_TCC_STATIC

#define RTLD_LAZY       0x001
#define RTLD_NOW        0x002
#define RTLD_GLOBAL     0x100
#define RTLD_DEFAULT    NULL

/* dummy function for profiling */
void *dlopen(const char *filename, int flag)
{
    return NULL;
}

const char *dlerror(void)
{
    return "error";
}

typedef struct TCCSyms {
    const char *str;
    void *ptr;
} TCCSyms;

#define TCCSYM(a) { #a, &a, },

/* add the symbol you want here if no dynamic linking is done */
static TCCSyms tcc_syms[] = {
    //TCCSYM(printf)
    //TCCSYM(fprintf)
    //TCCSYM(fopen)
    //TCCSYM(fclose)
    { NULL, NULL },
};

void *dlsym(void *handle, const char *symbol)
{
    TCCSyms *p;
    p = tcc_syms;
    while (p->str != NULL) {
        if (!strcmp(p->str, symbol))
            return p->ptr;
        p++;
    }
    return NULL;
}

#endif

/********************************************************/

/* we use our own 'finite' function to avoid potential problems with
   non standard math libs */
/* XXX: endianness dependant */
int ieee_finite(double d)
{
    int *p = (int *)&d;
    return ((unsigned)((p[1] | 0x800fffff) + 1)) >> 31;
}

/* copy a string and truncate it. */
char *pstrcpy(char *buf, int buf_size, const char *s)
{
    char *q, *q_end;
    int c;

    if (buf_size > 0) {
        q = buf;
        q_end = buf + buf_size - 1;
        while (q < q_end) {
            c = *s++;
            if (c == '\0')
                break;
            *q++ = c;
        }
        *q = '\0';
    }
    return buf;
}

/* strcat and truncate. */
static char *pstrcat(char *buf, int buf_size, const char *s)
{
    int len;
    len = strlen(buf);
    if (len < buf_size) 
        pstrcpy(buf + len, buf_size - len, s);
    return buf;
}

/* memory management */
#ifdef MEM_DEBUG
int mem_cur_size;
int mem_max_size;
#endif

inline void tcc_free(void *ptr)
{
#ifdef MEM_DEBUG
    mem_cur_size -= malloc_usable_size(ptr);
#endif
    free(ptr);
}

void *tcc_malloc(unsigned long size)
{
    void *ptr;
    ptr = malloc(size);
    if (!ptr && size)
        error("memory full");
#ifdef MEM_DEBUG
    mem_cur_size += malloc_usable_size(ptr);
    if (mem_cur_size > mem_max_size)
        mem_max_size = mem_cur_size;
#endif
    return ptr;
}

static void *tcc_mallocz(unsigned long size)
{
    void *ptr;
    ptr = tcc_malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

static inline void *tcc_realloc(void *ptr, unsigned long size)
{
    void *ptr1;
#ifdef MEM_DEBUG
    mem_cur_size -= malloc_usable_size(ptr);
#endif
    ptr1 = realloc(ptr, size);
#ifdef MEM_DEBUG
    /* NOTE: count not correct if alloc error, but not critical */
    mem_cur_size += malloc_usable_size(ptr1);
    if (mem_cur_size > mem_max_size)
        mem_max_size = mem_cur_size;
#endif
    return ptr1;
}

static char *tcc_strdup(const char *str)
{
    char *ptr;
    ptr = (char *)tcc_malloc(strlen(str) + 1);
    strcpy(ptr, str);
    return ptr;
}

#define free(p) use_tcc_free(p)
#define malloc(s) use_tcc_malloc(s)
#define realloc(p, s) use_tcc_realloc(p, s)

static void dynarray_add(void ***ptab, int *nb_ptr, void *data)
{
    int nb, nb_alloc;
    void **pp;
    
    nb = *nb_ptr;
    pp = *ptab;
    /* every power of two we double array size */
    if ((nb & (nb - 1)) == 0) {
        if (!nb)
            nb_alloc = 1;
        else
            nb_alloc = nb * 2;
        pp = (void **)tcc_realloc(pp, nb_alloc * sizeof(void *));
        if (!pp)
            error("memory full");
        *ptab = pp;
    }
    pp[nb++] = data;
    *nb_ptr = nb;
}

Section *new_section(TCCState *s1, const char *name, int sh_type, int sh_flags)
{
    Section *sec;

    sec = (Section *)tcc_mallocz(sizeof(Section));
    pstrcpy(sec->name, sizeof(sec->name), name);
    sec->sh_type = sh_type;
    sec->sh_flags = sh_flags;
    switch(sh_type) {
    case SHT_HASH:
    case SHT_REL:
    case SHT_DYNSYM:
    case SHT_SYMTAB:
    case SHT_DYNAMIC:
        sec->sh_addralign = 4;
        break;
    case SHT_STRTAB:
        sec->sh_addralign = 1;
        break;
    default:
        sec->sh_addralign = 32; /* default conservative alignment */
        break;
    }

    /* only add section if not private */
    if (!(sh_flags & SHF_PRIVATE)) {
        sec->sh_num = s1->nb_sections;
        dynarray_add((void ***)&s1->sections, &s1->nb_sections, sec);
    }
    return sec;
}

static void free_section(Section *s)
{
    tcc_free(s->data);
    tcc_free(s);
}

/* realloc section and set its content to zero */
static void section_realloc(Section *sec, unsigned long new_size)
{
    unsigned long size;
    unsigned char *data;
    
    size = sec->data_allocated;
    if (size == 0)
        size = 1;
    while (size < new_size)
        size = size * 2;
    data = (unsigned char *)tcc_realloc(sec->data, size);
    if (!data)
        error("memory full");
    memset(data + sec->data_allocated, 0, size - sec->data_allocated);
    sec->data = data;
    sec->data_allocated = size;
}

/* reserve at least 'size' bytes in section 'sec' from
   sec->data_offset. */
static void *section_ptr_add(Section *sec, unsigned long size)
{
    unsigned long offset, offset1;

    offset = sec->data_offset;
    offset1 = offset + size;
    if (offset1 > sec->data_allocated)
        section_realloc(sec, offset1);
    sec->data_offset = offset1;
    return sec->data + offset;
}

/* return a reference to a section, and create it if it does not
   exists */
Section *find_section(TCCState *s1, const char *name)
{
    Section *sec;
    int i;
    for(i = 1; i < s1->nb_sections; i++) {
        sec = s1->sections[i];
        if (!strcmp(name, sec->name)) 
            return sec;
    }
    /* sections are created as PROGBITS */
    return new_section(s1, name, SHT_PROGBITS, SHF_ALLOC);
}

/* update sym->c so that it points to an external symbol in section
   'section' with value 'value' */
static void put_extern_sym(Sym *sym, Section *section, 
                           unsigned long value, unsigned long size)
{
    int sym_type, sym_bind, sh_num, info;
    Elf32_Sym *esym;
    const char *name;

    if (section)
        sh_num = section->sh_num;
    else
        sh_num = SHN_UNDEF;
    if (!sym->c) {
        if ((sym->type.t & VT_BTYPE) == VT_FUNC)
            sym_type = STT_FUNC;
        else
            sym_type = STT_OBJECT;
        if (sym->type.t & VT_STATIC)
            sym_bind = STB_LOCAL;
        else
            sym_bind = STB_GLOBAL;
        
        name = get_tok_str(sym->v, NULL);
#ifdef CONFIG_TCC_BCHECK
        if (do_bounds_check) {
            char buf[32];

            /* XXX: avoid doing that for statics ? */
            /* if bound checking is activated, we change some function
               names by adding the "__bound" prefix */
            switch(sym->v) {
#if 0
            /* XXX: we rely only on malloc hooks */
            case TOK_malloc: 
            case TOK_free: 
            case TOK_realloc: 
            case TOK_memalign: 
            case TOK_calloc: 
#endif
            case TOK_memcpy: 
            case TOK_memmove:
            case TOK_memset:
            case TOK_strlen:
            case TOK_strcpy:
                strcpy(buf, "__bound_");
                strcat(buf, name);
                name = buf;
                break;
            }
        }
#endif
        info = ELF32_ST_INFO(sym_bind, sym_type);
        sym->c = add_elf_sym(symtab_section, value, size, info, sh_num, name, sym->type.t);
    } else {
        esym = &((Elf32_Sym *)symtab_section->data)[sym->c];
        esym->st_value = value;
        esym->st_size = size;
        esym->st_shndx = sh_num;
    }
}

/* add a new relocation entry to symbol 'sym' in section 's' */
static void greloc(Section *s, Sym *sym, unsigned long offset, int type)
{
    if (!sym->c) 
        put_extern_sym(sym, NULL, 0, 0);
    /* now we can add ELF relocation info */
    put_elf_reloc(symtab_section, s, offset, type, sym->c);
}

static inline int isid(int c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

static inline int isnum(int c)
{
    return c >= '0' && c <= '9';
}

static inline int isoct(int c)
{
    return c >= '0' && c <= '7';
}

static inline int toup(int c)
{
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 'A';
    else
        return c;
}

static void strcat_vprintf(char *buf, int buf_size, const char *fmt, va_list ap)
{
    int len;
    len = strlen(buf);
    vsnprintf(buf + len, buf_size - len, fmt, ap);
}

static void strcat_printf(char *buf, int buf_size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    strcat_vprintf(buf, buf_size, fmt, ap);
    va_end(ap);
}

void error1(TCCState *s1, int is_warning, const char *fmt, va_list ap)
{
    char buf[2048];
    BufferedFile **f;
    
    buf[0] = '\0';
    if (file) {
        for(f = s1->include_stack; f < s1->include_stack_ptr; f++)
            strcat_printf(buf, sizeof(buf), "In file included from %s:%d:\n", 
                          (*f)->filename, (*f)->line_num);
        if (file->line_num > 0) {
            strcat_printf(buf, sizeof(buf), 
                          "%s:%d: ", file->filename, file->line_num);
        } else {
            strcat_printf(buf, sizeof(buf),
                          "%s: ", file->filename);
        }
    } else {
        strcat_printf(buf, sizeof(buf),
                      "tcc: ");
    }
    if (is_warning)
        strcat_printf(buf, sizeof(buf), "warning: ");
    strcat_vprintf(buf, sizeof(buf), fmt, ap);

    if (!s1->error_func) {
        /* default case: stderr */
        fprintf(stderr, "%s\n", buf);
    } else {
        s1->error_func(s1->error_opaque, buf);
    }
    if (!is_warning)
        s1->nb_errors++;
}

#ifdef LIBTCC
void tcc_set_error_func(TCCState *s, void *error_opaque,
                        void (*error_func)(void *opaque, const char *msg))
{
    s->error_opaque = error_opaque;
    s->error_func = error_func;
}
#endif

/* error without aborting current compilation */
void error_noabort(const char *fmt, ...)
{
    TCCState *s1 = tcc_state;
    va_list ap;

    va_start(ap, fmt);
    error1(s1, 0, fmt, ap);
    va_end(ap);
}

void error(const char *fmt, ...)
{
    TCCState *s1 = tcc_state;
    va_list ap;

    va_start(ap, fmt);
    error1(s1, 0, fmt, ap);
    va_end(ap);
    /* better than nothing: in some cases, we accept to handle errors */
    if (s1->error_set_jmp_enabled) {
        longjmp(s1->error_jmp_buf, 1);
    } else {
        /* XXX: suppress it someday */
        exit(1);
    }
}

void expect(const char *msg)
{
    error("%s expected", msg);
}

void warning(const char *fmt, ...)
{
    TCCState *s1 = tcc_state;
    va_list ap;

    va_start(ap, fmt);
    error1(s1, 1, fmt, ap);
    va_end(ap);
}

void skip(int c)
{
    if (tok != c)
        error("'%c' expected", c);
    next();
}

static void test_lvalue(void)
{
    if (!(vtop->r & VT_LVAL))
        expect("lvalue");
}

/* allocate a new token */
static TokenSym *tok_alloc_new(TokenSym **pts, const char *str, int len)
{
    TokenSym *ts, **ptable;
    int i;

    if (tok_ident >= SYM_FIRST_ANOM) 
        error("memory full");

    /* expand token table if needed */
    i = tok_ident - TOK_IDENT;
    if ((i % TOK_ALLOC_INCR) == 0) {
        ptable = (TokenSym **)tcc_realloc(table_ident, (i + TOK_ALLOC_INCR) * sizeof(TokenSym *));
        if (!ptable)
            error("memory full");
        table_ident = ptable;
    }

    ts = (TokenSym *)tcc_malloc(sizeof(TokenSym) + len);
    table_ident[i] = ts;
    ts->tok = tok_ident++;
    ts->sym_define = NULL;
    ts->sym_label = NULL;
    ts->sym_struct = NULL;
    ts->sym_identifier = NULL;
    ts->len = len;
    ts->hash_next = NULL;
    memcpy(ts->str, str, len);
    ts->str[len] = '\0';
    *pts = ts;
    return ts;
}

#define TOK_HASH_INIT 1
#define TOK_HASH_FUNC(h, c) ((h) * 263 + (c))

/* find a token and add it if not found */
TokenSym *tok_alloc(const char *str, int len)
{
    TokenSym *ts, **pts;
    int i;
    unsigned int h;
    
    h = TOK_HASH_INIT;
    for(i=0;i<len;i++)
        h = TOK_HASH_FUNC(h, ((unsigned char *)str)[i]);
    h &= (TOK_HASH_SIZE - 1);

    pts = &hash_ident[h];
    for(;;) {
        ts = *pts;
        if (!ts)
            break;
        if (ts->len == len && !memcmp(ts->str, str, len))
            return ts;
        pts = &(ts->hash_next);
    }
    return tok_alloc_new(pts, str, len);
}

/* CTString handling */

static void cstr_realloc(CTString *cstr, int new_size)
{
    int size;
    void *data;

    size = cstr->size_allocated;
    if (size == 0)
        size = 8; /* no need to allocate a too small first string */
    while (size < new_size)
        size = size * 2;
    data = tcc_realloc(cstr->data_allocated, size);
    if (!data)
        error("memory full");
    cstr->data_allocated = data;
    cstr->size_allocated = size;
    cstr->data = data;
}

/* add a byte */
static void cstr_ccat(CTString *cstr, int ch)
{
    int size;
    size = cstr->size + 1;
    if (size > cstr->size_allocated)
        cstr_realloc(cstr, size);
    ((unsigned char *)cstr->data)[size - 1] = ch;
    cstr->size = size;
}

static void cstr_cat(CTString *cstr, const char *str)
{
    int c;
    for(;;) {
        c = *str;
        if (c == '\0')
            break;
        cstr_ccat(cstr, c);
        str++;
    }
}

/* add a wide char */
static void cstr_wccat(CTString *cstr, int ch)
{
    int size;
    size = cstr->size + sizeof(int);
    if (size > cstr->size_allocated)
        cstr_realloc(cstr, size);
    *(int *)(((unsigned char *)cstr->data) + size - sizeof(int)) = ch;
    cstr->size = size;
}

static void cstr_new(CTString *cstr)
{
    memset(cstr, 0, sizeof(CTString));
}

/* free string and reset it to NULL */
static void cstr_free(CTString *cstr)
{
    tcc_free(cstr->data_allocated);
    cstr_new(cstr);
}

#define cstr_reset(cstr) cstr_free(cstr)

static CTString *cstr_dup(CTString *cstr1)
{
    CTString *cstr;
    int size;

    cstr = (CTString *)tcc_malloc(sizeof(CTString));
    size = cstr1->size;
    cstr->size = size;
    cstr->size_allocated = size;
    cstr->data_allocated = tcc_malloc(size);
    cstr->data = cstr->data_allocated;
    memcpy(cstr->data_allocated, cstr1->data_allocated, size);
    return cstr;
}

/* XXX: unicode ? */
static void add_char(CTString *cstr, int c)
{
    if (c == '\'' || c == '\"' || c == '\\') {
        /* XXX: could be more precise if char or string */
        cstr_ccat(cstr, '\\');
    }
    if (c >= 32 && c <= 126) {
        cstr_ccat(cstr, c);
    } else {
        cstr_ccat(cstr, '\\');
        if (c == '\n') {
            cstr_ccat(cstr, 'n');
        } else {
            cstr_ccat(cstr, '0' + ((c >> 6) & 7));
            cstr_ccat(cstr, '0' + ((c >> 3) & 7));
            cstr_ccat(cstr, '0' + (c & 7));
        }
    }
}

/* XXX: buffer overflow */
/* XXX: float tokens */
char *get_tok_str(int v, CValue *cv)
{
    static char buf[STRING_MAX_SIZE + 1];
    static CTString cstr_buf;
    CTString *cstr;
    unsigned char *q;
    char *p;
    int i, len;

    /* NOTE: to go faster, we give a fixed buffer for small strings */
    cstr_reset(&cstr_buf);
    cstr_buf.data = buf;
    cstr_buf.size_allocated = sizeof(buf);
    p = buf;

    switch(v) {
    case TOK_CINT:
    case TOK_CUINT:
        /* XXX: not quite exact, but only useful for testing */
        sprintf(p, "%u", cv->ui);
        break;
    case TOK_CLLONG:
    case TOK_CULLONG:
        /* XXX: not quite exact, but only useful for testing  */
        sprintf(p, "%Lu", (unsigned long long)cv->ull);
        break;
    case TOK_CCHAR:
    case TOK_LCHAR:
        cstr_ccat(&cstr_buf, '\'');
        add_char(&cstr_buf, cv->i);
        cstr_ccat(&cstr_buf, '\'');
        cstr_ccat(&cstr_buf, '\0');
        break;
    case TOK_PPNUM:
        cstr = cv->cstr;
        len = cstr->size - 1;
        for(i=0;i<len;i++)
            add_char(&cstr_buf, ((unsigned char *)cstr->data)[i]);
        cstr_ccat(&cstr_buf, '\0');
        break;
    case TOK_STR:
    case TOK_LSTR:
        cstr = cv->cstr;
        cstr_ccat(&cstr_buf, '\"');
        if (v == TOK_STR) {
            len = cstr->size - 1;
            for(i=0;i<len;i++)
                add_char(&cstr_buf, ((unsigned char *)cstr->data)[i]);
        } else {
            len = (cstr->size / sizeof(int)) - 1;
            for(i=0;i<len;i++)
                add_char(&cstr_buf, ((int *)cstr->data)[i]);
        }
        cstr_ccat(&cstr_buf, '\"');
        cstr_ccat(&cstr_buf, '\0');
        break;
    case TOK_LT:
        v = '<';
        goto addv;
    case TOK_GT:
        v = '>';
        goto addv;
    case TOK_A_SHL:
        return strcpy(p, "<<=");
    case TOK_A_SAR:
        return strcpy(p, ">>=");
    default:
        if (v < TOK_IDENT) {
            /* search in two bytes table */
            q = (unsigned char *)tok_two_chars;
            while (*q) {
                if (q[2] == v) {
                    *p++ = q[0];
                    *p++ = q[1];
                    *p = '\0';
                    return buf;
                }
                q += 3;
            }
        addv:
            *p++ = v;
            *p = '\0';
        } else if (v < tok_ident) {
            return table_ident[v - TOK_IDENT]->str;
        } else if (v >= SYM_FIRST_ANOM) {
            /* special name for anonymous symbol */
            sprintf(p, "L.%u", v - SYM_FIRST_ANOM);
        } else {
            /* should never happen */
            return NULL;
        }
        break;
    }
    return (char *)cstr_buf.data;
}

/* push, without hashing */
static Sym *sym_push2(Sym **ps, int v, int t, int c)
{
    Sym *s;
    s = (Sym *)tcc_malloc(sizeof(Sym));
    s->v = v;
    s->type.t = t;
    s->c = c;
    s->next = NULL;
    /* add in stack */
    s->prev = *ps;
    *ps = s;
    return s;
}

/* find a symbol and return its associated structure. 's' is the top
   of the symbol stack */
static Sym *sym_find2(Sym *s, int v)
{
    while (s) {
        if (s->v == v)
            return s;
        s = s->prev;
    }
    return NULL;
}

/* structure lookup */
static Sym *struct_find(int v)
{
    v -= TOK_IDENT;
    if ((unsigned)v >= (unsigned)(tok_ident - TOK_IDENT))
        return NULL;
    return table_ident[v]->sym_struct;
}

/* find an identifier */
static inline Sym *sym_find(int v)
{
    v -= TOK_IDENT;
    if ((unsigned)v >= (unsigned)(tok_ident - TOK_IDENT))
        return NULL;
    return table_ident[v]->sym_identifier;
}

/* push a given symbol on the symbol stack */
static Sym *sym_push(int v, CType *type, int r, int c)
{
    Sym *s, **ps;
    TokenSym *ts;

    if (local_stack)
        ps = &local_stack;
    else
        ps = &global_stack;
    s = sym_push2(ps, v, type->t, c);
    s->type.ref = type->ref;
    s->r = r;
    /* don't record fields or anonymous symbols */
    /* XXX: simplify */
    if (!(v & SYM_FIELD) && (v & ~SYM_STRUCT) < SYM_FIRST_ANOM) {
        /* record symbol in token array */
        ts = table_ident[(v & ~SYM_STRUCT) - TOK_IDENT];
        if (v & SYM_STRUCT)
            ps = &ts->sym_struct;
        else
            ps = &ts->sym_identifier;
        s->prev_tok = *ps;
        *ps = s;
    }
    return s;
}

/* push a global identifier */
static Sym *global_identifier_push(int v, int t, int c)
{
    Sym *s, **ps;
    s = sym_push2(&global_stack, v, t, c);
    /* don't record anonymous symbol */
    if (v < SYM_FIRST_ANOM) {
        ps = &table_ident[v - TOK_IDENT]->sym_identifier;
        /* modify the top most local identifier, so that
           sym_identifier will point to 's' when popped */
        while (*ps != NULL)
            ps = &(*ps)->prev_tok;
        s->prev_tok = NULL;
        *ps = s;
    }
    return s;
}

/* pop symbols until top reaches 'b' */
static void sym_pop(Sym **ptop, Sym *b)
{
    Sym *s, *ss, **ps;
    TokenSym *ts;
    int v;

    s = *ptop;
    while(s != b) {
        ss = s->prev;
        v = s->v;
        /* remove symbol in token array */
        /* XXX: simplify */
        if (!(v & SYM_FIELD) && (v & ~SYM_STRUCT) < SYM_FIRST_ANOM) {
            ts = table_ident[(v & ~SYM_STRUCT) - TOK_IDENT];
            if (v & SYM_STRUCT)
                ps = &ts->sym_struct;
            else
                ps = &ts->sym_identifier;
            *ps = s->prev_tok;
        }
        tcc_free(s);
        s = ss;
    }
    *ptop = b;
}

/* I/O layer */

BufferedFile *tcc_open(TCCState *s1, const char *filename)
{
    int fd;
    BufferedFile *bf;
	#ifndef WIN32
	fd = open(filename, O_RDONLY);
	//int err = errno;
	#else
	int err = _sopen_s(&fd,filename,_O_RDONLY,_SH_DENYWR,_S_IREAD);
	#endif

    if (fd < 0)
        return NULL;
    bf = (BufferedFile *)tcc_malloc(sizeof(BufferedFile));
    if (!bf) {
        _close(fd);
        return NULL;
    }
    bf->fd = fd;
    bf->buf_ptr = bf->buffer;
    bf->buf_end = bf->buffer;
    bf->buffer[0] = CH_EOB; /* put eob symbol */
    pstrcpy(bf->filename, sizeof(bf->filename), filename);
    bf->line_num = 1;
    bf->ifndef_macro = 0;
    bf->ifdef_stack_ptr = s1->ifdef_stack_ptr;
    //    printf("opening '%s'\n", filename);
    return bf;
}

void tcc_close(BufferedFile *bf)
{
    total_lines += bf->line_num;
    _close(bf->fd);
    tcc_free(bf);
}

/* fill input buffer and peek next char */
static int tcc_peekc_slow(BufferedFile *bf)
{
    int len;
    /* only tries to read if really end of buffer */
    if (bf->buf_ptr >= bf->buf_end) {
        if (bf->fd != -1) {
#if defined(PARSE_DEBUG)
            len = 8;
#else
            len = IO_BUF_SIZE;
#endif
            len = _read(bf->fd, bf->buffer, len);
            if (len < 0)
                len = 0;
        } else {
            len = 0;
        }
        total_bytes += len;
        bf->buf_ptr = bf->buffer;
        bf->buf_end = bf->buffer + len;
        *bf->buf_end = CH_EOB;
    }
    if (bf->buf_ptr < bf->buf_end) {
        return bf->buf_ptr[0];
    } else {
        bf->buf_ptr = bf->buf_end;
        return CH_EOF;
    }
}

/* return the current character, handling end of block if necessary
   (but not stray) */
static int handle_eob(void)
{
    return tcc_peekc_slow(file);
}

/* read next char from current input file and handle end of input buffer */
static inline void inp(void)
{
    ch = *(++(file->buf_ptr));
    /* end of buffer/file handling */
    if (ch == CH_EOB)
        ch = handle_eob();
}

/* handle '\[\r]\n' */
static void handle_stray(void)
{
    while (ch == '\\') {
        inp();
        if (ch == '\n') {
            file->line_num++;
            inp();
        } else if (ch == '\r') {
            inp();
            if (ch != '\n')
                goto fail;
            file->line_num++;
            inp();
        } else {
        fail:
            error("stray '\\' in program");
        }
    }
}

/* skip the stray and handle the \\n case. Output an error if
   incorrect char after the stray */
static int handle_stray1(uint8_t *p)
{
    int c;

    if (p >= file->buf_end) {
        file->buf_ptr = p;
        c = handle_eob();
        p = file->buf_ptr;
        if (c == '\\')
            goto parse_stray;
    } else {
    parse_stray:
        file->buf_ptr = p;
        ch = *p;
        handle_stray();
        p = file->buf_ptr;
        c = *p;
    }
    return c;
}

/* handle the complicated stray case */
#define PEEKC(c, p)\
{\
    p++;\
    c = *p;\
    if (c == '\\') {\
        c = handle_stray1(p);\
        p = file->buf_ptr;\
    }\
}

/* input with '\[\r]\n' handling. Note that this function cannot
   handle other characters after '\', so you cannot call it inside
   strings or comments */
static void minp(void)
{
    inp();
    if (ch == '\\') 
        handle_stray();
}


static void parse_line_comment(void)
{
    /* single line C++ comments */
    /* XXX: accept '\\\n' ? */
    inp();
    while (ch != '\n' && ch != CH_EOF)
        inp();
}

static void parse_comment(void)
{
    uint8_t *p;
    int c;
    
    /* C comments */
    p = file->buf_ptr;
    p++;
    for(;;) {
        /* fast skip loop */
        for(;;) {
            c = *p;
            if (c == '\n' || c == '*' || c == '\\')
                break;
            p++;
            c = *p;
            if (c == '\n' || c == '*' || c == '\\')
                break;
            p++;
        }
        /* now we can handle all the cases */
        if (c == '\n') {
            file->line_num++;
            p++;
        } else if (c == '*') {
            p++;
            for(;;) {
                c = *p;
                if (c == '*') {
                    p++;
                } else if (c == '/') {
                    goto end_of_comment;
                } else if (c == '\\') {
                    file->buf_ptr = p;
                    c = handle_eob();
                    if (c == '\\') {
                        /* skip '\\n', but if '\' followed but another
                           char, behave asif a stray was parsed */
                        ch = file->buf_ptr[0];
                        while (ch == '\\') {
                            inp();
                            if (ch == '\n') {
                                file->line_num++;
                                inp();
                            } else if (ch == '\r') {
                                inp();
                                if (ch == '\n') {
                                    file->line_num++;
                                    inp();
                                }
                            } else {
                                p = file->buf_ptr;
                                break;
                            }
                        }
                    }
                    p = file->buf_ptr;
                } else {
                    break;
                }
            }
        } else {
            /* stray, eob or eof */
            file->buf_ptr = p;
            c = handle_eob();
            p = file->buf_ptr;
            if (c == CH_EOF) {
                error("unexpected end of file in comment");
            } else if (c == '\\') {
                p++;
            }
        }
    }
 end_of_comment:
    p++;
    file->buf_ptr = p;
    ch = *p;
}

#define cinp minp

/* space exlcuding newline */
static inline int is_space(int ch)
{
    return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r';
}

static inline void skip_spaces(void)
{
    while (is_space(ch))
        cinp();
}

/* skip block of text until #else, #elif or #endif. skip also pairs of
   #if/#endif */
void preprocess_skip(void)
{
    int a, start_of_line, sep, c;
    uint8_t *p;

    p = file->buf_ptr;
    start_of_line = 1;
    a = 0;
    for(;;) {
    redo_no_start:
        c = *p;
        switch(c) {
        case ' ':
        case '\t':
        case '\f':
        case '\v':
        case '\r':
            p++;
            goto redo_no_start;
        case '\n':
            start_of_line = 1;
            file->line_num++;
            p++;
            goto redo_no_start;
        case '\\':
            file->buf_ptr = p;
            c = handle_eob();
            if (c == CH_EOF) {
                expect("#endif");
            } else if (c == '\\') {
                /* XXX: incorrect: should not give an error */
                ch = file->buf_ptr[0];
                handle_stray();
            }
            p = file->buf_ptr;
            goto redo_no_start;
            /* skip strings */
        case '\"':
        case '\'':
            sep = c;
            p++;
            for(;;) {
                c = *p;
                if (c == sep) {
                    break;
                } else if (c == '\\') {
                    file->buf_ptr = p;
                    c = handle_eob();
                    p = file->buf_ptr;
                    if (c == CH_EOF) {
                        /* XXX: better error message */
                        error("unterminated string");
                    } else if (c == '\\') {
                        /* ignore next char */
                        p++;
                        c = *p;
                        if (c == '\\') {
                            file->buf_ptr = p;
                            c = handle_eob();
                            p = file->buf_ptr;
                        }
                        if (c == '\n')
                            file->line_num++;
                        else if (c != CH_EOF)
                            p++;
                    }
                } else if (c == '\n') {
                    file->line_num++;
                    p++;
                } else {
                    p++;
                }
            }
            p++;
            break;
            /* skip comments */
        case '/':
            file->buf_ptr = p;
            ch = *p;
            minp();
            if (ch == '*') {
                parse_comment();
            } else if (ch == '/') {
                parse_line_comment();
            }
            p = file->buf_ptr;
            break;

        case '#':
            p++;
            if (start_of_line) {
                file->buf_ptr = p;
                next_nomacro();
                p = file->buf_ptr;
                if (a == 0 && 
                    (tok == TOK_ELSE || tok == TOK_ELIF || tok == TOK_ENDIF))
                    goto the_end;
                if (tok == TOK_IF || tok == TOK_IFDEF || tok == TOK_IFNDEF)
                    a++;
                else if (tok == TOK_ENDIF)
                    a--;
            }
            break;
        default:
            p++;
            break;
        }
        start_of_line = 0;
    }
 the_end: ;
    file->buf_ptr = p;
}

/* ParseState handling */

/* XXX: currently, no include file info is stored. Thus, we cannot display
   accurate messages if the function or data definition spans multiple
   files */

/* save current parse state in 's' */
void save_parse_state(ParseState *s)
{
    s->line_num = file->line_num;
    s->macro_ptr = macro_ptr;
    s->tok = tok;
    s->tokc = tokc;
}

/* restore parse state from 's' */
void restore_parse_state(ParseState *s)
{
    file->line_num = s->line_num;
    macro_ptr = s->macro_ptr;
    tok = s->tok;
    tokc = s->tokc;
}

/* return the number of additionnal 'ints' necessary to store the
   token */
static inline int tok_ext_size(int t)
{
    switch(t) {
        /* 4 bytes */
    case TOK_CINT:
    case TOK_CUINT:
    case TOK_CCHAR:
    case TOK_LCHAR:
    case TOK_STR:
    case TOK_LSTR:
    case TOK_CFLOAT:
    case TOK_LINENUM:
    case TOK_PPNUM:
        return 1;
    case TOK_CDOUBLE:
    case TOK_CLLONG:
    case TOK_CULLONG:
        return 2;
    case TOK_CLDOUBLE:
        return LDOUBLE_SIZE / 4;
    default:
        return 0;
    }
}

/* token string handling */

static inline void tok_str_new(TokenString *s)
{
    s->str = NULL;
    s->len = 0;
    s->allocated_len = 0;
    s->last_line_num = -1;
}

static void tok_str_free(int *str)
{
    const int *p;
    CTString *cstr;
    int t;

    p = str;
    for(;;) {
        t = *p;
        /* NOTE: we test zero separately so that GCC can generate a
           table for the following switch */
        if (t == 0)
            break;
        switch(t) {
        case TOK_CINT:
        case TOK_CUINT:
        case TOK_CCHAR:
        case TOK_LCHAR:
        case TOK_CFLOAT:
        case TOK_LINENUM:
            p += 2;
            break;
        case TOK_PPNUM:
        case TOK_STR:
        case TOK_LSTR:
            /* XXX: use a macro to be portable on 64 bit ? */
            cstr = (CTString *)INT2PTR(p[1]);
            cstr_free(cstr);
            tcc_free(cstr);
            p += 2;
            break;
        case TOK_CDOUBLE:
        case TOK_CLLONG:
        case TOK_CULLONG:
            p += 3;
            break;
        case TOK_CLDOUBLE:
            p += 1 + (LDOUBLE_SIZE / 4);
            break;
        default:
            p++;
            break;
        }
    }
    tcc_free(str);
}

static int *tok_str_realloc(TokenString *s)
{
    int *str, len;

    len = s->allocated_len + TOK_STR_ALLOC_INCR;
    str = (int *)tcc_realloc(s->str, len * sizeof(int));
    if (!str)
        error("memory full");
    s->allocated_len = len;
    s->str = str;
    return str;
}

static void tok_str_add(TokenString *s, int t)
{
    int len, *str;

    len = s->len;
    str = s->str;
    if (len >= s->allocated_len)
        str = tok_str_realloc(s);
    str[len++] = t;
    s->len = len;
}

static void tok_str_add2(TokenString *s, int t, CValue *cv)
{
    int len, *str;

    len = s->len;
    str = s->str;

    /* allocate space for worst case */
    if (len + TOK_MAX_SIZE > s->allocated_len)
        str = tok_str_realloc(s);
    str[len++] = t;
    switch(t) {
    case TOK_CINT:
    case TOK_CUINT:
    case TOK_CCHAR:
    case TOK_LCHAR:
    case TOK_CFLOAT:
    case TOK_LINENUM:
        str[len++] = cv->tab[0];
        break;
    case TOK_PPNUM:
    case TOK_STR:
    case TOK_LSTR:
        str[len++] = PTR2INT(cstr_dup(cv->cstr));
        break;
    case TOK_CDOUBLE:
    case TOK_CLLONG:
    case TOK_CULLONG:
        str[len++] = cv->tab[0];
        str[len++] = cv->tab[1];
        break;
    case TOK_CLDOUBLE:
#if LDOUBLE_SIZE == 8
        str[len++] = cv->tab[0];
        str[len++] = cv->tab[1];
#else
#error add long double size support
#endif
        break;
    default:
        break;
    }
    s->len = len;
}

/* add the current parse token in token string 's' */
static void tok_str_add_tok(TokenString *s)
{
    CValue cval;

    /* save line number info */
    if (file->line_num != s->last_line_num) {
        s->last_line_num = file->line_num;
        cval.i = s->last_line_num;
        tok_str_add2(s, TOK_LINENUM, &cval);
    }
    tok_str_add2(s, tok, &tokc);
}

#if LDOUBLE_SIZE == 8
#define LDOUBLE_GET(p, cv)                      \
        cv.tab[0] = p[0];                       \
        cv.tab[1] = p[1];                       
#else
#error add long double size support
#endif


/* get a token from an integer array and increment pointer
   accordingly. we code it as a macro to avoid pointer aliasing. */
#define TOK_GET(t, p, cv)                       \
{                                               \
    t = *p++;                                   \
    switch(t) {                                 \
    case TOK_CINT:                              \
    case TOK_CUINT:                             \
    case TOK_CCHAR:                             \
    case TOK_LCHAR:                             \
    case TOK_CFLOAT:                            \
    case TOK_LINENUM:                           \
        cv.tab[0] = *p++;                       \
        break;                                  \
    case TOK_STR:                               \
    case TOK_LSTR:                              \
    case TOK_PPNUM:                             \
        cv.cstr = (CTString *)INT2PTR(*p++);    \
        break;                                  \
    case TOK_CDOUBLE:                           \
    case TOK_CLLONG:                            \
    case TOK_CULLONG:                           \
        cv.tab[0] = p[0];                       \
        cv.tab[1] = p[1];                       \
        p += 2;                                 \
        break;                                  \
    case TOK_CLDOUBLE:                          \
        LDOUBLE_GET(p, cv);                     \
        p += LDOUBLE_SIZE / 4;                  \
        break;                                  \
    default:                                    \
        break;                                  \
    }                                           \
}

/* defines handling */
static inline void define_push(int v, int macro_type, int *str, Sym *first_arg)
{
    Sym *s;

    s = sym_push2(&define_stack, v, macro_type, PTR2INT(str));
    s->next = first_arg;
    table_ident[v - TOK_IDENT]->sym_define = s;
}

/* undefined a define symbol. Its name is just set to zero */
static void define_undef(Sym *s)
{
    int v;
    v = s->v;
    if (v >= TOK_IDENT && v < tok_ident)
        table_ident[v - TOK_IDENT]->sym_define = NULL;
    s->v = 0;
}

static inline Sym *define_find(int v)
{
    v -= TOK_IDENT;
    if ((unsigned)v >= (unsigned)(tok_ident - TOK_IDENT))
        return NULL;
    return table_ident[v]->sym_define;
}

/* free define stack until top reaches 'b' */
static void free_defines(Sym *b)
{
    Sym *top, *top1;
    int v;

    top = define_stack;
    while (top != b) {
        top1 = top->prev;
        /* do not free args or predefined defines */
        if (top->c)
            tok_str_free((int *)INT2PTR(top->c));
        v = top->v;
        if (v >= TOK_IDENT && v < tok_ident)
            table_ident[v - TOK_IDENT]->sym_define = NULL;
        tcc_free(top);
        top = top1;
    }
    define_stack = b;
}

/* label lookup */
static Sym *label_find(int v)
{
    v -= TOK_IDENT;
    if ((unsigned)v >= (unsigned)(tok_ident - TOK_IDENT))
        return NULL;
    return table_ident[v]->sym_label;
}

static Sym *label_push(Sym **ptop, int v, int flags)
{
    Sym *s, **ps;
    s = sym_push2(ptop, v, 0, 0);
    s->r = flags;
    ps = &table_ident[v - TOK_IDENT]->sym_label;
    if (ptop == &global_label_stack) {
        /* modify the top most local identifier, so that
           sym_identifier will point to 's' when popped */
        while (*ps != NULL)
            ps = &(*ps)->prev_tok;
    }
    s->prev_tok = *ps;
    *ps = s;
    return s;
}

/* pop labels until element last is reached. Look if any labels are
   undefined. Define symbols if '&&label' was used. */
static void label_pop(Sym **ptop, Sym *slast)
{
    Sym *s, *s1;
    for(s = *ptop; s != slast; s = s1) {
        s1 = s->prev;
        if (s->r == LABEL_DECLARED) {
            warning("label '%s' declared but not used", get_tok_str(s->v, NULL));
        } else if (s->r == LABEL_FORWARD) {
                error("label '%s' used but not defined",
                      get_tok_str(s->v, NULL));
        } else {
            if (s->c) {
                /* define corresponding symbol. A size of
                   1 is put. */
                put_extern_sym(s, cur_text_section, (long)s->next, 1);
            }
        }
        /* remove label */
        table_ident[s->v - TOK_IDENT]->sym_label = s->prev_tok;
        tcc_free(s);
    }
    *ptop = slast;
}

/* eval an expression for #if/#elif */
static int expr_preprocess(void)
{
    int c, t;
    TokenString str;
    
    tok_str_new(&str);
    while (tok != TOK_LINEFEED && tok != TOK_EOF) {
        next(); /* do macro subst */
        if (tok == TOK_DEFINED) {
            next_nomacro();
            t = tok;
            if (t == '(') 
                next_nomacro();
            c = define_find(tok) != 0;
            if (t == '(')
                next_nomacro();
            tok = TOK_CINT;
            tokc.i = c;
        } else if (tok >= TOK_IDENT) {
            /* if undefined macro */
            tok = TOK_CINT;
            tokc.i = 0;
        }
        tok_str_add_tok(&str);
    }
    tok_str_add(&str, -1); /* simulate end of file */
    tok_str_add(&str, 0);
    /* now evaluate C constant expression */
    macro_ptr = str.str;
    next();
    c = expr_const();
    macro_ptr = NULL;
    tok_str_free(str.str);
    return c != 0;
}

#if defined(PARSE_DEBUG) || defined(PP_DEBUG)
static void tok_print(int *str)
{
    int t;
    CValue cval;

    while (1) {
        TOK_GET(t, str, cval);
        if (!t)
            break;
        printf(" %s", get_tok_str(t, &cval));
    }
    printf("\n");
}
#endif

/* parse after #define */
static void parse_define(void)
{
    Sym *s, *first, **ps;
    int v, t, varg, is_vaargs, c;
    TokenString str;
    
    v = tok;
    if (v < TOK_IDENT)
        error("invalid macro name '%s'", get_tok_str(tok, &tokc));
    /* XXX: should check if same macro (ANSI) */
    first = NULL;
    t = MACRO_OBJ;
    /* '(' must be just after macro definition for MACRO_FUNC */
    c = file->buf_ptr[0];
    if (c == '\\')
        c = handle_stray1(file->buf_ptr);
    if (c == '(') {
        next_nomacro();
        next_nomacro();
        ps = &first;
        while (tok != ')') {
            varg = tok;
            next_nomacro();
            is_vaargs = 0;
            if (varg == TOK_DOTS) {
                varg = TOK___VA_ARGS__;
                is_vaargs = 1;
            } else if (tok == TOK_DOTS && gnu_ext) {
                is_vaargs = 1;
                next_nomacro();
            }
            if (varg < TOK_IDENT)
                error("badly punctuated parameter list");
            s = sym_push2(&define_stack, varg | SYM_FIELD, is_vaargs, 0);
            *ps = s;
            ps = &s->next;
            if (tok != ',')
                break;
            next_nomacro();
        }
        t = MACRO_FUNC;
    }
    tok_str_new(&str);
    next_nomacro();
    /* EOF testing necessary for '-D' handling */
    while (tok != TOK_LINEFEED && tok != TOK_EOF) {
        tok_str_add2(&str, tok, &tokc);
        next_nomacro();
    }
    tok_str_add(&str, 0);
#ifdef PP_DEBUG
    printf("define %s %d: ", get_tok_str(v, NULL), t);
    tok_print(str.str);
#endif
    define_push(v, t, str.str, first);
}

/* XXX: use a token or a hash table to accelerate matching ? */
static CachedInclude *search_cached_include(TCCState *s1,
                                            int type, const char *filename)
{
    CachedInclude *e;
    int i;

    for(i = 0;i < s1->nb_cached_includes; i++) {
        e = s1->cached_includes[i];
        if (e->type == type && !strcmp(e->filename, filename))
            return e;
    }
    return NULL;
}

static inline void add_cached_include(TCCState *s1, int type, 
                                      const char *filename, int ifndef_macro)
{
    CachedInclude *e;

    if (search_cached_include(s1, type, filename))
        return;
#ifdef INC_DEBUG
    printf("adding cached '%s' %s\n", filename, get_tok_str(ifndef_macro, NULL));
#endif
    e = (CachedInclude *)tcc_malloc(sizeof(CachedInclude) + strlen(filename));
    if (!e)
        return;
    e->type = type;
    strcpy(e->filename, filename);
    e->ifndef_macro = ifndef_macro;
    dynarray_add((void ***)&s1->cached_includes, &s1->nb_cached_includes, e);
}

/* is_bof is true if first non space token at beginning of file */
static void preprocess(int is_bof)
{
    TCCState *s1 = tcc_state;
    int size, i, c, n, saved_parse_flags;
    char buf[1024], *q, *p;
    char buf1[1024];
    BufferedFile *f;
    Sym *s;
    CachedInclude *e;
    
    saved_parse_flags = parse_flags;
    parse_flags = PARSE_FLAG_PREPROCESS | PARSE_FLAG_TOK_NUM | 
        PARSE_FLAG_LINEFEED;
    next_nomacro();
 redo:
    switch(tok) {
    case TOK_DEFINE:
        next_nomacro();
        parse_define();
        break;
    case TOK_UNDEF:
        next_nomacro();
        s = define_find(tok);
        /* undefine symbol by putting an invalid name */
        if (s)
            define_undef(s);
        break;
    case TOK_INCLUDE:
        ch = file->buf_ptr[0];
        /* XXX: incorrect if comments : use next_nomacro with a special mode */
        skip_spaces();
        if (ch == '<') {
            c = '>';
            goto read_name;
        } else if (ch == '\"') {
            c = ch;
        read_name:
            /* XXX: better stray handling */

#if (DO_C67)
            inp();
            q = buf;
            while (ch != c && ch != '\n' && ch != CH_EOF) {
                if ((q - buf) < sizeof(buf) - 1)
                    *q++ = ch;
                inp();
            }
            *q = '\0';
#else
            minp();
            q = buf;
            while (ch != c && ch != '\n' && ch != CH_EOF) {
                if ((q - buf) < sizeof(buf) - 1)
                    *q++ = ch;
                minp();
            }
            *q = '\0';
#endif
            minp();
#if 0
            /* eat all spaces and comments after include */
            /* XXX: slightly incorrect */
            while (ch1 != '\n' && ch1 != CH_EOF)
                inp();
#endif
        } else {
            /* computed #include : either we have only strings or
               we have anything enclosed in '<>' */
            next();
            buf[0] = '\0';
            if (tok == TOK_STR) {
                while (tok != TOK_LINEFEED) {
                    if (tok != TOK_STR) {
                    include_syntax:
                        error("'#include' expects \"FILENAME\" or <FILENAME>");
                    }
                    pstrcat(buf, sizeof(buf), (char *)tokc.cstr->data);
                    next();
                }
                c = '\"';
            } else {
                int len;
                while (tok != TOK_LINEFEED) {
                    pstrcat(buf, sizeof(buf), get_tok_str(tok, &tokc));
                    next();
                }
                len = strlen(buf);
                /* check syntax and remove '<>' */
                if (len < 2 || buf[0] != '<' || buf[len - 1] != '>')
                    goto include_syntax;
                memmove(buf, buf + 1, len - 2);
                buf[len - 2] = '\0';
                c = '>';
            }
        }

        e = search_cached_include(s1, c, buf);
        if (e && define_find(e->ifndef_macro)) {
            /* no need to parse the include because the 'ifndef macro'
               is defined */
#ifdef INC_DEBUG
            printf("%s: skipping %s\n", file->filename, buf);
#endif
        } else {
            if (c == '\"') {
                /* first search in current dir if "header.h" */
                size = 0;
                p = strrchr(file->filename, '/');
                if (p) 
                    size = p + 1 - file->filename;
                if ((size_t)size > sizeof(buf1) - 1)
                    size = sizeof(buf1) - 1;
                memcpy(buf1, file->filename, size);
                buf1[size] = '\0';
                pstrcat(buf1, sizeof(buf1), buf);
                f = tcc_open(s1, buf1);
                if (f)
                    goto found;
            }
            if (s1->include_stack_ptr >= s1->include_stack + INCLUDE_STACK_SIZE)
                error("#include recursion too deep");
            /* now search in all the include paths */
            n = s1->nb_include_paths + s1->nb_sysinclude_paths;
            for(i = 0; i < n; i++) {
                const char *path;
                if (i < s1->nb_include_paths)
                    path = s1->include_paths[i];
                else
                    path = s1->sysinclude_paths[i - s1->nb_include_paths];
                pstrcpy(buf1, sizeof(buf1), path);
                pstrcat(buf1, sizeof(buf1), "/");
                pstrcat(buf1, sizeof(buf1), buf);
                f = tcc_open(s1, buf1);
                if (f)
                    goto found;
            }
            error("include file '%s' not found", buf);
            f = NULL;
        found:
#ifdef INC_DEBUG
            printf("%s: including %s\n", file->filename, buf1);
#endif
            f->inc_type = c;
            pstrcpy(f->inc_filename, sizeof(f->inc_filename), buf);
            /* push current file in stack */
            /* XXX: fix current line init */
            *s1->include_stack_ptr++ = file;
            file = f;
            /* add include file debug info */
            if (do_debug) {
#if (DO_C67)  // debug works better with no path
				{
				char *s1,*s2;

				s1=strrchr((char *)file->filename, '\\');
				s2=strrchr((char *)file->filename, '/');

				if (s2>s1) s1=s2;

				if (s1==NULL)
					s1=file->filename;
				else
					s1++;

				put_stabs(s1, N_BINCL, 0, 0, 0);

				/* an elf symbol of type STT_FILE must be put so that STB_LOCAL
				   symbols can be safely used */
				put_elf_sym(symtab_section, 0, 0, 
							ELF32_ST_INFO(STB_LOCAL, STT_FILE), 0, 
							SHN_ABS, s1);
				}
#else
				put_stabs(file->filename, N_BINCL, 0, 0, 0);
#endif
			}
            tok_flags |= TOK_FLAG_BOF | TOK_FLAG_BOL;
            ch = file->buf_ptr[0];
            goto the_end;
        }
        break;
    case TOK_IFNDEF:
        c = 1;
        goto do_ifdef;
    case TOK_IF:
        c = expr_preprocess();
        goto do_if;
    case TOK_IFDEF:
        c = 0;
    do_ifdef:
        next_nomacro();
        if (tok < TOK_IDENT)
            error("invalid argument for '#if%sdef'", c ? "n" : "");
        if (is_bof) {
            if (c) {
#ifdef INC_DEBUG
                printf("#ifndef %s\n", get_tok_str(tok, NULL));
#endif
                file->ifndef_macro = tok;
            }
        }
        c = (define_find(tok) != 0) ^ c;
    do_if:
        if (s1->ifdef_stack_ptr >= s1->ifdef_stack + IFDEF_STACK_SIZE)
            error("memory full");
        *s1->ifdef_stack_ptr++ = c;
        goto test_skip;
    case TOK_ELSE:
        if (s1->ifdef_stack_ptr == s1->ifdef_stack)
            error("#else without matching #if");
        if (s1->ifdef_stack_ptr[-1] & 2)
            error("#else after #else");
        c = (s1->ifdef_stack_ptr[-1] ^= 3);
        goto test_skip;
    case TOK_ELIF:
        if (s1->ifdef_stack_ptr == s1->ifdef_stack)
            error("#elif without matching #if");
        c = s1->ifdef_stack_ptr[-1];
        if (c > 1)
            error("#elif after #else");
        /* last #if/#elif expression was true: we skip */
        if (c == 1)
            goto skip;
        c = expr_preprocess();
        s1->ifdef_stack_ptr[-1] = c;
    test_skip:
        if (!(c & 1)) {
        skip:
            preprocess_skip();
            is_bof = 0;
            goto redo;
        }
        break;
    case TOK_ENDIF:
        if (s1->ifdef_stack_ptr <= file->ifdef_stack_ptr)
            error("#endif without matching #if");
        s1->ifdef_stack_ptr--;
        /* '#ifndef macro' was at the start of file. Now we check if
           an '#endif' is exactly at the end of file */
        if (file->ifndef_macro &&
            s1->ifdef_stack_ptr == file->ifdef_stack_ptr) {
            file->ifndef_macro_saved = file->ifndef_macro;
            /* need to set to zero to avoid false matches if another
               #ifndef at middle of file */
            file->ifndef_macro = 0;
            while (tok != TOK_LINEFEED)
                next_nomacro();
            tok_flags |= TOK_FLAG_ENDIF;
            goto the_end;
        }
        break;
    case TOK_LINE:
        next();
        if (tok != TOK_CINT)
            error("#line");
        file->line_num = tokc.i - 1; /* the line number will be incremented after */
        next();
        if (tok != TOK_LINEFEED) {
            if (tok != TOK_STR)
                error("#line");
            pstrcpy(file->filename, sizeof(file->filename), 
                    (char *)tokc.cstr->data);
        }
        break;
    case TOK_ERROR:
    case TOK_WARNING:
        c = tok;
        ch = file->buf_ptr[0];
        skip_spaces();
        q = buf;
        while (ch != '\n' && ch != CH_EOF) {
            if ((q - buf) < sizeof(buf) - 1)
                *q++ = ch;
            minp();
        }
        *q = '\0';
        if (c == TOK_ERROR)
            error("#error %s", buf);
        else
            warning("#warning %s", buf);
        break;
    case TOK_PRAGMA:
        /* ignored */
        break;
    default:
        if (tok == TOK_LINEFEED || tok == '!' || tok == TOK_CINT) {
            /* '!' is ignored to allow C scripts. numbers are ignored
               to emulate cpp behaviour */
        } else {
            error("invalid preprocessing directive #%s", get_tok_str(tok, &tokc));
        }
        break;
    }
    /* ignore other preprocess commands or #! for C scripts */
    while (tok != TOK_LINEFEED)
        next_nomacro();
 the_end:
    parse_flags = saved_parse_flags;
}

/* read a number in base b */
static int getn(int b)
{
    int n, t;
    n = 0;
    while (1) {
        if (ch >= 'a' && ch <= 'f')
            t = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            t = ch - 'A' + 10;
        else if (isnum(ch))
            t = ch - '0';
        else
            break;
        if (t < 0 || t >= b)
            break;
        n = n * b + t;
        inp();
    }
    return n;
}

/* read a character for string or char constant and eval escape codes */
static int getq(void)
{
    int c;

 redo:
    c = ch;
    inp();
    if (c == '\\') {
        switch(ch) {
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
            /* at most three octal digits */
            c = ch - '0';
            inp();
            if (isoct(ch)) {
                c = c * 8 + ch - '0';
                inp();
                if (isoct(ch)) {
                    c = c * 8 + ch - '0';
                    inp();
                }
            }
            return c;
        case 'x':
            inp();
            return getn(16);
        case 'a':
            c = '\a';
            break;
        case 'b':
            c = '\b';
            break;
        case 'f':
            c = '\f';
            break;
        case 'n':
            c = '\n';
            break;
        case 'r':
            c = '\r';
            break;
        case 't':
            c = '\t';
            break;
        case 'v':
            c = '\v';
            break;
        case 'e':
            if (!gnu_ext)
                goto invalid_escape;
            c = 27;
            break;
        case '\'':
        case '\"':
        case '\\': 
        case '?':
            c = ch;
            break;
        case '\n':
            inp();
            goto redo;
        case '\r':
            inp();
            if (ch != '\n')
                goto invalid_escape;
            inp();
            goto redo;
        default:
        invalid_escape:
            error("invalid escaped char");
        }
        inp();
    } else if (c == '\r' && ch == '\n') {
        inp();
        c = '\n';
    }
    return c;
}

/* we use 64 bit numbers */
#define BN_SIZE 2

/* bn = (bn << shift) | or_val */
void bn_lshift(unsigned int *bn, int shift, int or_val)
{
    int i;
    unsigned int v;
    for(i=0;i<BN_SIZE;i++) {
        v = bn[i];
        bn[i] = (v << shift) | or_val;
        or_val = v >> (32 - shift);
    }
}

void bn_zero(unsigned int *bn)
{
    int i;
    for(i=0;i<BN_SIZE;i++) {
        bn[i] = 0;
    }
}

/* parse number in null terminated string 'p' and return it in the
   current token */
void parse_number(const char *p)
{
    int b, t, shift, frac_bits, s, exp_val, ch;
    char *q;
    unsigned int bn[BN_SIZE];
    double d;

    /* number */
    q = token_buf;
    ch = *p++;
    t = ch;
    ch = *p++;
    *q++ = t;
    b = 10;
    if (t == '.') {
        goto float_frac_parse;
    } else if (t == '0') {
        if (ch == 'x' || ch == 'X') {
            q--;
            ch = *p++;
            b = 16;
        } else if (tcc_ext && (ch == 'b' || ch == 'B')) {
            q--;
            ch = *p++;
            b = 2;
        }
    }
    /* parse all digits. cannot check octal numbers at this stage
       because of floating point constants */
    while (1) {
        if (ch >= 'a' && ch <= 'f')
            t = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            t = ch - 'A' + 10;
        else if (isnum(ch))
            t = ch - '0';
        else if (ch == 0 || ch == '.' ||
        ((ch == 'e' || ch == 'E') && b == 10) ||
        ((ch == 'p' || ch == 'P') && (b == 16 || b == 2)) ||
		(ch == 'U' || ch == 'u') || 
		(ch == 'L' || ch == 'l') || 
		(ch == 'F' || ch == 'f'))
            break;
        else
			error("unexpected character in constant");

        if (t >= b)
            break;
        if (q >= token_buf + STRING_MAX_SIZE) {
        num_too_long:
            error("number too long");
        }
        *q++ = ch;
        ch = *p++;
    }
    if (ch == '.' ||
        ((ch == 'e' || ch == 'E') && b == 10) ||
        ((ch == 'p' || ch == 'P') && (b == 16 || b == 2))) {
        if (b != 10) {
            /* NOTE: strtox should support that for hexa numbers, but
               non ISOC99 libcs do not support it, so we prefer to do
               it by hand */
            /* hexadecimal or binary floats */
            /* XXX: handle overflows */
            *q = '\0';
            if (b == 16)
                shift = 4;
            else 
                shift = 2;
            bn_zero(bn);
            q = token_buf;
            while (1) {
                t = *q++;
                if (t == '\0') {
                    break;
                } else if (t >= 'a') {
                    t = t - 'a' + 10;
                } else if (t >= 'A') {
                    t = t - 'A' + 10;
                } else {
                    t = t - '0';
                }
                bn_lshift(bn, shift, t);
            }
            frac_bits = 0;
            if (ch == '.') {
                ch = *p++;
                while (1) {
                    t = ch;
                    if (t >= 'a' && t <= 'f') {
                        t = t - 'a' + 10;
                    } else if (t >= 'A' && t <= 'F') {
                        t = t - 'A' + 10;
                    } else if (t >= '0' && t <= '9') {
                        t = t - '0';
                    } else {
                        break;
                    }
                    if (t >= b)
                        error("invalid digit");
                    bn_lshift(bn, shift, t);
                    frac_bits += shift;
                    ch = *p++;
                }
            }
            if (ch != 'p' && ch != 'P')
                expect("exponent");
            ch = *p++;
            s = 1;
            exp_val = 0;
            if (ch == '+') {
                ch = *p++;
            } else if (ch == '-') {
                s = -1;
                ch = *p++;
            }
            if (ch < '0' || ch > '9')
                expect("exponent digits");
            while (ch >= '0' && ch <= '9') {
                exp_val = exp_val * 10 + ch - '0';
                ch = *p++;
            }
            exp_val = exp_val * s;
            
            /* now we can generate the number */
            /* XXX: should patch directly float number */
            d = (double)bn[1] * 4294967296.0 + (double)bn[0];
            d = ldexp(d, exp_val - frac_bits);
            t = toup(ch);
            if (t == 'F') {
                ch = *p++;
                tok = TOK_CFLOAT;
                /* float : should handle overflow */
                tokc.f = (float)d;
            } else if (t == 'L') {
                ch = *p++;
                tok = TOK_CLDOUBLE;
                /* XXX: not large enough */
                tokc.ld = (long double)d;
            } else {
                tok = TOK_CDOUBLE;
                tokc.d = d;
            }
        } else {
            /* decimal floats */
            if (ch == '.') {
                if (q >= token_buf + STRING_MAX_SIZE)
                    goto num_too_long;
                *q++ = ch;
                ch = *p++;
            float_frac_parse:
                while (ch >= '0' && ch <= '9') {
                    if (q >= token_buf + STRING_MAX_SIZE)
                        goto num_too_long;
                    *q++ = ch;
                    ch = *p++;
                }
            }
            if (ch == 'e' || ch == 'E') {
                if (q >= token_buf + STRING_MAX_SIZE)
                    goto num_too_long;
                *q++ = ch;
                ch = *p++;
                if (ch == '-' || ch == '+') {
                    if (q >= token_buf + STRING_MAX_SIZE)
                        goto num_too_long;
                    *q++ = ch;
                    ch = *p++;
                }
                if (ch < '0' || ch > '9')
                    expect("exponent digits");
                while (ch >= '0' && ch <= '9') {
                    if (q >= token_buf + STRING_MAX_SIZE)
                        goto num_too_long;
                    *q++ = ch;
                    ch = *p++;
                }
            }
            *q = '\0';
            t = toup(ch);
            errno = 0;
            if (t == 'F') {
                ch = *p++;
                tok = TOK_CFLOAT;
                tokc.f = strtof(token_buf, NULL);
            } else if (t == 'L') {
                ch = *p++;
                tok = TOK_CLDOUBLE;
                tokc.ld = strtold(token_buf, NULL);
            } else {
                tok = TOK_CDOUBLE;
                tokc.d = strtod(token_buf, NULL);
            }
        }
    } else {
        __uint64 n, n1;
        int lcount, ucount;

        /* integer number */
        *q = '\0';
        q = token_buf;
        if (b == 10 && *q == '0') {
            b = 8;
            q++;
        }
        n = 0;
        while(1) {
            t = *q++;
            /* no need for checks except for base 10 / 8 errors */
            if (t == '\0') {
                break;
            } else if (t >= 'a') {
                t = t - 'a' + 10;
            } else if (t >= 'A') {
                t = t - 'A' + 10;
            } else {
                t = t - '0';
                if (t >= b)
                    error("invalid digit");
            }
            n1 = n;
            n = n * b + t;
            /* detect overflow */
            /* XXX: this test is not reliable */
            if (n < n1)
                error("integer constant overflow");
        }
        
        /* XXX: not exactly ANSI compliant */
        if ((n & 0xffffffff00000000LL) != 0) {
            if ((n >> 63) != 0)
                tok = TOK_CULLONG;
            else
                tok = TOK_CLLONG;
        } else if (n > 0x7fffffff) {
            tok = TOK_CUINT;
        } else {
            tok = TOK_CINT;
        }
        lcount = 0;
        ucount = 0;
        for(;;) {
            t = toup(ch);
            if (t == 'L') {
                if (lcount >= 2)
                    error("three 'l's in integer constant");
                lcount++;
                if (lcount == 2) {
                    if (tok == TOK_CINT)
                        tok = TOK_CLLONG;
                    else if (tok == TOK_CUINT)
                        tok = TOK_CULLONG;
                }
                ch = *p++;
            } else if (t == 'U') {
                if (ucount >= 1)
                    error("two 'u's in integer constant");
                ucount++;
                if (tok == TOK_CINT)
                    tok = TOK_CUINT;
                else if (tok == TOK_CLLONG)
                    tok = TOK_CULLONG;
                ch = *p++;
            } else {
                break;
            }
        }
        if (tok == TOK_CINT || tok == TOK_CUINT)
            tokc.ui = (int)n;
        else
            tokc.ull = n;
    }
}


#define PARSE2(c1, tok1, c2, tok2)              \
    case c1:                                    \
        PEEKC(c, p);                            \
        if (c == c2) {                          \
            p++;                                \
            tok = tok2;                         \
        } else {                                \
            tok = tok1;                         \
        }                                       \
        break;

/* return next token without macro substitution */
static inline void next_nomacro1(void)
{
    int b, t, c;
    TokenSym *ts;
    uint8_t *p, *p1;
    unsigned int h;

    p = file->buf_ptr;
 redo_no_start:
    c = *p;
    switch(c) {
    case ' ':
    case '\t':
    case '\f':
    case '\v':
    case '\r':
        p++;
        goto redo_no_start;
        
    case '\\':
        /* first look if it is in fact an end of buffer */
        if (p >= file->buf_end) {
            file->buf_ptr = p;
            handle_eob();
            p = file->buf_ptr;
            if (p >= file->buf_end)
                goto parse_eof;
            else
                goto redo_no_start;
        } else {
            file->buf_ptr = p;
            ch = *p;
            handle_stray();
            p = file->buf_ptr;
            goto redo_no_start;
        }
    parse_eof:
        {
            TCCState *s1 = tcc_state;
            if (parse_flags & PARSE_FLAG_LINEFEED) {
                tok = TOK_LINEFEED;
            } else if (s1->include_stack_ptr == s1->include_stack ||
                       !(parse_flags & PARSE_FLAG_PREPROCESS)) {
                /* no include left : end of file. */
                tok = TOK_EOF;
            } else {
                /* pop include file */
                
                /* test if previous '#endif' was after a #ifdef at
                   start of file */
                if (tok_flags & TOK_FLAG_ENDIF) {
#ifdef INC_DEBUG
                    printf("#endif %s\n", get_tok_str(file->ifndef_macro_saved, NULL));
#endif
                    add_cached_include(s1, file->inc_type, file->inc_filename,
                                       file->ifndef_macro_saved);
                }

                /* add end of include file debug info */
                if (do_debug) {
                    put_stabd(N_EINCL, 0, 0);
                }
                /* pop include stack */
                tcc_close(file);
                s1->include_stack_ptr--;
                file = *s1->include_stack_ptr;
                p = file->buf_ptr;
                goto redo_no_start;
            }
        }
        break;

    case '\n':
        if (parse_flags & PARSE_FLAG_LINEFEED) {
            tok = TOK_LINEFEED;
        } else {
            file->line_num++;
            tok_flags |= TOK_FLAG_BOL;
            p++;
            goto redo_no_start;
        }
        break;

    case '#':
        /* XXX: simplify */
        PEEKC(c, p);
        if ((tok_flags & TOK_FLAG_BOL) && 
            (parse_flags & PARSE_FLAG_PREPROCESS)) {
            file->buf_ptr = p;
            preprocess(tok_flags & TOK_FLAG_BOF);
            p = file->buf_ptr;
            goto redo_no_start;
        } else {
            if (c == '#') {
                p++;
                tok = TOK_TWOSHARPS;
            } else {
                tok = '#';
            }
        }
        break;

    case 'a': case 'b': case 'c': case 'd':
    case 'e': case 'f': case 'g': case 'h':
    case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p':
    case 'q': case 'r': case 's': case 't':
    case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z': 
    case 'A': case 'B': case 'C': case 'D':
    case 'E': case 'F': case 'G': case 'H':
    case 'I': case 'J': case 'K': 
    case 'M': case 'N': case 'O': case 'P':
    case 'Q': case 'R': case 'S': case 'T':
    case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z': 
    case '_':
    parse_ident_fast:
        p1 = p;
        h = TOK_HASH_INIT;
        h = TOK_HASH_FUNC(h, c);
        p++;
        for(;;) {
            c = *p;
            if (!isidnum_table[c])
                break;
            h = TOK_HASH_FUNC(h, c);
            p++;
        }
        if (c != '\\') {
            TokenSym **pts;
            int len;

            /* fast case : no stray found, so we have the full token
               and we have already hashed it */
            len = p - p1;
            h &= (TOK_HASH_SIZE - 1);
            pts = &hash_ident[h];
            for(;;) {
                ts = *pts;
                if (!ts)
                    break;
                if (ts->len == len && !memcmp(ts->str, p1, len))
                    goto token_found;
                pts = &(ts->hash_next);
            }
            ts = (TokenSym *)tok_alloc_new(pts, (const char *)p1, len);
        token_found: ;
        } else {
            /* slower case */
            cstr_reset(&tokcstr);

            while (p1 < p) {
                cstr_ccat(&tokcstr, *p1);
                p1++;
            }
            p--;
            PEEKC(c, p);
        parse_ident_slow:
            while (isidnum_table[c]) {
                cstr_ccat(&tokcstr, c);
                PEEKC(c, p);
            }
            ts = tok_alloc((const char *)tokcstr.data, tokcstr.size);
        }
        tok = ts->tok;
        break;
    case 'L':
        t = p[1];
        if (t != '\\' && t != '\'' && t != '\"') {
            /* fast case */
            goto parse_ident_fast;
        } else {
            PEEKC(c, p);
            if (c == '\'') {
                tok = TOK_LCHAR;
                goto char_const; 
            } else if (c == '\"') {
                tok = TOK_LSTR;
                goto str_const;
            } else {
                cstr_reset(&tokcstr);
                cstr_ccat(&tokcstr, 'L');
                goto parse_ident_slow;
            }
        }
        break;
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    case '8': case '9':

        cstr_reset(&tokcstr);
        /* after the first digit, accept digits, alpha, '.' or sign if
           prefixed by 'eEpP' */
    parse_num:
        for(;;) {
            t = c;
            cstr_ccat(&tokcstr, c);
            PEEKC(c, p);
            if (!(isnum(c) || isid(c) || c == '.' ||
                  ((c == '+' || c == '-') && 
                   (t == 'e' || t == 'E' || t == 'p' || t == 'P'))))
                break;
        }
        /* We add a trailing '\0' to ease parsing */
        cstr_ccat(&tokcstr, '\0');
        tokc.cstr = &tokcstr;
        tok = TOK_PPNUM;
        break;
    case '.':
        /* special dot handling because it can also start a number */
        PEEKC(c, p);
        if (isnum(c)) {
            cstr_reset(&tokcstr);
            cstr_ccat(&tokcstr, '.');
            goto parse_num;
        } else if (c == '.') {
            PEEKC(c, p);
            if (c != '.')
                expect("'.'");
            PEEKC(c, p);
            tok = TOK_DOTS;
        } else {
            tok = '.';
        }
        break;
    case '\'':
        tok = TOK_CCHAR;
    char_const:
        file->buf_ptr = p;
        inp();
        b = getq();
        /* this cast is needed if >= 128 */
        if (tok == TOK_CCHAR)
            b = (char)b; 
        tokc.i = b;
        if (ch != '\'')
            error("unterminated character constant");
        p = file->buf_ptr;
        p++;
        break;
    case '\"':
        tok = TOK_STR;
    str_const:
        file->buf_ptr = p;
        inp();
        cstr_reset(&tokcstr);
        while (ch != '\"') {
            b = getq();
            if (ch == CH_EOF)
                error("unterminated string");
            if (tok == TOK_STR)
                cstr_ccat(&tokcstr, b);
            else
                cstr_wccat(&tokcstr, b);
        }
        if (tok == TOK_STR)
            cstr_ccat(&tokcstr, '\0');
        else
            cstr_wccat(&tokcstr, '\0');
        tokc.cstr = &tokcstr;
        p = file->buf_ptr;
        p++;
        break;

    case '<':
        PEEKC(c, p);
        if (c == '=') {
            p++;
            tok = TOK_LE;
        } else if (c == '<') {
            PEEKC(c, p);
            if (c == '=') {
                p++;
                tok = TOK_A_SHL;
            } else {
                tok = TOK_SHL;
            }
        } else {
            tok = TOK_LT;
        }
        break;
        
    case '>':
        PEEKC(c, p);
        if (c == '=') {
            p++;
            tok = TOK_GE;
        } else if (c == '>') {
            PEEKC(c, p);
            if (c == '=') {
                p++;
                tok = TOK_A_SAR;
            } else {
                tok = TOK_SAR;
            }
        } else {
            tok = TOK_GT;
        }
        break;
        
    case '&':
        PEEKC(c, p);
        if (c == '&') {
            p++;
            tok = TOK_LAND;
        } else if (c == '=') {
            p++;
            tok = TOK_A_AND;
        } else {
            tok = '&';
        }
        break;
        
    case '|':
        PEEKC(c, p);
        if (c == '|') {
            p++;
            tok = TOK_LOR;
        } else if (c == '=') {
            p++;
            tok = TOK_A_OR;
        } else {
            tok = '|';
        }
        break;

    case '+':
        PEEKC(c, p);
        if (c == '+') {
            p++;
            tok = TOK_INC;
        } else if (c == '=') {
            p++;
            tok = TOK_A_ADD;
        } else {
            tok = '+';
        }
        break;
        
    case '-':
        PEEKC(c, p);
        if (c == '-') {
            p++;
            tok = TOK_DEC;
        } else if (c == '=') {
            p++;
            tok = TOK_A_SUB;
        } else if (c == '>') {
            p++;
            tok = TOK_ARROW;
        } else {
            tok = '-';
        }
        break;

    PARSE2('!', '!', '=', TOK_NE)
    PARSE2('=', '=', '=', TOK_EQ)
    PARSE2('*', '*', '=', TOK_A_MUL)
    PARSE2('%', '%', '=', TOK_A_MOD)
    PARSE2('^', '^', '=', TOK_A_XOR)
        
        /* comments or operator */
    case '/':
        PEEKC(c, p);
        if (c == '*') {
            file->buf_ptr = p;
            parse_comment();
            p = file->buf_ptr;
            goto redo_no_start;
        } else if (c == '/') {
            file->buf_ptr = p;
            parse_line_comment();
            p = file->buf_ptr;
            goto redo_no_start;
        } else if (c == '=') {
            p++;
            tok = TOK_A_DIV;
        } else {
            tok = '/';
        }
        break;
        
        /* simple tokens */
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case ',':
    case ';':
    case ':':
    case '?':
    case '~':
    case '$': /* only used in assembler */
        tok = c;
        p++;
        break;
    default:
        error("unrecognized character \\x%02x", c);
        break;
    }
    file->buf_ptr = p;
    tok_flags = 0;
#if defined(PARSE_DEBUG)
    printf("token = %s\n", get_tok_str(tok, &tokc));
#endif
}

/* return next token without macro substitution. Can read input from
   macro_ptr buffer */
static void next_nomacro(void)
{
    if (macro_ptr) {
    redo:
        tok = *macro_ptr;
        if (tok) {
            TOK_GET(tok, macro_ptr, tokc);
            if (tok == TOK_LINENUM) {
                file->line_num = tokc.i;
                goto redo;
            }
        }
    } else {
        next_nomacro1();
    }
}

/* substitute args in macro_str and return allocated string */
static int *macro_arg_subst(Sym **nested_list, int *macro_str, Sym *args)
{
    int *st, last_tok, t, notfirst;
    Sym *s;
    CValue cval;
    TokenString str;
    CTString cstr;

    tok_str_new(&str);
    last_tok = 0;
    while(1) {
        TOK_GET(t, macro_str, cval);
        if (!t)
            break;
        if (t == '#') {
            /* stringize */
            TOK_GET(t, macro_str, cval);
            if (!t)
                break;
            s = sym_find2(args, t);
            if (s) {
                cstr_new(&cstr);
                st = (int *)INT2PTR(s->c);
                notfirst = 0;
                while (*st) {
                    if (notfirst)
                        cstr_ccat(&cstr, ' ');
                    TOK_GET(t, st, cval);
                    cstr_cat(&cstr, get_tok_str(t, &cval));
                    notfirst = 1;
                }
                cstr_ccat(&cstr, '\0');
#ifdef PP_DEBUG
                printf("stringize: %s\n", (char *)cstr.data);
#endif
                /* add string */
                cval.cstr = &cstr;
                tok_str_add2(&str, TOK_STR, &cval);
                cstr_free(&cstr);
            } else {
                tok_str_add2(&str, t, &cval);
            }
        } else if (t >= TOK_IDENT) {
            s = sym_find2(args, t);
            if (s) {
                st = (int *)INT2PTR(s->c);
                /* if '##' is present before or after, no arg substitution */
                if (*macro_str == TOK_TWOSHARPS || last_tok == TOK_TWOSHARPS) {
                    /* special case for var arg macros : ## eats the
                       ',' if empty VA_ARGS variable. */
                    /* XXX: test of the ',' is not 100%
                       reliable. should fix it to avoid security
                       problems */
                    if (gnu_ext && s->type.t &&
                        last_tok == TOK_TWOSHARPS && 
                        str.len >= 2 && str.str[str.len - 2] == ',') {
                        if (*st == 0) {
                            /* suppress ',' '##' */
                            str.len -= 2;
                        } else {
                            /* suppress '##' and add variable */
                            str.len--;
                            goto add_var;
                        }
                    } else {
                        int t1;
                    add_var:
                        for(;;) {
                            TOK_GET(t1, st, cval);
                            if (!t1)
                                break;
                            tok_str_add2(&str, t1, &cval);
                        }
                    }
                } else {
                    macro_subst(&str, nested_list, st);
                }
            } else {
                tok_str_add(&str, t);
            }
        } else {
            tok_str_add2(&str, t, &cval);
        }
        last_tok = t;
    }
    tok_str_add(&str, 0);
    return str.str;
}

/* do macro substitution of current token with macro 's' and add
   result to (tok_str,tok_len). 'nested_list' is the list of all
   macros we got inside to avoid recursing. Return non zero if no
   substitution needs to be done */
static int macro_subst_tok(TokenString *tok_str,
                           Sym **nested_list, Sym *s)
{
    Sym *args, *sa, *sa1;
    int mstr_allocated, parlevel, *mstr, t;
    TokenString str;
    const char *cstrval;
    CValue cval;
    CTString cstr;
            
    /* if symbol is a macro, prepare substitution */

    /* special macros */
    if (tok == TOK___LINE__) {
        cval.i = file->line_num;
        tok_str_add2(tok_str, TOK_CINT, &cval);
    } else if (tok == TOK___FILE__) {
        cstrval = file->filename;
        goto add_cstr;
        tok_str_add2(tok_str, TOK_STR, &cval);
    } else if (tok == TOK___DATE__) {
        cstrval = "Jan  1 2002";
        goto add_cstr;
    } else if (tok == TOK___TIME__) {
        cstrval = "00:00:00";
    add_cstr:
        cstr_new(&cstr);
        cstr_cat(&cstr, cstrval);
        cstr_ccat(&cstr, '\0');
        cval.cstr = &cstr;
        tok_str_add2(tok_str, TOK_STR, &cval);
        cstr_free(&cstr);
    } else {
        mstr = (int *)INT2PTR(s->c);
        mstr_allocated = 0;
        if (s->type.t == MACRO_FUNC) {
            /* NOTE: we do not use next_nomacro to avoid eating the
               next token. XXX: find better solution */
            if (macro_ptr) {
                t = *macro_ptr;
            } else {
                /* XXX: incorrect with comments */
                ch = file->buf_ptr[0];
                while (is_space(ch) || ch == '\n')
                    cinp();
                t = ch;
            }
            if (t != '(') /* no macro subst */
                return -1;
                    
            /* argument macro */
            next_nomacro();
            next_nomacro();
            args = NULL;
            sa = s->next;
            /* NOTE: empty args are allowed, except if no args */
            for(;;) {
                /* handle '()' case */
                if (!args && tok == ')')
                    break;
                if (!sa)
                    error("macro '%s' used with too many args",
                          get_tok_str(s->v, 0));
                tok_str_new(&str);
                parlevel = 0;
                /* NOTE: non zero sa->t indicates VA_ARGS */
                while ((parlevel > 0 || 
                        (tok != ')' && 
                         (tok != ',' || sa->type.t))) && 
                       tok != -1) {
                    if (tok == '(')
                        parlevel++;
                    else if (tok == ')')
                        parlevel--;
                    tok_str_add2(&str, tok, &tokc);
                    next_nomacro();
                }
                tok_str_add(&str, 0);
                sym_push2(&args, sa->v & ~SYM_FIELD, sa->type.t, PTR2INT(str.str));
                sa = sa->next;
                if (tok == ')') {
                    /* special case for gcc var args: add an empty
                       var arg argument if it is omitted */
                    if (sa && sa->type.t && gnu_ext)
                        continue;
                    else
                        break;
                }
                if (tok != ',')
                    expect(",");
                next_nomacro();
            }
            if (sa) {
                error("macro '%s' used with too few args",
                      get_tok_str(s->v, 0));
            }

            /* now subst each arg */
            mstr = macro_arg_subst(nested_list, mstr, args);
            /* free memory */
            sa = args;
            while (sa) {
                sa1 = sa->prev;
                tok_str_free((int *)INT2PTR(sa->c));
                tcc_free(sa);
                sa = sa1;
            }
            mstr_allocated = 1;
        }
        sym_push2(nested_list, s->v, 0, 0);
        macro_subst(tok_str, nested_list, mstr);
        /* pop nested defined symbol */
        sa1 = *nested_list;
        *nested_list = sa1->prev;
        tcc_free(sa1);
        if (mstr_allocated)
            tok_str_free(mstr);
    }
    return 0;
}

/* handle the '##' operator. Return NULL if no '##' seen. Otherwise
   return the resulting string (which must be freed). */
static inline int *macro_twosharps(const int *macro_str)
{
    TokenSym *ts;
    const int *macro_ptr1, *start_macro_ptr, *ptr, *saved_macro_ptr;
    int t;
    const char *p1, *p2;
    CValue cval;
    TokenString macro_str1;
    CTString cstr;

    start_macro_ptr = macro_str;
    /* we search the first '##' */
    for(;;) {
        macro_ptr1 = macro_str;
        TOK_GET(t, macro_str, cval);
        /* nothing more to do if end of string */
        if (t == 0)
            return NULL;
        if (*macro_str == TOK_TWOSHARPS)
            break;
    }

    /* we saw '##', so we need more processing to handle it */
    cstr_new(&cstr);
    tok_str_new(&macro_str1);
    tok = t;
    tokc = cval;

    /* add all tokens seen so far */
    for(ptr = start_macro_ptr; ptr < macro_ptr1;) {
        TOK_GET(t, ptr, cval);
        tok_str_add2(&macro_str1, t, &cval);
    }
    saved_macro_ptr = macro_ptr;
    /* XXX: suppress the use of macro_ptr here */
    macro_ptr = (int *)macro_str;
    for(;;) {
        while (*macro_ptr == TOK_TWOSHARPS) {
            macro_ptr++;
            macro_ptr1 = macro_ptr;
            t = *macro_ptr;
            if (t) {
                TOK_GET(t, macro_ptr, cval);
                /* We concatenate the two tokens if we have an
                   identifier or a preprocessing number */
                cstr_reset(&cstr);
                p1 = get_tok_str(tok, &tokc);
                cstr_cat(&cstr, p1);
                p2 = get_tok_str(t, &cval);
                cstr_cat(&cstr, p2);
                cstr_ccat(&cstr, '\0');
                
                if ((tok >= TOK_IDENT || tok == TOK_PPNUM) && 
                    (t >= TOK_IDENT || t == TOK_PPNUM)) {
                    if (tok == TOK_PPNUM) {
                        /* if number, then create a number token */
                        /* NOTE: no need to allocate because
                           tok_str_add2() does it */
                        tokc.cstr = &cstr;
                    } else {
                        /* if identifier, we must do a test to
                           validate we have a correct identifier */
                        if (t == TOK_PPNUM) {
                            const char *p;
                            int c;

                            p = p2;
                            for(;;) {
                                c = *p;
                                if (c == '\0')
                                    break;
                                p++;
                                if (!isnum(c) && !isid(c))
                                    goto error_pasting;
                            }
                        }
                        ts = tok_alloc((const char *)cstr.data, strlen((const char *)cstr.data));
                        tok = ts->tok; /* modify current token */
                    }
                } else {
                    const char *str;
                    const unsigned char *q;

                    str = (const char *)cstr.data;  // avoid warning tktk 

                    /* we look for a valid token */
                    /* XXX: do more extensive checks */
                    if (!strcmp(str, ">>=")) {
                        tok = TOK_A_SAR;
                    } else if (!strcmp(str, "<<=")) {
                        tok = TOK_A_SHL;
                    } else if (strlen(str) == 2) {
                        /* search in two bytes table */
                        q = (const unsigned char *)tok_two_chars;
                        for(;;) {
                            if (!*q)
                                goto error_pasting;
                            if (q[0] == str[0] && q[1] == str[1])
                                break;
                            q += 3;
                        }
                        tok = q[2];
                    } else {
                    error_pasting:
                        str = (const char *)cstr.data;  // avoid warning tktk 
                        /* NOTE: because get_tok_str use a static buffer,
                           we must save it */
                        cstr_reset(&cstr);
                        p1 = get_tok_str(tok, &tokc);
                        cstr_cat(&cstr, p1);
                        cstr_ccat(&cstr, '\0');
                        p2 = get_tok_str(t, &cval);
                        warning("pasting \"%s\" and \"%s\" does not give a valid preprocessing token", cstr.data, p2);
                        /* cannot merge tokens: just add them separately */
                        tok_str_add2(&macro_str1, tok, &tokc);
                        /* XXX: free associated memory ? */
                        tok = t;
                        tokc = cval;
                    }
                }
            }
        }
        tok_str_add2(&macro_str1, tok, &tokc);
        next_nomacro();
        if (tok == 0)
            break;
    }
    macro_ptr = (int *)saved_macro_ptr;
    cstr_free(&cstr);
    tok_str_add(&macro_str1, 0);
    return macro_str1.str;
}


/* do macro substitution of macro_str and add result to
   (tok_str,tok_len). 'nested_list' is the list of all macros we got
   inside to avoid recursing. */
static void macro_subst(TokenString *tok_str,
                        Sym **nested_list, const int *macro_str)
{
    Sym *s;
    int *saved_macro_ptr, *macro_str1;
    const int *ptr;
    int t, ret;
    CValue cval;

    /* first scan for '##' operator handling */
    ptr = macro_str;
    macro_str1 = macro_twosharps(ptr);
    if (macro_str1) 
        ptr = macro_str1;
    while (1) {
        TOK_GET(t, ptr, cval);
        if (t == 0)
            break;
        s = define_find(t);
        if (s != NULL) {
            /* if nested substitution, do nothing */
            if (sym_find2(*nested_list, t))
                goto no_subst;
            saved_macro_ptr = macro_ptr;
            macro_ptr = (int *)ptr;
            tok = t;
            ret = macro_subst_tok(tok_str, nested_list, s);
            ptr = (int *)macro_ptr;
            macro_ptr = saved_macro_ptr;
            if (ret != 0)
                goto no_subst;
        } else {
        no_subst:
            tok_str_add2(tok_str, t, &cval);
        }
    }
    if (macro_str1)
        tok_str_free(macro_str1);
}

/* return next token with macro substitution */
static void next(void)
{
    Sym *nested_list, *s;
    TokenString str;

 redo:
    next_nomacro();
    if (!macro_ptr) {
        /* if not reading from macro substituted string, then try
           to substitute macros */
        if (tok >= TOK_IDENT &&
            (parse_flags & PARSE_FLAG_PREPROCESS)) {
            s = define_find(tok);
            if (s) {
                /* we have a macro: we try to substitute */
                tok_str_new(&str);
                nested_list = NULL;
                if (macro_subst_tok(&str, &nested_list, s) == 0) {
                    /* substitution done, NOTE: maybe empty */
                    tok_str_add(&str, 0);
                    macro_ptr = str.str;
                    macro_ptr_allocated = str.str;
                    goto redo;
                }
            }
        }
    } else {
        if (tok == 0) {
            /* end of macro or end of unget buffer */
            if (unget_buffer_enabled) {
                macro_ptr = unget_saved_macro_ptr;
                unget_buffer_enabled = 0;
            } else {
                /* end of macro string: free it */
                tok_str_free(macro_ptr_allocated);
                macro_ptr = NULL;
            }
            goto redo;
        }
    }
    
    /* convert preprocessor tokens into C tokens */
    if (tok == TOK_PPNUM &&
        (parse_flags & PARSE_FLAG_TOK_NUM)) {
        parse_number((char *)tokc.cstr->data);
    }


	// tktk 
#ifdef ASSEMBLY_LISTING_C67
	if (!f)
	{
		f=fopen("TCC67_out.txt","wt");
	}
	fprintf(f,"                                     %s\n", get_tok_str(tok, &tokc));
#endif

}

/* push back current token and set current token to 'last_tok'. Only
   identifier case handled for labels. */
static inline void unget_tok(int last_tok)
{
    int i, n;
    int *q;
    unget_saved_macro_ptr = macro_ptr;
    unget_buffer_enabled = 1;
    q = unget_saved_buffer;
    macro_ptr = q;
    *q++ = tok;
    n = tok_ext_size(tok) - 1;
    for(i=0;i<n;i++)
        *q++ = tokc.tab[i];
    *q = 0; /* end of token string */
    tok = last_tok;
}


void swap(int *p, int *q)
{
    int t;
    t = *p;
    *p = *q;
    *q = t;
}

void vsetc(CType *type, int r, CValue *vc)
{
    int v;

    if (vtop >= vstack + VSTACK_SIZE)
        error("memory full");
    /* cannot let cpu flags if other instruction are generated. Also
       avoid leaving VT_JMP anywhere except on the top of the stack
       because it would complicate the code generator. */
    if (vtop >= vstack) {
        v = vtop->r & VT_VALMASK;
        if (v == VT_CMP || (v & ~1) == VT_JMP)
            gv(RC_INT);
    }
    vtop++;
    vtop->type = *type;
    vtop->r = r;
    vtop->r2 = VT_CONST;
    vtop->c = *vc;
}

/* push integer constant */
void vpushi(int v)
{
    CValue cval;
    cval.i = v;
    vsetc(&int_type, VT_CONST, &cval);
}

/* Return a static symbol pointing to a section */
static Sym *get_sym_ref(CType *type, Section *sec, 
                        unsigned long offset, unsigned long size)
{
    int v;
    Sym *sym;

    v = anon_sym++;
    sym = global_identifier_push(v, type->t | VT_STATIC, 0);
    sym->type.ref = type->ref;
    sym->r = VT_CONST | VT_SYM;
    put_extern_sym(sym, sec, offset, size);
    return sym;
}

/* push a reference to a section offset by adding a dummy symbol */
static void vpush_ref(CType *type, Section *sec, unsigned long offset, unsigned long size)
{
    CValue cval;

    cval.ul = 0;
    vsetc(type, VT_CONST | VT_SYM, &cval);
    vtop->sym = get_sym_ref(type, sec, offset, size);
}

/* define a new external reference to a symbol 'v' of type 'u' */
static Sym *external_global_sym(int v, CType *type, int r)
{
    Sym *s;

    s = sym_find(v);
    if (!s) {
        /* push forward reference */
        s = global_identifier_push(v, type->t | VT_EXTERN, 0);
        s->type.ref = type->ref;
        s->r = r | VT_CONST | VT_SYM;
    }
    return s;
}

/* define a new external reference to a symbol 'v' of type 'u' */
static Sym *external_sym(int v, CType *type, int r)
{
    Sym *s;

    s = sym_find(v);
    if (!s) {
        /* push forward reference */
        s = sym_push(v, type, r | VT_CONST | VT_SYM, 0);
        s->type.t |= VT_EXTERN;
    } else {
        if (!is_compatible_types(&s->type, type))
            error("incompatible types for redefinition of '%s'", 
                  get_tok_str(v, NULL));
    }
    return s;
}

/* push a reference to global symbol v */
void vpush_global_sym(CType *type, int v)
{
    Sym *sym;
    CValue cval;

    sym = external_global_sym(v, type, 0);
    cval.ul = 0;
    vsetc(type, VT_CONST | VT_SYM, &cval);
    vtop->sym = sym;
}

void vset(CType *type, int r, int v)
{
    CValue cval;

    cval.i = v;
    vsetc(type, r, &cval);
}

void vseti(int r, int v)
{
    CType type;
    type.t = VT_INT;
    vset(&type, r, v);
}

void vswap(void)
{
    SValue tmp;

    tmp = vtop[0];
    vtop[0] = vtop[-1];
    vtop[-1] = tmp;
}

void vpushv(SValue *v)
{
    if (vtop >= vstack + VSTACK_SIZE)
        error("memory full");
    vtop++;
    *vtop = *v;
}

void vdup(void)
{
    vpushv(vtop);
}

/* save r to the memory stack, and mark it as being free */
void save_reg(int r)
{
    int l, saved, size, align;
    SValue *p, sv;
    CType *type;

    /* modify all stack values */
    saved = 0;
    l = 0;
    for(p=vstack;p<=vtop;p++) {
        if ((p->r & VT_VALMASK) == r ||
            (p->r2 & VT_VALMASK) == r) {
            /* must save value on stack if not already done */
            if (!saved) {
                /* NOTE: must reload 'r' because r might be equal to r2 */
                r = p->r & VT_VALMASK;
                /* store register in the stack */
                type = &p->type;
                if ((p->r & VT_LVAL) || 
                    (!is_float(type->t) && (type->t & VT_BTYPE) != VT_LLONG))
                    type = &int_type;
                size = type_size(type, &align);
                loc = (loc - size) & -align;
                sv.type.t = type->t;
                sv.r = VT_LOCAL | VT_LVAL;
                sv.c.ul = loc;
                store(r, &sv);
#ifdef TCC_TARGET_I386
                /* x86 specific: need to pop fp register ST0 if saved */
                if (r == TREG_ST0) {
                    o(0xd9dd); /* fstp %st(1) */
                }
#endif
                /* special long long case */
                if ((type->t & VT_BTYPE) == VT_LLONG) {
                    sv.c.ul += 4;
                    store(p->r2, &sv);
                }
                l = loc;
                saved = 1;
            }
            /* mark that stack entry as being saved on the stack */
            if (p->r & VT_LVAL) {
                /* also suppress the bounded flag because the
                   relocation address of the function was stored in
                   p->c.ul */
                p->r = (p->r & ~(VT_VALMASK | VT_BOUNDED)) | VT_LLOCAL;
            } else {
                p->r = lvalue_type(p->type.t) | VT_LOCAL;
            }
            p->r2 = VT_CONST;
            p->c.ul = l;
        }
    }
}

/* find a free register of class 'rc'. If none, save one register */
int get_reg(int rc)
{
    int r;
    SValue *p;

#if (DO_C67)  

	// note C67 uses register pairs for doubles
	//
	// so to consider if a float register is available
	// we must scan the stack for both the register candidate 
	// and its associated pair (always the following reg)


    /* find a free register */
    for(r=0;r<NB_REGS;r++) {
        if (reg_classes[r] & rc) {
            for(p=vstack;p<=vtop;p++) {
                if ((p->r  & VT_VALMASK) == r ||
                    (p->r2 & VT_VALMASK) == r ||
					(is_pair(rc) &&
						((p->r  & VT_VALMASK) == r+1 ||
						 (p->r2 & VT_VALMASK) == r+1)))
                   goto notfound;
            }
            return r;
        }
    notfound: ;
    }

    /* no register left : free the first one on the stack (VERY
       IMPORTANT to start from the bottom to ensure that we don't
       spill registers used in gen_opi()) */
    
	
	for(p=vstack;p<=vtop;p++) {
        r = p->r & VT_VALMASK;
        if (r < VT_CONST && (reg_classes[r] & rc)) {
            save_reg(r);

			/* if we need a float class (pair) 
			   check if the other register needs to be 
			   saved also */
	        
			if (is_pair(rc)) {
				save_reg(r+1);
			}
			return r;
        }

		/* if we need a float class (pair) also 
		   check if the used register associated
		   register is of class float */
        
		if (is_pair(rc) && r < VT_CONST && (reg_classes[r-1] & rc)) {

			save_reg(r);

			/* if we need a float class (pair) 
			   check if the other register needs to be 
			   saved also */
	        
			if (is_pair(rc)) {
				save_reg(r-1);
			}
            return r-1;
        }

		/* also look at second register (if long long) */
        r = p->r2 & VT_VALMASK;
        if (r < VT_CONST && (reg_classes[r] & rc)) {
            save_reg(r);
            return r;
        }

		/* if we need a float class (pair) also 
		   check if the used register associated
		   register is of class float */
        
		if (is_pair(rc) && r < VT_CONST && (reg_classes[r-1] & rc)) {
            save_reg(r);
            return r-1;
        }
    }
    /* Should never comes here */
	error("Unable to assign registers for operation");
    return -1;

#else
    /* find a free register */
    for(r=0;r<NB_REGS;r++) {
        if (reg_classes[r] & rc) {
            for(p=vstack;p<=vtop;p++) {
                if ((p->r & VT_VALMASK) == r ||
                    (p->r2 & VT_VALMASK) == r)
                    goto notfound;
            }
            return r;
        }
    notfound: ;
    }

    /* no register left : free the first one on the stack (VERY
       IMPORTANT to start from the bottom to ensure that we don't
       spill registers used in gen_opi()) */
    for(p=vstack;p<=vtop;p++) {
        r = p->r & VT_VALMASK;
        if (r < VT_CONST && (reg_classes[r] & rc))
            goto save_found;
        /* also look at second register (if long long) */
        r = p->r2 & VT_VALMASK;
        if (r < VT_CONST && (reg_classes[r] & rc)) {
        save_found:
            save_reg(r);
            return r;
        }
    }
    /* Should never comes here */
    return -1;
#endif


}

/* save registers up to (vtop - n) stack entry */
void save_regs(int n)
{
    int r;
    SValue *p, *p1;
    p1 = vtop - n;
    for(p = vstack;p <= p1; p++) {
        r = p->r & VT_VALMASK;
        if (r < VT_CONST) {
            save_reg(r);
        }
    }
}

/* move register 's' to 'r', and flush previous value of r to memory
   if needed */
void move_reg(int r, int s)
{
    SValue sv;

    if (r != s) {
        save_reg(r);
        sv.type.t = VT_INT;
        sv.r = s;
        sv.c.ul = 0;
        load(r, &sv);
    }
}

/* get address of vtop (vtop MUST BE an lvalue) */
void gaddrof(void)
{
    vtop->r &= ~VT_LVAL;
    /* tricky: if saved lvalue, then we can go back to lvalue */
    if ((vtop->r & VT_VALMASK) == VT_LLOCAL)
        vtop->r = (vtop->r & ~(VT_VALMASK | VT_LVAL_TYPE)) | VT_LOCAL | VT_LVAL;
}

#ifdef CONFIG_TCC_BCHECK
/* generate lvalue bound code */
void gbound(void)
{
    int lval_type;
    CType type1;

    vtop->r &= ~VT_MUSTBOUND;
    /* if lvalue, then use checking code before dereferencing */
    if (vtop->r & VT_LVAL) {
        /* if not VT_BOUNDED value, then make one */
        if (!(vtop->r & VT_BOUNDED)) {
            lval_type = vtop->r & (VT_LVAL_TYPE | VT_LVAL);
            /* must save type because we must set it to int to get pointer */
            type1 = vtop->type;
            vtop->type.t = VT_INT;
            gaddrof();
            vpushi(0);
            gen_bounded_ptr_add();
            vtop->r |= lval_type;
            vtop->type = type1;
        }
        /* then check for dereferencing */
        gen_bounded_ptr_deref();
    }
}
#endif

/* store vtop a register belonging to class 'rc'. lvalues are
   converted to values. Cannot be used if cannot be converted to
   register value (such as structures). */
int gv(int rc)
{
    int r, r2, rc2, bit_pos, bit_size, size, align, i;
    __uint64 ll;

    /* NOTE: get_reg can modify vstack[] */
    if (vtop->type.t & VT_BITFIELD) {
        bit_pos = (vtop->type.t >> VT_STRUCT_SHIFT) & 0x3f;
        bit_size = (vtop->type.t >> (VT_STRUCT_SHIFT + 6)) & 0x3f;
        /* remove bit field info to avoid loops */
        vtop->type.t &= ~(VT_BITFIELD | (-1 << VT_STRUCT_SHIFT));
        /* generate shifts */
        vpushi(32 - (bit_pos + bit_size));
        gen_op(TOK_SHL);
        vpushi(32 - bit_size);
        /* NOTE: transformed to SHR if unsigned */
        gen_op(TOK_SAR);
        r = gv(rc);
    } else {
        if (is_float(vtop->type.t) && 
            (vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
            Sym *sym;
            int *ptr;
            unsigned long offset;
            
            /* XXX: unify with initializers handling ? */
            /* CPUs usually cannot use float constants, so we store them
               generically in data segment */
            size = type_size(&vtop->type, &align);
            offset = (data_section->data_offset + align - 1) & -align;
            data_section->data_offset = offset;
            /* XXX: not portable yet */
            ptr = (int *)section_ptr_add(data_section, size);
            size = size >> 2;
            for(i=0;i<size;i++)
                ptr[i] = vtop->c.tab[i];
            sym = get_sym_ref(&vtop->type, data_section, offset, size << 2);
            vtop->r |= VT_LVAL | VT_SYM;
            vtop->sym = sym;
            vtop->c.ul = 0;
        }
#ifdef CONFIG_TCC_BCHECK
        if (vtop->r & VT_MUSTBOUND) 
            gbound();
#endif

        r = vtop->r & VT_VALMASK;
        /* need to reload if:
           - constant
           - lvalue (need to dereference pointer)
           - already a register, but not in the right class
		   - if class is RC_FLOAT verify the 2nd register is not in use*/
        if (r >= VT_CONST || 
            (vtop->r & VT_LVAL) ||
            !(reg_classes[r] & rc) ||
            ((vtop->type.t & VT_BTYPE) == VT_LLONG && 
             !(reg_classes[vtop->r2] & rc)) || 
			 (is_pair(rc) && !check_r2_free(r+1))) {
            r = get_reg(rc);
            if ((vtop->type.t & VT_BTYPE) == VT_LLONG) {
                /* two register type load : expand to two words
                   temporarily */
                if ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
                    /* load constant */
                    ll = vtop->c.ull;
                    vtop->c.ui = (unsigned int)ll; /* first word */
                    load(r, vtop);
                    vtop->r = r; /* save register value */
                    vpushi((int)(ll >> 32)); /* second word */
                } else if (r >= VT_CONST || 
                           (vtop->r & VT_LVAL)) {
                    /* load from memory */
                    load(r, vtop);
                    vdup();
                    vtop[-1].r = r; /* save register value */
                    /* increment pointer to get second word */
                    vtop->type.t = VT_INT;
                    gaddrof();
                    vpushi(4);
                    gen_op('+');
                    vtop->r |= VT_LVAL;
                } else {
                    /* move registers */
                    load(r, vtop);
                    vdup();
                    vtop[-1].r = r; /* save register value */
                    vtop->r = vtop[-1].r2;
                }
                /* allocate second register */
                rc2 = RC_INT;
                if (rc == RC_IRET)
                    rc2 = RC_LRET;
                r2 = get_reg(rc2);
                load(r2, vtop);
                vpop();
                /* write second register */
                vtop->r2 = r2;
            } else if ((vtop->r & VT_LVAL) && !is_float(vtop->type.t)) {
                int t1, t;
                /* lvalue of scalar type : need to use lvalue type
                   because of possible cast */
                t = vtop->type.t;
                t1 = t;
                /* compute memory access type */
                if (vtop->r & VT_LVAL_BYTE)
                    t = VT_BYTE;
                else if (vtop->r & VT_LVAL_SHORT)
                    t = VT_SHORT;
                if (vtop->r & VT_LVAL_UNSIGNED)
                    t |= VT_UNSIGNED;
                vtop->type.t = t;
                load(r, vtop);
                /* restore wanted type */
                vtop->type.t = t1;
            } else {
                /* one register type load */
                load(r, vtop);
            }
        }
        vtop->r = r;
#if (DO_C67)  // uses register pairs for doubles
		if ((vtop->type.t & VT_BTYPE) == VT_DOUBLE) 
			vtop->r2 = r+1;
#endif
    }
    return r;
}


// check if the 2nd of a register pair is already in use as a primary register

int check_r2_free(int r)
{
    SValue *p;

    for(p=vstack;p<=vtop;p++)
        if ((p->r  & VT_VALMASK) == r)
           return 0;
    
    return 1;
}


/* generate vtop[-1] and vtop[0] in resp. classes rc1 and rc2 */
void gv2(int rc1, int rc2)
{
    int v;

    /* generate more generic register first. But VT_JMP or VT_CMP
       values must be generated first in all cases to avoid possible
       reload errors */
    v = vtop[0].r & VT_VALMASK;
    if (v != VT_CMP && (v & ~1) != VT_JMP && rc1 <= rc2) {
        vswap();
        gv(rc1);
        vswap();
        gv(rc2);
        /* test if reload is needed for first register */
        if ((vtop[-1].r & VT_VALMASK) >= VT_CONST) {
            vswap();
            gv(rc1);
            vswap();
        }
    } else {
        gv(rc2);
        vswap();
        gv(rc1);
        vswap();
        /* test if reload is needed for first register */
        if ((vtop[0].r & VT_VALMASK) >= VT_CONST) {
            gv(rc2);
        }
    }
}

/* expand long long on stack in two int registers */
void lexpand(void)
{
    int u;

    u = vtop->type.t & VT_UNSIGNED;
    gv(RC_INT);
    vdup();
    vtop[0].r = vtop[-1].r2;
    vtop[0].r2 = VT_CONST;
    vtop[-1].r2 = VT_CONST;
    vtop[0].type.t = VT_INT | u;
    vtop[-1].type.t = VT_INT | u;
}

/* build a long long from two ints */
void lbuild(int t)
{
    gv2(RC_INT, RC_INT);
    vtop[-1].r2 = vtop[0].r;
    vtop[-1].type.t = t;
    vpop();
}

/* rotate n first stack elements to the bottom */
void vrotb(int n)
{
    int i;
    SValue tmp;

    tmp = vtop[-n + 1];
    for(i=-n+1;i!=0;i++)
        vtop[i] = vtop[i+1];
    vtop[0] = tmp;
}

/* pop stack value */
void vpop(void)
{
    int v;
    v = vtop->r & VT_VALMASK;
#ifdef TCC_TARGET_I386
#if (!DO_C67)
    /* for x86, we need to pop the FP stack */
    if (v == TREG_ST0 && !nocode_wanted) {
        o(0xd9dd); /* fstp %st(1) */
    } else
#endif
#endif
    if (v == VT_JMP || v == VT_JMPI) {
        /* need to put correct jump if && or || without test */
        gsym(vtop->c.ul);
    }
    vtop--;
}

/* convert stack entry to register and duplicate its value in another
   register */
void gv_dup(void)
{
    int rc, t, r, r1;
    SValue sv;

    t = vtop->type.t;
    if ((t & VT_BTYPE) == VT_LLONG) {
        lexpand();
        gv_dup();
        vswap();
        vrotb(3);
        gv_dup();
        vrotb(4);
        /* stack: H L L1 H1 */
        lbuild(t);
        vrotb(3);
        vrotb(3);
        vswap();
        lbuild(t);
        vswap();
    } else {
        /* duplicate value */
        rc = RC_INT;
        sv.type.t = VT_INT;
        if (is_float(t)) {
            rc = RC_FLOAT;
            sv.type.t = t;
        }
        r = gv(rc);
        r1 = get_reg(rc);
        sv.r = r;
        sv.c.ul = 0;
        load(r1, &sv); /* move r to r1 */
        vdup();
        /* duplicates value */
        vtop->r = r1;
    }
}

/* generate CPU independent (unsigned) long long operations */
void gen_opl(int op)
{
    int t, a, b, op1, c, i;
    int func;
    GFuncContext gf;
    SValue tmp;

    switch(op) {
    case '/':
    case TOK_PDIV:
        func = TOK___divdi3;
        goto gen_func;
    case TOK_UDIV:
        func = TOK___udivdi3;
        goto gen_func;
    case '%':
        func = TOK___moddi3;
        goto gen_func;
    case TOK_UMOD:
        func = TOK___umoddi3;
    gen_func:
        /* call generic long long function */
		gfunc_start(&gf, FUNC_CDECL);
        gfunc_param(&gf);
        gfunc_param(&gf);
        vpush_global_sym(&func_old_type, func);
        gfunc_call(&gf);
        vpushi(0);
        vtop->r = REG_IRET;
        vtop->r2 = REG_LRET;
        break;
    case '^':
    case '&':
    case '|':
    case '*':
    case '+':
    case '-':
        t = vtop->type.t;
        vswap();
        lexpand();
        vrotb(3);
        lexpand();
        /* stack: L1 H1 L2 H2 */
        tmp = vtop[0];
        vtop[0] = vtop[-3];
        vtop[-3] = tmp;
        tmp = vtop[-2];
        vtop[-2] = vtop[-3];
        vtop[-3] = tmp;
        vswap();
        /* stack: H1 H2 L1 L2 */
        if (op == '*') {
            vpushv(vtop - 1);
            vpushv(vtop - 1);
            gen_op(TOK_UMULL);
            lexpand();
            /* stack: H1 H2 L1 L2 ML MH */
            for(i=0;i<4;i++)
                vrotb(6);
            /* stack: ML MH H1 H2 L1 L2 */
            tmp = vtop[0];
            vtop[0] = vtop[-2];
            vtop[-2] = tmp;
            /* stack: ML MH H1 L2 H2 L1 */
            gen_op('*');
            vrotb(3);
            vrotb(3);
            gen_op('*');
            /* stack: ML MH M1 M2 */
            gen_op('+');
            gen_op('+');
        } else if (op == '+' || op == '-') {
            /* XXX: add non carry method too (for MIPS or alpha) */
            if (op == '+')
                op1 = TOK_ADDC1;
            else
                op1 = TOK_SUBC1;
            gen_op(op1);
            /* stack: H1 H2 (L1 op L2) */
            vrotb(3);
            vrotb(3);
            gen_op(op1 + 1); /* TOK_xxxC2 */
        } else {
            gen_op(op);
            /* stack: H1 H2 (L1 op L2) */
            vrotb(3);
            vrotb(3);
            /* stack: (L1 op L2) H1 H2 */
            gen_op(op);
            /* stack: (L1 op L2) (H1 op H2) */
        }
        /* stack: L H */
        lbuild(t);
        break;
    case TOK_SAR:
    case TOK_SHR:
    case TOK_SHL:
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            t = vtop[-1].type.t;
            vswap();
            lexpand();
            vrotb(3);
            /* stack: L H shift */
            c = (int)vtop->c.i;
            /* constant: simpler */
            /* NOTE: all comments are for SHL. the other cases are
               done by swaping words */
            vpop();
            if (op != TOK_SHL)
                vswap();
            if (c >= 32) {
                /* stack: L H */
                vpop();
                if (c > 32) {
                    vpushi(c - 32);
                    gen_op(op);
                }
                if (op != TOK_SAR) {
                    vpushi(0);
                } else {
                    gv_dup();
                    vpushi(31);
                    gen_op(TOK_SAR);
                }
                vswap();
            } else {
                vswap();
                gv_dup();
                /* stack: H L L */
                vpushi(c);
                gen_op(op);
                vswap();
                vpushi(32 - c);
                if (op == TOK_SHL)
                    gen_op(TOK_SHR);
                else
                    gen_op(TOK_SHL);
                vrotb(3);
                /* stack: L L H */
                vpushi(c);
                if (op == TOK_SHL)
                    gen_op(TOK_SHL);
                else
                    gen_op(TOK_SHR);
                gen_op('|');
            }
            if (op != TOK_SHL)
                vswap();
            lbuild(t);
        } else {
            /* XXX: should provide a faster fallback on x86 ? */
            switch(op) {
            case TOK_SAR:
                func = TOK___sardi3;
                goto gen_func;
            case TOK_SHR:
                func = TOK___shrdi3;
                goto gen_func;
            case TOK_SHL:
                func = TOK___shldi3;
                goto gen_func;
            }
        }
        break;
    default:
        /* compare operations */
        t = vtop->type.t;
        vswap();
        lexpand();
        vrotb(3);
        lexpand();
        /* stack: L1 H1 L2 H2 */
        tmp = vtop[-1];
        vtop[-1] = vtop[-2];
        vtop[-2] = tmp;
        /* stack: L1 L2 H1 H2 */
        /* compare high */
        op1 = op;
        /* when values are equal, we need to compare low words. since
           the jump is inverted, we invert the test too. */
        if (op1 == TOK_LT)
            op1 = TOK_LE;
        else if (op1 == TOK_GT)
            op1 = TOK_GE;
        else if (op1 == TOK_ULT)
            op1 = TOK_ULE;
        else if (op1 == TOK_UGT)
            op1 = TOK_UGE;
        a = 0;
        b = 0;
        gen_op(op1);
        if (op1 != TOK_NE) {
            a = gtst(1, 0);
        }
        if (op != TOK_EQ) {
            /* generate non equal test */
            /* XXX: NOT PORTABLE yet */
            if (a == 0) {
                b = gtst(0, 0);
            } else {
#ifdef TCC_TARGET_I386
                b = psym(0x850f, 0);
#else
                error("not implemented");
#endif
            }
        }
        /* compare low. Always unsigned */
        op1 = op;
        if (op1 == TOK_LT)
            op1 = TOK_ULT;
        else if (op1 == TOK_LE)
            op1 = TOK_ULE;
        else if (op1 == TOK_GT)
            op1 = TOK_UGT;
        else if (op1 == TOK_GE)
            op1 = TOK_UGE;
        gen_op(op1);
        a = gtst(1, a);
        gsym(b);
        vseti(VT_JMPI, a);
        break;
    }
}

/* handle integer constant optimizations and various machine
   independant opt */
void gen_opic(int op)
{
    int fc, c1, c2, n;
    SValue *v1, *v2;

    v1 = vtop - 1;
    v2 = vtop;
    /* currently, we cannot do computations with forward symbols */
    c1 = (v1->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST;
    c2 = (v2->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST;
    if (c1 && c2) {
        fc = v2->c.i;
        switch(op) {
        case '+': v1->c.i += fc; break;
        case '-': v1->c.i -= fc; break;
        case '&': v1->c.i &= fc; break;
        case '^': v1->c.i ^= fc; break;
        case '|': v1->c.i |= fc; break;
        case '*': v1->c.i *= fc; break;

        case TOK_PDIV:
        case '/':
        case '%':
        case TOK_UDIV:
        case TOK_UMOD:
            /* if division by zero, generate explicit division */
            if (fc == 0) {
                if (const_wanted)
                    error("division by zero in constant");
                goto general_case;
            }
            switch(op) {
            default: v1->c.i /= fc; break;
            case '%': v1->c.i %= fc; break;
            case TOK_UDIV: v1->c.i = (unsigned)v1->c.i / fc; break;
            case TOK_UMOD: v1->c.i = (unsigned)v1->c.i % fc; break;
            }
            break;
        case TOK_SHL: v1->c.i <<= fc; break;
        case TOK_SHR: v1->c.i = (unsigned)v1->c.i >> fc; break;
        case TOK_SAR: v1->c.i >>= fc; break;
            /* tests */
        case TOK_ULT: v1->c.i = (unsigned)v1->c.i < (unsigned)fc; break;
        case TOK_UGE: v1->c.i = (unsigned)v1->c.i >= (unsigned)fc; break;
        case TOK_EQ: v1->c.i = v1->c.i == fc; break;
        case TOK_NE: v1->c.i = v1->c.i != fc; break;
        case TOK_ULE: v1->c.i = (unsigned)v1->c.i <= (unsigned)fc; break;
        case TOK_UGT: v1->c.i = (unsigned)v1->c.i > (unsigned)fc; break;
        case TOK_LT: v1->c.i = v1->c.i < fc; break;
        case TOK_GE: v1->c.i = v1->c.i >= fc; break;
        case TOK_LE: v1->c.i = v1->c.i <= fc; break;
        case TOK_GT: v1->c.i = v1->c.i > fc; break;
            /* logical */
        case TOK_LAND: v1->c.i = v1->c.i && fc; break;
        case TOK_LOR: v1->c.i = v1->c.i || fc; break;
        default:
            goto general_case;
        }
        vtop--;
    } else {
        /* if commutative ops, put c2 as constant */
        if (c1 && (op == '+' || op == '&' || op == '^' || 
                   op == '|' || op == '*')) {
            vswap();
            swap(&c1, &c2);
        }
        fc = vtop->c.i;
        if (c2 && (((op == '*' || op == '/' || op == TOK_UDIV || 
                     op == TOK_PDIV) && 
                    fc == 1) ||
                   ((op == '+' || op == '-' || op == '|' || op == '^' || 
                     op == TOK_SHL || op == TOK_SHR || op == TOK_SAR) && 
                    fc == 0) ||
                   (op == '&' && 
                    fc == -1))) {
            /* nothing to do */
            vtop--;
        } else if (c2 && (op == '*' || op == TOK_PDIV || op == TOK_UDIV)) {
            /* try to use shifts instead of muls or divs */
            if (fc > 0 && (fc & (fc - 1)) == 0) {
                n = -1;
                while (fc) {
                    fc >>= 1;
                    n++;
                }
                vtop->c.i = n;
                if (op == '*')
                    op = TOK_SHL;
                else if (op == TOK_PDIV)
                    op = TOK_SAR;
                else
                    op = TOK_SHR;
            }
            goto general_case;
        } else if (c2 && (op == '+' || op == '-') &&
                   (vtop[-1].r & (VT_VALMASK | VT_LVAL | VT_SYM)) == 
                   (VT_CONST | VT_SYM)) {
            /* symbol + constant case */
            if (op == '-')
                fc = -fc;
            vtop--;
            vtop->c.i += fc;
        } else {
        general_case:
            if (!nocode_wanted) {
                /* call low level op generator */
                gen_opi(op);
            } else {
                vtop--;
            }
        }
    }
}

/* generate a floating point operation with constant propagation */
void gen_opif(int op)
{
    int c1, c2;
    SValue *v1, *v2;
    long double f1, f2;

    v1 = vtop - 1;
    v2 = vtop;
    /* currently, we cannot do computations with forward symbols */
    c1 = (v1->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST;
    c2 = (v2->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST;
    if (c1 && c2) {
        if (v1->type.t == VT_FLOAT) {
            f1 = v1->c.f;
            f2 = v2->c.f;
        } else if (v1->type.t == VT_DOUBLE) {
            f1 = v1->c.d;
            f2 = v2->c.d;
        } else {
            f1 = v1->c.ld;
            f2 = v2->c.ld;
        }

        /* NOTE: we only do constant propagation if finite number (not
           NaN or infinity) (ANSI spec) */
        if (!ieee_finite(f1) || !ieee_finite(f2))
            goto general_case;

        switch(op) {
        case '+': f1 += f2; break;
        case '-': f1 -= f2; break;
        case '*': f1 *= f2; break;
        case '/': 
            if (f2 == 0.0) {
                if (const_wanted)
                    error("division by zero in constant");
                goto general_case;
            }
            f1 /= f2; 
            break;
            /* XXX: also handles tests ? */
        default:
            goto general_case;
        }
        /* XXX: overflow test ? */
        if (v1->type.t == VT_FLOAT) {
            v1->c.f = (float)f1;
        } else if (v1->type.t == VT_DOUBLE) {
            v1->c.d = f1;
        } else {
            v1->c.ld = f1;
        }
        vtop--;
    } else {
    general_case:
        if (!nocode_wanted) {
            gen_opf(op);
        } else {
            vtop--;
        }
    }
}

static int pointed_size(CType *type)
{
    int align;
    return type_size(pointed_type(type), &align);
}

#if 0
void check_pointer_types(SValue *p1, SValue *p2)
{
    char buf1[256], buf2[256];
    int t1, t2;
    t1 = p1->t;
    t2 = p2->t;
    if (!is_compatible_types(t1, t2)) {
        type_to_str(buf1, sizeof(buf1), t1, NULL);
        type_to_str(buf2, sizeof(buf2), t2, NULL);
        error("incompatible pointers '%s' and '%s'", buf1, buf2);
    }
}
#endif

/* generic gen_op: handles types problems */
void gen_op(int op)
{
    int u, t1, t2, bt1, bt2, t;
    CType type1;

    t1 = vtop[-1].type.t;
    t2 = vtop[0].type.t;
    bt1 = t1 & VT_BTYPE;
    bt2 = t2 & VT_BTYPE;
        
    if (bt1 == VT_PTR || bt2 == VT_PTR) {
        /* at least one operand is a pointer */
        /* relationnal op: must be both pointers */
        if (op >= TOK_ULT && op <= TOK_GT) {
            //            check_pointer_types(vtop, vtop - 1);
            /* pointers are handled are unsigned */
            t = VT_INT | VT_UNSIGNED;
            goto std_op;
        }
        /* if both pointers, then it must be the '-' op */
        if (bt1 == VT_PTR && bt2 == VT_PTR) {
            if (op != '-')
                error("cannot use pointers here");
            //            check_pointer_types(vtop - 1, vtop);
            /* XXX: check that types are compatible */
            u = pointed_size(&vtop[-1].type);
            gen_opic(op);
            /* set to integer type */
            vtop->type.t = VT_INT; 
            vpushi(u);
            gen_op(TOK_PDIV);
        } else {
            /* exactly one pointer : must be '+' or '-'. */
            if (op != '-' && op != '+')
                error("cannot use pointers here");
            /* Put pointer as first operand */
            if (bt2 == VT_PTR) {
                vswap();
                swap(&t1, &t2);
            }
            type1 = vtop[-1].type;
            /* XXX: cast to int ? (long long case) */
            vpushi(pointed_size(&vtop[-1].type));
            gen_op('*');
#ifdef CONFIG_TCC_BCHECK
            /* if evaluating constant expression, no code should be
               generated, so no bound check */
            if (do_bounds_check && !const_wanted) {
                /* if bounded pointers, we generate a special code to
                   test bounds */
                if (op == '-') {
                    vpushi(0);
                    vswap();
                    gen_op('-');
                }
                gen_bounded_ptr_add();
            } else
#endif
            {
                gen_opic(op);
            }
            /* put again type if gen_opic() swaped operands */
            vtop->type = type1;
        }
    } else if (is_float(bt1) || is_float(bt2)) {
        /* compute bigger type and do implicit casts */
        if (bt1 == VT_LDOUBLE || bt2 == VT_LDOUBLE) {
            t = VT_LDOUBLE;
        } else if (bt1 == VT_DOUBLE || bt2 == VT_DOUBLE) {
            t = VT_DOUBLE;
        } else {
            t = VT_FLOAT;
        }
        /* floats can only be used for a few operations */
        if (op != '+' && op != '-' && op != '*' && op != '/' &&
            (op < TOK_ULT || op > TOK_GT))
            error("invalid operands for binary operation");
        goto std_op;
    } else if (bt1 == VT_LLONG || bt2 == VT_LLONG) {
        /* cast to biggest op */
        t = VT_LLONG;
        /* convert to unsigned if it does not fit in a long long */
        if ((t1 & (VT_BTYPE | VT_UNSIGNED)) == (VT_LLONG | VT_UNSIGNED) ||
            (t2 & (VT_BTYPE | VT_UNSIGNED)) == (VT_LLONG | VT_UNSIGNED))
            t |= VT_UNSIGNED;
        goto std_op;
    } else {
        /* integer operations */
        t = VT_INT;
        /* convert to unsigned if it does not fit in an integer */
        if ((t1 & (VT_BTYPE | VT_UNSIGNED)) == (VT_INT | VT_UNSIGNED) ||
            (t2 & (VT_BTYPE | VT_UNSIGNED)) == (VT_INT | VT_UNSIGNED))
            t |= VT_UNSIGNED;
    std_op:
        /* XXX: currently, some unsigned operations are explicit, so
           we modify them here */
        if (t & VT_UNSIGNED) {
            if (op == TOK_SAR)
                op = TOK_SHR;
            else if (op == '/')
                op = TOK_UDIV;
            else if (op == '%')
                op = TOK_UMOD;
            else if (op == TOK_LT)
                op = TOK_ULT;
            else if (op == TOK_GT)
                op = TOK_UGT;
            else if (op == TOK_LE)
                op = TOK_ULE;
            else if (op == TOK_GE)
                op = TOK_UGE;
        }
        vswap();
        type1.t = t;
        gen_cast(&type1);
        vswap();
        /* special case for shifts and long long: we keep the shift as
           an integer */
        if (op == TOK_SHR || op == TOK_SAR || op == TOK_SHL)
            type1.t = VT_INT;
        gen_cast(&type1);
        if (is_float(t))
            gen_opif(op);
        else if ((t & VT_BTYPE) == VT_LLONG)
            gen_opl(op);
        else
            gen_opic(op);
        if (op >= TOK_ULT && op <= TOK_GT) {
            /* relationnal op: the result is an int */
            vtop->type.t = VT_INT;
        } else {
            vtop->type.t = t;
        }
    }
}

/* generic itof for unsigned long long case */
void gen_cvt_itof1(int t)
{
    GFuncContext gf;

    if ((vtop->type.t & (VT_BTYPE | VT_UNSIGNED)) == 
        (VT_LLONG | VT_UNSIGNED)) {

		gfunc_start(&gf, FUNC_CDECL);
        gfunc_param(&gf);
        if (t == VT_FLOAT)
            vpush_global_sym(&func_old_type, TOK___ulltof);
        else if (t == VT_DOUBLE)
            vpush_global_sym(&func_old_type, TOK___ulltod);
        else
            vpush_global_sym(&func_old_type, TOK___ulltold);
        gfunc_call(&gf);
        vpushi(0);
        vtop->r = REG_FRET;
    } else {
        gen_cvt_itof(t);
    }
}

/* generic ftoi for unsigned long long case */
void gen_cvt_ftoi1(int t)
{
    GFuncContext gf;
    int st;

    if (t == (VT_LLONG | VT_UNSIGNED)) {
        /* not handled natively */
		gfunc_start(&gf, FUNC_CDECL);
        st = vtop->type.t & VT_BTYPE;
        gfunc_param(&gf);
        if (st == VT_FLOAT)
            vpush_global_sym(&func_old_type, TOK___fixunssfdi);
        else if (st == VT_DOUBLE)
            vpush_global_sym(&func_old_type, TOK___fixunsdfdi);
        else
            vpush_global_sym(&func_old_type, TOK___fixunsxfdi);
        gfunc_call(&gf);
        vpushi(0);
        vtop->r = REG_IRET;
        vtop->r2 = REG_LRET;
    } else {
        gen_cvt_ftoi(t);
    }
}

/* force char or short cast */
void force_charshort_cast(int t)
{
    int bits, dbt;
    dbt = t & VT_BTYPE;
    /* XXX: add optimization if lvalue : just change type and offset */
    if (dbt == VT_BYTE)
        bits = 8;
    else
        bits = 16;
    if (t & VT_UNSIGNED) {
        vpushi((1 << bits) - 1);
        gen_op('&');
    } else {
        bits = 32 - bits;
        vpushi(bits);
        gen_op(TOK_SHL);
        vpushi(bits);
        gen_op(TOK_SAR);
    }
}

/* cast 'vtop' to 'type' */
static void gen_cast(CType *type)
{
    int sbt, dbt, sf, df, c;

    /* special delayed cast for char/short */
    /* XXX: in some cases (multiple cascaded casts), it may still
       be incorrect */
    if (vtop->r & VT_MUSTCAST) {
        vtop->r &= ~VT_MUSTCAST;
        force_charshort_cast(vtop->type.t);
    }
    
    dbt = type->t & (VT_BTYPE | VT_UNSIGNED);
    sbt = vtop->type.t & (VT_BTYPE | VT_UNSIGNED);

    if (sbt != dbt && !nocode_wanted) {
        sf = is_float(sbt);
        df = is_float(dbt);
        c = (vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST;
        if (sf && df) {
            /* convert from fp to fp */
            if (c) {
                /* constant case: we can do it now */
                /* XXX: in ISOC, cannot do it if error in convert */
                if (dbt == VT_FLOAT && sbt == VT_DOUBLE) 
                    vtop->c.f = (float)vtop->c.d;
                else if (dbt == VT_FLOAT && sbt == VT_LDOUBLE) 
                    vtop->c.f = (float)vtop->c.ld;
                else if (dbt == VT_DOUBLE && sbt == VT_FLOAT) 
                    vtop->c.d = (double)vtop->c.f;
                else if (dbt == VT_DOUBLE && sbt == VT_LDOUBLE) 
                    vtop->c.d = (double)vtop->c.ld;
                else if (dbt == VT_LDOUBLE && sbt == VT_FLOAT) 
                    vtop->c.ld = (long double)vtop->c.f;
                else if (dbt == VT_LDOUBLE && sbt == VT_DOUBLE) 
                    vtop->c.ld = (long double)vtop->c.d;
            } else {
                /* non constant case: generate code */
                gen_cvt_ftof(dbt);
            }
        } else if (df) {
            /* convert int to fp */
            if (c) {
                switch(sbt) {
                case VT_LLONG | VT_UNSIGNED:
                case VT_LLONG:
                    /* XXX: add const cases for long long */
                    goto do_itof;
                case VT_INT | VT_UNSIGNED:
                    switch(dbt) {
                    case VT_FLOAT: vtop->c.f = (float)vtop->c.ui; break;
                    case VT_DOUBLE: vtop->c.d = (double)vtop->c.ui; break;
                    case VT_LDOUBLE: vtop->c.ld = (long double)vtop->c.ui; break;
                    }
                    break;
                default:
                    switch(dbt) {
                    case VT_FLOAT: vtop->c.f = (float)vtop->c.i; break;
                    case VT_DOUBLE: vtop->c.d = (double)vtop->c.i; break;
                    case VT_LDOUBLE: vtop->c.ld = (long double)vtop->c.i; break;
                    }
                    break;
                }
            } else {
            do_itof:
                gen_cvt_itof1(dbt);
            }
        } else if (sf) {
            /* convert fp to int */
            /* we handle char/short/etc... with generic code */
            if (dbt != (VT_INT | VT_UNSIGNED) &&
                dbt != (VT_LLONG | VT_UNSIGNED) &&
                dbt != VT_LLONG)
                dbt = VT_INT;
            if (c) {
                switch(dbt) {
                case VT_LLONG | VT_UNSIGNED:
                case VT_LLONG:
                    /* XXX: add const cases for long long */
                    goto do_ftoi;
                case VT_INT | VT_UNSIGNED:
                    switch(sbt) {
//tktk                    case VT_FLOAT: vtop->c.ui = (unsigned int)vtop->c.d; break;
                    case VT_FLOAT: vtop->c.ui = (unsigned int)vtop->c.f; break;
                    case VT_DOUBLE: vtop->c.ui = (unsigned int)vtop->c.d; break;
//tktk                    case VT_LDOUBLE: vtop->c.ui = (unsigned int)vtop->c.d; break;
                    case VT_LDOUBLE: vtop->c.ui = (unsigned int)vtop->c.ld; break;
                    }
                    break;
                default:
                    /* int case */
                    switch(sbt) {
//tktk                    case VT_FLOAT: vtop->c.i = (int)vtop->c.d; break;
                    case VT_FLOAT: vtop->c.i = (int)vtop->c.f; break;
                    case VT_DOUBLE: vtop->c.i = (int)vtop->c.d; break;
//tktk                    case VT_LDOUBLE: vtop->c.i = (int)vtop->c.d; break;
                    case VT_LDOUBLE: vtop->c.i = (int)vtop->c.ld; break;
                    }
                    break;
                }
            } else {
            do_ftoi:
                gen_cvt_ftoi1(dbt);
            }
            if (dbt == VT_INT && (type->t & (VT_BTYPE | VT_UNSIGNED)) != dbt) {
                /* additionnal cast for char/short/bool... */
                vtop->type.t = dbt;
                gen_cast(type);
            }
        } else if ((dbt & VT_BTYPE) == VT_LLONG) {
            if ((sbt & VT_BTYPE) != VT_LLONG) {
                /* scalar to long long */
                if (c) {
                    if (sbt == (VT_INT | VT_UNSIGNED))
                        vtop->c.ll = vtop->c.ui;
                    else
                        vtop->c.ll = vtop->c.i;
                } else {
                    /* machine independant conversion */
                    gv(RC_INT);
                    /* generate high word */
                    if (sbt == (VT_INT | VT_UNSIGNED)) {
                        vpushi(0);
                        gv(RC_INT);
                    } else {
                        gv_dup();
                        vpushi(31);
                        gen_op(TOK_SAR);
                    }
                    /* patch second register */
                    vtop[-1].r2 = vtop->r;
                    vpop();
                }
            }
        } else if (dbt == VT_BOOL) {
            /* scalar to bool */
            vpushi(0);
            gen_op(TOK_NE);
        } else if ((dbt & VT_BTYPE) == VT_BYTE || 
                   (dbt & VT_BTYPE) == VT_SHORT) {
            force_charshort_cast(dbt);
        } else if ((dbt & VT_BTYPE) == VT_INT) {
            /* scalar to int */
            if (sbt == VT_LLONG) {
                /* from long long: just take low order word */
                lexpand();
                vpop();
            } 
            /* if lvalue and single word type, nothing to do because
               the lvalue already contains the real type size (see
               VT_LVAL_xxx constants) */
        }
    }
    vtop->type = *type;
}

/* return type size. Put alignment at 'a' */
static int type_size(CType *type, int *a)
{
    Sym *s;
    int bt;

    bt = type->t & VT_BTYPE;
    if (bt == VT_STRUCT) {
        /* struct/union */
        s = type->ref;
        *a = s->r;
        return s->c;
    } else if (bt == VT_PTR) {
        if (type->t & VT_ARRAY) {
            s = type->ref;
            return type_size(&s->type, a) * s->c;
        } else {
            *a = PTR_SIZE;
            return PTR_SIZE;
        }
    } else if (bt == VT_LDOUBLE) {
        *a = LDOUBLE_ALIGN;
        return LDOUBLE_SIZE;
    } else if (bt == VT_DOUBLE || bt == VT_LLONG) {
#if (!DO_C67)
        *a = 4; /* XXX: i386 specific */
#else
        *a = 8; /* XXX: C67 must align on 8 byte boundaries */
#endif
        return 8;
    } else if (bt == VT_INT || bt == VT_ENUM || bt == VT_FLOAT) {
        *a = 4;
        return 4;
    } else if (bt == VT_SHORT) {
        *a = 2;
        return 2;
    } else {
        /* char, void, function, _Bool */
        *a = 1;
        return 1;
    }
    return 1;   // shouldn't get here.  quiet the compiler.
}

/* return the pointed type of t */
static inline CType *pointed_type(CType *type)
{
    return &type->ref->type;
}

/* modify type so that its it is a pointer to type. */
static void mk_pointer(CType *type)
{
    Sym *s;
    s = sym_push(SYM_FIELD, type, 0, -1);
    type->t = VT_PTR | (type->t & ~VT_TYPE);
    type->ref = s;
}

static int is_compatible_types(CType *type1, CType *type2)
{
    Sym *s1, *s2;
    int bt1, bt2, t1, t2;

    t1 = type1->t & VT_TYPE;
    t2 = type2->t & VT_TYPE;
    bt1 = t1 & VT_BTYPE;
    bt2 = t2 & VT_BTYPE;
    if (bt1 == VT_PTR) {
        type1 = pointed_type(type1);
        /* if function, then convert implicitely to function pointer */
        if (bt2 != VT_FUNC) {
            if (bt2 != VT_PTR)
                return 0;
            type2 = pointed_type(type2);
        }
        /* void matches everything */
        /* XXX: not fully compliant */
        if ((type1->t & VT_TYPE) == VT_VOID || (type2->t & VT_TYPE) == VT_VOID)
            return 1;
        return is_compatible_types(type1, type2);
    } else if (bt1 == VT_STRUCT || bt2 == VT_STRUCT) {
        return (type1->ref == type2->ref);
    } else if (bt1 == VT_FUNC) {
        if (bt2 != VT_FUNC)
            return 0;
        s1 = type1->ref;
        s2 = type2->ref;
        if (!is_compatible_types(&s1->type, &s2->type))
            return 0;
        /* XXX: not complete */
        if (s1->c == FUNC_OLD || s2->c == FUNC_OLD)
            return 1;
        if (s1->c != s2->c)
            return 0;
        while (s1 != NULL) {
            if (s2 == NULL)
                return 0;
            if (!is_compatible_types(&s1->type, &s2->type))
                return 0;
            s1 = s1->next;
            s2 = s2->next;
        }
        if (s2)
            return 0;
        return 1;
    } else {
        /* XXX: not complete */
        return 1;
    }
}

/* print a type. If 'varstr' is not NULL, then the variable is also
   printed in the type */
/* XXX: union */
/* XXX: add array and function pointers */
void type_to_str(char *buf, int buf_size, 
                 CType *type, const char *varstr)
{
    int bt, v, t;
    Sym *s, *sa;
    char buf1[256];
    const char *tstr;

    t = type->t & VT_TYPE;
    bt = t & VT_BTYPE;
    buf[0] = '\0';
    if (t & VT_UNSIGNED)
        pstrcat(buf, buf_size, "unsigned ");
    switch(bt) {
    case VT_VOID:
        tstr = "void";
        goto add_tstr;
    case VT_BOOL:
        tstr = "_Bool";
        goto add_tstr;
    case VT_BYTE:
        tstr = "char";
        goto add_tstr;
    case VT_SHORT:
        tstr = "short";
        goto add_tstr;
    case VT_INT:
        tstr = "int";
        goto add_tstr;
    case VT_LONG:
        tstr = "long";
        goto add_tstr;
    case VT_LLONG:
        tstr = "long long";
        goto add_tstr;
    case VT_FLOAT:
        tstr = "float";
        goto add_tstr;
    case VT_DOUBLE:
        tstr = "double";
        goto add_tstr;
    case VT_LDOUBLE:
        tstr = "long double";
    add_tstr:
        pstrcat(buf, buf_size, tstr);
        break;
    case VT_ENUM:
    case VT_STRUCT:
        if (bt == VT_STRUCT)
            tstr = "struct ";
        else
            tstr = "enum ";
        pstrcat(buf, buf_size, tstr);
        v = type->ref->v & ~SYM_STRUCT;
        if (v >= SYM_FIRST_ANOM)
            pstrcat(buf, buf_size, "<anonymous>");
        else
            pstrcat(buf, buf_size, get_tok_str(v, NULL));
        break;
    case VT_FUNC:
        s = type->ref;
        type_to_str(buf, buf_size, &s->type, varstr);
        pstrcat(buf, buf_size, "(");
        sa = s->next;
        while (sa != NULL) {
            type_to_str(buf1, sizeof(buf1), &sa->type, NULL);
            pstrcat(buf, buf_size, buf1);
            sa = sa->next;
            if (sa)
                pstrcat(buf, buf_size, ", ");
        }
        pstrcat(buf, buf_size, ")");
        goto no_var;
    case VT_PTR:
        s = type->ref;
        pstrcpy(buf1, sizeof(buf1), "*");
        if (varstr)
            pstrcat(buf1, sizeof(buf1), varstr);
        type_to_str(buf, buf_size, &s->type, buf1);
        goto no_var;
    }
    if (varstr) {
        pstrcat(buf, buf_size, " ");
        pstrcat(buf, buf_size, varstr);
    }
 no_var: ;
}

/* verify type compatibility to store vtop in 'dt' type, and generate
   casts if needed. */
static void gen_assign_cast(CType *dt)
{
    CType *st;
    char buf1[256], buf2[256];
    int dbt, sbt;

    st = &vtop->type; /* source type */
    dbt = dt->t & VT_BTYPE;
    sbt = st->t & VT_BTYPE;
    if (dbt == VT_PTR) {
        /* special cases for pointers */
        /* a function is implicitely a function pointer */
        if (sbt == VT_FUNC) {
            if (!is_compatible_types(pointed_type(dt), st))
                goto error;
            else
                goto type_ok;
        }
        /* '0' can also be a pointer */
        if (sbt == VT_INT &&
            ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) &&
            vtop->c.i == 0)
            goto type_ok;
        /* accept implicit pointer to integer cast with warning */
        if (sbt == VT_BYTE || sbt == VT_SHORT || 
            sbt == VT_INT || sbt == VT_LLONG) {
            warning("assignment makes pointer from integer without a cast");
            goto type_ok;
        }
    } else if (dbt == VT_BYTE || dbt == VT_SHORT || 
               dbt == VT_INT || dbt == VT_LLONG) {
        if (sbt == VT_PTR || sbt == VT_FUNC) {
            warning("assignment makes integer from pointer without a cast");
            goto type_ok;
        }
    }
    if (!is_compatible_types(dt, st)) {
    error:
        type_to_str(buf1, sizeof(buf1), st, NULL);
        type_to_str(buf2, sizeof(buf2), dt, NULL);
        error("cannot cast '%s' to '%s'", buf1, buf2);
    }
 type_ok:
    gen_cast(dt);
}

/* store vtop in lvalue pushed on stack */
void vstore(void)
{
    int sbt, dbt, ft, r, t, size, align, bit_size, bit_pos, rc, delayed_cast;
    GFuncContext gf;

    ft = vtop[-1].type.t;
    sbt = vtop->type.t & VT_BTYPE;
    dbt = ft & VT_BTYPE;
    if (((sbt == VT_INT || sbt == VT_SHORT) && dbt == VT_BYTE) ||
        (sbt == VT_INT && dbt == VT_SHORT)) {
        /* optimize char/short casts */
        delayed_cast = VT_MUSTCAST;
        vtop->type.t = ft & VT_TYPE;
    } else {
        delayed_cast = 0;
        gen_assign_cast(&vtop[-1].type);
    }

    if (sbt == VT_STRUCT) {
        /* if structure, only generate pointer */
        /* structure assignment : generate memcpy */
        /* XXX: optimize if small size */
        if (!nocode_wanted) {
			gfunc_start(&gf, FUNC_CDECL);
            /* destination */
            vswap();
            vtop->type.t = VT_INT;
            gaddrof();
            gfunc_param(&gf);
            /* source */
            vdup();            
            size = type_size(&vtop->type, &align);
			vtop->type.t = VT_INT;
            gaddrof();
            gfunc_param(&gf);
			/* type size */
            vpushi(size);
            gfunc_param(&gf);
            save_regs(0);
            vpush_global_sym(&func_old_type, TOK_memcpy);
            gfunc_call(&gf);
        } else {
            vswap();
            vpop();
        }
        /* leave source on stack */
    } else if (ft & VT_BITFIELD) {
        /* bitfield store handling */
        bit_pos = (ft >> VT_STRUCT_SHIFT) & 0x3f;
        bit_size = (ft >> (VT_STRUCT_SHIFT + 6)) & 0x3f;
        /* remove bit field info to avoid loops */
        vtop[-1].type.t = ft & ~(VT_BITFIELD | (-1 << VT_STRUCT_SHIFT));

        /* duplicate destination */
        vdup();
        vtop[-1] = vtop[-2];

        /* mask and shift source */
        vpushi((1 << bit_size) - 1);
        gen_op('&');
        vpushi(bit_pos);
        gen_op(TOK_SHL);
        /* load destination, mask and or with source */
        vswap();
        vpushi(~(((1 << bit_size) - 1) << bit_pos));
        gen_op('&');
        gen_op('|');
        /* store result */
        vstore();
    } else {
#ifdef CONFIG_TCC_BCHECK
        /* bound check case */
        if (vtop[-1].r & VT_MUSTBOUND) {
            vswap();
            gbound();
            vswap();
        }
#endif
        if (!nocode_wanted) {
            rc = RC_INT;
            if (is_float(ft))
                rc = RC_FLOAT;
            r = gv(rc);  /* generate value */
            /* if lvalue was saved on stack, must read it */
            if ((vtop[-1].r & VT_VALMASK) == VT_LLOCAL) {
                SValue sv;
                t = get_reg(RC_INT);
                sv.type.t = VT_INT;
                sv.r = VT_LOCAL | VT_LVAL;
                sv.c.ul = vtop[-1].c.ul;
                load(t, &sv);
                vtop[-1].r = t | VT_LVAL;
            }
            store(r, vtop - 1);
            /* two word case handling : store second register at word + 4 */
            if ((ft & VT_BTYPE) == VT_LLONG) {
                vswap();
                /* convert to int to increment easily */
                vtop->type.t = VT_INT;
                gaddrof();
                vpushi(4);
                gen_op('+');
                vtop->r |= VT_LVAL;
                vswap();
                /* XXX: it works because r2 is spilled last ! */
                store(vtop->r2, vtop - 1);
            }
        }
        vswap();
        vtop--; /* NOT vpop() because on x86 it would flush the fp stack */
        vtop->r |= delayed_cast;
    }
}

/* post defines POST/PRE add. c is the token ++ or -- */
void inc(int post, int c)
{
    test_lvalue();
    vdup(); /* save lvalue */
    if (post) {
        gv_dup(); /* duplicate value */
        vrotb(3);
        vrotb(3);
    }
    /* add constant */
    vpushi(c - TOK_MID); 
    gen_op('+');
    vstore(); /* store value */
    if (post)
        vpop(); /* if post op, return saved value */
}

/* Parse GNUC __attribute__ extension. Currently, the following
   extensions are recognized:
   - aligned(n) : set data/function alignment.
   - section(x) : generate data/code in this section.
   - unused : currently ignored, but may be used someday.
 */
static void parse_attribute(AttributeDef *ad)
{
    int t, n;
    
    while (tok == TOK_ATTRIBUTE1 || tok == TOK_ATTRIBUTE2) {
    next();
    skip('(');
    skip('(');
    while (tok != ')') {
        if (tok < TOK_IDENT)
            expect("attribute name");
        t = tok;
        next();
        switch(t) {
        case TOK_SECTION1:
        case TOK_SECTION2:
            skip('(');
            if (tok != TOK_STR)
                expect("section name");
            ad->section = find_section(tcc_state, (char *)tokc.cstr->data);
            next();
            skip(')');
            break;
        case TOK_ALIGNED1:
        case TOK_ALIGNED2:
            if (tok == '(') {
                next();
                n = expr_const();
                if (n <= 0 || (n & (n - 1)) != 0) 
                    error("alignment must be a positive power of two");
                skip(')');
            } else {
                n = MAX_ALIGN;
            }
            ad->aligned = n;
            break;
        case TOK_UNUSED1:
        case TOK_UNUSED2:
            /* currently, no need to handle it because tcc does not
               track unused objects */
            break;
        case TOK_NORETURN1:
        case TOK_NORETURN2:
            /* currently, no need to handle it because tcc does not
               track unused objects */
            break;
        case TOK_CDECL1:
        case TOK_CDECL2:
        case TOK_CDECL3:
            ad->func_call = FUNC_CDECL;
            break;
        case TOK_STDCALL1:
        case TOK_STDCALL2:
        case TOK_STDCALL3:
            ad->func_call = FUNC_STDCALL;
            break;
        default:
            //            warning("'%s' attribute ignored", get_tok_str(t, NULL));
            /* skip parameters */
            /* XXX: skip parenthesis too */
            if (tok == '(') {
                next();
                while (tok != ')' && tok != -1)
                    next();
                next();
            }
            break;
        }
        if (tok != ',')
            break;
        next();
    }
    skip(')');
    skip(')');
    }
}

/* enum/struct/union declaration. u is either VT_ENUM or VT_STRUCT */
static void struct_decl(CType *type, int u)
{
    int a, v, size, align, maxalign, c, offset;
    int bit_size, bit_pos, bsize, bt, lbit_pos;
    Sym *s, *ss, **ps;
    AttributeDef ad;
    CType type1, btype;

    a = tok; /* save decl type */
    next();
    if (tok != '{') {
        v = tok;
        next();
        /* struct already defined ? return it */
        /* XXX: check consistency */
        s = struct_find(v);
        if (s) {
            if (s->type.t != a)
                error("invalid type");
            goto do_decl;
        }
    } else {
        v = anon_sym++;
    }
    type1.t = a;
    s = sym_push(v | SYM_STRUCT, &type1, 0, 0);
    /* put struct/union/enum name in type */
 do_decl:
    type->t = u;
    type->ref = s;
    
    if (tok == '{') {
        next();
        if (s->c)
            error("struct/union/enum already defined");
        /* cannot be empty */
        c = 0;
        /* non empty enums are not allowed */
        if (a == TOK_ENUM) {
            for(;;) {
                v = tok;
                if (v < TOK_UIDENT)
                    expect("identifier");
                next();
                if (tok == '=') {
                    next();
                    c = expr_const();
                }
                /* enum symbols have static storage */
                ss = sym_push(v, &int_type, VT_CONST, c);
                ss->type.t |= VT_STATIC;
                if (tok != ',')
                    break;
                next();
                c++;
                /* NOTE: we accept a trailing comma */
                if (tok == '}')
                    break;
            }
            skip('}');
        } else {
            maxalign = 1;
            ps = &s->next;
            bit_pos = 0;
            offset = 0;
            while (tok != '}') {
                parse_btype(&btype, &ad);
                while (1) {
                    bit_size = -1;
                    v = 0;
                    type1 = btype;
                    if (tok != ':') {
                        type_decl(&type1, &ad, &v, TYPE_DIRECT);
                        if ((type1.t & VT_BTYPE) == VT_FUNC ||
                            (type1.t & (VT_TYPEDEF | VT_STATIC | VT_EXTERN | VT_INLINE)))
                            error("invalid type for '%s'", 
                                  get_tok_str(v, NULL));
                    }
                    if (tok == ':') {
                        next();
                        bit_size = expr_const();
                        /* XXX: handle v = 0 case for messages */
                        if (bit_size < 0)
                            error("negative width in bit-field '%s'", 
                                  get_tok_str(v, NULL));
                        if (v && bit_size == 0)
                            error("zero width for bit-field '%s'", 
                                  get_tok_str(v, NULL));
                    }
                    size = type_size(&type1, &align);
                    lbit_pos = 0;
                    if (bit_size >= 0) {
                        bt = type1.t & VT_BTYPE;
                        if (bt != VT_INT && 
                            bt != VT_BYTE && 
                            bt != VT_SHORT &&
                            bt != VT_ENUM)
                            error("bitfields must have scalar type");
                        bsize = size * 8;
                        if (bit_size > bsize) {
                            error("width of '%s' exceeds its type",
                                  get_tok_str(v, NULL));
                        } else if (bit_size == bsize) {
                            /* no need for bit fields */
                            bit_pos = 0;
                        } else if (bit_size == 0) {
                            /* XXX: what to do if only padding in a
                               structure ? */
                            /* zero size: means to pad */
                            if (bit_pos > 0)
                                bit_pos = bsize;
                        } else {
                            /* we do not have enough room ? */
                            if ((bit_pos + bit_size) > bsize)
                                bit_pos = 0;
                            lbit_pos = bit_pos;
                            /* XXX: handle LSB first */
                            type1.t |= VT_BITFIELD | 
                                (bit_pos << VT_STRUCT_SHIFT) |
                                (bit_size << (VT_STRUCT_SHIFT + 6));
                            bit_pos += bit_size;
                        }
                    } else {
                        bit_pos = 0;
                    }
                    if (v) {
                        /* add new memory data only if starting
                           bit field */
                        if (lbit_pos == 0) {
                            if (a == TOK_STRUCT) {
                                c = (c + align - 1) & -align;
                                offset = c;
                                c += size;
                            } else {
                                offset = 0;
                                if (size > c)
                                    c = size;
                            }
                            if (align > maxalign)
                                maxalign = align;
                        }
#if 0
                        printf("add field %s offset=%d", 
                               get_tok_str(v, NULL), offset);
                        if (type1.t & VT_BITFIELD) {
                            printf(" pos=%d size=%d", 
                                   (type1.t >> VT_STRUCT_SHIFT) & 0x3f,
                                   (type1.t >> (VT_STRUCT_SHIFT + 6)) & 0x3f);
                        }
                        printf("\n");
#endif
                        ss = sym_push(v | SYM_FIELD, &type1, 0, offset);
                        *ps = ss;
                        ps = &ss->next;
                    }
                    if (tok == ';' || tok == TOK_EOF)
                        break;
                    skip(',');
                }
                skip(';');
            }
            skip('}');
            /* store size and alignment */
            s->c = (c + maxalign - 1) & -maxalign; 
            s->r = maxalign;
        }
    }
}

/* return 0 if no type declaration. otherwise, return the basic type
   and skip it. 
 */
static int parse_btype(CType *type, AttributeDef *ad)
{
    int t, u, type_found;
    Sym *s;
    CType type1;

    memset(ad, 0, sizeof(AttributeDef));
    type_found = 0;
    t = 0;
    while(1) {
        switch(tok) {
        case TOK_EXTENSION:
            /* currently, we really ignore extension */
            next();
            continue;

            /* basic types */
        case TOK_CHAR:
            u = VT_BYTE;
        basic_type:
            next();
        basic_type1:
            if ((t & VT_BTYPE) != 0)
                error("too many basic types");
            t |= u;
            break;
        case TOK_VOID:
            u = VT_VOID;
            goto basic_type;
        case TOK_SHORT:
            u = VT_SHORT;
            goto basic_type;
        case TOK_INT:
            next();
            break;
        case TOK_LONG:
            next();
            if ((t & VT_BTYPE) == VT_DOUBLE) {
                t = (t & ~VT_BTYPE) | VT_LDOUBLE;
            } else if ((t & VT_BTYPE) == VT_LONG) {
                t = (t & ~VT_BTYPE) | VT_LLONG;
            } else {
                u = VT_LONG;
                goto basic_type1;
            }
            break;
        case TOK_BOOL:
            u = VT_BOOL;
            goto basic_type;
        case TOK_FLOAT:
            u = VT_FLOAT;
            goto basic_type;
        case TOK_DOUBLE:
            next();
            if ((t & VT_BTYPE) == VT_LONG) {
                t = (t & ~VT_BTYPE) | VT_LDOUBLE;
            } else {
                u = VT_DOUBLE;
                goto basic_type1;
            }
            break;
        case TOK_ENUM:
            struct_decl(&type1, VT_ENUM);
        basic_type2:
            u = type1.t;
            type->ref = type1.ref;
            goto basic_type1;
        case TOK_STRUCT:
        case TOK_UNION:
            struct_decl(&type1, VT_STRUCT);
            goto basic_type2;

            /* type modifiers */
        case TOK_CONST1:
        case TOK_CONST2:
        case TOK_CONST3:
        case TOK_VOLATILE1:
        case TOK_VOLATILE2:
        case TOK_VOLATILE3:
        case TOK_REGISTER:
        case TOK_SIGNED1:
        case TOK_SIGNED2:
        case TOK_SIGNED3:
        case TOK_AUTO:
        case TOK_RESTRICT1:
        case TOK_RESTRICT2:
        case TOK_RESTRICT3:
            next();
            break;
        case TOK_UNSIGNED:
            t |= VT_UNSIGNED;
            next();
            break;

            /* storage */
        case TOK_EXTERN:
            t |= VT_EXTERN;
            next();
            break;
        case TOK_STATIC:
            t |= VT_STATIC;
            next();
            break;
        case TOK_TYPEDEF:
            t |= VT_TYPEDEF;
            next();
            break;
        case TOK_INLINE1:
        case TOK_INLINE2:
        case TOK_INLINE3:
            t |= VT_INLINE;
            next();
            break;

            /* GNUC attribute */
        case TOK_ATTRIBUTE1:
        case TOK_ATTRIBUTE2:
            parse_attribute(ad);
            break;
            /* GNUC typeof */
        case TOK_TYPEOF1:
        case TOK_TYPEOF2:
        case TOK_TYPEOF3:
            next();
            parse_expr_type(&type1);
            goto basic_type2;
        default:
            s = sym_find(tok);
            if (!s || !(s->type.t & VT_TYPEDEF))
                goto the_end;
            t |= (s->type.t & ~VT_TYPEDEF);
            type->ref = s->type.ref;
            next();
            break;
        }
        type_found = 1;
    }
the_end:
    /* long is never used as type */
    if ((t & VT_BTYPE) == VT_LONG)
        t = (t & ~VT_BTYPE) | VT_INT;
    type->t = t;
    return type_found;
}

static void post_type(CType *type, AttributeDef *ad)
{
    int n, l, t1;
    Sym **plast, *s, *first;
    AttributeDef ad1;
    CType pt;

    if (tok == '(') {
        /* function declaration */
        next();
        l = 0;
        first = NULL;
        plast = &first;
        while (tok != ')') {
            /* read param name and compute offset */
            if (l != FUNC_OLD) {
                if (!parse_btype(&pt, &ad1)) {
                    if (l) {
                        error("invalid type");
                    } else {
                        l = FUNC_OLD;
                        goto old_proto;
                    }
                }
                l = FUNC_NEW;
                if ((pt.t & VT_BTYPE) == VT_VOID && tok == ')')
                    break;
                type_decl(&pt, &ad1, &n, TYPE_DIRECT | TYPE_ABSTRACT);
                if ((pt.t & VT_BTYPE) == VT_VOID)
                    error("parameter declared as void");
            } else {
            old_proto:
                n = tok;
                pt.t = VT_INT;
                next();
            }
            /* array must be transformed to pointer according to ANSI C */
            pt.t &= ~VT_ARRAY;
            s = sym_push(n | SYM_FIELD, &pt, 0, 0);
            *plast = s;
            plast = &s->next;
            if (tok == ',') {
                next();
                if (l == FUNC_NEW && tok == TOK_DOTS) {
                    l = FUNC_ELLIPSIS;
                    next();
                    break;
                }
            }
        }
        /* if no parameters, then old type prototype */
        if (l == 0)
            l = FUNC_OLD;
        skip(')');
        t1 = type->t & VT_STORAGE;
        type->t &= ~VT_STORAGE;
        post_type(type, ad);
        /* we push a anonymous symbol which will contain the function prototype */
        s = sym_push(SYM_FIELD, type, ad->func_call, l);
        s->next = first;
        type->t = t1 | VT_FUNC;
        type->ref = s;
    } else if (tok == '[') {
        /* array definition */
        next();
        n = -1;
        if (tok != ']') {
            n = expr_const();
            if (n < 0)
                error("invalid array size");    
        }
        skip(']');
        /* parse next post type */
        t1 = type->t & VT_STORAGE;
        type->t &= ~VT_STORAGE;
        post_type(type, ad);
        
        /* we push a anonymous symbol which will contain the array
           element type */
        s = sym_push(SYM_FIELD, type, 0, n);
        type->t = t1 | VT_ARRAY | VT_PTR;
        type->ref = s;
    }
}

/* Parse a type declaration (except basic type), and return the type
   in 'type'. 'td' is a bitmask indicating which kind of type decl is
   expected. 'type' should contain the basic type. 'ad' is the
   attribute definition of the basic type. It can be modified by
   type_decl(). 
 */
static void type_decl(CType *type, AttributeDef *ad, int *v, int td)
{
    Sym *s;
    CType type1, *type2;

    while (tok == '*') {
        next();
    redo:
        switch(tok) {
        case TOK_CONST1:
        case TOK_CONST2:
        case TOK_CONST3:
        case TOK_VOLATILE1:
        case TOK_VOLATILE2:
        case TOK_VOLATILE3:
        case TOK_RESTRICT1:
        case TOK_RESTRICT2:
        case TOK_RESTRICT3:
            next();
            goto redo;
        }
        mk_pointer(type);
    }
    
    /* XXX: clarify attribute handling */
    if (tok == TOK_ATTRIBUTE1 || tok == TOK_ATTRIBUTE2)
        parse_attribute(ad);

    /* recursive type */
    /* XXX: incorrect if abstract type for functions (e.g. 'int ()') */
    type1.t = 0; /* XXX: same as int */
    if (tok == '(') {
        next();
        /* XXX: this is not correct to modify 'ad' at this point, but
           the syntax is not clear */
        if (tok == TOK_ATTRIBUTE1 || tok == TOK_ATTRIBUTE2)
            parse_attribute(ad);
        type_decl(&type1, ad, v, td);
        skip(')');
    } else {
        /* type identifier */
        if (tok >= TOK_IDENT && (td & TYPE_DIRECT)) {
            *v = tok;
            next();
        } else {
            if (!(td & TYPE_ABSTRACT))
                expect("identifier");
            *v = 0;
        }
    }
    post_type(type, ad);
    if (tok == TOK_ATTRIBUTE1 || tok == TOK_ATTRIBUTE2)
        parse_attribute(ad);
    if (!type1.t)
        return;
    /* append type at the end of type1 */
    type2 = &type1;
    for(;;) {
        s = type2->ref;
        type2 = &s->type;
        if (!type2->t) {
            *type2 = *type;
            break;
        }
    }
    *type = type1;
}

/* compute the lvalue VT_LVAL_xxx needed to match type t. */
static int lvalue_type(int t)
{
    int bt, r;
    r = VT_LVAL;
    bt = t & VT_BTYPE;
    if (bt == VT_BYTE)
        r |= VT_LVAL_BYTE;
    else if (bt == VT_SHORT)
        r |= VT_LVAL_SHORT;
    else
        return r;
    if (t & VT_UNSIGNED)
        r |= VT_LVAL_UNSIGNED;
    return r;
}

/* indirection with full error checking and bound check */
static void indir(void)
{
    if ((vtop->type.t & VT_BTYPE) != VT_PTR)
        expect("pointer");
    if ((vtop->r & VT_LVAL) && !nocode_wanted)
        gv(RC_INT);
    vtop->type = *pointed_type(&vtop->type);
    /* an array is never an lvalue */
    if (!(vtop->type.t & VT_ARRAY)) {
        vtop->r |= lvalue_type(vtop->type.t);
        /* if bound checking, the referenced pointer must be checked */
        if (do_bounds_check) 
            vtop->r |= VT_MUSTBOUND;
    }
}

/* pass a parameter to a function and do type checking and casting */
void gfunc_param_typed(GFuncContext *gf, Sym *func, Sym *arg)
{
    int func_type;
    CType type;

    func_type = func->c;
    if (func_type == FUNC_OLD ||
        (func_type == FUNC_ELLIPSIS && arg == NULL)) {
        /* default casting : only need to convert float to double */
        if ((vtop->type.t & VT_BTYPE) == VT_FLOAT) {
            type.t = VT_DOUBLE;
            gen_cast(&type);
        }
    } else if (arg == NULL) {
        error("too many arguments to function");
    } else {
        gen_assign_cast(&arg->type);
    }
    if (!nocode_wanted) {
        gfunc_param(gf);
    } else {
        vpop();
    }
}

/* parse an expression of the form '(type)' or '(expr)' and return its
   type */
static void parse_expr_type(CType *type)
{
    int n;
    AttributeDef ad;

    skip('(');
    if (parse_btype(type, &ad)) {
        type_decl(type, &ad, &n, TYPE_ABSTRACT);
    } else {
        expr_type(type);
    }
    skip(')');
}

static void vpush_tokc(int t)
{
    CType type;
    type.t = t;
    vsetc(&type, VT_CONST, &tokc);
}

static void unary(void)
{
    int n, t, align, size, r;
    CType type;
    Sym *s;
    GFuncContext gf;
    AttributeDef ad;

    /* XXX: GCC 2.95.3 does not generate a table although it should be
       better here */
 tok_next:
    switch(tok) {
    case TOK_EXTENSION:
        next();
        goto tok_next;
    case TOK_CINT:
    case TOK_CCHAR: 
    case TOK_LCHAR:
        vpushi(tokc.i);
        next();
        break;
    case TOK_CUINT:
        vpush_tokc(VT_INT | VT_UNSIGNED);
        next();
        break;
    case TOK_CLLONG:
        vpush_tokc(VT_LLONG);
        next();
        break;
    case TOK_CULLONG:
        vpush_tokc(VT_LLONG | VT_UNSIGNED);
        next();
        break;
    case TOK_CFLOAT:
        vpush_tokc(VT_FLOAT);
        next();
        break;
    case TOK_CDOUBLE:
        vpush_tokc(VT_DOUBLE);
        next();
        break;
    case TOK_CLDOUBLE:
        vpush_tokc(VT_LDOUBLE);
        next();
        break;
    case TOK___FUNCTION__:
        if (!gnu_ext)
            goto tok_identifier;
        /* fall thru */
    case TOK___FUNC__:
        {
            void *ptr;
            int len;
            /* special function name identifier */
            len = strlen(funcname) + 1;
            /* generate char[len] type */
            type.t = VT_BYTE;
            mk_pointer(&type);
            type.t |= VT_ARRAY;
            type.ref->c = len;
            vpush_ref(&type, data_section, data_section->data_offset, len);
            ptr = section_ptr_add(data_section, len);
            memcpy(ptr, funcname, len);
            next();
        }
        break;
    case TOK_LSTR:
        t = VT_INT;
        goto str_init;
    case TOK_STR:
        /* string parsing */
        t = VT_BYTE;
    str_init:
        type.t = t;
        mk_pointer(&type);
        type.t |= VT_ARRAY;
        memset(&ad, 0, sizeof(AttributeDef));
        decl_initializer_alloc(&type, &ad, VT_CONST, 2, 0, 0);
        break;
    case '(':
        next();
        /* cast ? */
        if (parse_btype(&type, &ad)) {
            type_decl(&type, &ad, &n, TYPE_ABSTRACT);
            skip(')');
            /* check ISOC99 compound literal */
            if (tok == '{') {
                    /* data is allocated locally by default */
                if (global_expr)
                    r = VT_CONST;
                else
                    r = VT_LOCAL;
                /* all except arrays are lvalues */
                if (!(type.t & VT_ARRAY))
                    r |= lvalue_type(type.t);
                memset(&ad, 0, sizeof(AttributeDef));
                decl_initializer_alloc(&type, &ad, r, 1, 0, 0);
            } else {
                unary();
                gen_cast(&type);
            }
        } else if (tok == '{') {
            /* save all registers */
            save_regs(0); 
            /* statement expression : we do not accept break/continue
               inside as GCC does */
            block(NULL, NULL, NULL, NULL, 0, 1);
            skip(')');
        } else {
            gexpr();
            skip(')');
        }
        break;
    case '*':
        next();
        unary();
        indir();
        break;
    case '&':
        next();
        unary();
        /* functions names must be treated as function pointers,
           except for unary '&' and sizeof. Since we consider that
           functions are not lvalues, we only have to handle it
           there and in function calls. */
        /* arrays can also be used although they are not lvalues */
        if ((vtop->type.t & VT_BTYPE) != VT_FUNC &&
            !(vtop->type.t & VT_ARRAY))
            test_lvalue();
        mk_pointer(&vtop->type);
        gaddrof();
        break;
    case '!':
        next();
        unary();
        if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST)
            vtop->c.i = !vtop->c.i;
        else if ((vtop->r & VT_VALMASK) == VT_CMP)
            vtop->c.i = vtop->c.i ^ 1;
        else
            vseti(VT_JMP, gtst(1, 0));
        break;
    case '~':
        next();
        unary();
        vpushi(-1);
        gen_op('^');
        break;
    case '+':
        next();
        /* in order to force cast, we add zero */
        unary();
        if ((vtop->type.t & VT_BTYPE) == VT_PTR)
            error("pointer not accepted for unary plus");
        vpushi(0);
        gen_op('+');
        break;
    case TOK_SIZEOF:
    case TOK_ALIGNOF1:
    case TOK_ALIGNOF2:
        t = tok;
        next();
        if (tok == '(') {
            parse_expr_type(&type);
        } else {
            unary_type(&type);
        }
        size = type_size(&type, &align);
        if (t == TOK_SIZEOF)
            vpushi(size);
        else
            vpushi(align);
        break;
        
    case TOK_INC:
    case TOK_DEC:
        t = tok;
        next();
        unary();
        inc(0, t);
        break;
    case '-':
        next();
        vpushi(0);
        unary();
        gen_op('-');
        break;
    case TOK_LAND:
        if (!gnu_ext)
            goto tok_identifier;
        next();
        /* allow to take the address of a label */
        if (tok < TOK_UIDENT)
            expect("label identifier");
        s = label_find(tok);
        if (!s) {
            s = label_push(&global_label_stack, tok, LABEL_FORWARD);
        } else {
            if (s->r == LABEL_DECLARED)
                s->r = LABEL_FORWARD;
        }
        if (!s->type.t) {
            s->type.t = VT_VOID;
            mk_pointer(&s->type);
            s->type.t |= VT_STATIC;
        }
        vset(&s->type, VT_CONST | VT_SYM, 0);
        vtop->sym = s;
        next();
        break;
    default:
    tok_identifier:
        t = tok;
        next();
        if (t < TOK_UIDENT)
            expect("identifier");
        s = sym_find(t);
        if (!s) {
            if (tok != '(')
                error("'%s' undeclared", get_tok_str(t, NULL));
            /* for simple function calls, we tolerate undeclared
               external reference to int() function */
            s = external_global_sym(t, &func_old_type, 0); 
        }
        vset(&s->type, s->r, s->c);
        /* if forward reference, we must point to s */
        if (vtop->r & VT_SYM) {
            vtop->sym = s;
            vtop->c.ul = 0;
        }
        break;
    }
    
    /* post operations */
    while (1) {
        if (tok == TOK_INC || tok == TOK_DEC) {
            inc(1, tok);
            next();
        } else if (tok == '.' || tok == TOK_ARROW) {
            /* field */ 
            if (tok == TOK_ARROW) 
                indir();
            test_lvalue();
            gaddrof();
            next();
            /* expect pointer on structure */
            if ((vtop->type.t & VT_BTYPE) != VT_STRUCT)
                expect("struct or union");
            s = vtop->type.ref;
            /* find field */
            tok |= SYM_FIELD;
            while ((s = s->next) != NULL) {
                if (s->v == tok)
                    break;
            }
            if (!s)
                error("field not found");
            /* add field offset to pointer */
            vtop->type = char_pointer_type; /* change type to 'char *' */
            vpushi(s->c);
            gen_op('+');
            /* change type to field type, and set to lvalue */
            vtop->type = s->type;
            /* an array is never an lvalue */
            if (!(vtop->type.t & VT_ARRAY)) {
                vtop->r |= lvalue_type(vtop->type.t);
                /* if bound checking, the referenced pointer must be checked */
                if (do_bounds_check) 
                    vtop->r |= VT_MUSTBOUND;
            }
            next();
        } else if (tok == '[') {
            next();
            gexpr();
            gen_op('+');
            indir();
            skip(']');
        } else if (tok == '(') {
            SValue ret;
            Sym *sa;

            /* function call  */
            if ((vtop->type.t & VT_BTYPE) != VT_FUNC) {
                /* pointer test (no array accepted) */
                if ((vtop->type.t & (VT_BTYPE | VT_ARRAY)) == VT_PTR) {
                    vtop->type = *pointed_type(&vtop->type);
                    if ((vtop->type.t & VT_BTYPE) != VT_FUNC)
                        goto error_func;
                } else {
                error_func:
                    expect("function pointer");
                }
            } else {
                vtop->r &= ~VT_LVAL; /* no lvalue */
            }
            /* get return type */
            s = vtop->type.ref;
            if (!nocode_wanted) {
                save_regs(0); /* save used temporary registers */
                gfunc_start(&gf, s->r);
            }
            next();
            sa = s->next; /* first parameter */
#ifdef INVERT_FUNC_PARAMS
            {
                int parlevel;
                Sym *args, *s1;
                ParseState saved_parse_state;
                TokenString str;
                
                /* read each argument and store it on a stack */
                args = NULL;
                if (tok != ')') {
                    for(;;) {
                        tok_str_new(&str);
                        parlevel = 0;
                        while ((parlevel > 0 || (tok != ')' && tok != ',')) && 
                               tok != TOK_EOF) {
                            if (tok == '(')
                                parlevel++;
                            else if (tok == ')')
                                parlevel--;
                            tok_str_add_tok(&str);
                            next();
                        }
                        tok_str_add(&str, -1); /* end of file added */
                        tok_str_add(&str, 0);
                        s1 = sym_push2(&args, 0, 0, (int)str.str);
                        s1->next = sa; /* add reference to argument */
                        if (sa)
                            sa = sa->next;
                        if (tok == ')')
                            break;
                        skip(',');
                    }
                }
                
                /* now generate code in reverse order by reading the stack */
                save_parse_state(&saved_parse_state);
                while (args) {
                    macro_ptr = (int *)args->c;
                    next();
                    expr_eq();
                    if (tok != -1)
                        expect("',' or ')'");
                    gfunc_param_typed(&gf, s, args->next);
                    s1 = args->prev;
                    tok_str_free((int *)args->c);
                    tcc_free(args);
                    args = s1;
                }
                restore_parse_state(&saved_parse_state);
            }
#endif
            /* compute first implicit argument if a structure is returned */
            if ((s->type.t & VT_BTYPE) == VT_STRUCT) {
                /* get some space for the returned structure */
                size = type_size(&s->type, &align);
                loc = (loc - size) & -align;
                ret.type = s->type;
                ret.r = VT_LOCAL | VT_LVAL;
                /* pass it as 'int' to avoid structure arg passing
                   problems */
                vseti(VT_LOCAL, loc);
                ret.c = vtop->c;
                if (!nocode_wanted)
                    gfunc_param(&gf);
                else
                    vtop--;
            } else {
                ret.type = s->type; 
                ret.r2 = VT_CONST;
                /* return in register */
                if (is_float(ret.type.t)) {
                    ret.r = REG_FRET; 
                } else {
                    if ((ret.type.t & VT_BTYPE) == VT_LLONG)
                        ret.r2 = REG_LRET;
                    ret.r = REG_IRET;
                }
                ret.c.i = 0;
            }
#ifndef INVERT_FUNC_PARAMS
            if (tok != ')') {
                for(;;) {
                    expr_eq();
                    gfunc_param_typed(&gf, s, sa);
                    if (sa)
                        sa = sa->next;
                    if (tok == ')')
                        break;
                    skip(',');
                }
            }
#endif
            if (sa)
                error("too few arguments to function");
            skip(')');
            if (!nocode_wanted)
                gfunc_call(&gf);
            else
                vtop--;
            /* return value */
            vsetc(&ret.type, ret.r, &ret.c);
            vtop->r2 = ret.r2;
        } else {
            break;
        }
    }
}

static void uneq(void)
{
    int t;
    
    unary();
    if (tok == '=' ||
        (tok >= TOK_A_MOD && tok <= TOK_A_DIV) ||
        tok == TOK_A_XOR || tok == TOK_A_OR ||
        tok == TOK_A_SHL || tok == TOK_A_SAR) {
        test_lvalue();
        t = tok;
        next();
        if (t == '=') {
            expr_eq();
        } else {
            vdup();
            expr_eq();
            gen_op(t & 0x7f);
        }
        vstore();
    }
}

static void expr_prod(void)
{
    int t;

    uneq();
    while (tok == '*' || tok == '/' || tok == '%') {
        t = tok;
        next();
        uneq();
        gen_op(t);
    }
}

static void expr_sum(void)
{
    int t;

    expr_prod();
    while (tok == '+' || tok == '-') {
        t = tok;
        next();
        expr_prod();
        gen_op(t);
    }
}

static void expr_shift(void)
{
    int t;

    expr_sum();
    while (tok == TOK_SHL || tok == TOK_SAR) {
        t = tok;
        next();
        expr_sum();
        gen_op(t);
    }
}

static void expr_cmp(void)
{
    int t;

    expr_shift();
    while ((tok >= TOK_ULE && tok <= TOK_GT) ||
           tok == TOK_ULT || tok == TOK_UGE) {
        t = tok;
        next();
        expr_shift();
        gen_op(t);
    }
}

static void expr_cmpeq(void)
{
    int t;

    expr_cmp();
    while (tok == TOK_EQ || tok == TOK_NE) {
        t = tok;
        next();
        expr_cmp();
        gen_op(t);
    }
}

static void expr_and(void)
{
    expr_cmpeq();
    while (tok == '&') {
        next();
        expr_cmpeq();
        gen_op('&');
    }
}

static void expr_xor(void)
{
    expr_and();
    while (tok == '^') {
        next();
        expr_and();
        gen_op('^');
    }
}

static void expr_or(void)
{
    expr_xor();
    while (tok == '|') {
        next();
        expr_xor();
        gen_op('|');
    }
}

/* XXX: suppress this mess */
static void expr_land_const(void)
{
    expr_or();
    while (tok == TOK_LAND) {
        next();
        expr_or();
        gen_op(TOK_LAND);
    }
}

/* XXX: suppress this mess */
static void expr_lor_const(void)
{
    expr_land_const();
    while (tok == TOK_LOR) {
        next();
        expr_land_const();
        gen_op(TOK_LOR);
    }
}

/* only used if non constant */
static void expr_land(void)
{
    int t;

    expr_or();
    if (tok == TOK_LAND) {
        t = 0;
        for(;;) {
            t = gtst(1, t);
            if (tok != TOK_LAND) {
                vseti(VT_JMPI, t);
                break;
            }
            next();
            expr_or();
        }
    }
}

static void expr_lor(void)
{
    int t;

    expr_land();
    if (tok == TOK_LOR) {
        t = 0;
        for(;;) {
            t = gtst(0, t);
            if (tok != TOK_LOR) {
                vseti(VT_JMP, t);
                break;
            }
            next();
            expr_land();
        }
    }
}

/* XXX: better constant handling */
static void expr_eq(void)
{
    int tt, u, r1, r2, rc, t1, t2, bt1, bt2;
    SValue sv;
    CType type, type1, type2;

    if (const_wanted) {
        int c1, c;
        expr_lor_const();
        if (tok == '?') {
            c = vtop->c.i;
            vpop();
            next();
            if (tok == ':' && gnu_ext) {
                c1 = c;
            } else {
                gexpr();
                c1 = vtop->c.i;
                vpop();
            }
            skip(':');
            expr_eq();
            if (c)
                vtop->c.i = c1;
        }
    } else {
        expr_lor();
        if (tok == '?') {
            next();
            if (vtop != vstack) {
                /* needed to avoid having different registers saved in
                   each branch */
                if (is_float(vtop->type.t))
                    rc = RC_FLOAT;
                else
                    rc = RC_INT;
                    gv(rc);
                    save_regs(1);
            }
            if (tok == ':' && gnu_ext) {
                gv_dup();
                tt = gtst(1, 0);
            } else {
                tt = gtst(1, 0);
                gexpr();
            }
            type1 = vtop->type;
            sv = *vtop; /* save value to handle it later */
            vtop--; /* no vpop so that FP stack is not flushed */
            skip(':');
            u = gjmp(0);
            gsym(tt);
            expr_eq();
            type2 = vtop->type;

            t1 = type1.t;
            bt1 = t1 & VT_BTYPE;
            t2 = type2.t;
            bt2 = t2 & VT_BTYPE;
            /* cast operands to correct type according to ISOC rules */
            if (is_float(bt1) || is_float(bt2)) {
                if (bt1 == VT_LDOUBLE || bt2 == VT_LDOUBLE) {
                    type.t = VT_LDOUBLE;
                } else if (bt1 == VT_DOUBLE || bt2 == VT_DOUBLE) {
                    type.t = VT_DOUBLE;
                } else {
                    type.t = VT_FLOAT;
                }
            } else if (bt1 == VT_LLONG || bt2 == VT_LLONG) {
                /* cast to biggest op */
                type.t = VT_LLONG;
                /* convert to unsigned if it does not fit in a long long */
                if ((t1 & (VT_BTYPE | VT_UNSIGNED)) == (VT_LLONG | VT_UNSIGNED) ||
                    (t2 & (VT_BTYPE | VT_UNSIGNED)) == (VT_LLONG | VT_UNSIGNED))
                    type.t |= VT_UNSIGNED;
            } else if (bt1 == VT_PTR || bt2 == VT_PTR) {
                /* XXX: test pointer compatibility */
                type = type1;
            } else if (bt1 == VT_STRUCT || bt2 == VT_STRUCT) {
                /* XXX: test structure compatibility */
                type = type1;
            } else if (bt1 == VT_VOID || bt2 == VT_VOID) {
                /* NOTE: as an extension, we accept void on only one side */
                type.t = VT_VOID;
            } else {
                /* integer operations */
                type.t = VT_INT;
                /* convert to unsigned if it does not fit in an integer */
                if ((t1 & (VT_BTYPE | VT_UNSIGNED)) == (VT_INT | VT_UNSIGNED) ||
                    (t2 & (VT_BTYPE | VT_UNSIGNED)) == (VT_INT | VT_UNSIGNED))
                    type.t |= VT_UNSIGNED;
            }
                
            /* now we convert second operand */
            gen_cast(&type);
            rc = RC_INT;
            if (is_float(type.t)) {
                rc = RC_FLOAT;
            } else if ((type.t & VT_BTYPE) == VT_LLONG) {
                /* for long longs, we use fixed registers to avoid having
                   to handle a complicated move */
                rc = RC_IRET; 
            }
            
            r2 = gv(rc);
            /* this is horrible, but we must also convert first
               operand */
            tt = gjmp(0);
            gsym(u);
            /* put again first value and cast it */
            *vtop = sv;
            gen_cast(&type);
            r1 = gv(rc);
            move_reg(r2, r1);
            vtop->r = r2;
            gsym(tt);
        }
    }
}

static void gexpr(void)
{
    while (1) {
        expr_eq();
        if (tok != ',')
            break;
        vpop();
        next();
    }
}

/* parse an expression and return its type without any side effect. */
static void expr_type(CType *type)
{
    int a;

    a = nocode_wanted;
    nocode_wanted = 1;
    gexpr();
    *type = vtop->type;
    vpop();
    nocode_wanted = a;
}

/* parse a unary expression and return its type without any side
   effect. */
static void unary_type(CType *type)
{
    int a;

    a = nocode_wanted;
    nocode_wanted = 1;
    unary();
    *type = vtop->type;
    vpop();
    nocode_wanted = a;
}

/* parse a constant expression and return value in vtop.  */
static void expr_const1(void)
{
    int a;
    a = const_wanted;
    const_wanted = 1;
    expr_eq();
    const_wanted = a;
}

/* parse an integer constant and return its value. */
static int expr_const(void)
{
    int c;
    expr_const1();
    if ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) != VT_CONST)
        expect("constant expression");
    c = vtop->c.i;
    vpop();
    return c;
}

/* return the label token if current token is a label, otherwise
   return zero */
static int is_label(void)
{
    int last_tok;

    /* fast test first */
    if (tok < TOK_UIDENT)
        return 0;
    /* no need to save tokc because tok is an identifier */
    last_tok = tok;
    next();
    if (tok == ':') {
        next();
        return last_tok;
    } else {
        unget_tok(last_tok);
        return 0;
    }
}

static void block(int *bsym, int *csym, int *case_sym, int *def_sym, 
                  int case_reg, int is_expr)
{
    int a, b, c, d;
    Sym *s;

    /* generate line number info */
    if (do_debug && 
        (last_line_num != file->line_num || last_ind != ind)) {
        put_stabn(N_SLINE, 0, file->line_num, ind - func_ind);
        last_ind = ind;
        last_line_num = file->line_num;
    }

    if (is_expr) {
        /* default return value is (void) */
        vpushi(0);
        vtop->type.t = VT_VOID;
    }

    if (tok == TOK_IF) {
        /* if test */
        next();
        skip('(');
        gexpr();
        skip(')');
        a = gtst(1, 0);
        block(bsym, csym, case_sym, def_sym, case_reg, 0);
        c = tok;
        if (c == TOK_ELSE) {
            next();
            d = gjmp(0);
            gsym(a);
            block(bsym, csym, case_sym, def_sym, case_reg, 0);
            gsym(d); /* patch else jmp */
        } else
            gsym(a);
    } else if (tok == TOK_WHILE) {
        next();
        d = ind;
        skip('(');
        gexpr();
        skip(')');
        a = gtst(1, 0);
        b = 0;
        block(&a, &b, case_sym, def_sym, case_reg, 0);
        gjmp_addr(d);
        gsym(a);
        gsym_addr(b, d);
    } else if (tok == '{') {
        Sym *llabel;
        
        next();
        /* record local declaration stack position */
        s = local_stack;
        llabel = local_label_stack;
        /* handle local labels declarations */
        if (tok == TOK_LABEL) {
            next();
            for(;;) {
                if (tok < TOK_UIDENT)
                    expect("label identifier");
                label_push(&local_label_stack, tok, LABEL_DECLARED);
                next();
                if (tok == ',') {
                    next();
                } else {
                    skip(';');
                    break;
                }
            }
        }
        while (tok != '}') {
            decl(VT_LOCAL);
            if (tok != '}') {
                if (is_expr)
                    vpop();
                block(bsym, csym, case_sym, def_sym, case_reg, is_expr);
            }
        }
        /* pop locally defined labels */
        label_pop(&local_label_stack, llabel);
        /* pop locally defined symbols */
        sym_pop(&local_stack, s);
        next();
    } else if (tok == TOK_RETURN) {
        next();
        if (tok != ';') {
            gexpr();
            gen_assign_cast(&func_vt);
            if ((func_vt.t & VT_BTYPE) == VT_STRUCT) {
                CType type;
                /* if returning structure, must copy it to implicit
                   first pointer arg location */
                type = func_vt;
                mk_pointer(&type);
                vset(&type, VT_LOCAL | VT_LVAL, func_vc);
                indir();
                vswap();
                /* copy structure value to pointer */
                vstore();
            } else if (is_float(func_vt.t)) {
                gv(RC_FRET);
            } else {
                gv(RC_IRET);
            }
            vtop--; /* NOT vpop() because on x86 it would flush the fp stack */
        }
        skip(';');
        rsym = gjmp(rsym); /* jmp */
    } else if (tok == TOK_BREAK) {
        /* compute jump */
        if (!bsym)
            error("cannot break");
        *bsym = gjmp(*bsym);
        next();
        skip(';');
    } else if (tok == TOK_CONTINUE) {
        /* compute jump */
        if (!csym)
            error("cannot continue");
        *csym = gjmp(*csym);
        next();
        skip(';');
    } else if (tok == TOK_FOR) {
        int e;
        next();
        skip('(');
        if (tok != ';') {
            gexpr();
            vpop();
        }
        skip(';');
        d = ind;
        c = ind;
        a = 0;
        b = 0;
        if (tok != ';') {
            gexpr();
            a = gtst(1, 0);
        }
        skip(';');
        if (tok != ')') {
            e = gjmp(0);
            c = ind;
            gexpr();
            vpop();
            gjmp_addr(d);
            gsym(e);
        }
        skip(')');
        block(&a, &b, case_sym, def_sym, case_reg, 0);
        gjmp_addr(c);
        gsym(a);
        gsym_addr(b, c);
    } else 
    if (tok == TOK_DO) {
        next();
        a = 0;
        b = 0;
        d = ind;
        block(&a, &b, case_sym, def_sym, case_reg, 0);
        skip(TOK_WHILE);
        skip('(');
        gsym(b);
        gexpr();
        c = gtst(0, 0);
        gsym_addr(c, d);
        skip(')');
        gsym(a);
        skip(';');
    } else
    if (tok == TOK_SWITCH) {
        next();
        skip('(');
        gexpr();
        /* XXX: other types than integer */
        case_reg = gv(RC_INT);
        vpop();
        skip(')');
        a = 0;
        b = gjmp(0); /* jump to first case */
        c = 0;
        block(&a, csym, &b, &c, case_reg, 0);
        /* if no default, jmp after switch */
        if (c == 0)
            c = ind;
        /* default label */
        gsym_addr(b, c);
        /* break label */
        gsym(a);
    } else
    if (tok == TOK_CASE) {
        int v1, v2;
        if (!case_sym)
            expect("switch");
        next();
        v1 = expr_const();
        v2 = v1;
        if (gnu_ext && tok == TOK_DOTS) {
            next();
            v2 = expr_const();
            if (v2 < v1)
                warning("empty case range");
        }
        /* since a case is like a label, we must skip it with a jmp */
        b = gjmp(0);
        gsym(*case_sym);
        vseti(case_reg, 0);
        vpushi(v1);
        if (v1 == v2) {
            gen_op(TOK_EQ);
            *case_sym = gtst(1, 0);
        } else {
            gen_op(TOK_GE);
            *case_sym = gtst(1, 0);
            vseti(case_reg, 0);
            vpushi(v2);
            gen_op(TOK_LE);
            *case_sym = gtst(1, *case_sym);
        }
        gsym(b);
        skip(':');
        block(bsym, csym, case_sym, def_sym, case_reg, 0);
    } else 
    if (tok == TOK_DEFAULT) {
        next();
        skip(':');
        if (!def_sym)
            expect("switch");
        if (*def_sym)
            error("too many 'default'");
        *def_sym = ind;
        block(bsym, csym, case_sym, def_sym, case_reg, 0);
    } else
    if (tok == TOK_GOTO) {
        next();
        if (tok == '*' && gnu_ext) {
            /* computed goto */
            next();
            gexpr();
            if ((vtop->type.t & VT_BTYPE) != VT_PTR)
                expect("pointer");
            ggoto();
        } else if (tok >= TOK_UIDENT) {
            s = label_find(tok);
            /* put forward definition if needed */
            if (!s) {
                s = label_push(&global_label_stack, tok, LABEL_FORWARD);
            } else {
                if (s->r == LABEL_DECLARED)
                    s->r = LABEL_FORWARD;
            }
            /* label already defined */
            if (s->r & LABEL_FORWARD) 
                s->next = (Sym *)INT2PTR(gjmp(PTR2INT(s->next)));
            else
                gjmp_addr(PTR2INT(s->next));
            next();
        } else {
            expect("label identifier");
        }
        skip(';');
    } else if (tok == TOK_ASM1 || tok == TOK_ASM2 || tok == TOK_ASM3) {
        asm_instr();
    } else {
        b = is_label();
        if (b) {
            /* label case */
            s = label_find(b);
            if (s) {
                if (s->r == LABEL_DEFINED)
                    error("duplicate label '%s'", get_tok_str(s->v, NULL));
                gsym((long)s->next);
                s->r = LABEL_DEFINED;
            } else {
                s = label_push(&global_label_stack, b, LABEL_DEFINED);
            }
            s->next = (Sym *)INT2PTR(ind);
            /* we accept this, but it is a mistake */
            if (tok == '}') {
                warning("deprecated use of label at end of compound statement");
            } else {
                if (is_expr)
                    vpop();
                block(bsym, csym, case_sym, def_sym, case_reg, is_expr);
            }
        } else {
            /* expression case */
            if (tok != ';') {
                if (is_expr) {
                    vpop();
                    gexpr();
                } else {
                    gexpr();
                    vpop();
                }
            }
            skip(';');
        }
    }
}

/* t is the array or struct type. c is the array or struct
   address. cur_index/cur_field is the pointer to the current
   value. 'size_only' is true if only size info is needed (only used
   in arrays) */
static void decl_designator(CType *type, Section *sec, unsigned long c, 
                            int *cur_index, Sym **cur_field, 
                            int size_only)
{
    Sym *s, *f;
    int notfirst, index, index_last, align, l, nb_elems, elem_size;
    CType type1;

    notfirst = 0;
    elem_size = 0;
    nb_elems = 1;
    if (gnu_ext && (l = is_label()) != 0)
        goto struct_field;
    while (tok == '[' || tok == '.') {
        if (tok == '[') {
            if (!(type->t & VT_ARRAY))
                expect("array type");
            s = type->ref;
            next();
            index = expr_const();
            if (index < 0 || (s->c >= 0 && index >= s->c))
                expect("invalid index");
            if (tok == TOK_DOTS && gnu_ext) {
                next();
                index_last = expr_const();
                if (index_last < 0 || 
                    (s->c >= 0 && index_last >= s->c) ||
                    index_last < index)
                    expect("invalid index");
            } else {
                index_last = index;
            }
            skip(']');
            if (!notfirst)
                *cur_index = index_last;
            type = pointed_type(type);
            elem_size = type_size(type, &align);
            c += index * elem_size;
            /* NOTE: we only support ranges for last designator */
            nb_elems = index_last - index + 1;
            if (nb_elems != 1) {
                notfirst = 1;
                break;
            }
        } else {
            next();
            l = tok;
            next();
        struct_field:
            if ((type->t & VT_BTYPE) != VT_STRUCT)
                expect("struct/union type");
            s = type->ref;
            l |= SYM_FIELD;
            f = s->next;
            while (f) {
                if (f->v == l)
                    break;
                f = f->next;
            }
            if (!f)
                expect("field");
            if (!notfirst)
                *cur_field = f;
            /* XXX: suppress this mess by using explicit storage field */
            type1 = f->type;
            type1.t |= (type->t & ~VT_TYPE);
            type = &type1;
            c += f->c;
        }
        notfirst = 1;
    }
    if (notfirst) {
        if (tok == '=') {
            next();
        } else {
            if (!gnu_ext)
                expect("=");
        }
    } else {
        if (type->t & VT_ARRAY) {
            index = *cur_index;
            type = pointed_type(type);
            c += index * type_size(type, &align);
        } else {
            f = *cur_field;
            if (!f)
                error("too many field init");
            /* XXX: suppress this mess by using explicit storage field */
            type1 = f->type;
            type1.t |= (type->t & ~VT_TYPE);
            type = &type1;
            c += f->c;
        }
    }
    decl_initializer(type, sec, c, 0, size_only);

    /* XXX: make it more general */
    if (!size_only && nb_elems > 1) {
        unsigned long c_end;
        uint8_t *src, *dst;
        int i;

        if (!sec)
            error("range init not supported yet for dynamic storage");
        c_end = c + nb_elems * elem_size;
        if (c_end > sec->data_allocated)
            section_realloc(sec, c_end);
        src = sec->data + c;
        dst = src;
        for(i = 1; i < nb_elems; i++) {
            dst += elem_size;
            memcpy(dst, src, elem_size);
        }
    }
}

#define EXPR_VAL   0
#define EXPR_CONST 1
#define EXPR_ANY   2

/* store a value or an expression directly in global data or in local array */
static void init_putv(CType *type, Section *sec, unsigned long c, 
                      int v, int expr_type)
{
    int saved_global_expr, bt, bit_pos, bit_size;
    void *ptr;
    __uint64 bit_mask;

    switch(expr_type) {
    case EXPR_VAL:
        vpushi(v);
        break;
    case EXPR_CONST:
        /* compound literals must be allocated globally in this case */
        saved_global_expr = global_expr;
        global_expr = 1;
        expr_const1();
        global_expr = saved_global_expr;
        /* NOTE: symbols are accepted */
        if ((vtop->r & (VT_VALMASK | VT_LVAL)) != VT_CONST)
            error("initializer element is not constant");
        break;
    case EXPR_ANY:
        expr_eq();
        break;
    }
    
    if (sec) {
        /* XXX: not portable */
        /* XXX: generate error if incorrect relocation */
        gen_assign_cast(type);
        bt = type->t & VT_BTYPE;
        ptr = sec->data + c;
        /* XXX: make code faster ? */
        if (!(type->t & VT_BITFIELD)) {
            bit_pos = 0;
            bit_size = 32;
            bit_mask = -1LL;
        } else {
            bit_pos = (vtop->type.t >> VT_STRUCT_SHIFT) & 0x3f;
            bit_size = (vtop->type.t >> (VT_STRUCT_SHIFT + 6)) & 0x3f;
            bit_mask = (1LL << bit_size) - 1;
        }
        if ((vtop->r & VT_SYM) &&
            (bt == VT_BYTE ||
             bt == VT_SHORT ||
             bt == VT_DOUBLE ||
             bt == VT_LDOUBLE ||
             bt == VT_LLONG ||
             (bt == VT_INT && bit_size != 32)))
            error("initializer element is not computable at load time");
        switch(bt) {
        case VT_BYTE:
            *(char *)ptr |= (vtop->c.i & bit_mask) << bit_pos;
            break;
        case VT_SHORT:
            *(short *)ptr |= (vtop->c.i & bit_mask) << bit_pos;
            break;
        case VT_DOUBLE:
            *(double *)ptr = vtop->c.d;
            break;
        case VT_LDOUBLE:
            *(long double *)ptr = vtop->c.ld;
            break;
        case VT_LLONG:
            *(__int64 *)ptr |= (vtop->c.ll & bit_mask) << bit_pos;
            break;
        default:
            if (vtop->r & VT_SYM) {
                greloc(sec, vtop->sym, c, R_DATA_32);
            }
            *(int *)ptr |= (vtop->c.i & bit_mask) << bit_pos;
            break;
        }
        vtop--;
    } else {
        vset(type, VT_LOCAL, c);
        vswap();
        vstore();
        vpop();
    }
}

/* put zeros for variable based init */
static void init_putz(CType *t, Section *sec, unsigned long c, int size)
{
    GFuncContext gf;

    if (sec) {
        /* nothing to do because globals are already set to zero */
    } else {
		gfunc_start(&gf, FUNC_CDECL);
        vpushi(size);
        gfunc_param(&gf);
        vpushi(0);
        gfunc_param(&gf);
        vseti(VT_LOCAL, c);
        gfunc_param(&gf);
        vpush_global_sym(&func_old_type, TOK_memset);
        gfunc_call(&gf);
    }
}

/* 't' contains the type and storage info. 'c' is the offset of the
   object in section 'sec'. If 'sec' is NULL, it means stack based
   allocation. 'first' is true if array '{' must be read (multi
   dimension implicit array init handling). 'size_only' is true if
   size only evaluation is wanted (only for arrays). */
static void decl_initializer(CType *type, Section *sec, unsigned long c, 
                             int first, int size_only)
{
    int index, array_length, n, no_oblock, nb, parlevel, i;
    int size1, align1, expr_type;
    Sym *s, *f;
    CType *t1;

    if (type->t & VT_ARRAY) {
        s = type->ref;
        n = s->c;
        array_length = 0;
        t1 = pointed_type(type);
        size1 = type_size(t1, &align1);

        no_oblock = 1;
        if ((first && tok != TOK_LSTR && tok != TOK_STR) || 
            tok == '{') {
            skip('{');
            no_oblock = 0;
        }

        /* only parse strings here if correct type (otherwise: handle
           them as ((w)char *) expressions */
        if ((tok == TOK_LSTR && 
             (t1->t & VT_BTYPE) == VT_INT) ||
            (tok == TOK_STR &&
             (t1->t & VT_BTYPE) == VT_BYTE)) {
            while (tok == TOK_STR || tok == TOK_LSTR) {
                int cstr_len, ch;
                CTString *cstr;

                cstr = tokc.cstr;
                /* compute maximum number of chars wanted */
                if (tok == TOK_STR)
                    cstr_len = cstr->size;
                else
                    cstr_len = cstr->size / sizeof(int);
                cstr_len--;
                nb = cstr_len;
                if (n >= 0 && nb > (n - array_length))
                    nb = n - array_length;
                if (!size_only) {
                    if (cstr_len > nb)
                        warning("initializer-string for array is too long");
                    /* in order to go faster for common case (char
                       string in global variable, we handle it
                       specifically */
                    if (sec && tok == TOK_STR && size1 == 1) {
                        memcpy(sec->data + c + array_length, cstr->data, nb);
                    } else {
                        for(i=0;i<nb;i++) {
                            if (tok == TOK_STR)
                                ch = ((unsigned char *)cstr->data)[i];
                            else
                                ch = ((int *)cstr->data)[i];
                            init_putv(t1, sec, c + (array_length + i) * size1,
                                      ch, EXPR_VAL);
                        }
                    }
                }
                array_length += nb;
                next();
            }
            /* only add trailing zero if enough storage (no
               warning in this case since it is standard) */
            if (n < 0 || array_length < n) {
                if (!size_only) {
                    init_putv(t1, sec, c + (array_length * size1), 0, EXPR_VAL);
                }
                array_length++;
            }
        } else {
            index = 0;
            while (tok != '}') {
                decl_designator(type, sec, c, &index, NULL, size_only);
                if (n >= 0 && index >= n)
                    error("index too large");
                /* must put zero in holes (note that doing it that way
                   ensures that it even works with designators) */
                if (!size_only && array_length < index) {
                    init_putz(t1, sec, c + array_length * size1, 
                              (index - array_length) * size1);
                }
                index++;
                if (index > array_length)
                    array_length = index;
                /* special test for multi dimensional arrays (may not
                   be strictly correct if designators are used at the
                   same time) */
                if (index >= n && no_oblock)
                    break;
                if (tok == '}')
                    break;
                skip(',');
            }
        }
        if (!no_oblock)
            skip('}');
        /* put zeros at the end */
        if (!size_only && n >= 0 && array_length < n) {
            init_putz(t1, sec, c + array_length * size1, 
                      (n - array_length) * size1);
        }
        /* patch type size if needed */
        if (n < 0)
            s->c = array_length;
    } else if ((type->t & VT_BTYPE) == VT_STRUCT &&
               (sec || !first || tok == '{')) {
        int par_count;

        /* NOTE: the previous test is a specific case for automatic
           struct/union init */
        /* XXX: union needs only one init */

        /* XXX: this test is incorrect for local initializers
           beginning with ( without {. It would be much more difficult
           to do it correctly (ideally, the expression parser should
           be used in all cases) */
        par_count = 0;
        if (tok == '(') {
            AttributeDef ad1;
            CType type1;
            next();
            while (tok == '(') {
                par_count++;
                next();
            }
            if (!parse_btype(&type1, &ad1))
                expect("cast");
            type_decl(&type1, &ad1, &n, TYPE_ABSTRACT);
            if (!is_compatible_types(type, &type1))
                error("invalid type for cast");
            skip(')');
        }
        no_oblock = 1;
        if (first || tok == '{') {
            skip('{');
            no_oblock = 0;
        }
        s = type->ref;
        f = s->next;
        array_length = 0;
        index = 0;
        n = s->c;
        while (tok != '}') {
            decl_designator(type, sec, c, NULL, &f, size_only);
            index = f->c;
            if (!size_only && array_length < index) {
                init_putz(type, sec, c + array_length, 
                          index - array_length);
            }
            index = index + type_size(&f->type, &align1);
            if (index > array_length)
                array_length = index;
            f = f->next;
            if (no_oblock && f == NULL)
                break;
            if (tok == '}')
                break;
            skip(',');
        }
        /* put zeros at the end */
        if (!size_only && array_length < n) {
            init_putz(type, sec, c + array_length, 
                      n - array_length);
        }
        if (!no_oblock)
            skip('}');
        while (par_count) {
            skip(')');
            par_count--;
        }
    } else if (tok == '{') {
        next();
        decl_initializer(type, sec, c, first, size_only);
        skip('}');
    } else if (size_only) {
        /* just skip expression */
        parlevel = 0;
        while ((parlevel > 0 || (tok != '}' && tok != ',')) && 
               tok != -1) {
            if (tok == '(')
                parlevel++;
            else if (tok == ')')
                parlevel--;
            next();
        }
    } else {
        /* currently, we always use constant expression for globals
           (may change for scripting case) */
        expr_type = EXPR_CONST;
        if (!sec)
            expr_type = EXPR_ANY;
        init_putv(type, sec, c, 0, expr_type);
    }
}

/* parse an initializer for type 't' if 'has_init' is non zero, and
   allocate space in local or global data space ('r' is either
   VT_LOCAL or VT_CONST). If 'v' is non zero, then an associated
   variable 'v' of scope 'scope' is declared before initializers are
   parsed. If 'v' is zero, then a reference to the new object is put
   in the value stack. If 'has_init' is 2, a special parsing is done
   to handle string constants. */
static void decl_initializer_alloc(CType *type, AttributeDef *ad, int r, 
                                   int has_init, int v, int scope)
{
    int size, align, addr, data_offset;
    int level;
    ParseState saved_parse_state;
    TokenString init_str;
    Section *sec;

    size = type_size(type, &align);
    /* If unknown size, we must evaluate it before
       evaluating initializers because
       initializers can generate global data too
       (e.g. string pointers or ISOC99 compound
       literals). It also simplifies local
       initializers handling */
    tok_str_new(&init_str);
    if (size < 0) {
        if (!has_init) 
            error("unknown type size");
        /* get all init string */
        if (has_init == 2) {
            /* only get strings */
            while (tok == TOK_STR || tok == TOK_LSTR) {
                tok_str_add_tok(&init_str);
                next();
            }
        } else {
            level = 0;
            while (level > 0 || (tok != ',' && tok != ';')) {
                if (tok < 0)
                    error("unexpected end of file in initializer");
                tok_str_add_tok(&init_str);
                if (tok == '{')
                    level++;
                else if (tok == '}') {
                    if (level == 0)
                        break;
                    level--;
                }
                next();
            }
        }
        tok_str_add(&init_str, -1);
        tok_str_add(&init_str, 0);
        
        /* compute size */
        save_parse_state(&saved_parse_state);

        macro_ptr = init_str.str;
        next();
        decl_initializer(type, NULL, 0, 1, 1);
        /* prepare second initializer parsing */
        macro_ptr = init_str.str;
        next();
        
        /* if still unknown size, error */
        size = type_size(type, &align);
        if (size < 0) 
            error("unknown type size");
    }
    /* take into account specified alignment if bigger */
    if (ad->aligned > align)
        align = ad->aligned;
    if ((r & VT_VALMASK) == VT_LOCAL) {
        sec = NULL;
        if (do_bounds_check && (type->t & VT_ARRAY)) 
            loc--;
        loc = (loc - size) & -align;
        addr = loc;
        /* handles bounds */
        /* XXX: currently, since we do only one pass, we cannot track
           '&' operators, so we add only arrays */
        if (do_bounds_check && (type->t & VT_ARRAY)) {
            unsigned long *bounds_ptr;
            /* add padding between regions */
            loc--;
            /* then add local bound info */
            bounds_ptr = (unsigned long *)section_ptr_add(lbounds_section, 2 * sizeof(unsigned long));
            bounds_ptr[0] = addr;
            bounds_ptr[1] = size;
        }
        if (v) {
            /* local variable */
            sym_push(v, type, r, addr);
        } else {
            /* push local reference */
            vset(type, r, addr);
        }
    } else {
        Sym *sym;

        sym = NULL;
        if (v && scope == VT_CONST) {
            /* see if the symbol was already defined */
            sym = sym_find(v);
            if (sym) {
                if (!is_compatible_types(&sym->type, type))
                    error("incompatible types for redefinition of '%s'", 
                          get_tok_str(v, NULL));
                if (sym->type.t & VT_EXTERN) {
                    /* if the variable is extern, it was not allocated */
                    sym->type.t &= ~VT_EXTERN;
                } else {
                    /* we accept several definitions of the same
                       global variable. this is tricky, because we
                       must play with the SHN_COMMON type of the symbol */
                    /* XXX: should check if the variable was already
                       initialized. It is incorrect to initialized it
                       twice */
                    /* no init data, we won't add more to the symbol */
                    if (!has_init)
                        goto no_alloc;
                }
            }
        }

        /* allocate symbol in corresponding section */
        sec = ad->section;
        if (!sec) {
            if (has_init)
                sec = data_section;
        }
        if (sec) {
            data_offset = sec->data_offset;
            data_offset = (data_offset + align - 1) & -align;
            addr = data_offset;
            /* very important to increment global pointer at this time
               because initializers themselves can create new initializers */
            data_offset += size;
            /* add padding if bound check */
            if (do_bounds_check)
                data_offset++;
            sec->data_offset = data_offset;
            /* allocate section space to put the data */
            if (sec->sh_type != SHT_NOBITS && 
                data_offset > (int)sec->data_allocated)
                section_realloc(sec, data_offset);
        } else {
            addr = 0; /* avoid warning */
        }

        if (v) {
            if (scope == VT_CONST) {
                if (!sym)
                    goto do_def;
            } else {
            do_def:
                sym = sym_push(v, type, r | VT_SYM, 0);
            }
            /* update symbol definition */
            if (sec) {
                put_extern_sym(sym, sec, addr, size);
            } else {
                Elf32_Sym *esym;
                /* put a common area */
                put_extern_sym(sym, NULL, align, size);
                /* XXX: find a nicer way */
                esym = &((Elf32_Sym *)symtab_section->data)[sym->c];
                esym->st_shndx = SHN_COMMON;
            }
        } else {
            CValue cval;

            /* push global reference */
            sym = get_sym_ref(type, sec, addr, size);
            cval.ul = 0;
            vsetc(type, VT_CONST | VT_SYM, &cval);
            vtop->sym = sym;
        }

        /* handles bounds now because the symbol must be defined
           before for the relocation */
        if (do_bounds_check) {
            unsigned long *bounds_ptr;

            greloc(bounds_section, sym, bounds_section->data_offset, R_DATA_32);
            /* then add global bound info */
            bounds_ptr = (unsigned long *)section_ptr_add(bounds_section, 2 * sizeof(long));
            bounds_ptr[0] = 0; /* relocated */
            bounds_ptr[1] = size;
        }
    }
    if (has_init) {
        decl_initializer(type, sec, addr, 1, 0);
        /* restore parse state if needed */
        if (init_str.str) {
            tok_str_free(init_str.str);
            restore_parse_state(&saved_parse_state);
        }
    }
 no_alloc: ;
}

void put_func_debug(Sym *sym)
{
    char buf[512];

    /* stabs info */
    /* XXX: we put here a dummy type */
    snprintf(buf, sizeof(buf), "%s:%c1", 
             funcname, sym->type.t & VT_STATIC ? 'f' : 'F');
    put_stabs_r(buf, N_FUN, 0, file->line_num, 0,
                cur_text_section, sym->c);
    last_ind = 0;
    last_line_num = 0;
}

/* not finished : try to put some local vars in registers */
//#define CONFIG_REG_VARS

#ifdef CONFIG_REG_VARS
void add_var_ref(int t)
{
    printf("%s:%d: &%s\n", 
           file->filename, file->line_num,
           get_tok_str(t, NULL));
}

/* first pass on a function with heuristic to extract variable usage
   and pointer references to local variables for register allocation */
void analyse_function(void)
{
    int level, t;

    for(;;) {
        if (tok == -1)
            break;
        /* any symbol coming after '&' is considered as being a
           variable whose reference is taken. It is highly unaccurate
           but it is difficult to do better without a complete parse */
        if (tok == '&') {
            next();
            /* if '& number', then no need to examine next tokens */
            if (tok == TOK_CINT ||
                tok == TOK_CUINT ||
                tok == TOK_CLLONG ||
                tok == TOK_CULLONG) {
                continue;
            } else if (tok >= TOK_UIDENT) {
                /* if '& ident [' or '& ident ->', then ident address
                   is not needed */
                t = tok;
                next();
                if (tok != '[' && tok != TOK_ARROW)
                    add_var_ref(t);
            } else {
                level = 0;
                while (tok != '}' && tok != ';' && 
                       !((tok == ',' || tok == ')') && level == 0)) {
                    if (tok >= TOK_UIDENT) {
                        add_var_ref(tok);
                    } else if (tok == '(') {
                        level++;
                    } else if (tok == ')') {
                        level--;
                    }
                    next();
                }
            }
        } else {
            next();
        }
    }
}
#endif

/* parse an old style function declaration list */
/* XXX: check multiple parameter */
static void func_decl_list(Sym *func_sym)
{
    AttributeDef ad;
    int v;
    Sym *s;
    CType btype, type;

    /* parse each declaration */
    while (tok != '{' && tok != ';' && tok != ',' && tok != TOK_EOF) {
        if (!parse_btype(&btype, &ad)) 
            expect("declaration list");
        if (((btype.t & VT_BTYPE) == VT_ENUM ||
             (btype.t & VT_BTYPE) == VT_STRUCT) && 
            tok == ';') {
            /* we accept no variable after */
        } else {
            for(;;) {
                type = btype;
                type_decl(&type, &ad, &v, TYPE_DIRECT);
                /* find parameter in function parameter list */
                s = func_sym->next;
                while (s != NULL) {
                    if ((s->v & ~SYM_FIELD) == v)
                        goto found;
                    s = s->next;
                }
                error("declaration for parameter '%s' but no such parameter",
                      get_tok_str(v, NULL));
            found:
                /* check that no storage specifier except 'register' was given */
                if (type.t & VT_STORAGE)
                    error("storage class specified for '%s'", get_tok_str(v, NULL));
                /* array must be transformed to pointer according to ANSI C */
                type.t &= ~VT_ARRAY;
                /* we can add the type (NOTE: it could be local to the function) */
                s->type = type;
                /* accept other parameters */
                if (tok == ',')
                    next();
                else
                    break;
            }
        }
        skip(';');
    }
}

/* 'l' is VT_LOCAL or VT_CONST to define default storage type */
static void decl(int l)
{
    int v, has_init, r;
    CType type, btype;
    Sym *sym;
    AttributeDef ad;
    
    while (1) {
        if (!parse_btype(&btype, &ad)) {
            /* skip redundant ';' */
            /* XXX: find more elegant solution */
            if (tok == ';') {
                next();
                continue;
            }
            /* special test for old K&R protos without explicit int
               type. Only accepted when defining global data */
            if (l == VT_LOCAL || tok < TOK_DEFINE)
                break;
            btype.t = VT_INT;
        }
        if (((btype.t & VT_BTYPE) == VT_ENUM ||
             (btype.t & VT_BTYPE) == VT_STRUCT) && 
            tok == ';') {
            /* we accept no variable after */
            next();
            continue;
        }
        while (1) { /* iterate thru each declaration */
            type = btype;
            type_decl(&type, &ad, &v, TYPE_DIRECT);
#if 0
            {
                char buf[500];
                type_to_str(buf, sizeof(buf), t, get_tok_str(v, NULL));
                printf("type = '%s'\n", buf);
            }
#endif
            if ((type.t & VT_BTYPE) == VT_FUNC) {
                /* if old style function prototype, we accept a
                   declaration list */
                sym = type.ref;
                if (sym->c == FUNC_OLD)
                    func_decl_list(sym);
            }

            if (tok == '{') {
#ifdef CONFIG_REG_VARS
                TokenString func_str;
                ParseState saved_parse_state;
                int block_level;
#endif

                if (l == VT_LOCAL)
                    error("cannot use local functions");
                if (!(type.t & VT_FUNC))
                    expect("function definition");
                /* XXX: cannot do better now: convert extern line to static inline */
                if ((type.t & (VT_EXTERN | VT_INLINE)) == (VT_EXTERN | VT_INLINE))
                    type.t = (type.t & ~VT_EXTERN) | VT_STATIC;

#ifdef CONFIG_REG_VARS
                /* parse all function code and record it */

                tok_str_new(&func_str);
                
                block_level = 0;
                for(;;) {
                    int t;
                    if (tok == -1)
                        error("unexpected end of file");
                    tok_str_add_tok(&func_str);
                    t = tok;
                    next();
                    if (t == '{') {
                        block_level++;
                    } else if (t == '}') {
                        block_level--;
                        if (block_level == 0)
                            break;
                    }
                }
                tok_str_add(&func_str, -1);
                tok_str_add(&func_str, 0);

                save_parse_state(&saved_parse_state);
    
                macro_ptr = func_str.str;
                next();
                analyse_function();
#endif

                /* compute text section */
                cur_text_section = ad.section;
                if (!cur_text_section)
                    cur_text_section = text_section;
                ind = cur_text_section->data_offset;
                funcname = get_tok_str(v, NULL);
                sym = sym_find(v);
                if (sym) {
                    /* if symbol is already defined, then put complete type */
                    sym->type = type;
                } else {
                    /* put function symbol */
                    sym = global_identifier_push(v, type.t, 0);
                    sym->type.ref = type.ref;
                }
                /* NOTE: we patch the symbol size later */
                put_extern_sym(sym, cur_text_section, ind, 0);
                func_ind = ind;
                sym->r = VT_SYM | VT_CONST;
                /* put debug symbol */
                if (do_debug)
                    put_func_debug(sym);
                /* push a dummy symbol to enable local sym storage */
                sym_push2(&local_stack, SYM_FIELD, 0, 0);
                gfunc_prolog(&type);
#if (!DO_C67)
                loc = 0;
#endif
                rsym = 0;
#ifdef CONFIG_REG_VARS
                macro_ptr = func_str.str;
                next();
#endif
                block(NULL, NULL, NULL, NULL, 0, 0);
                gsym(rsym);
                gfunc_epilog();
                cur_text_section->data_offset = ind;
                label_pop(&global_label_stack, NULL);
                sym_pop(&local_stack, NULL); /* reset local stack */
                /* end of function */
                /* patch symbol size */
                ((Elf32_Sym *)symtab_section->data)[sym->c].st_size = 
                    ind - func_ind;
                if (do_debug) {
                    put_stabn(N_FUN, 0, 0, ind - func_ind);
                }
                funcname = ""; /* for safety */
                func_vt.t = VT_VOID; /* for safety */
                ind = 0; /* for safety */

#ifdef CONFIG_REG_VARS
                tok_str_free(func_str.str);
                restore_parse_state(&saved_parse_state);
#endif
                break;
            } else {
                if (btype.t & VT_TYPEDEF) {
                    /* save typedefed type  */
                    /* XXX: test storage specifiers ? */
                    sym = sym_push(v, &type, 0, 0);
                    sym->type.t |= VT_TYPEDEF;
                } else if ((type.t & VT_BTYPE) == VT_FUNC) {
                    /* external function definition */
                    external_sym(v, &type, 0);
                } else {
                    /* not lvalue if array */
                    r = 0;
                    if (!(type.t & VT_ARRAY))
                        r |= lvalue_type(type.t);
                    has_init = (tok == '=');
                    if ((btype.t & VT_EXTERN) || 
                        ((type.t & VT_ARRAY) && (type.t & VT_STATIC) &&
                         !has_init && l == VT_CONST && type.ref->c < 0)) {
                        /* external variable */
                        /* NOTE: as GCC, uninitialized global static
                           arrays of null size are considered as
                           extern */
                        external_sym(v, &type, r);
                    } else {
                        if (type.t & VT_STATIC)
                            r |= VT_CONST;
                        else
                            r |= l;
                        if (has_init)
                            next();
                        decl_initializer_alloc(&type, &ad, r, 
                                               has_init, v, l);
                    }
                }
                if (tok != ',') {
                    skip(';');
                    break;
                }
                next();
            }
        }
    }
}

/* better than nothing, but needs extension to handle '-E' option
   correctly too */
static void preprocess_init(TCCState *s1)
{
    s1->include_stack_ptr = s1->include_stack;
    /* XXX: move that before to avoid having to initialize
       file->ifdef_stack_ptr ? */
    s1->ifdef_stack_ptr = s1->ifdef_stack;
    file->ifdef_stack_ptr = s1->ifdef_stack_ptr;

    /* XXX: not ANSI compliant: bound checking says error */
    vtop = vstack - 1;
}

/* compile the C file opened in 'file'. Return non zero if errors. */
static int tcc_compile(TCCState *s1)
{
#if (DO_C67) 
	char *fs1,*fs2;
#endif

    Sym *define_start;
    char buf[512];
    volatile int section_sym;

#ifdef INC_DEBUG
    printf("%s: **** new file\n", file->filename);
#endif
    preprocess_init(s1);

    funcname = "";
    anon_sym = SYM_FIRST_ANOM; 

    /* file info: full path + filename */
    section_sym = 0; /* avoid warning */
    if (do_debug) {
        section_sym = put_elf_sym(symtab_section, 0, 0, 
                                  ELF32_ST_INFO(STB_LOCAL, STT_SECTION), 0, 
                                  text_section->sh_num, NULL);
        (void)_getcwd(buf, sizeof(buf));
        pstrcat(buf, sizeof(buf), "/");
        put_stabs_r(buf, N_SO, 0, 0, 
                    text_section->data_offset, text_section, section_sym);

#if (DO_C67)  // debug works better with no path

		fs1=strrchr((char *)file->filename, '\\');
		fs2=strrchr((char *)file->filename, '/');

		if (fs2>fs1) fs1=fs2;

		if (fs1==NULL)
			fs1=file->filename;
		else
			fs1++;
    
		put_stabs_r(fs1, N_SO, 0, 0, 
					text_section->data_offset, text_section, section_sym);
    }
    /* an elf symbol of type STT_FILE must be put so that STB_LOCAL
       symbols can be safely used */
    put_elf_sym(symtab_section, 0, 0, 
                ELF32_ST_INFO(STB_LOCAL, STT_FILE), 0, 
                SHN_ABS, file->filename);
#else
        put_stabs_r(file->filename, N_SO, 0, 0, 
                    text_section->data_offset, text_section, section_sym);
    }
    /* an elf symbol of type STT_FILE must be put so that STB_LOCAL
       symbols can be safely used */
    put_elf_sym(symtab_section, 0, 0, 
                ELF32_ST_INFO(STB_LOCAL, STT_FILE), 0, 
                SHN_ABS, file->filename);
#endif

    /* define some often used types */
    int_type.t = VT_INT;

    char_pointer_type.t = VT_BYTE;
    mk_pointer(&char_pointer_type);

    func_old_type.t = VT_FUNC;
    func_old_type.ref = sym_push(SYM_FIELD, &int_type, FUNC_CDECL, FUNC_OLD);

#if 0
    /* define 'void *alloca(unsigned int)' builtin function */
    {
        Sym *s1;

        p = anon_sym++;
        sym = sym_push(p, mk_pointer(VT_VOID), FUNC_CDECL, FUNC_NEW);
        s1 = sym_push(SYM_FIELD, VT_UNSIGNED | VT_INT, 0, 0);
        s1->next = NULL;
        sym->next = s1;
        sym_push(TOK_alloca, VT_FUNC | (p << VT_STRUCT_SHIFT), VT_CONST, 0);
    }
#endif

    define_start = define_stack;

    if (setjmp(s1->error_jmp_buf) == 0) {
        s1->nb_errors = 0;
        s1->error_set_jmp_enabled = 1;

        ch = file->buf_ptr[0];
        tok_flags = TOK_FLAG_BOL | TOK_FLAG_BOF;
        parse_flags = PARSE_FLAG_PREPROCESS | PARSE_FLAG_TOK_NUM;
        next();
        decl(VT_CONST);
        if (tok != -1)
            expect("declaration");

        /* end of translation unit info */
        if (do_debug) {
            put_stabs_r(NULL, N_SO, 0, 0, 
                        text_section->data_offset, text_section, section_sym);
        }
    }
    s1->error_set_jmp_enabled = 0;

    /* reset define stack, but leave -Dsymbols (may be incorrect if
       they are undefined) */
    free_defines(define_start); 

    sym_pop(&global_stack, NULL);

    return s1->nb_errors != 0 ? -1 : 0;
}

#ifdef LIBTCC
int tcc_compile_string(TCCState *s, const char *str)
{
    BufferedFile bf1, *bf = &bf1;
    int ret, len;
    char *buf;

    /* init file structure */
    bf->fd = -1;
    /* XXX: avoid copying */
    len = strlen(str);
    buf = tcc_malloc(len + 1);
    if (!buf)
        return -1;
    memcpy(buf, str, len);
    buf[len] = CH_EOB;
    bf->buf_ptr = buf;
    bf->buf_end = buf + len;
    pstrcpy(bf->filename, sizeof(bf->filename), "<string>");
    bf->line_num = 1;
    file = bf;
    
    ret = tcc_compile(s);
    
    tcc_free(buf);

    /* currently, no need to close */
    return ret;
}
#endif

/* define a preprocessor symbol. A value can also be provided with the '=' operator */
void tcc_define_symbol(TCCState *s1, const char *sym, const char *value)
{
    BufferedFile bf1, *bf = &bf1;

    pstrcpy((char *)bf->buffer, IO_BUF_SIZE, sym);
    pstrcat((char *)bf->buffer, IO_BUF_SIZE, " ");
    /* default value */
    if (!value) 
        value = "1";
    pstrcat((char *)bf->buffer, IO_BUF_SIZE, value);
    
    /* init file structure */
    bf->fd = -1;
    bf->buf_ptr = bf->buffer;
    bf->buf_end = bf->buffer + strlen((const char *)bf->buffer);
    *bf->buf_end = CH_EOB;
    bf->filename[0] = '\0';
    bf->line_num = 1;
    file = bf;
    
    s1->include_stack_ptr = s1->include_stack;

    /* parse with define parser */
    ch = file->buf_ptr[0];
    next_nomacro();
    parse_define();
    file = NULL;
}

/* undefine a preprocessor symbol */
void tcc_undefine_symbol(TCCState *s1, const char *sym)
{
    TokenSym *ts;
    Sym *s;
    ts = tok_alloc(sym, strlen(sym));
    s = define_find(ts->tok);
    /* undefine symbol by putting an invalid name */
    if (s)
        define_undef(s);
}

#ifdef CONFIG_TCC_ASM

#include "i386-asm.c"
#include "tccasm.c"

#else
static void asm_instr(void)
{
    error("inline asm() not supported");
}
#endif

#include "tccelf.c"

#if !DO_C67     // SJH - "run" code doesn't make sense for cross compiler...

/* print the position in the source file of PC value 'pc' by reading
   the stabs debug information */
static void rt_printline(unsigned long wanted_pc)
{
    Stab_Sym *sym, *sym_end;
    char func_name[128], last_func_name[128];
    unsigned long func_addr, last_pc, pc;
    const char *incl_files[INCLUDE_STACK_SIZE];
    int incl_index, len, last_line_num, i;
    const char *str, *p;

    fprintf(stderr, "0x%08lx:", wanted_pc);

    func_name[0] = '\0';
    func_addr = 0;
    incl_index = 0;
    last_func_name[0] = '\0';
    last_pc = 0xffffffff;
    last_line_num = 1;
    sym = (Stab_Sym *)stab_section->data + 1;
    sym_end = (Stab_Sym *)(stab_section->data + stab_section->data_offset);
    while (sym < sym_end) {
        switch(sym->n_type) {
            /* function start or end */
        case N_FUN:
            if (sym->n_strx == 0) {
                /* we test if between last line and end of function */
                pc = sym->n_value + func_addr;
                if (wanted_pc >= last_pc && wanted_pc < pc)
                    goto found;
                func_name[0] = '\0';
                func_addr = 0;
            } else {
                str = (const char *)stabstr_section->data + sym->n_strx;
                p = strchr(str, ':');
                if (!p) {
                    pstrcpy(func_name, sizeof(func_name), str);
                } else {
                    len = p - str;
                    if ((size_t)len > sizeof(func_name) - 1)
                        len = sizeof(func_name) - 1;
                    memcpy(func_name, str, len);
                    func_name[len] = '\0';
                }
                func_addr = sym->n_value;
            }
            break;
            /* line number info */
        case N_SLINE:
            pc = sym->n_value + func_addr;
            if (wanted_pc >= last_pc && wanted_pc < pc)
                goto found;
            last_pc = pc;
            last_line_num = sym->n_desc;
            /* XXX: slow! */
            strcpy(last_func_name, func_name);
            break;
            /* include files */
        case N_BINCL:
            str = (const char *)stabstr_section->data + sym->n_strx;
        add_incl:
            if (incl_index < INCLUDE_STACK_SIZE) {
                incl_files[incl_index++] = str;
            }
            break;
        case N_EINCL:
            if (incl_index > 1)
                incl_index--;
            break;
        case N_SO:
            if (sym->n_strx == 0) {
                incl_index = 0; /* end of translation unit */
            } else {
                str = (const char *)stabstr_section->data + sym->n_strx;
                /* do not add path */
                len = strlen(str);
                if (len > 0 && str[len - 1] != '/')
                    goto add_incl;
            }
            break;
        }
        sym++;
    }

    /* second pass: we try symtab symbols (no line number info) */
    incl_index = 0;
    {
        Elf32_Sym *sym, *sym_end;
        int type;

        sym_end = (Elf32_Sym *)(symtab_section->data + symtab_section->data_offset);
        for(sym = (Elf32_Sym *)symtab_section->data + 1; 
            sym < sym_end;
            sym++) {
            type = ELF32_ST_TYPE(sym->st_info);
            if (type == STT_FUNC) {
                if (wanted_pc >= sym->st_value &&
                    wanted_pc < sym->st_value + sym->st_size) {
                    pstrcpy(last_func_name, sizeof(last_func_name),
                            (const char *)strtab_section->data + sym->st_name);
                    goto found;
                }
            }
        }
    }
    /* did not find any info: */
    fprintf(stderr, " ???\n");
    return;
 found:
    if (last_func_name[0] != '\0') {
        fprintf(stderr, " %s()", last_func_name);
    }
    if (incl_index > 0) {
        fprintf(stderr, " (%s:%d", 
                incl_files[incl_index - 1], last_line_num);
        for(i = incl_index - 2; i >= 0; i--)
            fprintf(stderr, ", included from %s", incl_files[i]);
        fprintf(stderr, ")");
    }
    fprintf(stderr, "\n");
}


#ifndef WIN32

#ifdef __i386__

/* fix for glibc 2.1 */
#ifndef REG_EIP
#define REG_EIP EIP
#define REG_EBP EBP
#endif

/* return the PC at frame level 'level'. Return non zero if not found */
static int rt_get_caller_pc(unsigned long *paddr, 
                            ucontext_t *uc, int level)
{
    unsigned long fp;
    int i;

    if (level == 0) {
#ifdef __FreeBSD__
        *paddr = uc->uc_mcontext.mc_eip;
#else
        *paddr = uc->uc_mcontext.gregs[REG_EIP];
#endif
        return 0;
    } else {
#ifdef __FreeBSD__
        fp = uc->uc_mcontext.mc_ebp;
#else
        fp = uc->uc_mcontext.gregs[REG_EBP];
#endif
        for(i=1;i<level;i++) {
            /* XXX: check address validity with program info */
            if (fp <= 0x1000 || fp >= 0xc0000000)
                return -1;
            fp = ((unsigned long *)fp)[0];
        }
        *paddr = ((unsigned long *)fp)[1];
        return 0;
    }
}
#else
#error add arch specific rt_get_caller_pc()
#endif

/* emit a run time error at position 'pc' */
void rt_error(ucontext_t *uc, const char *fmt, ...)
{
    va_list ap;
    unsigned long pc;
    int i;

    va_start(ap, fmt);
    fprintf(stderr, "Runtime error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    for(i=0;i<num_callers;i++) {
        if (rt_get_caller_pc(&pc, uc, i) < 0)
            break;
        if (i == 0)
            fprintf(stderr, "at ");
        else
            fprintf(stderr, "by ");
        rt_printline(pc);
    }
    exit(255);
    va_end(ap);
}

/* signal handler for fatal errors */
static void sig_error(int signum, siginfo_t *siginf, void *puc)
{
    ucontext_t *uc = (ucontext_t *)puc;

    switch(signum) {
    case SIGFPE:
        switch(siginf->si_code) {
        case FPE_INTDIV:
        case FPE_FLTDIV:
            rt_error(uc, "division by zero");
            break;
        default:
            rt_error(uc, "floating point exception");
            break;
        }
        break;
    case SIGBUS:
    case SIGSEGV:
        if (rt_bound_error_msg && *rt_bound_error_msg)
            rt_error(uc, *rt_bound_error_msg);
        else
            rt_error(uc, "dereferencing invalid pointer");
        break;
    case SIGILL:
        rt_error(uc, "illegal instruction");
        break;
    case SIGABRT:
        rt_error(uc, "abort() called");
        break;
    default:
        rt_error(uc, "caught signal %d", signum);
        break;
    }
    exit(255);
}
#endif  //!WIN32

/* do all relocations (needed before using tcc_get_symbol()) */
int tcc_relocate(TCCState *s1)
{
    Section *s;
    int i;

    s1->nb_errors = 0;
    
    tcc_add_runtime(s1);

    relocate_common_syms();

    /* compute relocation address : section are relocated in place. We
       also alloc the bss space */
    for(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        if (s->sh_flags & SHF_ALLOC) {
            if (s->sh_type == SHT_NOBITS)
                s->data = (unsigned char *)tcc_mallocz(s->data_offset);
            s->sh_addr = (unsigned long)s->data;
        }
    }

    relocate_syms(s1, 1);

    if (s1->nb_errors != 0)
        return -1;

    /* relocate each section */
    for(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        if (s->reloc)
            relocate_section(s1, s);
    }
    return 0;
}

/* launch the compiled program with the given arguments */
int tcc_run(TCCState *s1, int argc, char **argv)
{
    int (*prog_main)(int, char **);

    if (tcc_relocate(s1) < 0)
        return -1;

    prog_main = (int (__cdecl *)(int,char ** ))tcc_get_symbol(s1, "main");
    
    if (do_debug) {
#ifdef WIN32
        error("debug mode currently not available for Windows");
#else        
        struct sigaction sigact;
        /* install TCC signal handlers to print debug info on fatal
           runtime errors */
        sigact.sa_flags = SA_SIGINFO | SA_RESETHAND;
        sigact.sa_sigaction = sig_error;
        sigemptyset(&sigact.sa_mask);
        sigaction(SIGFPE, &sigact, NULL);
        sigaction(SIGILL, &sigact, NULL);
        sigaction(SIGSEGV, &sigact, NULL);
        sigaction(SIGBUS, &sigact, NULL);
        sigaction(SIGABRT, &sigact, NULL);
#endif
    }

#ifdef CONFIG_TCC_BCHECK
    if (do_bounds_check) {
        void (*bound_init)(void);

        /* set error function */
        rt_bound_error_msg = (void *)tcc_get_symbol(s1, "__bound_error_msg");

        /* XXX: use .init section so that it also work in binary ? */
        bound_init = (void *)tcc_get_symbol(s1, "__bound_init");
        bound_init();
    }
#endif

#if (DO_C67)    // SJH  <- this test redundant since we're in a !DO_C67 block
	return 0;  // don't run the program
#else
    return (*prog_main)(argc, argv);
#endif
}
#endif  // !DO_C67

TCCState *tcc_new(void)
{
    const char *p, *r;
    TCCState *s;
    TokenSym *ts;
    int i, c;

    s = (TCCState *)tcc_mallocz(sizeof(TCCState));
    if (!s)
        return NULL;
    tcc_state = s;
    s->output_type = TCC_OUTPUT_MEMORY;

    /* init isid table */
    for(i=0;i<256;i++)
        isidnum_table[i] = isid(i) || isnum(i);

    /* add all tokens */
    table_ident = NULL;
    memset(hash_ident, 0, TOK_HASH_SIZE * sizeof(TokenSym *));
    
    tok_ident = TOK_IDENT;
    p = tcc_keywords;
    while (*p) {
        r = p;
        for(;;) {
            c = *r++;
            if (c == '\0')
                break;
        }
        ts = tok_alloc(p, r - p - 1);
        p = r;
    }

    /* we add dummy defines for some special macros to speed up tests
       and to have working defined() */
    define_push(TOK___LINE__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___FILE__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___DATE__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___TIME__, MACRO_OBJ, NULL, NULL);

    /* standard defines */
    tcc_define_symbol(s, "__STDC__", NULL);
#if defined(TCC_TARGET_I386)
    tcc_define_symbol(s, "__i386__", NULL);
#endif
#if defined(linux)
    tcc_define_symbol(s, "__linux__", NULL);
    tcc_define_symbol(s, "linux", NULL);
#endif
    /* tiny C specific defines */
    tcc_define_symbol(s, "__TINYC__", NULL);

    /* tiny C & gcc defines */
    tcc_define_symbol(s, "__SIZE_TYPE__", "unsigned int");
    tcc_define_symbol(s, "__PTRDIFF_TYPE__", "int");
    tcc_define_symbol(s, "__WCHAR_TYPE__", "int");
    
    /* default library paths */
    tcc_add_library_path(s, "/usr/local/lib");
    tcc_add_library_path(s, "/usr/lib");
    tcc_add_library_path(s, "/lib");

    /* no section zero */
    dynarray_add((void ***)&s->sections, &s->nb_sections, NULL);

    /* create standard sections */
    text_section = new_section(s, ".text", SHT_PROGBITS, SHF_ALLOC | SHF_EXECINSTR);
    data_section = new_section(s, ".data", SHT_PROGBITS, SHF_ALLOC | SHF_WRITE);
    bss_section = new_section(s, ".bss", SHT_NOBITS, SHF_ALLOC | SHF_WRITE);

    /* symbols are always generated for linking stage */
    symtab_section = new_symtab(s, ".symtab", SHT_SYMTAB, 0,
                                ".strtab",
                                ".hashtab", SHF_PRIVATE); 
    strtab_section = symtab_section->link;
    
    /* private symbol table for dynamic symbols */
    s->dynsymtab_section = new_symtab(s, ".dynsymtab", SHT_SYMTAB, SHF_PRIVATE,
                                      ".dynstrtab", 
                                      ".dynhashtab", SHF_PRIVATE);
    return s;
}

void tcc_delete(TCCState *s1)
{
    int i, n;

    /* free -D defines */
    free_defines(NULL);

    /* free tokens */
    n = tok_ident - TOK_IDENT;
    for(i = 0; i < n; i++)
        tcc_free(table_ident[i]);
    tcc_free(table_ident);

    /* free all sections */

    free_section(symtab_section->hash);

    free_section(s1->dynsymtab_section->hash);
    free_section(s1->dynsymtab_section->link);
    free_section(s1->dynsymtab_section);

    for(i = 1; i < s1->nb_sections; i++)
        free_section(s1->sections[i]);
    tcc_free(s1->sections);
    
    /* free loaded dlls array */
    for(i = 0; i < s1->nb_loaded_dlls; i++)
        tcc_free(s1->loaded_dlls[i]);
    tcc_free(s1->loaded_dlls);

    /* library paths */
    for(i = 0; i < s1->nb_library_paths; i++)
        tcc_free(s1->library_paths[i]);
    tcc_free(s1->library_paths);

    /* cached includes */
    for(i = 0; i < s1->nb_cached_includes; i++)
        tcc_free(s1->cached_includes[i]);
    tcc_free(s1->cached_includes);

    for(i = 0; i < s1->nb_include_paths; i++)
        tcc_free(s1->include_paths[i]);
    tcc_free(s1->include_paths);

    for(i = 0; i < s1->nb_sysinclude_paths; i++)
        tcc_free(s1->sysinclude_paths[i]);
    tcc_free(s1->sysinclude_paths);

    tcc_free(s1);
}

int tcc_add_include_path(TCCState *s1, const char *pathname)
{
    char *pathname1;
    
    pathname1 = tcc_strdup(pathname);
    dynarray_add((void ***)&s1->include_paths, &s1->nb_include_paths, pathname1);
    return 0;
}

int tcc_add_sysinclude_path(TCCState *s1, const char *pathname)
{
    char *pathname1;
    
    pathname1 = tcc_strdup(pathname);
    dynarray_add((void ***)&s1->sysinclude_paths, &s1->nb_sysinclude_paths, pathname1);
    return 0;
}

static int tcc_add_file_internal(TCCState *s1, const char *filename, int flags)
{
    const char *ext, *filename1;
    Elf32_Ehdr ehdr;
    int fd, ret;
    BufferedFile *saved_file;
    
    /* find source file type with extension */
    filename1 = strrchr(filename, '/');
    if (filename1)
        filename1++;
    else
        filename1 = filename;
    ext = strrchr(filename1, '.');
    if (ext)
        ext++;

    /* open the file */
    saved_file = file;
    file = tcc_open(s1, filename);
    if (!file) {
        if (flags & AFF_PRINT_ERROR) {
            error_noabort("file '%s' not found", filename);
        }
        ret = -1;
        goto fail1;
    }

#if (DO_C67)

	if (!strcmp(ext, "out"))
	{
		ret = ReadCoff(s1, filename);  // import global symbols
	}
	else
#endif

    if (!ext || !strcmp(ext, "c")) {
        /* C file assumed */
        ret = tcc_compile(s1);
    } else 
#ifdef CONFIG_TCC_ASM
    if (!strcmp(ext, "S")) {
        /* preprocessed assembler */
        ret = tcc_assemble(s1, 1);
    } else if (!strcmp(ext, "s")) {
        /* non preprocessed assembler */
        ret = tcc_assemble(s1, 0);
    } else 
#endif
    {
        fd = file->fd;
        /* assume executable format: auto guess file type */
        if (_read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr)) {
            error_noabort("could not read header (unknown file extention type)");
            goto fail;
        }
        _lseek(fd, 0, SEEK_SET);
        
        if (ehdr.e_ident[0] == ELFMAG0 &&
            ehdr.e_ident[1] == ELFMAG1 &&
            ehdr.e_ident[2] == ELFMAG2 &&
            ehdr.e_ident[3] == ELFMAG3) {
            file->line_num = 0; /* do not display line number if error */
            if (ehdr.e_type == ET_REL) {
                ret = tcc_load_object_file(s1, fd, 0);
            } else if (ehdr.e_type == ET_DYN) {
                ret = tcc_load_dll(s1, fd, filename, 
                                   (flags & AFF_REFERENCED_DLL) != 0);
            } else {
                error_noabort("unrecognized ELF file");
                goto fail;
            }
        } else if (memcmp((char *)&ehdr, ARMAG, 8) == 0) {
            file->line_num = 0; /* do not display line number if error */
            ret = tcc_load_archive(s1, fd);
        } else {
            /* as GNU ld, consider it is an ld script if not recognized */
            ret = tcc_load_ldscript(s1);
            if (ret < 0) {
                error_noabort("unrecognized file type");
                goto fail;
            }
        }
    }
 the_end:
    tcc_close(file);
 fail1:
    file = saved_file;
    return ret;
 fail:
    ret = -1;
    goto the_end;
}

int tcc_add_file(TCCState *s, const char *filename)
{
    return tcc_add_file_internal(s, filename, AFF_PRINT_ERROR);
}

int tcc_add_library_path(TCCState *s, const char *pathname)
{
    char *pathname1;
    
    pathname1 = tcc_strdup(pathname);
    dynarray_add((void ***)&s->library_paths, &s->nb_library_paths, pathname1);
    return 0;
}

/* find and load a dll. Return non zero if not found */
/* XXX: add '-rpath' option support ? */
static int tcc_add_dll(TCCState *s, const char *filename, int flags)
{
    char buf[1024];
    int i;

    for(i = 0; i < s->nb_library_paths; i++) {
        snprintf(buf, sizeof(buf), "%s/%s", 
                 s->library_paths[i], filename);
        if (tcc_add_file_internal(s, buf, flags) == 0)
            return 0;
    }
    return -1;
}

/* the library name is the same as the argument of the '-l' option */
int tcc_add_library(TCCState *s, const char *libraryname)
{
    char buf[1024];
    int i;
    void *h;
    
    /* first we look for the dynamic library if not static linking */
    if (!s->static_link) {
        snprintf(buf, sizeof(buf), "lib%s.so", libraryname);
        /* if we output to memory, then we simply we dlopen(). */
        if (s->output_type == TCC_OUTPUT_MEMORY) {
            /* Since the libc is already loaded, we don't need to load it again */
            if (!strcmp(libraryname, "c"))
                return 0;
            h = dlopen(buf, RTLD_GLOBAL | RTLD_LAZY);
            if (h)
                return 0;
        } else {
            if (tcc_add_dll(s, buf, 0) == 0)
                return 0;
        }
    }

    /* then we look for the static library */
    for(i = 0; i < s->nb_library_paths; i++) {
        snprintf(buf, sizeof(buf), "%s/lib%s.a", 
                 s->library_paths[i], libraryname);
        if (tcc_add_file_internal(s, buf, 0) == 0)
            return 0;
    }
    return -1;
}

int tcc_add_symbol(TCCState *s, const char *name, unsigned long val)
{
    add_elf_sym(symtab_section, val, 0, 
                ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE),
                SHN_ABS, name, 0);
    return 0;
}

int tcc_set_output_type(TCCState *s, int output_type)
{
    char buf[1024];

    s->output_type = output_type;

    if (!s->nostdinc) {
        /* default include paths */
        /* XXX: reverse order needed if -isystem support */
        tcc_add_sysinclude_path(s, "/usr/local/include");
        tcc_add_sysinclude_path(s, "/usr/include");
        snprintf(buf, sizeof(buf), "%s/include", tcc_lib_path);
        tcc_add_sysinclude_path(s, buf);
    }

    /* if bound checking, then add corresponding sections */
#ifdef CONFIG_TCC_BCHECK
    if (do_bounds_check) {
        /* define symbol */
        tcc_define_symbol(s, "__BOUNDS_CHECKING_ON", NULL);
        /* create bounds sections */
        bounds_section = new_section(s, ".bounds", 
                                     SHT_PROGBITS, SHF_ALLOC);
        lbounds_section = new_section(s, ".lbounds", 
                                      SHT_PROGBITS, SHF_ALLOC);
    }
#endif

    /* add debug sections */
    if (do_debug) {
        /* stab symbols */
        stab_section = new_section(s, ".stab", SHT_PROGBITS, 0);
        stab_section->sh_entsize = sizeof(Stab_Sym);
        stabstr_section = new_section(s, ".stabstr", SHT_STRTAB, 0);
        put_elf_str(stabstr_section, "");
        stab_section->link = stabstr_section;
        /* put first entry */
        put_stabs("", 0, 0, 0, 0);
    }

    /* add libc crt1/crti objects */
// tktk     if (output_type == TCC_OUTPUT_EXE || 
//         output_type == TCC_OUTPUT_DLL) {
//         if (output_type != TCC_OUTPUT_DLL)
//             tcc_add_file(s, CONFIG_TCC_CRT_PREFIX "/crt1.o");
//         tcc_add_file(s, CONFIG_TCC_CRT_PREFIX "/crti.o");
//     }

    
	
	return 0;
}

#if !defined(LIBTCC)

static int64_t getclock_us(void)
{
#ifdef WIN32
    struct _timeb tb;
    _ftime(&tb);
    return (tb.time * 1000LL + tb.millitm) * 1000LL;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
#endif
}

void help(void)
{
    printf("tcc version 0.9.16 - Tiny C Compiler - Copyright (C) 2001, 2002 Fabrice Bellard\n" 
           "usage: tcc [-c] [-o outfile] [-Bdir] [-bench] [-Idir] [-Dsym[=val]] [-Usym]\n"
           "           [-g] [-b] [-bt N] [-Ldir] [-llib] [-shared] [-static]\n"
           "           [-text X] [-data X] [-bss X]\n"
           "           [--] infile1 [infile2... --] [infile_args...]\n"
           "\n"
           "General options:\n"
           "  -c          compile only - generate an object file\n"
           "  -o outfile  set output filename\n"
           "  --          allows multiples input files if no -o option given. Also\n" 
           "              separate input files from runtime arguments\n"
           "  -Bdir       set tcc internal library path\n"
           "  -bench      output compilation statistics\n"
           "Preprocessor options:\n"
           "  -Idir       add include path 'dir'\n"
           "  -Dsym[=val] define 'sym' with value 'val'\n"
           "  -Usym       undefine 'sym'\n"
           "Linker options:\n"
           "  -Ldir       add library path 'dir'\n"
           "  -llib       link with dynamic or static library 'lib'\n"
           "  -shared     generate a shared library\n"
           "  -static     static linking\n"
           "  -r          relocatable output\n"
           "Debugger options:\n"
           "  -g          generate runtime debug info\n"
#ifdef CONFIG_TCC_BCHECK
           "  -b          compile with built-in memory and bounds checker (implies -g)\n"
#endif
           "  -bt N       show N callers in stack traces\n"
#if (DO_C67)
           "  -text x     spec in hex where to link text section\n"
           "  -data x     spec in hex where to link data section (default after text)\n"
           "  -bss x      spec in hex where to link bss  section (default after data)\n"
#endif
           );
}

#define TCC_OPTION_HAS_ARG 0x0001
#define TCC_OPTION_NOSEP   0x0002 /* cannot have space before option and arg */

typedef struct TCCOption {
    const char *name;
    uint16_t index;
    uint16_t flags;
} TCCOption;

enum {
    TCC_OPTION_HELP,
    TCC_OPTION_MARKER,
    TCC_OPTION_I,
    TCC_OPTION_D,
    TCC_OPTION_U,
    TCC_OPTION_L,
    TCC_OPTION_B,
    TCC_OPTION_l,
    TCC_OPTION_bench,
    TCC_OPTION_bt,
    TCC_OPTION_b,
    TCC_OPTION_g,
    TCC_OPTION_c,
    TCC_OPTION_static,
    TCC_OPTION_shared,
    TCC_OPTION_o,
    TCC_OPTION_r,
    TCC_OPTION_W,
    TCC_OPTION_O,
    TCC_OPTION_m,
    TCC_OPTION_f,
    TCC_OPTION_nostdinc,
    TCC_OPTION_print_search_dirs,
#if (DO_C67)
    TCC_OPTION_text,  // specify where the text section goes
    TCC_OPTION_data,  // specify where the data section goes
    TCC_OPTION_bss,   // specify where the bss  section goes
#endif
    TCC_OPTION_rdynamic,
};

static const TCCOption tcc_options[] = {
    { "h", TCC_OPTION_HELP, 0 },
    { "?", TCC_OPTION_HELP, 0 },
    { "-", TCC_OPTION_MARKER, 0 },
    { "I", TCC_OPTION_I, TCC_OPTION_HAS_ARG },
    { "D", TCC_OPTION_D, TCC_OPTION_HAS_ARG },
    { "U", TCC_OPTION_U, TCC_OPTION_HAS_ARG },
    { "L", TCC_OPTION_L, TCC_OPTION_HAS_ARG },
    { "B", TCC_OPTION_B, TCC_OPTION_HAS_ARG },
    { "l", TCC_OPTION_l, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "bench", TCC_OPTION_bench, 0 },
    { "bt", TCC_OPTION_bt, TCC_OPTION_HAS_ARG },
#ifdef CONFIG_TCC_BCHECK
    { "b", TCC_OPTION_b, 0 },
#endif
    { "g", TCC_OPTION_g, 0 },
    { "c", TCC_OPTION_c, 0 },
    { "static", TCC_OPTION_static, 0 },
    { "shared", TCC_OPTION_shared, 0 },
    { "o", TCC_OPTION_o, TCC_OPTION_HAS_ARG },
    { "rdynamic", TCC_OPTION_rdynamic, 0 }, /* currently ignored */
    { "r", TCC_OPTION_r, 0 },
    { "W", TCC_OPTION_W, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "O", TCC_OPTION_O, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "m", TCC_OPTION_m, TCC_OPTION_HAS_ARG },
    { "f", TCC_OPTION_f, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "nostdinc", TCC_OPTION_nostdinc, 0 },
    { "print-search-dirs", TCC_OPTION_print_search_dirs, 0 }, 
#if (DO_C67)
    { "text", TCC_OPTION_text, TCC_OPTION_HAS_ARG },
    { "data", TCC_OPTION_data, TCC_OPTION_HAS_ARG },
    { "bss",  TCC_OPTION_bss,  TCC_OPTION_HAS_ARG },
#endif
    { NULL },
};

int main(int argc, char **argv)
{
    char *r;
    int optind, output_type, multiple_files, i, reloc_output;
    TCCState *s;
    char **files;
    int nb_files, nb_libraries, nb_objfiles, dminus, ret;
    char objfilename[1024];
    int64_t start_time = 0;
    const TCCOption *popt;
    const char *optarg, *p1, *r1, *outfile;
    int print_search_dirs;

    s = tcc_new();
    output_type = TCC_OUTPUT_MEMORY;

    optind = 1;
    outfile = NULL;
    multiple_files = 0;
    dminus = 0;
    files = NULL;
    nb_files = 0;
    nb_libraries = 0;
    reloc_output = 0;
    print_search_dirs = 0;
    while (1) {
        if (optind >= argc) {
            if (nb_files == 0 && !print_search_dirs)
                goto show_help;
            else
                break;
        }
        r = argv[optind++];
        if (r[0] != '-') {
            /* add a new file */
            dynarray_add((void ***)&files, &nb_files, r);
            if (!multiple_files) {
                optind--;
                /* argv[0] will be this file */
                break;
            }
        } else {
            /* find option in table (match only the first chars */
            popt = tcc_options;
            for(;;) {
                p1 = popt->name;
                if (p1 == NULL)
                    error("invalid option -- '%s'", r);
                r1 = r + 1;
                for(;;) {
                    if (*p1 == '\0')
                        goto option_found;
                    if (*r1 != *p1)
                        break;
                    p1++;
                    r1++;
                }
                popt++;
            }
        option_found:
            if (popt->flags & TCC_OPTION_HAS_ARG) {
                if (*r1 != '\0' || (popt->flags & TCC_OPTION_NOSEP)) {
                    optarg = r1;
                } else {
                    if (optind >= argc)
                        error("argument to '%s' is missing", r);
                    optarg = argv[optind++];
                }
            } else {
                if (*r1 != '\0')
                    goto show_help;
                optarg = NULL;
            }
                
            switch(popt->index) {
            case TCC_OPTION_HELP:
            show_help:
                help();
                return 1;
            case TCC_OPTION_MARKER:
                /* '--' enables multiple files input and also ends several
                   file input */
                if (dminus && multiple_files) {
                    optind--; /* argv[0] will be '--' */
                    goto end_parse;
                }
                dminus = 1;
                multiple_files = 1;
            case TCC_OPTION_I:
                if (tcc_add_include_path(s, optarg) < 0)
                    error("too many include paths");
                break;
            case TCC_OPTION_D:
                {
                    char *sym, *value;
                    sym = (char *)optarg;
                    value = strchr(sym, '=');
                    if (value) {
                        *value = '\0';
                        value++;
                    }
                    tcc_define_symbol(s, sym, value);
                }
                break;
            case TCC_OPTION_U:
                tcc_undefine_symbol(s, optarg);
                break;
            case TCC_OPTION_L:
                tcc_add_library_path(s, optarg);
                break;
            case TCC_OPTION_B:
                /* set tcc utilities path (mainly for tcc development) */
                tcc_lib_path = optarg;
                break;
            case TCC_OPTION_l:
                dynarray_add((void ***)&files, &nb_files, r);
                nb_libraries++;
                break;
            case TCC_OPTION_bench:
                do_bench = 1;
                break;
            case TCC_OPTION_bt:
                num_callers = atoi(optarg);
                break;
#ifdef CONFIG_TCC_BCHECK
            case TCC_OPTION_b:
                do_bounds_check = 1;
                do_debug = 1;
                break;
#endif
            case TCC_OPTION_g:
                do_debug = 1;
                break;
            case TCC_OPTION_c:
                multiple_files = 1;
                output_type = TCC_OUTPUT_OBJ;
                break;
            case TCC_OPTION_static:
                s->static_link = 1;
                break;
            case TCC_OPTION_shared:
                output_type = TCC_OUTPUT_DLL;
                break;
            case TCC_OPTION_o:
                multiple_files = 1;
                outfile = optarg;
                break;
            case TCC_OPTION_r:
                /* generate a .o merging several output files */
                reloc_output = 1;
                output_type = TCC_OUTPUT_OBJ;
                break;
            case TCC_OPTION_nostdinc:
                s->nostdinc = 1;
                break;
            case TCC_OPTION_print_search_dirs:
                print_search_dirs = 1;
                break;
#if (DO_C67)
            case TCC_OPTION_text:
				if (sscanf(optarg,"%x",&text_sect_addr)!=1) 
				{
					error("Invalid text address option");
				}
				if ((text_sect_addr & (~31)) != text_sect_addr)
				{
					error(".text linker section must be aligned to fetch packet (32 byte boundary)");
				}
                break;
            case TCC_OPTION_data:
				if (sscanf(optarg,"%x",&data_sect_addr)!=1) 
					error("Invalid data address option");
                break;
            case TCC_OPTION_bss:
				if (sscanf(optarg,"%x",&bss_sect_addr)!=1) 
					error("Invalid bss address option");
                break;
#endif
            default:
                break;
            }
        }
    }
 end_parse:
    if (print_search_dirs) {
        /* enough for Linux kernel */
        printf("install: %s/\n", tcc_lib_path);
        return 0;
    }

    nb_objfiles = nb_files - nb_libraries;

    /* if outfile provided without other options, we output an
       executable */
    if (outfile && output_type == TCC_OUTPUT_MEMORY)
        output_type = TCC_OUTPUT_EXE;

    /* check -c consistency : only single file handled. XXX: checks file type */
    if (output_type == TCC_OUTPUT_OBJ && !reloc_output) {
        /* accepts only a single input file */
        if (nb_objfiles != 1)
            error("cannot specify multiple files with -c");
        if (nb_libraries != 0)
            error("cannot specify libraries with -c");
    }
    
    /* compute default outfile name */
    if (output_type != TCC_OUTPUT_MEMORY && !outfile) {
        if (output_type == TCC_OUTPUT_OBJ && !reloc_output) {
            char *ext;
            /* add .o extension */
            pstrcpy(objfilename, sizeof(objfilename) - 1, files[0]);
            ext = strrchr(objfilename, '.');
            if (!ext)
                goto default_outfile;
            strcpy(ext + 1, "o");
        } else {
        default_outfile:
            pstrcpy(objfilename, sizeof(objfilename), "a.out");
        }
        outfile = objfilename;
    }

    if (do_bench) {
        start_time = getclock_us();
    }

    tcc_set_output_type(s, output_type);

#if (DO_C67)
	InitializeExternalC67Intrinsics();
#endif

    /* compile or add each files or library */
    for(i = 0;i < nb_files; i++) {
        const char *filename;

        filename = files[i];
        if (filename[0] == '-') {
            if (tcc_add_library(s, filename + 2) < 0)
                error("cannot find %s", filename);
        } else {
            if (tcc_add_file(s, filename) < 0) {
                ret = 1;
                goto the_end;
            }
        }
    }

    /* free all files */
    tcc_free(files);

    if (do_bench) {
        double total_time;
        total_time = (double)(getclock_us() - start_time) / 1000000.0;
        if (total_time < 0.001)
            total_time = 0.001;
        if (total_bytes < 1)
            total_bytes = 1;
        printf("%d idents, %d lines, %d bytes, %0.3f s, %d lines/s, %0.1f MB/s\n", 
               tok_ident - TOK_IDENT, total_lines, total_bytes,
               total_time, (int)(total_lines / total_time), 
               total_bytes / total_time / 1000000.0); 
    }

    if (s->output_type != TCC_OUTPUT_MEMORY) {
        tcc_output_file(s, outfile);
        ret = 0;
    } else {
        #if !DO_C67
        ret = tcc_run(s, argc - optind, argv + optind);
        #else
        ret = 0;
        #endif
    }
 the_end:
    /* XXX: cannot do it with bound checking because of the malloc hooks */
    if (!do_bounds_check)
        tcc_delete(s);

#ifdef MEM_DEBUG
    if (do_bench) {
        printf("memory: %d bytes, max = %d bytes\n", mem_cur_size, mem_max_size);
    }
#endif

#ifdef ASSEMBLY_LISTING_C67
	if (f)
		fclose(f);
#endif
    
	return ret;
}

#endif
