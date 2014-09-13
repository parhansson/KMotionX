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
#include <locale.h>
#include "HiResTimer.h"
#include "KMotionApp.h"
#include "KMotionDLL.h"
#include "FTD2xx.h"
#include "KMotionIO.h"
#include "KMotionDLL_Direct.h"
#include "KMotionLocal.h"
#include "..\dsp_kmotion\pc-dsp.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__FB66C01F_579E_48D7_AEAB_9134F7125114__INCLUDED_)
