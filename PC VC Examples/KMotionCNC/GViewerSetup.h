// GViewerSetup.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#if !defined(AFX_GViewerSetup_H__B214FDDD_CAE9_412F_84BA_A4EB6886B322__INCLUDED_)
#define AFX_GViewerSetup_H__B214FDDD_CAE9_412F_84BA_A4EB6886B322__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGViewerSetup dialog

class CGViewerSetup : public CDialog
{
// Construction
public:
	CGViewerSetup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGViewerSetup)
	enum { IDD = IDD_GViewerSetup };
	CString	m_ToolShapeFile;
	double	m_BoxX;
	double	m_BoxY;
	double	m_BoxZ;
	double	m_BoxOffsetX;
	double	m_BoxOffsetY;
	double	m_BoxOffsetZ;
	float	m_AxisSize;
	float	m_ToolSize;
	float	m_ToolOffX;
	float	m_ToolOffY;
	float	m_ToolOffZ;
	BOOL m_IncludeA;
	BOOL m_IncludeB;
	BOOL m_IncludeC;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGViewerSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGViewerSetup)
	afx_msg void OnBrowseToolShapeFile();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLFILE_H__B214FDDD_CAE9_412F_84BA_A4EB6886B322__INCLUDED_)
