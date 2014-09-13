// RichEditCtrlEx.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#if !defined(AFX_RICHEDITCTRLEX_H__1FAF8777_42B9_4E83_88CA_62478A88F92D__INCLUDED_)
#define AFX_RICHEDITCTRLEX_H__1FAF8777_42B9_4E83_88CA_62478A88F92D__INCLUDED_

#include "TransformDlg.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_FLAG_LINES 100

enum { MODE_C, MODE_G, MODE_CMD };
 

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx window

class CRichEditCtrlEx : public CScintillaCtrl
{
// Construction
public:
	CRichEditCtrlEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	CTransformDlg TransformDlg;
	DWORD m_t0_esc_time;
	bool m_esc_pushed;
	bool m_SingleLineOnly;
	bool escape_time();
	bool ShiftIsDown;
	bool CtrlIsDown;
	int  ModeCode;
	void SetupForCCode();
	void SetupForGCode();
	void SetupForCMD();
	void SetAStyle(int style, COLORREF fore, COLORREF back=RGB(0xff, 0xff, 0xff), int size=-1, const char *face=0);
	virtual ~CRichEditCtrlEx();



	void AdjustFindDialogPosition();
	void OnEditFindReplace(BOOL bFindOnly);
	void OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
	void OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase,	BOOL bWord, BOOL bRegularExpression, LPCTSTR lpszReplace);
	void OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
	BOOL SameAsSelected(LPCTSTR lpszCompare, BOOL bCase, BOOL /*bWord*/, BOOL bRegularExpression);
	BOOL FindTextSimple(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
	long FindAndSelect(DWORD dwFlags, TextToFind& ft);
	void TextNotFound();
	BOOL FindTextView(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);


	CScintillaFindReplaceDlg* m_pFindReplaceDlg;    //find or replace dialog
	BOOL                      m_bFindOnly;          //Is pFindReplace the find or replace?
	CString                   m_strFind;            //last find string
	CString                   m_strReplace;         //last replace string
	BOOL                      m_bCase;              //TRUE==case sensitive, FALSE==not
	int                       m_bNext;              //TRUE==search down, FALSE== search up
	BOOL                      m_bWord;              //TRUE==match whole word, FALSE==not
	BOOL                      m_bFirstSearch;       //Is this the first search
	BOOL                      m_bRegularExpression; //TRUE== regular expression, FALSE==not

	int DoAutoComplete(CString word, CString &List, CString WordList);
	int CheckMatch(CString word, CString v, CString &List);
	bool iswhitespace(char s);
	bool isalpha_numeric(char s);
	bool arrow_key(char s);

	CList<CString,CString> GlobalFuncsNames;
	CList<CString,CString> GlobalFuncsDefs;

	void CreateGlobalFunctionDefinitions(CString fname) ;

	CString FindTokenBackwards(CString s, int r);
	CString CRichEditCtrlEx::WhitespaceToSpace(CString s);
	int CheckForChars(CString s, CString chs);


	// Generated message map functions
protected:
	CString m_SpecialContext;
	CPoint ContextPoint;       // point where context menu was clicked

	//{{AFX_MSG(CRichEditCtrlEx)
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnDelete();
	afx_msg void OnSelectAll();
	afx_msg void OnFind();
	afx_msg void OnReplace();
	afx_msg void OnSpecialContext();
	afx_msg void OnTransformSel();
	afx_msg void OnShowLineNumbers();
    afx_msg LONG OnFindReplaceCmd(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHEDITCTRLEX_H__1FAF8777_42B9_4E83_88CA_62478A88F92D__INCLUDED_)
