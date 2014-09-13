// EditFixtures.cpp : implementation file
//

#include "stdafx.h"
#include "..\GCodeInterpreter\GCodeInterpreter.h"
#include "EditFixtures.h"
#include "MainFrm.h"

#define MAX_TOOL_LINE 200

// CEditFixtures dialog

IMPLEMENT_DYNAMIC(CEditFixtures, CDialog)

CEditFixtures::CEditFixtures(CWnd* pParent,	CGCodeInterpreter *Interp)
	: CDialog(CEditFixtures::IDD, pParent)
{
	m_Interpreter=Interp;
}

CEditFixtures::~CEditFixtures()
{
}




void CEditFixtures::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FixtureX0, m_Fixture[0][0]);
	DDX_Text(pDX, IDC_FixtureX1, m_Fixture[0][1]);
	DDX_Text(pDX, IDC_FixtureX2, m_Fixture[0][2]);
	DDX_Text(pDX, IDC_FixtureX3, m_Fixture[0][3]);
	DDX_Text(pDX, IDC_FixtureX4, m_Fixture[0][4]);
	DDX_Text(pDX, IDC_FixtureX5, m_Fixture[0][5]);
	DDX_Text(pDX, IDC_FixtureX6, m_Fixture[0][6]);
	DDX_Text(pDX, IDC_FixtureX7, m_Fixture[0][7]);
	DDX_Text(pDX, IDC_FixtureX8, m_Fixture[0][8]);

	DDX_Text(pDX, IDC_FixtureY0, m_Fixture[1][0]);
	DDX_Text(pDX, IDC_FixtureY1, m_Fixture[1][1]);
	DDX_Text(pDX, IDC_FixtureY2, m_Fixture[1][2]);
	DDX_Text(pDX, IDC_FixtureY3, m_Fixture[1][3]);
	DDX_Text(pDX, IDC_FixtureY4, m_Fixture[1][4]);
	DDX_Text(pDX, IDC_FixtureY5, m_Fixture[1][5]);
	DDX_Text(pDX, IDC_FixtureY6, m_Fixture[1][6]);
	DDX_Text(pDX, IDC_FixtureY7, m_Fixture[1][7]);
	DDX_Text(pDX, IDC_FixtureY8, m_Fixture[1][8]);

	DDX_Text(pDX, IDC_FixtureZ0, m_Fixture[2][0]);
	DDX_Text(pDX, IDC_FixtureZ1, m_Fixture[2][1]);
	DDX_Text(pDX, IDC_FixtureZ2, m_Fixture[2][2]);
	DDX_Text(pDX, IDC_FixtureZ3, m_Fixture[2][3]);
	DDX_Text(pDX, IDC_FixtureZ4, m_Fixture[2][4]);
	DDX_Text(pDX, IDC_FixtureZ5, m_Fixture[2][5]);
	DDX_Text(pDX, IDC_FixtureZ6, m_Fixture[2][6]);
	DDX_Text(pDX, IDC_FixtureZ7, m_Fixture[2][7]);
	DDX_Text(pDX, IDC_FixtureZ8, m_Fixture[2][8]);

	DDX_Text(pDX, IDC_FixtureA0, m_Fixture[3][0]);
	DDX_Text(pDX, IDC_FixtureA1, m_Fixture[3][1]);
	DDX_Text(pDX, IDC_FixtureA2, m_Fixture[3][2]);
	DDX_Text(pDX, IDC_FixtureA3, m_Fixture[3][3]);
	DDX_Text(pDX, IDC_FixtureA4, m_Fixture[3][4]);
	DDX_Text(pDX, IDC_FixtureA5, m_Fixture[3][5]);
	DDX_Text(pDX, IDC_FixtureA6, m_Fixture[3][6]);
	DDX_Text(pDX, IDC_FixtureA7, m_Fixture[3][7]);
	DDX_Text(pDX, IDC_FixtureA8, m_Fixture[3][8]);

	DDX_Text(pDX, IDC_FixtureB0, m_Fixture[4][0]);
	DDX_Text(pDX, IDC_FixtureB1, m_Fixture[4][1]);
	DDX_Text(pDX, IDC_FixtureB2, m_Fixture[4][2]);
	DDX_Text(pDX, IDC_FixtureB3, m_Fixture[4][3]);
	DDX_Text(pDX, IDC_FixtureB4, m_Fixture[4][4]);
	DDX_Text(pDX, IDC_FixtureB5, m_Fixture[4][5]);
	DDX_Text(pDX, IDC_FixtureB6, m_Fixture[4][6]);
	DDX_Text(pDX, IDC_FixtureB7, m_Fixture[4][7]);
	DDX_Text(pDX, IDC_FixtureB8, m_Fixture[4][8]);

	DDX_Text(pDX, IDC_FixtureC0, m_Fixture[5][0]);
	DDX_Text(pDX, IDC_FixtureC1, m_Fixture[5][1]);
	DDX_Text(pDX, IDC_FixtureC2, m_Fixture[5][2]);
	DDX_Text(pDX, IDC_FixtureC3, m_Fixture[5][3]);
	DDX_Text(pDX, IDC_FixtureC4, m_Fixture[5][4]);
	DDX_Text(pDX, IDC_FixtureC5, m_Fixture[5][5]);
	DDX_Text(pDX, IDC_FixtureC6, m_Fixture[5][6]);
	DDX_Text(pDX, IDC_FixtureC7, m_Fixture[5][7]);
	DDX_Text(pDX, IDC_FixtureC8, m_Fixture[5][8]);

	DDX_Text(pDX, IDC_GlobalX, m_Global[0]);
	DDX_Text(pDX, IDC_GlobalY, m_Global[1]);
	DDX_Text(pDX, IDC_GlobalZ, m_Global[2]);
	DDX_Text(pDX, IDC_GlobalA, m_Global[3]);
	DDX_Text(pDX, IDC_GlobalB, m_Global[4]);
	DDX_Text(pDX, IDC_GlobalC, m_Global[5]);
}


