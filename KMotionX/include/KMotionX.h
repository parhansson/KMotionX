#ifndef KMOTIONX_CROSSOVER_H_
#define KMOTIONX_CROSSOVER_H_

#include "dbg.h"

#ifdef _WINDOWS
//downloaded from http://msinttypes.googlecode.com/svn/trunk/stdint.h
#include "win/stdint.h"

//#include <Shlwapi.h>
//#define strcasestr StrStrI
#define PATH_SEPARATOR '\\'
#define LINE_BREAK "\r"

#else // Not _WINDOWS
#include <ctype.h>
#include <MessageBox.h>
#ifndef ASSERT
	#include <assert.h>
	#define ASSERT(f) assert((f))
#endif


#ifndef M_PI
	#define M_PI = 3.14159265358979323846264338327950288
#endif
#ifndef PI
	#define PI M_PI
//	#define PI 3.141592653589793238
#endif
#define Sleep(X) usleep(1000*X)
#define _hypot hypot
#define __int64 int64_t

#define MAX_PATH 256
#define PATH_SEPARATOR '/'
#define LINE_BREAK "\n"

extern char* _strupr(char* s);
extern char* _strlwr(char* s);
extern long GetTickCount();
extern unsigned int timeGetTime();


#endif

#endif //KMOTIONX_CROSSOVER_H_
