// ToolSetupM1Page.cpp : implementation file
//

#include "stdafx.h"
#include "ToolSetupM1Page.h"
#include "MainFrm.h"

// CToolSetupM1Page dialog

IMPLEMENT_DYNAMIC(CToolSetupM1Page, CToolSetupPage)

CToolSetupM1Page::CToolSetupM1Page()
	: CToolSetupPage(CToolSetupM1Page::IDD)
{
	InitDialogComplete=FALSE;
}

CToolSetupM1Page::~CToolSetupM1Page()
{
}

void CToolSetupM1Page::DoDataExchange(CDataExchange* pDX)
{
	if (!pDX->m_bSaveAndValidate)
	{
		m_PS_M3 = McodeActions[3 ].String;
		m_PS_M4 = McodeActions[4 ].String;
		m_PS_M5 = McodeActions[5 ].String;
		m_PS_M6 = McodeActions[6 ].String;
		m_PS_M7 = McodeActions[7 ].String;
		m_PS_M8 = McodeActions[8 ].String;
		m_PS_M9 = McodeActions[9 ].String;
		m_PS_M10= McodeActions[10].String;
	}


	//{{AFX_DATA_MAP(CToolSetupM1Page)
	DDX_Text(pDX, IDC_PS_M3, m_PS_M3);
	DDV_MaxChars(pDX, m_PS_M3, 255);
	DDX_Text(pDX, IDC_PS_M4, m_PS_M4);
	DDV_MaxChars(pDX, m_PS_M4, 255);
	DDX_Text(pDX, IDC_PS_M5, m_PS_M5);
	DDV_MaxChars(pDX, m_PS_M5, 255);
	DDX_Text(pDX, IDC_PS_M6, m_PS_M6);
	DDV_MaxChars(pDX, m_PS_M6, 255);
	DDX_Text(pDX, IDC_PS_M7, m_PS_M7);
	DDV_MaxChars(pDX, m_PS_M7, 255);
	DDX_Text(pDX, IDC_PS_M8, m_PS_M8);
	DDV_MaxChars(pDX, m_PS_M8, 255);
	DDX_Text(pDX, IDC_PS_M9, m_PS_M9);
	DDV_MaxChars(pDX, m_PS_M9, 255);
	DDX_Text(pDX, IDC_PS_M10, m_PS_M10);
	DDV_MaxChars(pDX, m_PS_M10, 255);
	//}}AFX_DATA_MAP

	DDX_CBIndex(pDX, IDC_Action_M3, McodeActions[3].Action);
	DDX_CBIndex(pDX, IDC_Action_M4, McodeActions[4].Action);
	DDX_CBIndex(pDX, IDC_Action_M5, McodeActions[5].Action);
	DDX_CBIndex(pDX, IDC_Action_M6, McodeActions[6].Action);
	DDX_CBIndex(pDX, IDC_Action_M7, McodeActions[7].Action);
	DDX_CBIndex(pDX, IDC_Action_M8, McodeActions[8].Action);
	DDX_CBIndex(pDX, IDC_Action_M9, McodeActions[9].Action);
	DDX_CBIndex(pDX, IDC_Action_M10, McodeActions[10].Action); // special case for spindle

	DDX_Text(pDX, IDC_P1_M3,  McodeActions[3 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M4,  McodeActions[4 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M5,  McodeActions[5 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M6,  McodeActions[6 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M7,  McodeActions[7 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M8,  McodeActions[8 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M9,  McodeActions[9 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M10, McodeActions[10].dParams[0]);

	DDX_Text(pDX, IDC_P2_M3,  McodeActions[3 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M4,  McodeActions[4 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M5,  McodeActions[5 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M6,  McodeActions[6 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M7,  McodeActions[7 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M8,  McodeActions[8 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M9,  McodeActions[9 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M10, McodeActions[10].dParams[1]);

	DDX_Text(pDX, IDC_P3_M3,  McodeActions[3 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M4,  McodeActions[4 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M5,  McodeActions[5 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M6,  McodeActions[6 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M7,  McodeActions[7 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M8,  McodeActions[8 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M9,  McodeActions[9 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M10, McodeActions[10].dParams[2]);

	DDX_Text(pDX, IDC_P4_M3,  McodeActions[3 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M4,  McodeActions[4 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M5,  McodeActions[5 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M6,  McodeActions[6 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M7,  McodeActions[7 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M8,  McodeActions[8 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M9,  McodeActions[9 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M10, McodeActions[10].dParams[3]);

	DDX_Text(pDX, IDC_P5_M3,  McodeActions[3 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M4,  McodeActions[4 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M5,  McodeActions[5 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M6,  McodeActions[6 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M7,  McodeActions[7 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M8,  McodeActions[8 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M9,  McodeActions[9 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M10, McodeActions[10].dParams[4]);

	if (pDX->m_bSaveAndValidate)
	{
		strncpy(McodeActions[3 ].String,m_PS_M3, 255);
		strncpy(McodeActions[4 ].String,m_PS_M4, 255);
		strncpy(McodeActions[5 ].String,m_PS_M5, 255);
		strncpy(McodeActions[6 ].String,m_PS_M6, 255);
		strncpy(McodeActions[7 ].String,m_PS_M7, 255);
		strncpy(McodeActions[8 ].String,m_PS_M8, 255);
		strncpy(McodeActions[9 ].String,m_PS_M9, 255);
		strncpy(McodeActions[10].String,m_PS_M10,255);
	}
}


BEGIN_MESSAGE_MAP(CToolSetupM1Page, CToolSetupPage)
	ON_BN_CLICKED(IDC_KMotion_HELP, OnIhelp)
	ON_BN_CLICKED(IDC_Dir_M3, OnDirM3)
	ON_BN_CLICKED(IDC_Dir_M4, OnDirM4)
	ON_BN_CLICKED(IDC_Dir_M5, OnDirM5)
	ON_BN_CLICKED(IDC_Dir_M6, OnDirM6)
	ON_BN_CLICKED(IDC_Dir_M7, OnDirM7)
	ON_BN_CLICKED(IDC_Dir_M8, OnDirM8)
	ON_BN_CLICKED(IDC_Dir_M9, OnDirM9)
	ON_BN_CLICKED(IDC_Dir_M10, OnDirM10)
	ON_CBN_CLOSEUP(IDC_Action_M3, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M4, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M5, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M6, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M7, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M8, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M9, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M10, OnCloseupAction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CToolSetupM1Page::OnIhelp() 
{
	TheFrame->HelpDlg.Show("KMotionCNC\\ToolSetupScreenM3.htm");
}

void CToolSetupM1Page::SetAllActions()
{
	SetAction(&McodeActions[3], IDC_Action_M3, IDC_S1_M3,IDC_S2_M3,IDC_S3_M3,IDC_S4_M3,IDC_S5_M3,IDC_Dir_M3,
								IDC_P1_M3,IDC_P2_M3,IDC_P3_M3,IDC_P4_M3,IDC_P5_M3,IDC_PS_M3);
	SetAction(&McodeActions[4], IDC_Action_M4, IDC_S1_M4,IDC_S2_M4,IDC_S3_M4,IDC_S4_M4,IDC_S5_M4,IDC_Dir_M4,
								IDC_P1_M4,IDC_P2_M4,IDC_P3_M4,IDC_P4_M4,IDC_P5_M4,IDC_PS_M4);
	SetAction(&McodeActions[5], IDC_Action_M5, IDC_S1_M5,IDC_S2_M5,IDC_S3_M5,IDC_S4_M5,IDC_S5_M5,IDC_Dir_M5,
								IDC_P1_M5,IDC_P2_M5,IDC_P3_M5,IDC_P4_M5,IDC_P5_M5,IDC_PS_M5);
	SetAction(&McodeActions[6], IDC_Action_M6, IDC_S1_M6,IDC_S2_M6,IDC_S3_M6,IDC_S4_M6,IDC_S5_M6,IDC_Dir_M6,
								IDC_P1_M6,IDC_P2_M6,IDC_P3_M6,IDC_P4_M6,IDC_P5_M6,IDC_PS_M6);
	SetAction(&McodeActions[7], IDC_Action_M7, IDC_S1_M7,IDC_S2_M7,IDC_S3_M7,IDC_S4_M7,IDC_S5_M7,IDC_Dir_M7,
								IDC_P1_M7,IDC_P2_M7,IDC_P3_M7,IDC_P4_M7,IDC_P5_M7,IDC_PS_M7);
	SetAction(&McodeActions[8], IDC_Action_M8, IDC_S1_M8,IDC_S2_M8,IDC_S3_M8,IDC_S4_M8,IDC_S5_M8,IDC_Dir_M8,
								IDC_P1_M8,IDC_P2_M8,IDC_P3_M8,IDC_P4_M8,IDC_P5_M8,IDC_PS_M8);
	SetAction(&McodeActions[9], IDC_Action_M9, IDC_S1_M9,IDC_S2_M9,IDC_S3_M9,IDC_S4_M9,IDC_S5_M9,IDC_Dir_M9,
								IDC_P1_M9,IDC_P2_M9,IDC_P3_M9,IDC_P4_M9,IDC_P5_M9,IDC_PS_M9);
	
	//Spindle S Action
	SetAction(&McodeActions[10], IDC_Action_M10, IDC_S1_M10,IDC_S2_M10,IDC_S3_M10,IDC_S4_M10,IDC_S5_M10,IDC_Dir_M10,
								  IDC_P1_M10,IDC_P2_M10,IDC_P3_M10,IDC_P4_M10,IDC_P5_M10,IDC_PS_M10);

}



void CToolSetupM1Page::OnDirM3() 
{
	DoDirectoryBrowse(&McodeActions[3]);
}

void CToolSetupM1Page::OnDirM4() 
{
	DoDirectoryBrowse(&McodeActions[4]);
}

void CToolSetupM1Page::OnDirM5() 
{
	DoDirectoryBrowse(&McodeActions[5]);
}

void CToolSetupM1Page::OnDirM6() 
{
	DoDirectoryBrowse(&McodeActions[6]);
}

void CToolSetupM1Page::OnDirM7() 
{
	DoDirectoryBrowse(&McodeActions[7]);
}

void CToolSetupM1Page::OnDirM8() 
{
	DoDirectoryBrowse(&McodeActions[8]);
}

void CToolSetupM1Page::OnDirM9() 
{
	DoDirectoryBrowse(&McodeActions[9]);
}

void CToolSetupM1Page::OnDirM10() 
{
	DoDirectoryBrowse(&McodeActions[10]);
}

BOOL CToolSetupM1Page::OnInitDialog() 
{
	CToolSetupPage::OnInitDialog();
	
	SetAllActions();	

	return TRUE;  
}

BOOL CToolSetupM1Page::OnKillActive()
{
	if (!UpdateData(TRUE)) return FALSE;
	if (InitDialogComplete)
	{
		if (CheckThreadNo(IDC_P1_M3,&McodeActions[3])) return FALSE;
		if (CheckThreadNo(IDC_P1_M4,&McodeActions[4])) return FALSE;
		if (CheckThreadNo(IDC_P1_M5,&McodeActions[5])) return FALSE;
		if (CheckThreadNo(IDC_P1_M6,&McodeActions[6])) return FALSE;
		if (CheckThreadNo(IDC_P1_M7,&McodeActions[7])) return FALSE;
		if (CheckThreadNo(IDC_P1_M8,&McodeActions[8])) return FALSE;
		if (CheckThreadNo(IDC_P1_M9,&McodeActions[9])) return FALSE;
		if (CheckThreadNo(IDC_P1_M10,&McodeActions[10])) return FALSE;
	}
	return CToolSetupPage::OnKillActive();
}

