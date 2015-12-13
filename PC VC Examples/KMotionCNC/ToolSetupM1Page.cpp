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
		m_PS_M0 = McodeActions[0 ].String;
		m_PS_M1 = McodeActions[1 ].String;
		m_PS_M2 = McodeActions[2 ].String;
		m_PS_M3 = McodeActions[3 ].String;
		m_PS_M4 = McodeActions[4 ].String;
		m_PS_M5 = McodeActions[5 ].String;
		m_PS_M6 = McodeActions[6 ].String;
		m_PS_M7 = McodeActions[7 ].String;
		m_PS_M8 = McodeActions[8 ].String;
		m_PS_M9 = McodeActions[9 ].String;
		m_PS_M10= McodeActions[10].String;
		m_PS_M24= McodeActions[11].String;
		m_PS_M25= McodeActions[12].String;
		m_PS_M26= McodeActions[13].String;
		m_PS_M27= McodeActions[14].String;
		m_PS_M28= McodeActions[15].String;
		m_PS_M29= McodeActions[16].String;
		m_PS_M30= McodeActions[17].String;
		m_PS_M31= McodeActions[18].String;
	}


	//{{AFX_DATA_MAP(CToolSetupM1Page)
	DDX_Text(pDX, IDC_PS_M0, m_PS_M0);
	DDV_MaxChars(pDX, m_PS_M0, 255);
	DDX_Text(pDX, IDC_PS_M1, m_PS_M1);
	DDV_MaxChars(pDX, m_PS_M1, 255);
	DDX_Text(pDX, IDC_PS_M2, m_PS_M2);
	DDV_MaxChars(pDX, m_PS_M2, 255);
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
	DDX_Text(pDX, IDC_PS_M24, m_PS_M24);
	DDV_MaxChars(pDX, m_PS_M24, 255);
	DDX_Text(pDX, IDC_PS_M25, m_PS_M25);
	DDV_MaxChars(pDX, m_PS_M25, 255);
	DDX_Text(pDX, IDC_PS_M26, m_PS_M26);
	DDV_MaxChars(pDX, m_PS_M26, 255);
	DDX_Text(pDX, IDC_PS_M27, m_PS_M27);
	DDV_MaxChars(pDX, m_PS_M27, 255);
	DDX_Text(pDX, IDC_PS_M28, m_PS_M28);
	DDV_MaxChars(pDX, m_PS_M28, 255);
	DDX_Text(pDX, IDC_PS_M29, m_PS_M29);
	DDV_MaxChars(pDX, m_PS_M29, 255);
	DDX_Text(pDX, IDC_PS_M30, m_PS_M30);
	DDV_MaxChars(pDX, m_PS_M30, 255);
	DDX_Text(pDX, IDC_PS_M31, m_PS_M31);
	DDV_MaxChars(pDX, m_PS_M31, 255);
	//}}AFX_DATA_MAP

	DDX_CBIndex(pDX, IDC_Action_M0, McodeActions[0].Action);
	DDX_CBIndex(pDX, IDC_Action_M1, McodeActions[1].Action);
	DDX_CBIndex(pDX, IDC_Action_M2, McodeActions[2].Action);
	DDX_CBIndex(pDX, IDC_Action_M3, McodeActions[3].Action);
	DDX_CBIndex(pDX, IDC_Action_M4, McodeActions[4].Action);
	DDX_CBIndex(pDX, IDC_Action_M5, McodeActions[5].Action);
	DDX_CBIndex(pDX, IDC_Action_M6, McodeActions[6].Action);
	DDX_CBIndex(pDX, IDC_Action_M7, McodeActions[7].Action);
	DDX_CBIndex(pDX, IDC_Action_M8, McodeActions[8].Action);
	DDX_CBIndex(pDX, IDC_Action_M9, McodeActions[9].Action);
	DDX_CBIndex(pDX, IDC_Action_M10, McodeActions[10].Action); // special case for spindle
	DDX_CBIndex(pDX, IDC_Action_M24, McodeActions[11].Action);
	DDX_CBIndex(pDX, IDC_Action_M25, McodeActions[12].Action);
	DDX_CBIndex(pDX, IDC_Action_M26, McodeActions[13].Action);
	DDX_CBIndex(pDX, IDC_Action_M27, McodeActions[14].Action);
	DDX_CBIndex(pDX, IDC_Action_M28, McodeActions[15].Action);
	DDX_CBIndex(pDX, IDC_Action_M29, McodeActions[16].Action);
	DDX_CBIndex(pDX, IDC_Action_M30, McodeActions[17].Action);
	DDX_CBIndex(pDX, IDC_Action_M31, McodeActions[18].Action);

	DDX_Text(pDX, IDC_P1_M0,  McodeActions[0 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M1,  McodeActions[1 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M2,  McodeActions[2 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M3,  McodeActions[3 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M4,  McodeActions[4 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M5,  McodeActions[5 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M6,  McodeActions[6 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M7,  McodeActions[7 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M8,  McodeActions[8 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M9,  McodeActions[9 ].dParams[0]);
	DDX_Text(pDX, IDC_P1_M10, McodeActions[10].dParams[0]);
	DDX_Text(pDX, IDC_P1_M24, McodeActions[11].dParams[0]);
	DDX_Text(pDX, IDC_P1_M25, McodeActions[12].dParams[0]);
	DDX_Text(pDX, IDC_P1_M26, McodeActions[13].dParams[0]);
	DDX_Text(pDX, IDC_P1_M27, McodeActions[14].dParams[0]);
	DDX_Text(pDX, IDC_P1_M28, McodeActions[15].dParams[0]);
	DDX_Text(pDX, IDC_P1_M29, McodeActions[16].dParams[0]);
	DDX_Text(pDX, IDC_P1_M30, McodeActions[17].dParams[0]);
	DDX_Text(pDX, IDC_P1_M31, McodeActions[18].dParams[0]);

	DDX_Text(pDX, IDC_P2_M0,  McodeActions[0 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M1,  McodeActions[1 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M2,  McodeActions[2 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M3,  McodeActions[3 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M4,  McodeActions[4 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M5,  McodeActions[5 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M6,  McodeActions[6 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M7,  McodeActions[7 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M8,  McodeActions[8 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M9,  McodeActions[9 ].dParams[1]);
	DDX_Text(pDX, IDC_P2_M10, McodeActions[10].dParams[1]);
	DDX_Text(pDX, IDC_P2_M24, McodeActions[11].dParams[1]);
	DDX_Text(pDX, IDC_P2_M25, McodeActions[12].dParams[1]);
	DDX_Text(pDX, IDC_P2_M26, McodeActions[13].dParams[1]);
	DDX_Text(pDX, IDC_P2_M27, McodeActions[14].dParams[1]);
	DDX_Text(pDX, IDC_P2_M28, McodeActions[15].dParams[1]);
	DDX_Text(pDX, IDC_P2_M29, McodeActions[16].dParams[1]);
	DDX_Text(pDX, IDC_P2_M30, McodeActions[17].dParams[1]);
	DDX_Text(pDX, IDC_P2_M31, McodeActions[18].dParams[1]);

	DDX_Text(pDX, IDC_P3_M0,  McodeActions[0 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M1,  McodeActions[1 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M2,  McodeActions[2 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M3,  McodeActions[3 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M4,  McodeActions[4 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M5,  McodeActions[5 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M6,  McodeActions[6 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M7,  McodeActions[7 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M8,  McodeActions[8 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M9,  McodeActions[9 ].dParams[2]);
	DDX_Text(pDX, IDC_P3_M10, McodeActions[10].dParams[2]);
	DDX_Text(pDX, IDC_P3_M24, McodeActions[11].dParams[2]);
	DDX_Text(pDX, IDC_P3_M25, McodeActions[12].dParams[2]);
	DDX_Text(pDX, IDC_P3_M26, McodeActions[13].dParams[2]);
	DDX_Text(pDX, IDC_P3_M27, McodeActions[14].dParams[2]);
	DDX_Text(pDX, IDC_P3_M28, McodeActions[15].dParams[2]);
	DDX_Text(pDX, IDC_P3_M29, McodeActions[16].dParams[2]);
	DDX_Text(pDX, IDC_P3_M30, McodeActions[17].dParams[2]);
	DDX_Text(pDX, IDC_P3_M31, McodeActions[18].dParams[2]);

	DDX_Text(pDX, IDC_P4_M0,  McodeActions[0 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M1,  McodeActions[1 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M2,  McodeActions[2 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M3,  McodeActions[3 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M4,  McodeActions[4 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M5,  McodeActions[5 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M6,  McodeActions[6 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M7,  McodeActions[7 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M8,  McodeActions[8 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M9,  McodeActions[9 ].dParams[3]);
	DDX_Text(pDX, IDC_P4_M10, McodeActions[10].dParams[3]);
	DDX_Text(pDX, IDC_P4_M24, McodeActions[11].dParams[3]);
	DDX_Text(pDX, IDC_P4_M25, McodeActions[12].dParams[3]);
	DDX_Text(pDX, IDC_P4_M26, McodeActions[13].dParams[3]);
	DDX_Text(pDX, IDC_P4_M27, McodeActions[14].dParams[3]);
	DDX_Text(pDX, IDC_P4_M28, McodeActions[15].dParams[3]);
	DDX_Text(pDX, IDC_P4_M29, McodeActions[16].dParams[3]);
	DDX_Text(pDX, IDC_P4_M30, McodeActions[17].dParams[3]);
	DDX_Text(pDX, IDC_P4_M31, McodeActions[18].dParams[3]);

	DDX_Text(pDX, IDC_P5_M0,  McodeActions[0 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M1,  McodeActions[1 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M2,  McodeActions[2 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M3,  McodeActions[3 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M4,  McodeActions[4 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M5,  McodeActions[5 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M6,  McodeActions[6 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M7,  McodeActions[7 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M8,  McodeActions[8 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M9,  McodeActions[9 ].dParams[4]);
	DDX_Text(pDX, IDC_P5_M10, McodeActions[10].dParams[4]);
	DDX_Text(pDX, IDC_P5_M24, McodeActions[11].dParams[4]);
	DDX_Text(pDX, IDC_P5_M25, McodeActions[12].dParams[4]);
	DDX_Text(pDX, IDC_P5_M26, McodeActions[13].dParams[4]);
	DDX_Text(pDX, IDC_P5_M27, McodeActions[14].dParams[4]);
	DDX_Text(pDX, IDC_P5_M28, McodeActions[15].dParams[4]);
	DDX_Text(pDX, IDC_P5_M29, McodeActions[16].dParams[4]);
	DDX_Text(pDX, IDC_P5_M30, McodeActions[17].dParams[4]);
	DDX_Text(pDX, IDC_P5_M31, McodeActions[18].dParams[4]);

	if (pDX->m_bSaveAndValidate)
	{
		strncpy(McodeActions[0 ].String,m_PS_M0, 255);
		strncpy(McodeActions[1 ].String,m_PS_M1, 255);
		strncpy(McodeActions[2 ].String,m_PS_M2, 255);
		strncpy(McodeActions[3 ].String,m_PS_M3, 255);
		strncpy(McodeActions[4 ].String,m_PS_M4, 255);
		strncpy(McodeActions[5 ].String,m_PS_M5, 255);
		strncpy(McodeActions[6 ].String,m_PS_M6, 255);
		strncpy(McodeActions[7 ].String,m_PS_M7, 255);
		strncpy(McodeActions[8 ].String,m_PS_M8, 255);
		strncpy(McodeActions[9 ].String,m_PS_M9, 255);
		strncpy(McodeActions[10].String,m_PS_M10,255);
		strncpy(McodeActions[11].String,m_PS_M24,255);
		strncpy(McodeActions[12].String,m_PS_M25,255);
		strncpy(McodeActions[13].String,m_PS_M26,255);
		strncpy(McodeActions[14].String,m_PS_M27,255);
		strncpy(McodeActions[15].String,m_PS_M28,255);
		strncpy(McodeActions[16].String,m_PS_M29,255);
		strncpy(McodeActions[17].String,m_PS_M30,255);
		strncpy(McodeActions[18].String,m_PS_M31,255);
	}
}


BEGIN_MESSAGE_MAP(CToolSetupM1Page, CToolSetupPage)
	ON_BN_CLICKED(IDC_KMotion_HELP, OnIhelp)
	ON_BN_CLICKED(IDC_Dir_M0, OnDirM0)
	ON_BN_CLICKED(IDC_Dir_M1, OnDirM1)
	ON_BN_CLICKED(IDC_Dir_M2, OnDirM2)
	ON_BN_CLICKED(IDC_Dir_M3, OnDirM3)
	ON_BN_CLICKED(IDC_Dir_M4, OnDirM4)
	ON_BN_CLICKED(IDC_Dir_M5, OnDirM5)
	ON_BN_CLICKED(IDC_Dir_M6, OnDirM6)
	ON_BN_CLICKED(IDC_Dir_M7, OnDirM7)
	ON_BN_CLICKED(IDC_Dir_M8, OnDirM8)
	ON_BN_CLICKED(IDC_Dir_M9, OnDirM9)
	ON_BN_CLICKED(IDC_Dir_M10,OnDirM10)
	ON_BN_CLICKED(IDC_Dir_M24,OnDirM24)
	ON_BN_CLICKED(IDC_Dir_M25,OnDirM25)
	ON_BN_CLICKED(IDC_Dir_M26,OnDirM26)
	ON_BN_CLICKED(IDC_Dir_M27,OnDirM27)
	ON_BN_CLICKED(IDC_Dir_M28,OnDirM28)
	ON_BN_CLICKED(IDC_Dir_M29,OnDirM29)
	ON_BN_CLICKED(IDC_Dir_M30,OnDirM30)
	ON_BN_CLICKED(IDC_Dir_M31,OnDirM31)

	ON_CBN_CLOSEUP(IDC_Action_M0, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M1, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M2, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M3, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M4, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M5, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M6, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M7, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M8, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M9, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M10,OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M24,OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M25,OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M26,OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M27,OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M28,OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M29,OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M30,OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M31,OnCloseupAction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CToolSetupM1Page::OnIhelp() 
{
	TheFrame->HelpDlg.Show("KMotionCNC\\ToolSetupScreenM3.htm");
}

void CToolSetupM1Page::SetAllActions()
{
	SetAction(&McodeActions[0], IDC_Action_M0, IDC_S1_M0,IDC_S2_M0,IDC_S3_M0,IDC_S4_M0,IDC_S5_M0,IDC_Dir_M0,
								IDC_P1_M0,IDC_P2_M0,IDC_P3_M0,IDC_P4_M0,IDC_P5_M0,IDC_PS_M0);
	SetAction(&McodeActions[1], IDC_Action_M1, IDC_S1_M1,IDC_S2_M1,IDC_S3_M1,IDC_S4_M1,IDC_S5_M1,IDC_Dir_M1,
								IDC_P1_M1,IDC_P2_M1,IDC_P3_M1,IDC_P4_M1,IDC_P5_M1,IDC_PS_M1);
	SetAction(&McodeActions[2], IDC_Action_M2, IDC_S1_M2,IDC_S2_M2,IDC_S3_M2,IDC_S4_M2,IDC_S5_M2,IDC_Dir_M2,
								IDC_P1_M2,IDC_P2_M2,IDC_P3_M2,IDC_P4_M2,IDC_P5_M2,IDC_PS_M2);
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
	SetAction(&McodeActions[10],IDC_Action_M10, IDC_S1_M10,IDC_S2_M10,IDC_S3_M10,IDC_S4_M10,IDC_S5_M10,IDC_Dir_M10,
							    IDC_P1_M10,IDC_P2_M10,IDC_P3_M10,IDC_P4_M10,IDC_P5_M10,IDC_PS_M10);
	SetAction(&McodeActions[11],IDC_Action_M24, IDC_S1_M24,IDC_S2_M24,IDC_S3_M24,IDC_S4_M24,IDC_S5_M24,IDC_Dir_M24,
								IDC_P1_M24,IDC_P2_M24,IDC_P3_M24,IDC_P4_M24,IDC_P5_M24,IDC_PS_M24);
	SetAction(&McodeActions[12],IDC_Action_M25, IDC_S1_M25,IDC_S2_M25,IDC_S3_M25,IDC_S4_M25,IDC_S5_M25,IDC_Dir_M25,
								IDC_P1_M25,IDC_P2_M25,IDC_P3_M25,IDC_P4_M25,IDC_P5_M25,IDC_PS_M25);
	SetAction(&McodeActions[13],IDC_Action_M26, IDC_S1_M26,IDC_S2_M26,IDC_S3_M26,IDC_S4_M26,IDC_S5_M26,IDC_Dir_M26,
								IDC_P1_M26,IDC_P2_M26,IDC_P3_M26,IDC_P4_M26,IDC_P5_M26,IDC_PS_M26);
	SetAction(&McodeActions[14],IDC_Action_M27, IDC_S1_M27,IDC_S2_M27,IDC_S3_M27,IDC_S4_M27,IDC_S5_M27,IDC_Dir_M27,
								IDC_P1_M27,IDC_P2_M27,IDC_P3_M27,IDC_P4_M27,IDC_P5_M27,IDC_PS_M27);
	SetAction(&McodeActions[15],IDC_Action_M28, IDC_S1_M28,IDC_S2_M28,IDC_S3_M28,IDC_S4_M28,IDC_S5_M28,IDC_Dir_M28,
								IDC_P1_M28,IDC_P2_M28,IDC_P3_M28,IDC_P4_M28,IDC_P5_M28,IDC_PS_M28);
	SetAction(&McodeActions[16],IDC_Action_M29, IDC_S1_M29,IDC_S2_M29,IDC_S3_M29,IDC_S4_M29,IDC_S5_M29,IDC_Dir_M29,
								IDC_P1_M29,IDC_P2_M29,IDC_P3_M29,IDC_P4_M29,IDC_P5_M29,IDC_PS_M29);
	SetAction(&McodeActions[17],IDC_Action_M30, IDC_S1_M30,IDC_S2_M30,IDC_S3_M30,IDC_S4_M30,IDC_S5_M30,IDC_Dir_M30,
								IDC_P1_M30,IDC_P2_M30,IDC_P3_M30,IDC_P4_M30,IDC_P5_M30,IDC_PS_M30);
	SetAction(&McodeActions[18],IDC_Action_M31, IDC_S1_M31,IDC_S2_M31,IDC_S3_M31,IDC_S4_M31,IDC_S5_M31,IDC_Dir_M31,
								IDC_P1_M31,IDC_P2_M31,IDC_P3_M31,IDC_P4_M31,IDC_P5_M31,IDC_PS_M31);

}



void CToolSetupM1Page::OnDirM0() 
{
	DoDirectoryBrowse(&McodeActions[0]);
}

void CToolSetupM1Page::OnDirM1() 
{
	DoDirectoryBrowse(&McodeActions[1]);
}

void CToolSetupM1Page::OnDirM2() 
{
	DoDirectoryBrowse(&McodeActions[2]);
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
void CToolSetupM1Page::OnDirM24() 
{
	DoDirectoryBrowse(&McodeActions[11]);
}

void CToolSetupM1Page::OnDirM25() 
{
	DoDirectoryBrowse(&McodeActions[12]);
}

void CToolSetupM1Page::OnDirM26() 
{
	DoDirectoryBrowse(&McodeActions[13]);
}

void CToolSetupM1Page::OnDirM27() 
{
	DoDirectoryBrowse(&McodeActions[14]);
}

void CToolSetupM1Page::OnDirM28() 
{
	DoDirectoryBrowse(&McodeActions[15]);
}

void CToolSetupM1Page::OnDirM29() 
{
	DoDirectoryBrowse(&McodeActions[16]);
}

void CToolSetupM1Page::OnDirM30() 
{
	DoDirectoryBrowse(&McodeActions[17]);
}

void CToolSetupM1Page::OnDirM31() 
{
	DoDirectoryBrowse(&McodeActions[18]);
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
		if (CheckThreadNo(IDC_P1_M0,&McodeActions[0])) return FALSE;
		if (CheckThreadNo(IDC_P1_M1,&McodeActions[1])) return FALSE;
		if (CheckThreadNo(IDC_P1_M2,&McodeActions[2])) return FALSE;
		if (CheckThreadNo(IDC_P1_M3,&McodeActions[3])) return FALSE;
		if (CheckThreadNo(IDC_P1_M4,&McodeActions[4])) return FALSE;
		if (CheckThreadNo(IDC_P1_M5,&McodeActions[5])) return FALSE;
		if (CheckThreadNo(IDC_P1_M6,&McodeActions[6])) return FALSE;
		if (CheckThreadNo(IDC_P1_M7,&McodeActions[7])) return FALSE;
		if (CheckThreadNo(IDC_P1_M8,&McodeActions[8])) return FALSE;
		if (CheckThreadNo(IDC_P1_M9,&McodeActions[9])) return FALSE;
		if (CheckThreadNo(IDC_P1_M10,&McodeActions[10])) return FALSE;
		if (CheckThreadNo(IDC_P1_M24,&McodeActions[11])) return FALSE;
		if (CheckThreadNo(IDC_P1_M25,&McodeActions[12])) return FALSE;
		if (CheckThreadNo(IDC_P1_M26,&McodeActions[13])) return FALSE;
		if (CheckThreadNo(IDC_P1_M27,&McodeActions[14])) return FALSE;
		if (CheckThreadNo(IDC_P1_M28,&McodeActions[15])) return FALSE;
		if (CheckThreadNo(IDC_P1_M29,&McodeActions[16])) return FALSE;
		if (CheckThreadNo(IDC_P1_M30,&McodeActions[17])) return FALSE;
		if (CheckThreadNo(IDC_P1_M31,&McodeActions[18])) return FALSE;
	}
	return CToolSetupPage::OnKillActive();
}

