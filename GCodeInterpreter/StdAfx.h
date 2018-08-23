// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__ED44CA45_759C_4678_81E2_44D6A764A3B8__INCLUDED_)
#define AFX_STDAFX_H__ED44CA45_759C_4678_81E2_44D6A764A3B8__INCLUDED_


//#define DEBUG_DOWNLOAD  //enable this to capture download times
#ifdef _KMOTIONX
#include "GCodeInterpreterX.h"
#else

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER _WIN32_WINNT_WINXP 
#define _WIN32_WINNT _WIN32_WINNT_WINXP 
#define _WIN32_IE _WIN32_WINNT_WINXP 
#define NTDDI_VERSION NTDDI_WINXP

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_DEPRECATE 1
#define NO_WARN_MBCS_MFC_DEPRECATION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

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
#include "../KMotionX/include/KMotionX.h"
#include <locale.h>
#include "canon.h"
#include "rs274ngc.h"
#include "driver.h"
#include "PT2D.h"
#include "PT3D.h"
#include "KMotionDLL.h"
#include "TrajectoryPlanner.h"
#include "Kinematics.h"
#include "Kinematics3Rod.h"
#include "KinematicsGeppetto.h"
#include "KinematicsGeppettoExtrude.h"
#include "Kinematics5AxisGimbalAB.h"
#include "Kinematics5AxisGimbalCB.h"
#include "Kinematics5AxisTableAC.h"
#include "Kinematics5AxisTableBC.h"
#include "GCodeInterpreter.h"
#endif

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__ED44CA45_759C_4678_81E2_44D6A764A3B8__INCLUDED_)
