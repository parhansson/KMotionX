#ifndef KMOTIONX_CROSSOVER_H_
#define KMOTIONX_CROSSOVER_H_
#include "dbg.h"
#include <stdint.h>
#include <string>
#define OLD_COMPILER 1 // Select default compiler: 1 to use 0.9.16 tcc, else 0 to use later version

#if OLD_COMPILER
#define KMX_COMPILER "tcc67" // For now, port of the old compiler (based on 0.9.16) works properly
#else
#define KMX_COMPILER "c67-tcc" // Later version (0.9.26) but not yet fully working.
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
#define DLL_HELPER_API_IMPORT __attribute__((visibility("default")))
#define DLL_HELPER_API_EXPORT __attribute__((visibility("default")))
#define DLL_HELPER_API_LOCAL __attribute__((visibility("hidden")))
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
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
typedef unsigned int BOOL;

#ifndef M_PI
#define M_PI = 3.14159265358979323846264338327950288
#endif
#ifndef PI
#define PI M_PI
//	#define PI 3.141592653589793238
#endif
#define Sleep(X) usleep(1000 * X)
#define _hypot hypot

#define MAX_PATH 256
// This gives segmentation fault ???
// #include <limits.h>
// #define MAX_PATH PATH_MAX
#define PATH_SEPARATOR '/'
#define LINE_BREAK "\n"

extern char *_strupr(char *s);
extern char *_strlwr(char *s);
extern uint32_t GetTickCount();
extern uint32_t timeGetTime();
extern FILE *kmx_stderr;
extern FILE *kmx_stdout;
#else // not _KMOTIONX Probably Windows

// downloaded from http://msinttypes.googlecode.com/svn/trunk/stdint.h
#include "win/stdint.h"

#define PATH_SEPARATOR '\\'
#define LINE_BREAK "\r"

#endif

namespace kmx
{

  enum
  {
    BOARD_TYPE_KFLOP = 2,
    BOARD_TYPE_KOGNA = 3,
  };
  extern const char *getMachineDataPath();
  extern const char *getLibexecPath();
  extern const char *getResourcePath();
  extern int getResourceValue(const char *searchKey, char * result, size_t maxLen);
  // KMOTIONX_HOME when set, otherwise ~/.kmotionx. Not the configured install prefix.
  extern const char *getKMotionXHomePath();
  // Deprecated compatibility alias; use getKMotionXHomePath() instead.
  extern const char *getInstallPath();
  extern const char *getBinPath();
  extern const char *getLocalLanguageFilePath();
  extern int LaunchServer(bool noEthernet = false);
  extern int getDspFile(char *OutFile, const int BoardType);
  extern int getCompiler(char *Compiler, int MaxCompilerLen);
  extern int getCompileCommand(const wchar_t *Name, const wchar_t *OutFile, uint32_t LoadAddress, const int BoardType, char *command, int cmd_len);


  extern std::string format(const char* fmt, ...);
  extern std::wstring format(const wchar_t *wformat, ...);
  // extern std::string wstrtostr(std::wstring wideString);
  extern std::string wstrtostr(const std::wstring &wideString);
  // extern std::wstring strtowstr(std::string narrowString);
  extern std::wstring strtowstr(const std::string &narrowString);
  // Use to override default compiler executable.  options controls whether -g (and other) option supplied.
  // tcc_minor_version should be set to e.g. 26 for tcc version 0.9.26 (controls options), or 0 to
  // not change the version.
  // If compiler is absolute path, then that exact compiler is used.  Otherwise, it should just be the
  // name of the compiler without any path, and it will be searched for in standard locations.
  // If NULL, compiler is set back to default.

  extern void SetCustomCompiler(const char *compiler = NULL, const char *options = NULL, int tcc_minor_version = 0);
  extern void getPath(const char *file, char *path);
  extern long int getThreadId(const char *callerId = NULL);
  extern uint8_t randomInt();

}

