// ToolSetupButtonsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ToolSetupButtonsPage.h"
#include "MainFrm.h"

// CToolSetupButtonsPage dialog

IMPLEMENT_DYNAMIC(CToolSetupButtonsPage, CToolSetupPage)

CToolSetupButtonsPage::CToolSetupButtonsPage()
	: CToolSetupPage(CToolSetupButtonsPage::IDD)
{
	InitDialogComplete=FALSE;
}

CToolSetupButtonsPage::~CToolSetupButtonsPage()
{
}

void CToolSetupButtonsPage::DoDataExchange(CDataExchange* pDX)
{
	if (!pDX->m_bSaveAndValidate)
	{
		m_PS_M11= McodeActions[11].String;
		m_PS_M12= McodeActions[12].String;
		m_PS_M13= McodeActions[13].String;
		m_PS_M14= McodeActions[14].String;
		m_PS_M15= McodeActions[15].String;
		m_PS_M16= McodeActions[16].String;
		m_PS_M17= McodeActions[17].String;
		m_PS_M18= McodeActions[18].String;
		m_PS_M19= McodeActions[19].String;
		m_PS_M20= McodeActions[20].String;
	}


	//{{AFX_DATA_MAP(CToolSetupButtonsPage)
	DDX_Text(pDX, IDC_PS_M11, m_PS_M11);
	DDV_MaxChars(pDX, m_PS_M11, 255);
	DDX_Text(pDX, IDC_PS_M12, m_PS_M12);
	DDV_MaxChars(pDX, m_PS_M12, 255);
	DDX_Text(pDX, IDC_PS_M13, m_PS_M13);
	DDV_MaxChars(pDX, m_PS_M13, 255);
	DDX_Text(pDX, IDC_PS_M14, m_PS_M14);
	DDV_MaxChars(pDX, m_PS_M14, 255);
	DDX_Text(pDX, IDC_PS_M15, m_PS_M15);
	DDV_MaxChars(pDX, m_PS_M15, 255);
	DDX_Text(pDX, IDC_PS_M16, m_PS_M16);
	DDV_MaxChars(pDX, m_PS_M16, 255);
	DDX_Text(pDX, IDC_PS_M17, m_PS_M17);
	DDV_MaxChars(pDX, m_PS_M17, 255);
	DDX_Text(pDX, IDC_PS_M18, m_PS_M18);
	DDV_MaxChars(pDX, m_PS_M18, 255);
	DDX_Text(pDX, IDC_PS_M19, m_PS_M19);
	DDV_MaxChars(pDX, m_PS_M19, 255);
	DDX_Text(pDX, IDC_PS_M20, m_PS_M20);
	DDV_MaxChars(pDX, m_PS_M20, 255);
	DDX_Text(pDX, IDC_Button0, m_Button0);
	DDV_MaxChars(pDX, m_Button0, 12);
	DDX_Text(pDX, IDC_Button2, m_Button2);
	DDV_MaxChars(pDX, m_Button2, 12);
	DDX_Text(pDX, IDC_Button1, m_Button1);
	DDV_MaxChars(pDX, m_Button1, 12);
	DDX_Text(pDX, IDC_Button3, m_Button3);
	DDV_MaxChars(pDX, m_Button3, 12);
	DDX_Text(pDX, IDC_Button4, m_Button4);
	DDV_MaxChars(pDX, m_Button4, 12);
	DDX_Text(pDX, IDC_Button5, m_Button5);
	DDV_MaxChars(pDX, m_Button5, 12);
	DDX_Text(pDX, IDC_Button6, m_Button6);
	DDV_MaxChars(pDX, m_Button6, 12);
	DDX_Text(pDX, IDC_Button7, m_Button7);
	DDV_MaxChars(pDX, m_Button7, 12);
	DDX_Text(pDX, IDC_Button8, m_Button8);
	DDV_MaxChars(pDX, m_Button8, 12);
	DDX_Text(pDX, IDC_Button9, m_Button9);
	DDV_MaxChars(pDX, m_Button9, 12);
	//}}AFX_DATA_MAP

	DDX_CBIndex(pDX, IDC_Action_M11, McodeActions[11].Action);
	DDX_CBIndex(pDX, IDC_Action_M12, McodeActions[12].Action);
	DDX_CBIndex(pDX, IDC_Action_M13, McodeActions[13].Action);
	DDX_CBIndex(pDX, IDC_Action_M14, McodeActions[14].Action);
	DDX_CBIndex(pDX, IDC_Action_M15, McodeActions[15].Action);
	DDX_CBIndex(pDX, IDC_Action_M16, McodeActions[16].Action);
	DDX_CBIndex(pDX, IDC_Action_M17, McodeActions[17].Action);
	DDX_CBIndex(pDX, IDC_Action_M18, McodeActions[18].Action);
	DDX_CBIndex(pDX, IDC_Action_M19, McodeActions[19].Action);
	DDX_CBIndex(pDX, IDC_Action_M20, McodeActions[20].Action);

	DDX_Text(pDX, IDC_P1_M11, McodeActions[11].dParams[0]);
	DDX_Text(pDX, IDC_P1_M12, McodeActions[12].dParams[0]);
	DDX_Text(pDX, IDC_P1_M13, McodeActions[13].dParams[0]);
	DDX_Text(pDX, IDC_P1_M14, McodeActions[14].dParams[0]);
	DDX_Text(pDX, IDC_P1_M15, McodeActions[15].dParams[0]);
	DDX_Text(pDX, IDC_P1_M16, McodeActions[16].dParams[0]);
	DDX_Text(pDX, IDC_P1_M17, McodeActions[17].dParams[0]);
	DDX_Text(pDX, IDC_P1_M18, McodeActions[18].dParams[0]);
	DDX_Text(pDX, IDC_P1_M19, McodeActions[19].dParams[0]);
	DDX_Text(pDX, IDC_P1_M20, McodeActions[20].dParams[0]);

	DDX_Text(pDX, IDC_P2_M11, McodeActions[11].dParams[1]);
	DDX_Text(pDX, IDC_P2_M12, McodeActions[12].dParams[1]);
	DDX_Text(pDX, IDC_P2_M13, McodeActions[13].dParams[1]);
	DDX_Text(pDX, IDC_P2_M14, McodeActions[14].dParams[1]);
	DDX_Text(pDX, IDC_P2_M15, McodeActions[15].dParams[1]);
	DDX_Text(pDX, IDC_P2_M16, McodeActions[16].dParams[1]);
	DDX_Text(pDX, IDC_P2_M17, McodeActions[17].dParams[1]);
	DDX_Text(pDX, IDC_P2_M18, McodeActions[18].dParams[1]);
	DDX_Text(pDX, IDC_P2_M19, McodeActions[19].dParams[1]);
	DDX_Text(pDX, IDC_P2_M20, McodeActions[20].dParams[1]);

	DDX_Text(pDX, IDC_P3_M11, McodeActions[11].dParams[2]);
	DDX_Text(pDX, IDC_P3_M12, McodeActions[12].dParams[2]);
	DDX_Text(pDX, IDC_P3_M13, McodeActions[13].dParams[2]);
	DDX_Text(pDX, IDC_P3_M14, McodeActions[14].dParams[2]);
	DDX_Text(pDX, IDC_P3_M15, McodeActions[15].dParams[2]);
	DDX_Text(pDX, IDC_P3_M16, McodeActions[16].dParams[2]);
	DDX_Text(pDX, IDC_P3_M17, McodeActions[17].dParams[2]);
	DDX_Text(pDX, IDC_P3_M18, McodeActions[18].dParams[2]);
	DDX_Text(pDX, IDC_P3_M19, McodeActions[19].dParams[2]);
	DDX_Text(pDX, IDC_P3_M20, McodeActions[20].dParams[2]);

	DDX_Text(pDX, IDC_P4_M11, McodeActions[11].dParams[3]);
	DDX_Text(pDX, IDC_P4_M12, McodeActions[12].dParams[3]);
	DDX_Text(pDX, IDC_P4_M13, McodeActions[13].dParams[3]);
	DDX_Text(pDX, IDC_P4_M14, McodeActions[14].dParams[3]);
	DDX_Text(pDX, IDC_P4_M15, McodeActions[15].dParams[3]);
	DDX_Text(pDX, IDC_P4_M16, McodeActions[16].dParams[3]);
	DDX_Text(pDX, IDC_P4_M17, McodeActions[17].dParams[3]);
	DDX_Text(pDX, IDC_P4_M18, McodeActions[18].dParams[3]);
	DDX_Text(pDX, IDC_P4_M19, McodeActions[19].dParams[3]);
	DDX_Text(pDX, IDC_P4_M20, McodeActions[20].dParams[3]);

	DDX_Text(pDX, IDC_P5_M11, McodeActions[11].dParams[4]);
	DDX_Text(pDX, IDC_P5_M12, McodeActions[12].dParams[4]);
	DDX_Text(pDX, IDC_P5_M13, McodeActions[13].dParams[4]);
	DDX_Text(pDX, IDC_P5_M14, McodeActions[14].dParams[4]);
	DDX_Text(pDX, IDC_P5_M15, McodeActions[15].dParams[4]);
	DDX_Text(pDX, IDC_P5_M16, McodeActions[16].dParams[4]);
	DDX_Text(pDX, IDC_P5_M17, McodeActions[17].dParams[4]);
	DDX_Text(pDX, IDC_P5_M18, McodeActions[18].dParams[4]);
	DDX_Text(pDX, IDC_P5_M19, McodeActions[19].dParams[4]);
	DDX_Text(pDX, IDC_P5_M20, McodeActions[20].dParams[4]);

	DDX_Text(pDX, IDC_PS_M11, m_PS_M11);
	DDV_MaxChars(pDX, m_PS_M11, 255);
	DDX_Text(pDX, IDC_PS_M12, m_PS_M12);
	DDV_MaxChars(pDX, m_PS_M12, 255);
	DDX_Text(pDX, IDC_PS_M13, m_PS_M13);
	DDV_MaxChars(pDX, m_PS_M13, 255);
	DDX_Text(pDX, IDC_PS_M14, m_PS_M14);
	DDV_MaxChars(pDX, m_PS_M14, 255);
	DDX_Text(pDX, IDC_PS_M15, m_PS_M15);
	DDV_MaxChars(pDX, m_PS_M15, 255);
	DDX_Text(pDX, IDC_PS_M16, m_PS_M16);
	DDV_MaxChars(pDX, m_PS_M16, 255);
	DDX_Text(pDX, IDC_PS_M17, m_PS_M17);
	DDV_MaxChars(pDX, m_PS_M17, 255);
	DDX_Text(pDX, IDC_PS_M18, m_PS_M18);
	DDV_MaxChars(pDX, m_PS_M18, 255);
	DDX_Text(pDX, IDC_PS_M19, m_PS_M19);
	DDV_MaxChars(pDX, m_PS_M19, 255);
	DDX_Text(pDX, IDC_PS_M20, m_PS_M20);
	DDV_MaxChars(pDX, m_PS_M20, 255);
	
	DDX_Text(pDX, IDC_ButtonKey0, UserButtonKeys[0]);
	DDX_Text(pDX, IDC_ButtonKey1, UserButtonKeys[1]);
	DDX_Text(pDX, IDC_ButtonKey2, UserButtonKeys[2]);
	DDX_Text(pDX, IDC_ButtonKey3, UserButtonKeys[3]);
	DDX_Text(pDX, IDC_ButtonKey4, UserButtonKeys[4]);
	DDX_Text(pDX, IDC_ButtonKey5, UserButtonKeys[5]);
	DDX_Text(pDX, IDC_ButtonKey6, UserButtonKeys[6]);
	DDX_Text(pDX, IDC_ButtonKey7, UserButtonKeys[7]);
	DDX_Text(pDX, IDC_ButtonKey8, UserButtonKeys[8]);
	DDX_Text(pDX, IDC_ButtonKey9, UserButtonKeys[9]);

	if (pDX->m_bSaveAndValidate)
	{
		strncpy(McodeActions[11].String,m_PS_M11,255);
		strncpy(McodeActions[12].String,m_PS_M12,255);
		strncpy(McodeActions[13].String,m_PS_M13,255);
		strncpy(McodeActions[14].String,m_PS_M14,255);
		strncpy(McodeActions[15].String,m_PS_M15,255);
		strncpy(McodeActions[16].String,m_PS_M16,255);
		strncpy(McodeActions[17].String,m_PS_M17,255);
		strncpy(McodeActions[18].String,m_PS_M18,255);
		strncpy(McodeActions[19].String,m_PS_M19,255);
		strncpy(McodeActions[20].String,m_PS_M20,255);
	}
}