BEGIN_MESSAGE_MAP(CEditFixtures, CDialog)
	ON_BN_CLICKED(ID_Save, &CEditFixtures::OnBnClickedSave)
	ON_BN_CLICKED(ID_ClearAll, &CEditFixtures::OnBnClickedClearall)
	ON_BN_CLICKED(ID_HelpFixture, &CEditFixtures::OnBnClickedHelpfixture)
	ON_BN_CLICKED(ID_Clear0, &CEditFixtures::OnBnClickedClear0)
	ON_BN_CLICKED(ID_Clear1, &CEditFixtures::OnBnClickedClear1)
	ON_BN_CLICKED(ID_Clear2, &CEditFixtures::OnBnClickedClear2)
	ON_BN_CLICKED(ID_Clear3, &CEditFixtures::OnBnClickedClear3)
	ON_BN_CLICKED(ID_Clear4, &CEditFixtures::OnBnClickedClear4)
	ON_BN_CLICKED(ID_Clear5, &CEditFixtures::OnBnClickedClear5)
	ON_BN_CLICKED(ID_Clear6, &CEditFixtures::OnBnClickedClear6)
	ON_BN_CLICKED(ID_Clear7, &CEditFixtures::OnBnClickedClear7)
	ON_BN_CLICKED(ID_Clear8, &CEditFixtures::OnBnClickedClear8)
	ON_BN_CLICKED(ID_Clear9, &CEditFixtures::OnBnClickedClear9)
	ON_BN_CLICKED(ID_ClearG, &CEditFixtures::OnBnClickedClearG)
END_MESSAGE_MAP()


// CEditFixtures message handlers




void CEditFixtures::OnBnClickedSave()
{
	double *Vars = m_Interpreter->p_setup->parameters;	

	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		for (int Fix=0; Fix<NFIXTURES; Fix++)
			Vars[5221+Fix*20+axis] = m_Fixture[axis][Fix];

	for (int axis=0; axis<NFIXAXES; axis++)
		Vars[5211+axis] = m_Global[axis];


	if (m_Interpreter->rs274ngc_save_parameters())
	{
		AfxMessageBox("Error Saving Interpreter variables");
	}
}

void CEditFixtures::OnBnClickedClearall()
{
	for (int axis=0; axis<NFIXAXES; axis++)
		for (int Fix=0; Fix<NFIXTURES; Fix++)
			m_Fixture[axis][Fix]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedHelpfixture()
{
	if (!UpdateData()) return;

	TheFrame->HelpDlg.Show("KMotionCNC\\EditFixtureOffsets.htm");
}

void CEditFixtures::OnBnClickedClear0()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][0]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedClear1()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][1]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedClear2()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][2]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedClear3()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][3]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedClear4()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][4]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedClear5()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][5]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedClear6()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][6]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedClear7()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][7]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedClear8()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][8]=0.0;
	
	UpdateData(FALSE);
}

void CEditFixtures::OnBnClickedClear9()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Fixture[axis][9]=0.0;
	
	UpdateData(FALSE);
}


void CEditFixtures::OnBnClickedClearG()
{
	if (!UpdateData()) return;

	for (int axis=0; axis<NFIXAXES; axis++)
		m_Global[axis]=0.0;
	
	UpdateData(FALSE);
}


BOOL CEditFixtures::OnInitDialog()
{
	CDialog::OnInitDialog();

	int ID,Fix = m_Interpreter->p_setup->origin_index-1;

	switch (Fix)
	{
		case 0: ID=IDC_Static0; break;
		case 1: ID=IDC_Static1; break;
		case 2: ID=IDC_Static2; break;
		case 3: ID=IDC_Static3; break;
		case 4: ID=IDC_Static4; break;
		case 5: ID=IDC_Static5; break;
		case 6: ID=IDC_Static6; break;
		case 7: ID=IDC_Static7; break;
		case 8: ID=IDC_Static8; break;
	}

	CString s;
	GetDlgItemText(ID, s);
	SetDlgItemText(ID,"***"+s);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
