/****************************************************************************/
/*  PARAMS.H  - Target dependent parameters.                                */
/****************************************************************************/

#if KMotionBd
typedef unsigned long  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned char  T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned long  T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */

#define MAGIC 194                       /* Kmotion/Code Composer            */

#define LOCTOBYTE(x)   x                /* RR addresses are same as bytes  */
#define BYTETOLOC(x)   x
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#endif


#if GSP
typedef unsigned long  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned short T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned short T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */

#define MAGIC         0x90
#define BYTETOLOC(x)  ((T_ADDR)(x)<<3)  /* CONVERT ADDRESSES TO BYTES       */
#define LOCTOBYTE(x)  ((x)>>3)          /* CONVERT BYTES TO ADDRESSES       */
#define BIT_OFFSET(a) ((a)&7)           /* BIT OFFSET OF ADDR WITHIN BYTE   */

#define LOADBUFSIZE   0x1000            /* 16K BUFFER FOR LOADING DATA      */
#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#endif


#if RR
typedef unsigned short T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned char  T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned char  T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */

#define MAGIC 0221                      /* RR Magic number                 */

#define LOCTOBYTE(x)   x                /* RR addresses are same as bytes  */
#define BYTETOLOC(x)   x
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#endif


#if BRAHMA
typedef unsigned long  T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned long  T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned long  T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */

#define MAGIC 0223                      /* Magic number for BRAHMA         */

#define LOCTOBYTE(x)   ((x)<<2)         /* BRAHMA word addrs to byte addrs */
#define BYTETOLOC(x)   ((x)>>2)         /* byte addrs to word addrs        */
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#define LOADBUFSIZE   0x0FFC            /* 4K-4 BUFFER FOR LOADING DATA     */
#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#endif


#if DSP320
typedef unsigned short T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned short T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned short T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */

#define MAGIC 0222                      /* Magic number for HIMIKO         */

#define LOCTOBYTE(x)   ((x)<<1)         /* 16-bit word addrs to byte addrs */
#define BYTETOLOC(x)   ((x)>>1)         /* byte addrs to word addrs        */
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#endif


#if EAGLE
typedef unsigned short T_ADDR;          /* TYPE FOR TARGET ADDRESS          */
typedef unsigned short T_DATA;          /* TYPE FOR TARGET DATA WORD        */
typedef unsigned short T_SIZE;          /* TYPE FOR CINIT SIZE FIELD        */

#define MAGIC 0224                      /* Magic number for EAGLE          */

#define LOCTOBYTE(x)   x                /* 16-bit byte addrs to byte addrs */
#define BYTETOLOC(x)   x                /* byte addrs to byte addrs        */
#define BIT_OFFSET(a)  (0)              /* BIT OFFSET OF ADDR WITHIN BYTE   */

#define LOADBUFSIZE   0x4000            /* 16K BUFFER FOR LOADING DATA      */
#define LOADWORDSIZE  2                 /* MINIMUM DIVISOR OF LOAD SIZE     */
#define CINIT         ".cinit"          /* NAME OF CINIT SECTION            */
#endif
