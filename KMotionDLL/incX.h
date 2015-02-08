/*
 * incX.h
 *
 *  Created on: 19 sep 2014
 *      Author: parhansson
 */

#ifndef INCX_H_
#define INCX_H_

#define timeBeginPeriod(x)
#define timeEndPeriod(x)

//#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
//#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
//#include <ctype.h>

//#include <CString.h>
#include <WinTypes.h>
#include <CMutex.h>

/*
#ifndef __EXCEPTIONS
// Iff -fno-exceptions, transform error handling code to work without it.
# define __try      if (true)
# define __catch(X) if (false)
# define __throw_exception_again
#else
// Else proceed normally.
# define __try      try
# define __catch(X) catch(X)
# define __throw_exception_again throw
#endif
*/

//#ifndef __try
//#define __try
//#endif
//#define __finally

#include <locale.h>
#include <HiResTimer.h>

#include "KMotionDLL.h"
#ifdef LIB_FTDI
#include <ftdi.h>
#else
#include <ftd2xx.h>
#endif
#include "KmotionIO.h"
#include "KMotionDLL_Direct.h"
#include "KMotionLocal.h"
#include "../DSP_KMotion/PC-DSP.h"


#endif /* INCX_H_ */