BEGIN_MESSAGE_MAP(CToolSetupButtonsPage, CToolSetupPage)
	ON_BN_CLICKED(IDC_KMotion_HELP, OnIhelp)
	ON_BN_CLICKED(IDC_Dir_M11, OnDirM11)
	ON_BN_CLICKED(IDC_Dir_M12, OnDirM12)
	ON_BN_CLICKED(IDC_Dir_M13, OnDirM13)
	ON_BN_CLICKED(IDC_Dir_M14, OnDirM14)
	ON_BN_CLICKED(IDC_Dir_M15, OnDirM15)
	ON_BN_CLICKED(IDC_Dir_M16, OnDirM16)
	ON_BN_CLICKED(IDC_Dir_M17, OnDirM17)
	ON_BN_CLICKED(IDC_Dir_M18, OnDirM18)
	ON_BN_CLICKED(IDC_Dir_M19, OnDirM19)
	ON_BN_CLICKED(IDC_Dir_M20, OnDirM20)
	ON_CBN_CLOSEUP(IDC_Action_M11, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M12, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M13, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M14, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M15, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M16, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M17, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M18, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M19, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M20, OnCloseupAction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CToolSetupButtonsPage::OnIhelp() 
{
	TheFrame->HelpDlg.Show("KMotionCNC\\ToolSetupScreenButtons.htm");
}

void CToolSetupButtonsPage::SetAllActions()
{
	// Button Actions
	SetAction(&McodeActions[11], IDC_Action_M11, IDC_S1_M11,IDC_S2_M11,IDC_S3_M11,IDC_S4_M11,IDC_S5_M11,IDC_Dir_M11,
								 IDC_P1_M11,IDC_P2_M11,IDC_P3_M11,IDC_P4_M11,IDC_P5_M11,IDC_PS_M11);
	SetAction(&McodeActions[12], IDC_Action_M12, IDC_S1_M12,IDC_S2_M12,IDC_S3_M12,IDC_S4_M12,IDC_S5_M12,IDC_Dir_M12,
								 IDC_P1_M12,IDC_P2_M12,IDC_P3_M12,IDC_P4_M12,IDC_P5_M12,IDC_PS_M12);
	SetAction(&McodeActions[13], IDC_Action_M13, IDC_S1_M13,IDC_S2_M13,IDC_S3_M13,IDC_S4_M13,IDC_S5_M13,IDC_Dir_M13,
								 IDC_P1_M13,IDC_P2_M13,IDC_P3_M13,IDC_P4_M13,IDC_P5_M13,IDC_PS_M13);
	SetAction(&McodeActions[14], IDC_Action_M14, IDC_S1_M14,IDC_S2_M14,IDC_S3_M14,IDC_S4_M14,IDC_S5_M14,IDC_Dir_M14,
								 IDC_P1_M14,IDC_P2_M14,IDC_P3_M14,IDC_P4_M14,IDC_P5_M14,IDC_PS_M14);
	SetAction(&McodeActions[15], IDC_Action_M15, IDC_S1_M15,IDC_S2_M15,IDC_S3_M15,IDC_S4_M15,IDC_S5_M15,IDC_Dir_M15,
								 IDC_P1_M15,IDC_P2_M15,IDC_P3_M15,IDC_P4_M15,IDC_P5_M15,IDC_PS_M15);
	SetAction(&McodeActions[16], IDC_Action_M16, IDC_S1_M16,IDC_S2_M16,IDC_S3_M16,IDC_S4_M16,IDC_S5_M16,IDC_Dir_M16,
								 IDC_P1_M16,IDC_P2_M16,IDC_P3_M16,IDC_P4_M16,IDC_P5_M16,IDC_PS_M16);
	SetAction(&McodeActions[17], IDC_Action_M17, IDC_S1_M17,IDC_S2_M17,IDC_S3_M17,IDC_S4_M17,IDC_S5_M17,IDC_Dir_M17,
								 IDC_P1_M17,IDC_P2_M17,IDC_P3_M17,IDC_P4_M17,IDC_P5_M17,IDC_PS_M17);
	SetAction(&McodeActions[18], IDC_Action_M18, IDC_S1_M18,IDC_S2_M18,IDC_S3_M18,IDC_S4_M18,IDC_S5_M18,IDC_Dir_M18,
								 IDC_P1_M18,IDC_P2_M18,IDC_P3_M18,IDC_P4_M18,IDC_P5_M18,IDC_PS_M18);
	SetAction(&McodeActions[19], IDC_Action_M19, IDC_S1_M19,IDC_S2_M19,IDC_S3_M19,IDC_S4_M19,IDC_S5_M19,IDC_Dir_M19,
								 IDC_P1_M19,IDC_P2_M19,IDC_P3_M19,IDC_P4_M19,IDC_P5_M19,IDC_PS_M19);
	SetAction(&McodeActions[20], IDC_Action_M20, IDC_S1_M20,IDC_S2_M20,IDC_S3_M20,IDC_S4_M20,IDC_S5_M20,IDC_Dir_M20,
								 IDC_P1_M20,IDC_P2_M20,IDC_P3_M20,IDC_P4_M20,IDC_P5_M20,IDC_PS_M20);
}



void CToolSetupButtonsPage::OnDirM11() 
{
	DoDirectoryBrowse(&McodeActions[11]);
}

void CToolSetupButtonsPage::OnDirM12() 
{
	DoDirectoryBrowse(&McodeActions[12]);
}

void CToolSetupButtonsPage::OnDirM13() 
{
	DoDirectoryBrowse(&McodeActions[13]);
}

void CToolSetupButtonsPage::OnDirM14() 
{
	DoDirectoryBrowse(&McodeActions[14]);
}

void CToolSetupButtonsPage::OnDirM15() 
{
	DoDirectoryBrowse(&McodeActions[15]);
}

void CToolSetupButtonsPage::OnDirM16() 
{
	DoDirectoryBrowse(&McodeActions[16]);
}

void CToolSetupButtonsPage::OnDirM17() 
{
	DoDirectoryBrowse(&McodeActions[17]);
}

void CToolSetupButtonsPage::OnDirM18() 
{
	DoDirectoryBrowse(&McodeActions[18]);
}

void CToolSetupButtonsPage::OnDirM19() 
{
	DoDirectoryBrowse(&McodeActions[19]);
}

void CToolSetupButtonsPage::OnDirM20() 
{
	DoDirectoryBrowse(&McodeActions[20]);
}

BOOL CToolSetupButtonsPage::OnInitDialog() 
{
	CToolSetupPage::OnInitDialog();
	
	SetAllActions();	
	return TRUE;
}

BOOL CToolSetupButtonsPage::OnKillActive()
{
	if (!UpdateData(TRUE)) return FALSE;
	if (InitDialogComplete)
	{
		if (CheckThreadNo(IDC_P1_M11,&McodeActions[11])) return FALSE;
		if (CheckThreadNo(IDC_P1_M12,&McodeActions[12])) return FALSE;
		if (CheckThreadNo(IDC_P1_M13,&McodeActions[13])) return FALSE;
		if (CheckThreadNo(IDC_P1_M14,&McodeActions[14])) return FALSE;
		if (CheckThreadNo(IDC_P1_M15,&McodeActions[15])) return FALSE;
		if (CheckThreadNo(IDC_P1_M16,&McodeActions[16])) return FALSE;
		if (CheckThreadNo(IDC_P1_M17,&McodeActions[17])) return FALSE;
		if (CheckThreadNo(IDC_P1_M18,&McodeActions[18])) return FALSE;
		if (CheckThreadNo(IDC_P1_M19,&McodeActions[19])) return FALSE;
		if (CheckThreadNo(IDC_P1_M20,&McodeActions[20])) return FALSE;
	}
	return CToolSetupPage::OnKillActive();
}
