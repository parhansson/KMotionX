// KMotionCNC.h : main header file for the KMOTIONCNC application
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined(AFX_KMOTIONCNC_H__9FAD4F05_99A9_465F_A423_B89A473361D2__INCLUDED_)
#define AFX_KMOTIONCNC_H__9FAD4F05_99A9_465F_A423_B89A473361D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKMotionCNCApp:
// See KMotionCNC.cpp for the implementation of this class
//

class CKMotionCNCApp : public CWinApp
{
public:
	CKMotionCNCApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKMotionCNCApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKMotionCNCApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KMOTIONCNC_H__9FAD4F05_99A9_465F_A423_B89A473361D2__INCLUDED_)
