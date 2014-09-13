// TransformDlg.h
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#if !defined(AFX_TRANSFORMDLG_H__25AC65BF_BF4A_45E2_9212_B08F92BD42BD__INCLUDED_)
#define AFX_TRANSFORMDLG_H__25AC65BF_BF4A_45E2_9212_B08F92BD42BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransformDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransformDlg dialog

class CTransformDlg : public CDialog
{
// Construction
public:
	int DoTransform(CString &s);
	CTransformDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTransformDlg)
	enum { IDD = IDD_Transform };
	double	m_OffsetX;
	double	m_OffsetY;
	double	m_OffsetZ;
	double	m_ScaleX;
	double	m_ScaleY;
	double	m_ScaleZ;
	UINT	m_Digits;
	BOOL	m_ScaleIJ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransformDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int ProcessVar(CString &t, CString v, double scale, double offset);
	// Generated message map functions
	//{{AFX_MSG(CTransformDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFORMDLG_H__25AC65BF_BF4A_45E2_9212_B08F92BD42BD__INCLUDED_)
