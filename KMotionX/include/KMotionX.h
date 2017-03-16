#ifndef KMOTIONX_CROSSOVER_H_
#define KMOTIONX_CROSSOVER_H_
#include "dbg.h"
#include <stdint.h>
#define OLD_COMPILER 1              // Select default compiler: 1 to use 0.9.16 tcc, else 0 to use later version

#if OLD_COMPILER
#define KMX_COMPILER "tcc67"        // For now, port of the old compiler (based on 0.9.16) works properly
#else
#define KMX_COMPILER "c67-tcc"    // Later version (0.9.26) but not yet fully working.
#endif


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
#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif
typedef unsigned int			BOOL;




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
//This gives segmentation fault ???
//#include <limits.h>
//#define MAX_PATH PATH_MAX
#define PATH_SEPARATOR '/'
#define LINE_BREAK "\n"

extern char* _strupr(char* s);
extern char* _strlwr(char* s);
extern uint32_t GetTickCount();
extern uint32_t timeGetTime();
#else // not _KMOTIONX Probably Windows

//downloaded from http://msinttypes.googlecode.com/svn/trunk/stdint.h
#include "win/stdint.h"

#define PATH_SEPARATOR '\\'
#define LINE_BREAK "\r"

#endif


namespace kmx {
  extern const char * getInstallPath();
  extern const char * getBinPath();
  extern int LaunchServer();
  extern int getDspFile(char * OutFile, bool KFLOP_board);
  extern int getCompiler(char * Compiler, int MaxCompilerLen);
  extern int getCompileCommand(const char * Name, const char * OutFile, uint32_t LoadAddress, bool KFLOP_board, char * command, int cmd_len);
  
  // Use to override default compiler executable.  options controls whether -g (and other) option supplied.
	// tcc_minor_version should be set to e.g. 26 for tcc version 0.9.26 (controls options), or 0 to
	// not change the version.
	// If compiler is absolute path, then that exact compiler is used.  Otherwise, it should just be the
	// name of the compiler without any path, and it will be searched for in standard locations.
	// If NULL, compiler is set back to default.

	extern void SetCustomCompiler(const char * compiler = NULL, const char * options = NULL, int tcc_minor_version = 0);   
  extern void getPath(const char * file, char * path);
}

#endif //KMOTIONX_CROSSOVER_H_
