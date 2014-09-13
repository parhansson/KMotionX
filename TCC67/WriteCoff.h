// WriteCoff.c
//
// writes a COFF file that is suitable to be loaded and executed
//
//
//
int WriteCoff(TCCState *s1, const char *OutFile);
int ReadCoff(TCCState *s1, const char *InFile);
int InitializeExternalC67Intrinsics();

Section* FindSection(TCCState *s1, CString sname);

extern int C67_main_entry_point;