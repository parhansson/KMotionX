// Frame.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#pragma once

#include "..\GCodeInterpreter\GCodeInterpreter.h"
#include "DlgX.h"
#include "KMotionCNCDlg.h"
#include "GViewDlg.h"
#include "IHelp.h"


/////////////////////////////////////////////////////////////////////////////
// CFrame dialog

class CFrame : public CDialog
{
// Construction
public:
	CFrame(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFrame();

	CKMotionCNCDlg GCodeDlg;
	CGViewDlg GViewDlg;
	CIHelp	HelpDlg;

	HICON m_hIcon;

	CString MainPathRoot;
	CString MainPath;

	CKMotionDLL *KMotionDLL;

// Dialog Data
	//{{AFX_DATA(CFrame)
	enum { IDD = IDD_Frame };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFrame)
	afx_msg void OnClose();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CFrame *TheFrame;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

