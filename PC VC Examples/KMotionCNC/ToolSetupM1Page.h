#pragma once
#include "ToolSetupPage.h"

// CToolSetupM1Page dialog

class CToolSetupM1Page : public CToolSetupPage
{
	DECLARE_DYNAMIC(CToolSetupM1Page)

public:
	CToolSetupM1Page();
	virtual ~CToolSetupM1Page();

// Dialog Data
	enum { IDD = IDD_ToolSetupM1Page };
	CString	m_PS_M3;
	CString	m_PS_M4;
	CString	m_PS_M5;
	CString	m_PS_M6;
	CString	m_PS_M7;
	CString	m_PS_M8;
	CString	m_PS_M9;
	CString	m_PS_M10;
	MCODE_ACTION McodeActions[MAX_MCODE_ACTIONS_M1];
	BOOL InitDialogComplete;

protected:
	void SetAllActions();

	afx_msg void OnIhelp();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnDirM3();
	afx_msg void OnDirM4();
	afx_msg void OnDirM5();
	afx_msg void OnDirM6();
	afx_msg void OnDirM7();
	afx_msg void OnDirM8();
	afx_msg void OnDirM9();
	afx_msg void OnDirM10();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnKillActive();
};
