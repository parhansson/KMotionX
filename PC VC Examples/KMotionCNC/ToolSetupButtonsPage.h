#pragma once
#include "ToolSetupPage.h"

// CToolSetupButtonsPage dialog

#define NUSERBUTTONS 10

class CToolSetupButtonsPage : public CToolSetupPage
{
	DECLARE_DYNAMIC(CToolSetupButtonsPage)

public:
	CToolSetupButtonsPage();
	virtual ~CToolSetupButtonsPage();

// Dialog Data
	enum { IDD = IDD_ToolSetupButtonsPage };
	CString	m_Button0;
	CString	m_Button2;
	CString	m_Button1;
	CString	m_Button3;
	CString	m_Button4;
	CString	m_Button5;
	CString	m_Button6;
	CString	m_Button7;
	CString	m_Button8;
	CString	m_Button9;
	CString	m_PS_M11;
	CString	m_PS_M12;
	CString	m_PS_M13;
	CString	m_PS_M14;
	CString	m_PS_M15;
	CString	m_PS_M16;
	CString	m_PS_M17;
	CString	m_PS_M18;
	CString	m_PS_M19;
	CString	m_PS_M20;
	MCODE_ACTION McodeActions[MAX_MCODE_ACTIONS_M1+MAX_MCODE_ACTIONS_BUTTONS];
	int UserButtonKeys[NUSERBUTTONS];
	BOOL InitDialogComplete;

protected:
	void SetAllActions();

	afx_msg void OnIhelp();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_MSG
	afx_msg void OnDirM11();
	afx_msg void OnDirM12();
	afx_msg void OnDirM13();
	afx_msg void OnDirM14();
	afx_msg void OnDirM15();
	afx_msg void OnDirM16();
	afx_msg void OnDirM17();
	afx_msg void OnDirM18();
	afx_msg void OnDirM19();
	afx_msg void OnDirM20();
	afx_msg void OnHelp();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnKillActive();
};