#ifdef _KMOTIONX
// Windows CRT compatibility shims.
// They let code taken unchanged from the Windows original (KMotion) compile as is,
// so the original source lines do not have to be edited for every upgrade.
#include <errno.h>
#include <stdio.h>
#include <wchar.h>

// Like wcscpy_s, but truncates instead of calling an invalid parameter handler.
inline int wcscpy_s(wchar_t *dest, size_t destsz, const wchar_t *src)
{
  if (!dest || destsz == 0)
    return EINVAL;
  size_t i = 0;
  if (src)
    for (; i < destsz - 1 && src[i]; i++)
      dest[i] = src[i];
  dest[i] = 0;
  return (src && src[i]) ? ERANGE : 0;
}
inline int wcscpy_s(wchar_t *dest, size_t destsz, const std::wstring &src)
{
  return wcscpy_s(dest, destsz, src.c_str());
}

// Windows types used by the original source.
// CString (wide) and CStringA (narrow) with the MFC member functions the KMotion sources use
#include "CStringCompat.h"
typedef int64_t __int64;

// File names are handled as UTF-8 by the C library on Linux and macOS
inline FILE *_wfopen(const wchar_t *filename, const wchar_t *mode)
{
  return fopen(kmx::wstrtostr(filename).c_str(), kmx::wstrtostr(mode).c_str());
}

// Atomically stores value in *target and returns the previous value
inline void *InterlockedExchangePointer(void **target, void *value)
{
  return __atomic_exchange_n(target, value, __ATOMIC_SEQ_CST);
}

inline int _wremove(const wchar_t *filename)
{
  return remove(kmx::wstrtostr(filename).c_str());
}
inline int _wrename(const wchar_t *oldname, const wchar_t *newname)
{
  return rename(kmx::wstrtostr(oldname).c_str(), kmx::wstrtostr(newname).c_str());
}

// On Windows ccs=UTF-8 decodes the file. Here the wide stdio functions decode using LC_CTYPE, and in the
// default "C" locale UTF-8 is not decoded (macOS reads each byte as a character, glibc rejects non-ASCII).
// So make sure LC_CTYPE is a UTF-8 locale. Only character handling is changed, number formatting is not.
#include <clocale>
#include <mutex>
inline void kmxEnsureUtf8Locale()
{
  static std::once_flag once;
  std::call_once(once, []() {
    if (MB_CUR_MAX > 1)
      return; // already a multibyte locale
    static const char *names[] = {"C.UTF-8", "en_US.UTF-8", "UTF-8"};
    for (const char *n : names)
      if (setlocale(LC_CTYPE, n))
        return;
  });
}

// Unicode TCHAR version of fopen_s. The Microsoft mode flags "t" and ",ccs=UTF-8" have no meaning here,
// text is converted by the wide stdio functions (fwscanf, fgetws ...) using the locale.
// Like Windows with ccs=UTF-8, a UTF-8 byte order mark at the start of a file opened for reading is skipped.
#define _T(x) L##x
inline int _tfopen_s(FILE **file, const wchar_t *filename, const wchar_t *mode)
{
  std::string m = kmx::wstrtostr(mode);
  bool skipBom = m.find("ccs=") != std::string::npos && m[0] == 'r';
  if (m.find("ccs=") != std::string::npos)
    kmxEnsureUtf8Locale();
  m = m.substr(0, m.find(','));
  m.erase(std::remove(m.begin(), m.end(), 't'), m.end());
  *file = fopen(kmx::wstrtostr(filename).c_str(), m.c_str());
  if (!*file)
    return errno;
  if (skipBom)
  {
    unsigned char bom[3];
    if (fread(bom, 1, 3, *file) != 3 || bom[0] != 0xEF || bom[1] != 0xBB || bom[2] != 0xBF)
      rewind(*file);
  }
  return 0;
}
#endif // _KMOTIONX

#endif // KMOTIONX_CROSSOVER_H_
