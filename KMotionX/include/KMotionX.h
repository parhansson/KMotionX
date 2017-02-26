#ifndef KMOTIONX_CROSSOVER_H_
#define KMOTIONX_CROSSOVER_H_
#include "dbg.h"


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the KMOTIONDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// KMOTIONDLL_API or GCODEINTERPRETER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
// Generic helper definitions for shared library support http://gcc.gnu.org/wiki/Visibility
#if defined _WIN32 || defined __CYGWIN__
  #define DLL_HELPER_API_IMPORT __declspec(dllimport)
  #define DLL_HELPER_API_EXPORT __declspec(dllexport)
  #define DLL_HELPER_API_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_HELPER_API_IMPORT __attribute__ ((visibility ("default")))
    #define DLL_HELPER_API_EXPORT __attribute__ ((visibility ("default")))
    #define DLL_HELPER_API_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_HELPER_API_IMPORT
    #define DLL_HELPER_API_EXPORT
    #define DLL_HELPER_API_LOCAL
	#endif
#endif

#ifdef _KMOTIONX

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

#define MAX_PATH 256
#define PATH_SEPARATOR '/'
#define LINE_BREAK "\n"

extern char* _strupr(char* s);
extern char* _strlwr(char* s);
extern long GetTickCount();
extern unsigned int timeGetTime();
#else // not _KMOTIONX Probably Windows

//downloaded from http://msinttypes.googlecode.com/svn/trunk/stdint.h
#include "win/stdint.h"

#define PATH_SEPARATOR '\\'
#define LINE_BREAK "\r"

#endif

#endif //KMOTIONX_CROSSOVER_H_
