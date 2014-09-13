#if !defined(AFX_SETVALUE_H__CCD5E1CE_62DC_4212_B3B3_55F1C42A0C0B__INCLUDED_)
#define AFX_SETVALUE_H__CCD5E1CE_62DC_4212_B3B3_55F1C42A0C0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetValue.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetValue dialog

class CSetValue : public CDialog
{
// Construction
public:
	double m_Value;
	CList<CString, CString> Recent;
	CSetValue(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetValue)
	enum { IDD = IDD_SetValue };
	CComboBox	m_ValueCombo;
	CString	m_ValueString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetValue)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetValue)
	afx_msg void OnClose();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETVALUE_H__CCD5E1CE_62DC_4212_B3B3_55F1C42A0C0B__INCLUDED_)
