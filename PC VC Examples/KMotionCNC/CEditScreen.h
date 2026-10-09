/*
	CEditScreen

	CEdit extended for custom Fonts

	Copyright:Dynomotion, Inc. 2016
*/

#pragma once


class CEditScreen;
typedef CEditScreen *LPCEditScreen;


class CEditScreen : public CEdit
{
	//	DECLARE_DYNAMIC(CEditScreen)

public:

	CEditScreen();
	virtual ~CEditScreen();
	void SetFont(const wchar_t *szFaceName, int height, bool Bold, bool Italic);
	CString ToolTipText;
	CString PrevWindowText;
	int Var;
	void Reset();
	void GetPersistText(void);
	static int SavePersists(void);
	CString GetWText();
	void SetWText(CString w);
	static bool PersistDirty;
	int GetID();
	CString GetIDName();
	static CList <LPCEditScreen, LPCEditScreen> EditScreens;
	void SetColors(COLORREF Text, COLORREF Back);  // CLR_DEFAULT = standard color
	HBRUSH CtlColor(CDC *pDC);



protected:
	CFont m_font;
	int CachedID;
	CString CachedIDName;
	COLORREF m_TextColor, m_BackColor;
	CBrush m_BackBrush;
	bool m_DarkTheme;  // Windows dark theme applied to the border and scroll bar
	// Overrides
	 // ClassWizard generated virtual function overrides
	 //{{AFX_VIRTUAL(CEditScreen)
public:
	//}}AFX_VIRTUAL
  // Generated message map functions
protected:
	//{{AFX_MSG(CEditScreen)
	//}}AFX_MSG
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};


