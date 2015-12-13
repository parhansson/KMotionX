// mysheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMySheet
#pragma once
class CMySheet : public CPropertySheet
{
protected:
//	void BuildPropPageArray();
	RECT m_rctPage;
	CFont m_fntPage;

	DECLARE_DYNAMIC(CMySheet)

// Construction
public:
	CMySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySheet)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMySheet();

	// Generated message map functions
protected:
	virtual void BuildPropPageArray ();
	//{{AFX_MSG(CMySheet)
	//}}AFX_MSG
	afx_msg LONG OnResizePage (UINT, LONG);
	afx_msg void OnHelp();
	DECLARE_MESSAGE_MAP()

	bool m_KeepCancelButton;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

/////////////////////////////////////////////////////////////////////////////
