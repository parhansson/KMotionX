// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#if !defined(AFX_STDAFX_H__30618C2B_E9A8_4042_8618_6371E1EB8333__INCLUDED_)
#define AFX_STDAFX_H__30618C2B_E9A8_4042_8618_6371E1EB8333__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define KMOTION_CNC

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
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
#include <afxtempl.h>
#include <gl\gl.h>
#include <gl\glu.h>
//#include <GL\glaux.h>

#include "Lib3d\Base3d.h"


//Pull in support for Scintilla
#include <platform.h>
#include <scintilla.h>
#include <SciLexer.h>
#include "ScintillaFindReplaceDlg.h"
#include "ScintillaCtrl.h"

#include <conio.h>
#include <float.h>
#include <math.h>
#include <mmsystem.h>
#include <locale.h>
#include <sys\stat.h>

#include "resource.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__30618C2B_E9A8_4042_8618_6371E1EB8333__INCLUDED_)
