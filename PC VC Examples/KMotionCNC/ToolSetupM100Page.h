#pragma once
#include "ToolSetupPage.h"

// CToolSetupM100Page dialog

class CToolSetupM100Page : public CToolSetupPage
{
	DECLARE_DYNAMIC(CToolSetupM100Page)

public:
	CToolSetupM100Page();
	virtual ~CToolSetupM100Page();

// Dialog Data
	enum { IDD = IDD_ToolSetupM100Page };
	CString	m_PS_M100;
	CString	m_PS_M101;
	CString	m_PS_M102;
	CString	m_PS_M103;
	CString	m_PS_M104;
	CString	m_PS_M105;
	CString	m_PS_M106;
	CString	m_PS_M107;
	CString	m_PS_M108;
	CString	m_PS_M109;
	CString	m_PS_M110;
	CString	m_PS_M111;
	CString	m_PS_M112;
	CString	m_PS_M113;
	CString	m_PS_M114;
	CString	m_PS_M115;
	CString	m_PS_M116;
	CString	m_PS_M117;
	CString	m_PS_M118;
	CString	m_PS_M119;
	MCODE_ACTION McodeActions[MAX_MCODE_ACTIONS_M100];
	BOOL InitDialogComplete;

protected:
	void SetAllActions();

	afx_msg void OnIhelp();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnDirM100();
	afx_msg void OnDirM101();
	afx_msg void OnDirM102();
	afx_msg void OnDirM103();
	afx_msg void OnDirM104();
	afx_msg void OnDirM105();
	afx_msg void OnDirM106();
	afx_msg void OnDirM107();
	afx_msg void OnDirM108();
	afx_msg void OnDirM109();
	afx_msg void OnDirM110();
	afx_msg void OnDirM111();
	afx_msg void OnDirM112();
	afx_msg void OnDirM113();
	afx_msg void OnDirM114();
	afx_msg void OnDirM115();
	afx_msg void OnDirM116();
	afx_msg void OnDirM117();
	afx_msg void OnDirM118();
	afx_msg void OnDirM119();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnKillActive();
};
