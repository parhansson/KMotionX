/*
	CComboBoxScreen

	CComboBox extended for custom Fonts

	Copyright:Dynomotion, Inc. 2016
*/

#pragma once


class CComboBoxScreen;
typedef CComboBoxScreen *LPCComboBoxScreen;


class CComboBoxScreen : public CComboBoxEx
{
	//	DECLARE_DYNAMIC(CComboBoxScreen)

public:

	CComboBoxScreen();
	virtual ~CComboBoxScreen();
	void SetFont(const wchar_t *szFaceName, int height, bool Bold, bool Italic);
	void InsertItem(CString p);
	CString ToolTipText;
	CString PrevWindowText;
	int Var;
	void Reset();
	void GetPersistText(void);
	CString Part(int n, CString p, bool & Done);
	void SetTextAndDropDown(CString s);
	void ResetAll();
	CString GetWText();
	void SetWText(CString w);
	int GetID();
	CString GetIDName();
	static CList <LPCComboBoxScreen, LPCComboBoxScreen> ComboBoxScreens;

	void FixMyComboboxExTip(CString Text);
	void SetColors(COLORREF Text, COLORREF Back, COLORREF SelText, COLORREF SelBack);  // CLR_DEFAULT = standard color


protected:
	CFont m_font;
	int CachedID;
	CString CachedIDName;
	bool m_CustomColors;
	COLORREF m_TextColor, m_BackColor, m_SelTextColor, m_SelBackColor;
	CBrush m_BackBrush;
	bool m_DarkTheme;  // Windows dark theme applied to the arrow button, border and scroll bar
	LRESULT CtlColor(HDC hDC);
	void DrawColoredItem(LPDRAWITEMSTRUCT d);
	static LRESULT CALLBACK InnerComboProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	// Overrides
	 // ClassWizard generated virtual function overrides
	 //{{AFX_VIRTUAL(CComboBoxScreen)
public:
	//}}AFX_VIRTUAL
  // Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxScreen)
	//}}AFX_MSG
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


