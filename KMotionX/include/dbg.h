//http://c.learncodethehardway.org/book/ex20.html
#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>


#ifdef KMX_DEBUG
#define debug(M, ...) fprintf(kmx_stderr, "[DEBUG] (%s::%s:%d:) " M "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#else
#define debug(M, ...)
#endif

#define __local__time__ time_t __now__ = time(NULL); struct tm *__timeinfo__; char __time_str__[80];__timeinfo__ = localtime(&__now__);strftime(__time_str__, sizeof(__time_str__), "%c", __timeinfo__)

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(kmx_stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(kmx_stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) if(1){__local__time__; fprintf(kmx_stderr, "[INFO] [%d] [%s] (%s:%d) " M "\n", (int)getpid(), __time_str__, __FILE__, __LINE__, ##__VA_ARGS__);}

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
