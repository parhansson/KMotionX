// WriteCoff.c
//
// writes a COFF file that is suitable to be loaded and executed
//
//
//

#ifndef WIN32
//#include <CString.h>
#define BOOL int
#define TRUE 1
#define FALSE 0

#endif

int WriteCoff(TCCState *s1, const char *OutFile);
int ReadCoff(TCCState *s1, const char *InFile);
int InitializeExternalC67Intrinsics();

Section* FindSection(TCCState *s1, const char *sname);

extern int C67_main_entry_point;

