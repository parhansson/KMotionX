// ScintillaFindReplaceDlg.h : header file
//
#if !defined(AFX_SCINTILLAFINDREPLACEDLG_H__8761CCEE_AAA2_4591_B20A_95B53154946C__INCLUDED_)
#define AFX_SCINTILLAFINDREPLACEDLG_H__8761CCEE_AAA2_4591_B20A_95B53154946C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CScintillaFindReplaceDlg dialog

class CScintillaFindReplaceDlg : public CFindReplaceDialog
{
public:
//Constructors / Destructors
  CScintillaFindReplaceDlg();

//Methods
  BOOL Create(BOOL bFindDialogOnly, LPCTSTR lpszFindWhat, LPCTSTR lpszReplaceWith = NULL, DWORD dwFlags = FR_DOWN, CWnd* pParentWnd = NULL);
  BOOL GetRegularExpression() const { return m_bRegularExpression; };
  void SetRegularExpression(BOOL bRegularExpression) { m_bRegularExpression = bRegularExpression; };

protected:
	//{{AFX_MSG(CScintillaFindReplaceDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
  afx_msg void OnRegularExpression();
  afx_msg void OnFindReplaceHelp();

  
//Member variables
  BOOL m_bRegularExpression;

	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCINTILLAFINDREPLACEDLG_H__8761CCEE_AAA2_4591_B20A_95B53154946C__INCLUDED_)
