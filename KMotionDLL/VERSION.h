/**********************************************************************/
/* VERSION.H                                                          */
/*                                                                    */
/* This file is included by all modules of all tools.  All            */
/* portability flags and version information is to appear HERE ONLY.  */
/*                                                                    */
/**********************************************************************/

#define TIPC        1     /* FOR MS-DOS 2.0+ ON IBM OR TIPC */
#define VAX_VMS     0     /* FOR VAX VMS 3.5                */
#define VAX_ULTRIX  0     /* FOR VAX ULTRIX OR BERKLEY 4.2  */
#define SV_unix     0     /* FOR SYSTEM V UNIX              */
#define MACINTOSH   0     /* FOR APPLE MACINTOSH OR MAC II  */

#define KMotionBd     1           /* KMotion/Code Composer        */
#define GSP     0               /* GSP                          */
#define RR      0               /* Roadrunner                   */
#define BRAHMA  0               /* Brahma                       */
#define DSP320  0               /* Shiva, Himiko                */
#define EAGLE   0               /* Eagle                        */

#if KMotion
#define VERSION   ""
#define COPYRITE  ""
#define VSTAMP    121             /* version stamp for output file */
#endif

#if GSP
#define VERSION   "Version 3.00.28"
#define COPYRITE  "(c) Copyright 1985, 1988, Texas Instruments Incorporated \n"
#define VSTAMP    300             /* version stamp for output file */
#endif

#if RR
#define VERSION   "Version 3.30"
#define COPYRITE  "(c) Copyright 1986, 1988, Texas Instruments Incorporated \n"
#define VSTAMP    330             /* version stamp for output file */
#endif

#if BRAHMA
#define VERSION   "Version 1.21.01"
#define COPYRITE  "(c) Copyright 1987, 1988, Texas Instruments Incorporated \n"
#define VSTAMP    121             /* version stamp for output file */
#endif

#if DSP320
#define VERSION   "Version 5.04.05"
#define COPYRITE  "(c) Copyright 1987, 1988, Texas Instruments Incorporated \n"
#define VSTAMP    504             /* version stamp for output file */
#endif

#if EAGLE
#define VERSION   "Version 0.00.02"
#define COPYRITE  "(c) Copyright 1988, Texas Instruments Incorporated \n"
#define VSTAMP    000             /* version stamp for output file */
#endif

