// KMotionApp.h : main header file for the KMotion DLL
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined(AFX_KMotion_H__9AF407A5_0FAA_4568_A58F_8ACD14A7E7C5__INCLUDED_)
#define AFX_KMotion_H__9AF407A5_0FAA_4568_A58F_8ACD14A7E7C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKMotionApp
// See KMotion.cpp for the implementation of this class
//

class CKMotionApp : public CWinApp
{
public:
	CKMotionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKMotionApp)
	public:
	virtual BOOL InitApplication();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CKMotionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KMotion_H__9AF407A5_0FAA_4568_A58F_8ACD14A7E7C5__INCLUDED_)
