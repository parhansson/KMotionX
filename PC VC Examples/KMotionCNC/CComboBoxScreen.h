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
	void SetFont(const char *szFaceName, int height, bool Bold, bool Italic);
	void InsertItemW(CStringW p);
	CStringW ToolTipText;
	CStringW PrevWindowText;
	int Var;
	void Reset();
	void GetPersistText(void);
	CStringW Part(int n, CStringW p, bool & Done);
	void SetTextAndDropDown(CStringW s);
	void ResetAll();
	CStringW GetWText();
	void SetWText(CStringW w);
	int GetID();
	CString GetIDName();
	static CList <LPCComboBoxScreen, LPCComboBoxScreen> ComboBoxScreens;

	void FixMyComboboxExTip(CStringW Text);


protected:
	CFont m_font;
	int CachedID;
	CString CachedIDName;
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


