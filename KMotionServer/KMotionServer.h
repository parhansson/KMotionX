// KMotionServer.h : main header file for the KMOTIONSERVER application
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined(AFX_KMOTIONSERVER_H__D55A2D14_C558_4180_AA7C_DBBEBA3D9553__INCLUDED_)
#define AFX_KMOTIONSERVER_H__D55A2D14_C558_4180_AA7C_DBBEBA3D9553__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerApp:
// See KMotionServer.cpp for the implementation of this class
//

class CKMotionServerApp : public CWinApp
{
public:
	CKMotionServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKMotionServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CKMotionServerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KMOTIONSERVER_H__D55A2D14_C558_4180_AA7C_DBBEBA3D9553__INCLUDED_)
