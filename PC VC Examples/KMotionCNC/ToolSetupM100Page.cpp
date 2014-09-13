// ToolSetupM100Page.cpp : implementation file
//

#include "stdafx.h"
#include "ToolSetupM100Page.h"
#include "MainFrm.h"

// CToolSetupM100Page dialog

IMPLEMENT_DYNAMIC(CToolSetupM100Page, CToolSetupPage)

CToolSetupM100Page::CToolSetupM100Page()
	: CToolSetupPage(CToolSetupM100Page::IDD)
{
	InitDialogComplete=FALSE;
}

CToolSetupM100Page::~CToolSetupM100Page()
{
}

void CToolSetupM100Page::DoDataExchange(CDataExchange* pDX)
{
	if (!pDX->m_bSaveAndValidate)
	{
		m_PS_M100 = McodeActions[0].String;
		m_PS_M101 = McodeActions[1].String;
		m_PS_M102 = McodeActions[2].String;
		m_PS_M103 = McodeActions[3].String;
		m_PS_M104 = McodeActions[4].String;
		m_PS_M105 = McodeActions[5].String;
		m_PS_M106 = McodeActions[6].String;
		m_PS_M107 = McodeActions[7].String;
		m_PS_M108 = McodeActions[8].String;
		m_PS_M109 = McodeActions[9].String;
		m_PS_M110 = McodeActions[10].String;
		m_PS_M111 = McodeActions[11].String;
		m_PS_M112 = McodeActions[12].String;
		m_PS_M113 = McodeActions[13].String;
		m_PS_M114 = McodeActions[14].String;
		m_PS_M115 = McodeActions[15].String;
		m_PS_M116 = McodeActions[16].String;
		m_PS_M117 = McodeActions[17].String;
		m_PS_M118 = McodeActions[18].String;
		m_PS_M119 = McodeActions[19].String;
	}


	//{{AFX_DATA_MAP(CToolSetupM100Page)
	DDX_Text(pDX, IDC_PS_M100, m_PS_M100);
	DDV_MaxChars(pDX, m_PS_M100, 255);
	DDX_Text(pDX, IDC_PS_M101, m_PS_M101);
	DDV_MaxChars(pDX, m_PS_M101, 255);
	DDX_Text(pDX, IDC_PS_M102, m_PS_M102);
	DDV_MaxChars(pDX, m_PS_M102, 255);
	DDX_Text(pDX, IDC_PS_M103, m_PS_M103);
	DDV_MaxChars(pDX, m_PS_M103, 255);
	DDX_Text(pDX, IDC_PS_M104, m_PS_M104);
	DDV_MaxChars(pDX, m_PS_M104, 255);
	DDX_Text(pDX, IDC_PS_M105, m_PS_M105);
	DDV_MaxChars(pDX, m_PS_M105, 255);
	DDX_Text(pDX, IDC_PS_M106, m_PS_M106);
	DDV_MaxChars(pDX, m_PS_M106, 255);
	DDX_Text(pDX, IDC_PS_M107, m_PS_M107);
	DDV_MaxChars(pDX, m_PS_M107, 255);
	DDX_Text(pDX, IDC_PS_M108, m_PS_M108);
	DDV_MaxChars(pDX, m_PS_M108, 255);
	DDX_Text(pDX, IDC_PS_M109, m_PS_M109);
	DDV_MaxChars(pDX, m_PS_M109, 255);
	DDX_Text(pDX, IDC_PS_M110, m_PS_M110);
	DDV_MaxChars(pDX, m_PS_M110, 255);
	DDX_Text(pDX, IDC_PS_M111, m_PS_M111);
	DDV_MaxChars(pDX, m_PS_M111, 255);
	DDX_Text(pDX, IDC_PS_M112, m_PS_M112);
	DDV_MaxChars(pDX, m_PS_M112, 255);
	DDX_Text(pDX, IDC_PS_M113, m_PS_M113);
	DDV_MaxChars(pDX, m_PS_M113, 255);
	DDX_Text(pDX, IDC_PS_M114, m_PS_M114);
	DDV_MaxChars(pDX, m_PS_M114, 255);
	DDX_Text(pDX, IDC_PS_M115, m_PS_M115);
	DDV_MaxChars(pDX, m_PS_M115, 255);
	DDX_Text(pDX, IDC_PS_M116, m_PS_M116);
	DDV_MaxChars(pDX, m_PS_M116, 255);
	DDX_Text(pDX, IDC_PS_M117, m_PS_M117);
	DDV_MaxChars(pDX, m_PS_M117, 255);
	DDX_Text(pDX, IDC_PS_M118, m_PS_M118);
	DDV_MaxChars(pDX, m_PS_M118, 255);
	DDX_Text(pDX, IDC_PS_M119, m_PS_M119);
	DDV_MaxChars(pDX, m_PS_M119, 255);

	//}}AFX_DATA_MAP

	DDX_CBIndex(pDX, IDC_Action_M100, McodeActions[0].Action);
	DDX_CBIndex(pDX, IDC_Action_M101, McodeActions[1].Action);
	DDX_CBIndex(pDX, IDC_Action_M102, McodeActions[2].Action);
	DDX_CBIndex(pDX, IDC_Action_M103, McodeActions[3].Action);
	DDX_CBIndex(pDX, IDC_Action_M104, McodeActions[4].Action);
	DDX_CBIndex(pDX, IDC_Action_M105, McodeActions[5].Action);
	DDX_CBIndex(pDX, IDC_Action_M106, McodeActions[6].Action);
	DDX_CBIndex(pDX, IDC_Action_M107, McodeActions[7].Action);
	DDX_CBIndex(pDX, IDC_Action_M108, McodeActions[8].Action);
	DDX_CBIndex(pDX, IDC_Action_M109, McodeActions[9].Action);
	DDX_CBIndex(pDX, IDC_Action_M110, McodeActions[10].Action);
	DDX_CBIndex(pDX, IDC_Action_M111, McodeActions[11].Action);
	DDX_CBIndex(pDX, IDC_Action_M112, McodeActions[12].Action);
	DDX_CBIndex(pDX, IDC_Action_M113, McodeActions[13].Action);
	DDX_CBIndex(pDX, IDC_Action_M114, McodeActions[14].Action);
	DDX_CBIndex(pDX, IDC_Action_M115, McodeActions[15].Action);
	DDX_CBIndex(pDX, IDC_Action_M116, McodeActions[16].Action);
	DDX_CBIndex(pDX, IDC_Action_M117, McodeActions[17].Action);
	DDX_CBIndex(pDX, IDC_Action_M118, McodeActions[18].Action);
	DDX_CBIndex(pDX, IDC_Action_M119, McodeActions[19].Action);

	DDX_Text(pDX, IDC_P1_M100,  McodeActions[0].dParams[0]);
	DDX_Text(pDX, IDC_P1_M101,  McodeActions[1].dParams[0]);
	DDX_Text(pDX, IDC_P1_M102,  McodeActions[2].dParams[0]);
	DDX_Text(pDX, IDC_P1_M103,  McodeActions[3].dParams[0]);
	DDX_Text(pDX, IDC_P1_M104,  McodeActions[4].dParams[0]);
	DDX_Text(pDX, IDC_P1_M105,  McodeActions[5].dParams[0]);
	DDX_Text(pDX, IDC_P1_M106,  McodeActions[6].dParams[0]);
	DDX_Text(pDX, IDC_P1_M107,  McodeActions[7].dParams[0]);
	DDX_Text(pDX, IDC_P1_M108,  McodeActions[8].dParams[0]);
	DDX_Text(pDX, IDC_P1_M109,  McodeActions[9].dParams[0]);
	DDX_Text(pDX, IDC_P1_M110,  McodeActions[10].dParams[0]);
	DDX_Text(pDX, IDC_P1_M111,  McodeActions[11].dParams[0]);
	DDX_Text(pDX, IDC_P1_M112,  McodeActions[12].dParams[0]);
	DDX_Text(pDX, IDC_P1_M113,  McodeActions[13].dParams[0]);
	DDX_Text(pDX, IDC_P1_M114,  McodeActions[14].dParams[0]);
	DDX_Text(pDX, IDC_P1_M115,  McodeActions[15].dParams[0]);
	DDX_Text(pDX, IDC_P1_M116,  McodeActions[16].dParams[0]);
	DDX_Text(pDX, IDC_P1_M117,  McodeActions[17].dParams[0]);
	DDX_Text(pDX, IDC_P1_M118,  McodeActions[18].dParams[0]);
	DDX_Text(pDX, IDC_P1_M119,  McodeActions[19].dParams[0]);

	DDX_Text(pDX, IDC_P2_M100,  McodeActions[0].dParams[1]);
	DDX_Text(pDX, IDC_P2_M101,  McodeActions[1].dParams[1]);
	DDX_Text(pDX, IDC_P2_M102,  McodeActions[2].dParams[1]);
	DDX_Text(pDX, IDC_P2_M103,  McodeActions[3].dParams[1]);
	DDX_Text(pDX, IDC_P2_M104,  McodeActions[4].dParams[1]);
	DDX_Text(pDX, IDC_P2_M105,  McodeActions[5].dParams[1]);
	DDX_Text(pDX, IDC_P2_M106,  McodeActions[6].dParams[1]);
	DDX_Text(pDX, IDC_P2_M107,  McodeActions[7].dParams[1]);
	DDX_Text(pDX, IDC_P2_M108,  McodeActions[8].dParams[1]);
	DDX_Text(pDX, IDC_P2_M109,  McodeActions[9].dParams[1]);
	DDX_Text(pDX, IDC_P2_M110,  McodeActions[10].dParams[1]);
	DDX_Text(pDX, IDC_P2_M111,  McodeActions[11].dParams[1]);
	DDX_Text(pDX, IDC_P2_M112,  McodeActions[12].dParams[1]);
	DDX_Text(pDX, IDC_P2_M113,  McodeActions[13].dParams[1]);
	DDX_Text(pDX, IDC_P2_M114,  McodeActions[14].dParams[1]);
	DDX_Text(pDX, IDC_P2_M115,  McodeActions[15].dParams[1]);
	DDX_Text(pDX, IDC_P2_M116,  McodeActions[16].dParams[1]);
	DDX_Text(pDX, IDC_P2_M117,  McodeActions[17].dParams[1]);
	DDX_Text(pDX, IDC_P2_M118,  McodeActions[18].dParams[1]);
	DDX_Text(pDX, IDC_P2_M119,  McodeActions[19].dParams[1]);

	DDX_Text(pDX, IDC_P3_M100,  McodeActions[0].dParams[2]);
	DDX_Text(pDX, IDC_P3_M101,  McodeActions[1].dParams[2]);
	DDX_Text(pDX, IDC_P3_M102,  McodeActions[2].dParams[2]);
	DDX_Text(pDX, IDC_P3_M103,  McodeActions[3].dParams[2]);
	DDX_Text(pDX, IDC_P3_M104,  McodeActions[4].dParams[2]);
	DDX_Text(pDX, IDC_P3_M105,  McodeActions[5].dParams[2]);
	DDX_Text(pDX, IDC_P3_M106,  McodeActions[6].dParams[2]);
	DDX_Text(pDX, IDC_P3_M107,  McodeActions[7].dParams[2]);
	DDX_Text(pDX, IDC_P3_M108,  McodeActions[8].dParams[2]);
	DDX_Text(pDX, IDC_P3_M109,  McodeActions[9].dParams[2]);
	DDX_Text(pDX, IDC_P3_M110,  McodeActions[10].dParams[2]);
	DDX_Text(pDX, IDC_P3_M111,  McodeActions[11].dParams[2]);
	DDX_Text(pDX, IDC_P3_M112,  McodeActions[12].dParams[2]);
	DDX_Text(pDX, IDC_P3_M113,  McodeActions[13].dParams[2]);
	DDX_Text(pDX, IDC_P3_M114,  McodeActions[14].dParams[2]);
	DDX_Text(pDX, IDC_P3_M115,  McodeActions[15].dParams[2]);
	DDX_Text(pDX, IDC_P3_M116,  McodeActions[16].dParams[2]);
	DDX_Text(pDX, IDC_P3_M117,  McodeActions[17].dParams[2]);
	DDX_Text(pDX, IDC_P3_M118,  McodeActions[18].dParams[2]);
	DDX_Text(pDX, IDC_P3_M119,  McodeActions[19].dParams[2]);

	DDX_Text(pDX, IDC_P4_M100,  McodeActions[0].dParams[3]);
	DDX_Text(pDX, IDC_P4_M101,  McodeActions[1].dParams[3]);
	DDX_Text(pDX, IDC_P4_M102,  McodeActions[2].dParams[3]);
	DDX_Text(pDX, IDC_P4_M103,  McodeActions[3].dParams[3]);
	DDX_Text(pDX, IDC_P4_M104,  McodeActions[4].dParams[3]);
	DDX_Text(pDX, IDC_P4_M105,  McodeActions[5].dParams[3]);
	DDX_Text(pDX, IDC_P4_M106,  McodeActions[6].dParams[3]);
	DDX_Text(pDX, IDC_P4_M107,  McodeActions[7].dParams[3]);
	DDX_Text(pDX, IDC_P4_M108,  McodeActions[8].dParams[3]);
	DDX_Text(pDX, IDC_P4_M109,  McodeActions[9].dParams[3]);
	DDX_Text(pDX, IDC_P4_M110,  McodeActions[10].dParams[3]);
	DDX_Text(pDX, IDC_P4_M111,  McodeActions[11].dParams[3]);
	DDX_Text(pDX, IDC_P4_M112,  McodeActions[12].dParams[3]);
	DDX_Text(pDX, IDC_P4_M113,  McodeActions[13].dParams[3]);
	DDX_Text(pDX, IDC_P4_M114,  McodeActions[14].dParams[3]);
	DDX_Text(pDX, IDC_P4_M115,  McodeActions[15].dParams[3]);
	DDX_Text(pDX, IDC_P4_M116,  McodeActions[16].dParams[3]);
	DDX_Text(pDX, IDC_P4_M117,  McodeActions[17].dParams[3]);
	DDX_Text(pDX, IDC_P4_M118,  McodeActions[18].dParams[3]);
	DDX_Text(pDX, IDC_P4_M119,  McodeActions[19].dParams[3]);

	DDX_Text(pDX, IDC_P5_M100,  McodeActions[0].dParams[4]);
	DDX_Text(pDX, IDC_P5_M101,  McodeActions[1].dParams[4]);
	DDX_Text(pDX, IDC_P5_M102,  McodeActions[2].dParams[4]);
	DDX_Text(pDX, IDC_P5_M103,  McodeActions[3].dParams[4]);
	DDX_Text(pDX, IDC_P5_M104,  McodeActions[4].dParams[4]);
	DDX_Text(pDX, IDC_P5_M105,  McodeActions[5].dParams[4]);
	DDX_Text(pDX, IDC_P5_M106,  McodeActions[6].dParams[4]);
	DDX_Text(pDX, IDC_P5_M107,  McodeActions[7].dParams[4]);
	DDX_Text(pDX, IDC_P5_M108,  McodeActions[8].dParams[4]);
	DDX_Text(pDX, IDC_P5_M109,  McodeActions[9].dParams[4]);
	DDX_Text(pDX, IDC_P5_M110,  McodeActions[10].dParams[4]);
	DDX_Text(pDX, IDC_P5_M111,  McodeActions[11].dParams[4]);
	DDX_Text(pDX, IDC_P5_M112,  McodeActions[12].dParams[4]);
	DDX_Text(pDX, IDC_P5_M113,  McodeActions[13].dParams[4]);
	DDX_Text(pDX, IDC_P5_M114,  McodeActions[14].dParams[4]);
	DDX_Text(pDX, IDC_P5_M115,  McodeActions[15].dParams[4]);
	DDX_Text(pDX, IDC_P5_M116,  McodeActions[16].dParams[4]);
	DDX_Text(pDX, IDC_P5_M117,  McodeActions[17].dParams[4]);
	DDX_Text(pDX, IDC_P5_M118,  McodeActions[18].dParams[4]);
	DDX_Text(pDX, IDC_P5_M119,  McodeActions[19].dParams[4]);

	if (pDX->m_bSaveAndValidate)
	{
		strncpy(McodeActions[0].String,m_PS_M100, 255);
		strncpy(McodeActions[1].String,m_PS_M101, 255);
		strncpy(McodeActions[2].String,m_PS_M102, 255);
		strncpy(McodeActions[3].String,m_PS_M103, 255);
		strncpy(McodeActions[4].String,m_PS_M104, 255);
		strncpy(McodeActions[5].String,m_PS_M105, 255);
		strncpy(McodeActions[6].String,m_PS_M106, 255);
		strncpy(McodeActions[7].String,m_PS_M107, 255);
		strncpy(McodeActions[8].String,m_PS_M108, 255);
		strncpy(McodeActions[9].String,m_PS_M109, 255);
		strncpy(McodeActions[10].String,m_PS_M110, 255);
		strncpy(McodeActions[11].String,m_PS_M111, 255);
		strncpy(McodeActions[12].String,m_PS_M112, 255);
		strncpy(McodeActions[13].String,m_PS_M113, 255);
		strncpy(McodeActions[14].String,m_PS_M114, 255);
		strncpy(McodeActions[15].String,m_PS_M115, 255);
		strncpy(McodeActions[16].String,m_PS_M116, 255);
		strncpy(McodeActions[17].String,m_PS_M117, 255);
		strncpy(McodeActions[18].String,m_PS_M118, 255);
		strncpy(McodeActions[19].String,m_PS_M119, 255);
	}
}


