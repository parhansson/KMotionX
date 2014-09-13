// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__FB66C01F_579E_48D7_AEAB_9134F7125114__INCLUDED_)
#define AFX_STDAFX_H__FB66C01F_579E_48D7_AEAB_9134F7125114__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_DEPRECATE
#define WINVER 0x0600 
#define _WIN32_IE 0x0600 

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#include <afxpriv.h>		// for AfxLoadString
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxmt.h>

#include <conio.h>
#include <math.h>
#include <mmsystem.h>
#ifndef _WINDOWS
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

#include <CString.h>
#include <WinTypes.h>
#include <MessageBox.h>
#include <CMutex.h>

#ifndef ASSERT
	#include <assert.h>
	#define ASSERT(f) assert((f))
#endif

#define SOCK_PATH "kmotionsocket"

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
#define __try
//#endif
#define __finally
#define Sleep(X) usleep(1000*X)
#define timeBeginPeriod(x)
#define timeEndPeriod(x)

#define MAX_PATH 256

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

extern char* _strupr(char* s);
extern unsigned int timeGetTime();

#endif // _WINDOWS

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__FB66C01F_579E_48D7_AEAB_9134F7125114__INCLUDED_)
