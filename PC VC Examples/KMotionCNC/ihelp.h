// IHelp.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#pragma once
#include "webbrowser.h"
#include "DlgX.h"

/////////////////////////////////////////////////////////////////////////////
// CIHelp dialog

class CIHelp : public CDlgX
{
// Construction
public:
	int Show(CString s);
	CIHelp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIHelp)
	enum { IDD = IDD_Help };
	CWebBrowser	m_Web;
	//}}AFX_DATA

	CString Contents;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIHelp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIHelp)
	afx_msg void OnBack();
	afx_msg void OnForward();
	afx_msg void OnPrintBrowser();
	afx_msg void OnCommandStateChangeExplorer1(long Command, BOOL Enable);
	afx_msg void OnPaint();
	afx_msg void OnContents();
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CBitmapButton m_BUTTON_Close;
	CBitmapButton m_BUTTON_Print;
	CBitmapButton m_Forward;
	CBitmapButton m_Contents;
	CBitmapButton m_Back;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