BEGIN_MESSAGE_MAP(CToolSetupM100Page, CToolSetupPage)
	ON_BN_CLICKED(IDC_KMotion_HELP, OnIhelp)
	ON_BN_CLICKED(IDC_Dir_M100, OnDirM100)
	ON_BN_CLICKED(IDC_Dir_M101, OnDirM101)
	ON_BN_CLICKED(IDC_Dir_M102, OnDirM102)
	ON_BN_CLICKED(IDC_Dir_M103, OnDirM103)
	ON_BN_CLICKED(IDC_Dir_M104, OnDirM104)
	ON_BN_CLICKED(IDC_Dir_M105, OnDirM105)
	ON_BN_CLICKED(IDC_Dir_M106, OnDirM106)
	ON_BN_CLICKED(IDC_Dir_M107, OnDirM107)
	ON_BN_CLICKED(IDC_Dir_M108, OnDirM108)
	ON_BN_CLICKED(IDC_Dir_M109, OnDirM109)
	ON_BN_CLICKED(IDC_Dir_M110, OnDirM110)
	ON_BN_CLICKED(IDC_Dir_M111, OnDirM111)
	ON_BN_CLICKED(IDC_Dir_M112, OnDirM112)
	ON_BN_CLICKED(IDC_Dir_M113, OnDirM113)
	ON_BN_CLICKED(IDC_Dir_M114, OnDirM114)
	ON_BN_CLICKED(IDC_Dir_M115, OnDirM115)
	ON_BN_CLICKED(IDC_Dir_M116, OnDirM116)
	ON_BN_CLICKED(IDC_Dir_M117, OnDirM117)
	ON_BN_CLICKED(IDC_Dir_M118, OnDirM118)
	ON_BN_CLICKED(IDC_Dir_M119, OnDirM119)
	ON_CBN_CLOSEUP(IDC_Action_M100, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M101, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M102, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M103, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M104, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M105, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M106, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M107, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M108, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M109, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M110, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M111, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M112, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M113, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M114, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M115, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M116, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M117, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M118, OnCloseupAction)
	ON_CBN_CLOSEUP(IDC_Action_M119, OnCloseupAction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CToolSetupM100Page::OnIhelp() 
{
	TheFrame->HelpDlg.Show("KMotionCNC\\ToolSetupScreenM100.htm");
}

void CToolSetupM100Page::SetAllActions()
{
	SetAction(&McodeActions[0], IDC_Action_M100, IDC_S1_M100,IDC_S2_M100,IDC_S3_M100,IDC_S4_M100,IDC_S5_M100,IDC_Dir_M100,
								IDC_P1_M100,IDC_P2_M100,IDC_P3_M100,IDC_P4_M100,IDC_P5_M100,IDC_PS_M100);
	SetAction(&McodeActions[1], IDC_Action_M101, IDC_S1_M101,IDC_S2_M101,IDC_S3_M101,IDC_S4_M101,IDC_S5_M101,IDC_Dir_M101,
								IDC_P1_M101,IDC_P2_M101,IDC_P3_M101,IDC_P4_M101,IDC_P5_M101,IDC_PS_M101);
	SetAction(&McodeActions[2], IDC_Action_M102, IDC_S1_M102,IDC_S2_M102,IDC_S3_M102,IDC_S4_M102,IDC_S5_M102,IDC_Dir_M102,
								IDC_P1_M102,IDC_P2_M102,IDC_P3_M102,IDC_P4_M102,IDC_P5_M102,IDC_PS_M102);
	SetAction(&McodeActions[3], IDC_Action_M103, IDC_S1_M103,IDC_S2_M103,IDC_S3_M103,IDC_S4_M103,IDC_S5_M103,IDC_Dir_M103,
								IDC_P1_M103,IDC_P2_M103,IDC_P3_M103,IDC_P4_M103,IDC_P5_M103,IDC_PS_M103);
	SetAction(&McodeActions[4], IDC_Action_M104, IDC_S1_M104,IDC_S2_M104,IDC_S3_M104,IDC_S4_M104,IDC_S5_M104,IDC_Dir_M104,
								IDC_P1_M104,IDC_P2_M104,IDC_P3_M104,IDC_P4_M104,IDC_P5_M104,IDC_PS_M104);
	SetAction(&McodeActions[5], IDC_Action_M105, IDC_S1_M105,IDC_S2_M105,IDC_S3_M105,IDC_S4_M105,IDC_S5_M105,IDC_Dir_M105,
								IDC_P1_M105,IDC_P2_M105,IDC_P3_M105,IDC_P4_M105,IDC_P5_M105,IDC_PS_M105);
	SetAction(&McodeActions[6], IDC_Action_M106, IDC_S1_M106,IDC_S2_M106,IDC_S3_M106,IDC_S4_M106,IDC_S5_M106,IDC_Dir_M106,
								IDC_P1_M106,IDC_P2_M106,IDC_P3_M106,IDC_P4_M106,IDC_P5_M106,IDC_PS_M106);
	SetAction(&McodeActions[7], IDC_Action_M107, IDC_S1_M107,IDC_S2_M107,IDC_S3_M107,IDC_S4_M107,IDC_S5_M107,IDC_Dir_M107,
								IDC_P1_M107,IDC_P2_M107,IDC_P3_M107,IDC_P4_M107,IDC_P5_M107,IDC_PS_M107);
	SetAction(&McodeActions[8], IDC_Action_M108, IDC_S1_M108,IDC_S2_M108,IDC_S3_M108,IDC_S4_M108,IDC_S5_M108,IDC_Dir_M108,
								IDC_P1_M108,IDC_P2_M108,IDC_P3_M108,IDC_P4_M108,IDC_P5_M108,IDC_PS_M108);
	SetAction(&McodeActions[9], IDC_Action_M109, IDC_S1_M109,IDC_S2_M109,IDC_S3_M109,IDC_S4_M109,IDC_S5_M109,IDC_Dir_M109,
								IDC_P1_M109,IDC_P2_M109,IDC_P3_M109,IDC_P4_M109,IDC_P5_M109,IDC_PS_M109);
	SetAction(&McodeActions[10], IDC_Action_M110, IDC_S1_M110,IDC_S2_M110,IDC_S3_M110,IDC_S4_M110,IDC_S5_M110,IDC_Dir_M110,
								IDC_P1_M110,IDC_P2_M110,IDC_P3_M110,IDC_P4_M110,IDC_P5_M110,IDC_PS_M110);
	SetAction(&McodeActions[11], IDC_Action_M111, IDC_S1_M111,IDC_S2_M111,IDC_S3_M111,IDC_S4_M111,IDC_S5_M111,IDC_Dir_M111,
								IDC_P1_M111,IDC_P2_M111,IDC_P3_M111,IDC_P4_M111,IDC_P5_M111,IDC_PS_M111);
	SetAction(&McodeActions[12], IDC_Action_M112, IDC_S1_M112,IDC_S2_M112,IDC_S3_M112,IDC_S4_M112,IDC_S5_M112,IDC_Dir_M112,
								IDC_P1_M112,IDC_P2_M112,IDC_P3_M112,IDC_P4_M112,IDC_P5_M112,IDC_PS_M112);
	SetAction(&McodeActions[13], IDC_Action_M113, IDC_S1_M113,IDC_S2_M113,IDC_S3_M113,IDC_S4_M113,IDC_S5_M113,IDC_Dir_M113,
								IDC_P1_M113,IDC_P2_M113,IDC_P3_M113,IDC_P4_M113,IDC_P5_M113,IDC_PS_M113);
	SetAction(&McodeActions[14], IDC_Action_M114, IDC_S1_M114,IDC_S2_M114,IDC_S3_M114,IDC_S4_M114,IDC_S5_M114,IDC_Dir_M114,
								IDC_P1_M114,IDC_P2_M114,IDC_P3_M114,IDC_P4_M114,IDC_P5_M114,IDC_PS_M114);
	SetAction(&McodeActions[15], IDC_Action_M115, IDC_S1_M115,IDC_S2_M115,IDC_S3_M115,IDC_S4_M115,IDC_S5_M115,IDC_Dir_M115,
								IDC_P1_M115,IDC_P2_M115,IDC_P3_M115,IDC_P4_M115,IDC_P5_M115,IDC_PS_M115);
	SetAction(&McodeActions[16], IDC_Action_M116, IDC_S1_M116,IDC_S2_M116,IDC_S3_M116,IDC_S4_M116,IDC_S5_M116,IDC_Dir_M116,
								IDC_P1_M116,IDC_P2_M116,IDC_P3_M116,IDC_P4_M116,IDC_P5_M116,IDC_PS_M116);
	SetAction(&McodeActions[17], IDC_Action_M117, IDC_S1_M117,IDC_S2_M117,IDC_S3_M117,IDC_S4_M117,IDC_S5_M117,IDC_Dir_M117,
								IDC_P1_M117,IDC_P2_M117,IDC_P3_M117,IDC_P4_M117,IDC_P5_M117,IDC_PS_M117);
	SetAction(&McodeActions[18], IDC_Action_M118, IDC_S1_M118,IDC_S2_M118,IDC_S3_M118,IDC_S4_M118,IDC_S5_M118,IDC_Dir_M118,
								IDC_P1_M118,IDC_P2_M118,IDC_P3_M118,IDC_P4_M118,IDC_P5_M118,IDC_PS_M118);
	SetAction(&McodeActions[19], IDC_Action_M119, IDC_S1_M119,IDC_S2_M119,IDC_S3_M119,IDC_S4_M119,IDC_S5_M119,IDC_Dir_M119,
								IDC_P1_M119,IDC_P2_M119,IDC_P3_M119,IDC_P4_M119,IDC_P5_M119,IDC_PS_M119);
	
}



void CToolSetupM100Page::OnDirM100() 
{
	DoDirectoryBrowse(&McodeActions[0]);
}

void CToolSetupM100Page::OnDirM101() 
{
	DoDirectoryBrowse(&McodeActions[1]);
}

void CToolSetupM100Page::OnDirM102() 
{
	DoDirectoryBrowse(&McodeActions[2]);
}

void CToolSetupM100Page::OnDirM103() 
{
	DoDirectoryBrowse(&McodeActions[3]);
}

void CToolSetupM100Page::OnDirM104() 
{
	DoDirectoryBrowse(&McodeActions[4]);
}

void CToolSetupM100Page::OnDirM105() 
{
	DoDirectoryBrowse(&McodeActions[5]);
}

void CToolSetupM100Page::OnDirM106() 
{
	DoDirectoryBrowse(&McodeActions[6]);
}

void CToolSetupM100Page::OnDirM107() 
{
	DoDirectoryBrowse(&McodeActions[7]);
}

void CToolSetupM100Page::OnDirM108() 
{
	DoDirectoryBrowse(&McodeActions[8]);
}

void CToolSetupM100Page::OnDirM109() 
{
	DoDirectoryBrowse(&McodeActions[9]);
}
void CToolSetupM100Page::OnDirM110() 
{
	DoDirectoryBrowse(&McodeActions[10]);
}

void CToolSetupM100Page::OnDirM111() 
{
	DoDirectoryBrowse(&McodeActions[11]);
}

void CToolSetupM100Page::OnDirM112() 
{
	DoDirectoryBrowse(&McodeActions[12]);
}

void CToolSetupM100Page::OnDirM113() 
{
	DoDirectoryBrowse(&McodeActions[13]);
}

void CToolSetupM100Page::OnDirM114() 
{
	DoDirectoryBrowse(&McodeActions[14]);
}

void CToolSetupM100Page::OnDirM115() 
{
	DoDirectoryBrowse(&McodeActions[15]);
}

void CToolSetupM100Page::OnDirM116() 
{
	DoDirectoryBrowse(&McodeActions[16]);
}

void CToolSetupM100Page::OnDirM117() 
{
	DoDirectoryBrowse(&McodeActions[17]);
}

void CToolSetupM100Page::OnDirM118() 
{
	DoDirectoryBrowse(&McodeActions[18]);
}

void CToolSetupM100Page::OnDirM119() 
{
	DoDirectoryBrowse(&McodeActions[19]);
}


BOOL CToolSetupM100Page::OnInitDialog() 
{
	CToolSetupPage::OnInitDialog();
	
	SetAllActions();	
	return TRUE;  
}


BOOL CToolSetupM100Page::OnKillActive()
{
	if (!UpdateData(TRUE)) return FALSE;
	if (InitDialogComplete)
	{
		if (CheckThreadNo(IDC_P1_M100,&McodeActions[0])) return FALSE;
		if (CheckThreadNo(IDC_P1_M101,&McodeActions[1])) return FALSE;
		if (CheckThreadNo(IDC_P1_M102,&McodeActions[2])) return FALSE;
		if (CheckThreadNo(IDC_P1_M103,&McodeActions[3])) return FALSE;
		if (CheckThreadNo(IDC_P1_M104,&McodeActions[4])) return FALSE;
		if (CheckThreadNo(IDC_P1_M105,&McodeActions[5])) return FALSE;
		if (CheckThreadNo(IDC_P1_M106,&McodeActions[6])) return FALSE;
		if (CheckThreadNo(IDC_P1_M107,&McodeActions[7])) return FALSE;
		if (CheckThreadNo(IDC_P1_M108,&McodeActions[8])) return FALSE;
		if (CheckThreadNo(IDC_P1_M109,&McodeActions[9])) return FALSE;
		if (CheckThreadNo(IDC_P1_M110,&McodeActions[10])) return FALSE;
		if (CheckThreadNo(IDC_P1_M111,&McodeActions[11])) return FALSE;
		if (CheckThreadNo(IDC_P1_M112,&McodeActions[12])) return FALSE;
		if (CheckThreadNo(IDC_P1_M113,&McodeActions[13])) return FALSE;
		if (CheckThreadNo(IDC_P1_M114,&McodeActions[14])) return FALSE;
		if (CheckThreadNo(IDC_P1_M115,&McodeActions[15])) return FALSE;
		if (CheckThreadNo(IDC_P1_M116,&McodeActions[16])) return FALSE;
		if (CheckThreadNo(IDC_P1_M117,&McodeActions[17])) return FALSE;
		if (CheckThreadNo(IDC_P1_M118,&McodeActions[18])) return FALSE;
		if (CheckThreadNo(IDC_P1_M119,&McodeActions[19])) return FALSE;
	}
	return CToolSetupPage::OnKillActive();
}
