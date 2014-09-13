// ToolSetupPage.cpp : implementation file
//

#include "stdafx.h"
#include "ToolSetupPage.h"
#include "MainFrm.h"
#include "OpenDlg.h"



// CToolSetupPage dialog

IMPLEMENT_DYNAMIC(CToolSetupPage, CPropertyPage)

CToolSetupPage::CToolSetupPage(UINT IDD)
	: CPropertyPage(IDD)
{
}

CToolSetupPage::~CToolSetupPage()
{
}


BOOL CToolSetupPage::OnInitDialog()
{
	m_BackgroundBrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	return CPropertyPage::OnInitDialog();
}

BEGIN_MESSAGE_MAP(CToolSetupPage, CPropertyPage)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_KMotion_Close, OnKMotionClose)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


HBRUSH CToolSetupPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}



void CToolSetupPage::OnKMotionClose() 
{
	GetParent()->SendMessage(WM_CLOSE);
}


BOOL CToolSetupPage::OnEraseBkgnd(CDC* pDC)
{
	return CPropertyPage::OnEraseBkgnd(pDC);
}

BOOL CToolSetupPage::CheckThreadNo(int ControlID, MCODE_ACTION *Action)
{
	if ((Action->Action == M_Action_Program || 
		 Action->Action == M_Action_Program_wait || 	
		 Action->Action == M_Action_Program_wait_sync) &&
		 (Action->dParams[0] < 1.0 || Action->dParams[0] > 7.0))
	{
		GetDlgItem(ControlID)->SetFocus();
		((CEdit*)GetDlgItem(ControlID))->SetSel(0,-1);		
		AfxMessageBox("Invalid User Thread Number");
		return TRUE;
	}

	return FALSE;
}

void CToolSetupPage::SetAction(MCODE_ACTION *M, int ID_Action, 
						  int ID_S1, int ID_S2, int ID_S3, int ID_S4, int ID_S5, int ID_Dir,
						  int ID_P1, int ID_P2, int ID_P3, int ID_P4, int ID_P5, int ID_PS)
{
	if (M->Action < 0) M->Action=0;

	switch (M->Action)
	{
	case M_Action_None:
	case M_Action_Callback:
		GetDlgItem(ID_S1)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S2)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S3)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P1)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P3)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_PS)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_Dir)->ShowWindow(SW_HIDE);
		break;

	case M_Action_Setbit:
		SetDlgItemText(ID_S1,"Set bit");
		SetDlgItemText(ID_S2,"to");
		GetDlgItem(ID_S1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S3)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P3)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_PS)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_Dir)->ShowWindow(SW_HIDE);
		break;

	case M_Action_Waitbit:
		SetDlgItemText(ID_S1,"Wait bit");
		SetDlgItemText(ID_S2,"till");
		GetDlgItem(ID_S1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S3)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P3)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_PS)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_Dir)->ShowWindow(SW_HIDE);
		break;

	case M_Action_SetTwoBits:
		SetDlgItemText(ID_S1,"Set bit");
		SetDlgItemText(ID_S2,"to");
		SetDlgItemText(ID_S3,"&& bit");
		SetDlgItemText(ID_S4,"to");
		GetDlgItem(ID_S1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S3)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S4)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P3)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P4)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_PS)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_Dir)->ShowWindow(SW_HIDE);
		break;

	case M_Action_DAC:
		SetDlgItemText(ID_S1,"Set DAC");
		SetDlgItemText(ID_S2,"scale");
		SetDlgItemText(ID_S3,"offset");
		SetDlgItemText(ID_S4,"min");
		SetDlgItemText(ID_S5,"max");
		GetDlgItem(ID_S1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S3)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S4)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S5)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P3)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P4)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P5)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_PS)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_Dir)->ShowWindow(SW_HIDE);
		break;



	case M_Action_Program:
	case M_Action_Program_wait:
	case M_Action_Program_wait_sync:
		SetDlgItemText(ID_S1,"Thread");
		SetDlgItemText(ID_S2,"VAR");
		SetDlgItemText(ID_S3,"C File");
		GetDlgItem(ID_S1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S3)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_P3)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_PS)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_Dir)->ShowWindow(SW_SHOW);
		break;
	case M_Action_Program_PC:
		SetDlgItemText(ID_S1,"Thread");
		SetDlgItemText(ID_S2,"VAR");
		SetDlgItemText(ID_S3,"File");
		GetDlgItem(ID_S1)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S2)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S3)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_S4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_S5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P1)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P3)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P4)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_P5)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_PS)->ShowWindow(SW_SHOW);
		GetDlgItem(ID_Dir)->ShowWindow(SW_SHOW);
		break;
	}
}

void CToolSetupPage::OnCloseupAction() 
{
	if (!UpdateData()) return;
	SetAllActions();
}

void CToolSetupPage::DoDirectoryBrowse(MCODE_ACTION *m)
{
	if (!UpdateData()) return;

	CPersistOpenDlg FileDlg (TRUE, ".tbl", NULL, 
		OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"C Files (*.c)|*.c|GCode Files (*.ngc)|*.ngc|All Files (*.*)|*.*||");
	
	if (FileDlg.DoModal() == IDOK)
	{
		strncpy(m->String,FileDlg.GetPathName(), 255);
		UpdateData(FALSE);
	}
}
