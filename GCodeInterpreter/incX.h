/*
 * incX.h
 *
 *  Created on: 19 sep 2014
 *      Author: parhansson
 */

#ifndef INCX_H_
#define INCX_H_
#include <pthread.h>
#include <WinTypes.h>
#include <MessageBox.h>
#include <CMutex.h>
#include <CString.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>


#ifndef ASSERT
	#include <assert.h>
	#define ASSERT(f) assert((f))
#endif

#ifndef M_PI
	M_PI = 3.14159265358979323846264338327950288
#endif
#ifndef PI
	#define PI M_PI
//	#define PI 3.141592653589793238
#endif

#define Sleep(X) usleep(1000*X)
#define min(X,Y) fmin(X,Y)
#define MAX_PATH 256
#define FALSE 0
#define TRUE 1
#define _hypot hypot
#define __int64 int64_t



#endif /* INCX_H_ */
