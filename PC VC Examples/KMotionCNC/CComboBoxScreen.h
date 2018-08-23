/*
	CComboBoxScreen

	CComboBox extended for custom Fonts

	Copyright:Dynomotion, Inc. 2016
*/

#pragma once


class CComboBoxScreen;
typedef CComboBoxScreen *LPCComboBoxScreen;


class CComboBoxScreen : public CComboBox
{
	//	DECLARE_DYNAMIC(CComboBoxScreen)

public:

	CComboBoxScreen();
	virtual ~CComboBoxScreen();
	void Reset();
	void SetFont(const char *szFaceName, int height, bool Bold, bool Italic);
	CStringW ToolTipText;
	int Var;

	static CList <LPCComboBoxScreen, LPCComboBoxScreen> ComboBoxScreens;




protected:
	CFont m_font;
	// Overrides
	 // ClassWizard generated virtual function overrides
	 //{{AFX_VIRTUAL(CComboBoxScreen)
public:
	//}}AFX_VIRTUAL
  // Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxScreen)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


