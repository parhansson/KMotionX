#pragma once
#include "..\GCodeInterpreter\GCodeInterpreter.h"

// CToolSetupPage dialog

#define MAX_STATUS 100


class CToolSetupPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CToolSetupPage)

public:
	CToolSetupPage(UINT IDD);
	virtual ~CToolSetupPage();

protected:
	
	virtual BOOL OnInitDialog();
	afx_msg void OnIhelp();
	afx_msg void OnKMotionClose();

	int VerifyIntegerRange(double value,int min,int max, CString s, CString MCode);
	int VerifyAction(int i);
	void DoDirectoryBrowse(MCODE_ACTION *m);
	void SetAction(MCODE_ACTION *M, int ID_Action, 
						  int ID_S1, int ID_S2, int ID_S3, int ID_S4, int ID_S5, int ID_Dir,
						  int ID_P1, int ID_P2, int ID_P3, int ID_P4, int ID_P5, int ID_PS);

	BOOL CheckThreadNo(int ControlID, MCODE_ACTION *Action);

	virtual void SetAllActions() {};

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	HBRUSH m_BackgroundBrush;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCloseupAction();
protected:
};
