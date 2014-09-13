// ToolSetupTPPage.cpp : implementation file
//

#include "stdafx.h"
#include "ToolSetupTPPage.h"
#include "MainFrm.h"

// CToolSetupTPPage dialog

IMPLEMENT_DYNAMIC(CToolSetupTPPage, CToolSetupPage)

CToolSetupTPPage::CToolSetupTPPage()
	: CToolSetupPage(CToolSetupTPPage::IDD)
{
	m_BreakAngle = 0.0;
	m_CollinearTol = 0.0;
	m_CornerTol = 0.0;
	m_FacetAngle = 0.0;
	m_RadiusC = 0.0;
	m_RadiusB = 0.0;
	m_RadiusA = 0.0;
	m_MaxAccelC = 0.0;
	m_MaxAccelB = 0.0;
	m_MaxAccelA = 0.0;
	m_MaxAccelX = 0.0;
	m_MaxAccelY = 0.0;
	m_MaxAccelZ = 0.0;
	m_MaxVelC = 0.0;
	m_MaxVelB = 0.0;
	m_MaxVelA = 0.0;
	m_MaxVelX = 0.0;
	m_MaxVelY = 0.0;
	m_MaxVelZ = 0.0;
	m_CountsPerInchC = 0.0;
	m_CountsPerInchB = 0.0;
	m_CountsPerInchA = 0.0;
	m_CountsPerInchX = 0.0;
	m_CountsPerInchY = 0.0;
	m_CountsPerInchZ = 0.0;
	m_ReverseRZ = FALSE;
	m_EnableGamePad = TRUE;
	m_ZeroUsingFixtures = FALSE;
	m_ArcsToSegs = TRUE;
	m_DisplayEncoder = FALSE;
	m_DegreesA = FALSE;
	m_DegreesB = FALSE;
	m_DegreesC = FALSE;
	m_Lathe = FALSE;
	m_Step0 = 0.0001;
	m_Step1 = 0.001;
	m_Step2 = 0.01;
	m_Step3 = 0.1;
	m_Step4 = 1.0;
	m_Step5 = 10.0;
	m_SpindleType = 0;
	m_SpindleAxis = 4;
	m_SpindleUpdateTime=0.1;
	m_SpindleTau = 0.1;
	m_SpindleCntsPerRev = 1000;
	InitDialogComplete=FALSE;
}

CToolSetupTPPage::~CToolSetupTPPage()
{
}

void CToolSetupTPPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CToolFile)
	DDX_Text(pDX, IDC_BreakAngle, m_BreakAngle);
	DDV_MinMaxDouble(pDX, m_BreakAngle, 0., 360.);
	DDX_Text(pDX, IDC_CollinearTol, m_CollinearTol);
	DDV_MinMaxDouble(pDX, m_CollinearTol, 0., 100.);
	DDX_Text(pDX, IDC_CornerTol, m_CornerTol);
	DDV_MinMaxDouble(pDX, m_CornerTol, 0., 100.);
	DDX_Text(pDX, IDC_FacetAngle, m_FacetAngle);
	DDV_MinMaxDouble(pDX, m_FacetAngle, 0., 100.);
	DDX_Text(pDX, IDC_TPLookahead, m_TPLookahead);
	DDV_MinMaxDouble(pDX, m_TPLookahead, 0., 10000.);
	DDX_Text(pDX, IDC_MaxAccelC, m_MaxAccelC);
	DDV_MinMaxDouble(pDX, m_MaxAccelC, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxAccelB, m_MaxAccelB);
	DDV_MinMaxDouble(pDX, m_MaxAccelB, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxAccelA, m_MaxAccelA);
	DDV_MinMaxDouble(pDX, m_MaxAccelA, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxAccelX, m_MaxAccelX);
	DDV_MinMaxDouble(pDX, m_MaxAccelX, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxAccelY, m_MaxAccelY);
	DDV_MinMaxDouble(pDX, m_MaxAccelY, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxAccelZ, m_MaxAccelZ);
	DDV_MinMaxDouble(pDX, m_MaxAccelZ, 0., 1000000000.);
	DDX_Text(pDX, IDC_RadiusC, m_RadiusC);
	DDV_MinMaxDouble(pDX, m_RadiusC, 0., 1000000000.);
	DDX_Text(pDX, IDC_RadiusB, m_RadiusB);
	DDV_MinMaxDouble(pDX, m_RadiusB, 0., 1000000000.);
	DDX_Text(pDX, IDC_RadiusA, m_RadiusA);
	DDV_MinMaxDouble(pDX, m_RadiusA, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxVelC, m_MaxVelC);
	DDV_MinMaxDouble(pDX, m_MaxVelC, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxVelB, m_MaxVelB);
	DDV_MinMaxDouble(pDX, m_MaxVelB, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxVelA, m_MaxVelA);
	DDV_MinMaxDouble(pDX, m_MaxVelA, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxVelX, m_MaxVelX);
	DDV_MinMaxDouble(pDX, m_MaxVelX, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxVelY, m_MaxVelY);
	DDV_MinMaxDouble(pDX, m_MaxVelY, 0., 1000000000.);
	DDX_Text(pDX, IDC_MaxVelZ, m_MaxVelZ);
	DDV_MinMaxDouble(pDX, m_MaxVelZ, 0., 1000000000.);
	DDX_Text(pDX, IDC_CountsPerInchC, m_CountsPerInchC);
	DDV_MinMaxDouble(pDX, m_CountsPerInchC, -1000000000., 1000000000.);
	DDX_Text(pDX, IDC_CountsPerInchB, m_CountsPerInchB);
	DDV_MinMaxDouble(pDX, m_CountsPerInchB, -1000000000., 1000000000.);
	DDX_Text(pDX, IDC_CountsPerInchA, m_CountsPerInchA);
	DDV_MinMaxDouble(pDX, m_CountsPerInchA, -1000000000., 1000000000.);
	DDX_Text(pDX, IDC_CountsPerInchX, m_CountsPerInchX);
	DDV_MinMaxDouble(pDX, m_CountsPerInchX, -1000000000., 1000000000.);
	DDX_Text(pDX, IDC_CountsPerInchY, m_CountsPerInchY);
	DDV_MinMaxDouble(pDX, m_CountsPerInchY, -1000000000., 1000000000.);
	DDX_Text(pDX, IDC_CountsPerInchZ, m_CountsPerInchZ);
	DDV_MinMaxDouble(pDX, m_CountsPerInchZ, -1000000000., 1000000000.);
	DDX_Text(pDX, IDC_JogSpeedC, m_JogSpeedC);
	DDV_MinMaxDouble(pDX, m_JogSpeedC, 0., 1000.);
	DDX_Text(pDX, IDC_JogSpeedB, m_JogSpeedB);
	DDV_MinMaxDouble(pDX, m_JogSpeedB, 0., 1000.);
	DDX_Text(pDX, IDC_JogSpeedA, m_JogSpeedA);
	DDV_MinMaxDouble(pDX, m_JogSpeedA, 0., 1000.);
	DDX_Text(pDX, IDC_JogSpeedX, m_JogSpeedX);
	DDV_MinMaxDouble(pDX, m_JogSpeedX, 0., 1000.);
	DDX_Text(pDX, IDC_JogSlowPercent, m_JogSlowPercent);
	DDV_MinMaxDouble(pDX, m_JogSlowPercent, 0., 100.);
	DDX_Text(pDX, IDC_JogSpeedY, m_JogSpeedY);
	DDV_MinMaxDouble(pDX, m_JogSpeedY, 0., 1000.);
	DDX_Text(pDX, IDC_JogSpeedZ, m_JogSpeedZ);
	DDV_MinMaxDouble(pDX, m_JogSpeedZ, 0., 1000.);
	DDX_Text(pDX, IDC_JogSlowPercent, m_JogSlowPercent);
	DDV_MinMaxDouble(pDX, m_JogSlowPercent, 0., 100.);
	DDX_Check(pDX, IDC_ReverseZA, m_ReverseRZ);
	DDX_Check(pDX, IDC_EnableGamePad, m_EnableGamePad);
	DDX_Check(pDX, IDC_ZeroUsingFixtures, m_ZeroUsingFixtures);
	DDX_Check(pDX, IDC_ArcsToSegs, m_ArcsToSegs);
	DDX_Check(pDX, IDC_DisplayEncoders, m_DisplayEncoder);
	DDX_Check(pDX, IDC_DegreesA, m_DegreesA);
	DDX_Check(pDX, IDC_DegreesB, m_DegreesB);
	DDX_Check(pDX, IDC_DegreesC, m_DegreesC);
	DDX_Check(pDX, IDC_Lathe, m_Lathe);
	DDX_Text(pDX, IDC_Step0, m_Step0);
	DDV_MinMaxDouble(pDX, m_Step0, 0., 1000000.);
	DDX_Text(pDX, IDC_Step1, m_Step1);
	DDV_MinMaxDouble(pDX, m_Step1, 0., 1000000.);
	DDX_Text(pDX, IDC_Step2, m_Step2);
	DDV_MinMaxDouble(pDX, m_Step2, 0., 1000000.);
	DDX_Text(pDX, IDC_Step3, m_Step3);
	DDV_MinMaxDouble(pDX, m_Step3, 0., 1000000.);
	DDX_Text(pDX, IDC_Step4, m_Step4);
	DDV_MinMaxDouble(pDX, m_Step4, 0., 1000000.);
	DDX_Text(pDX, IDC_Step5, m_Step5);
	DDV_MinMaxDouble(pDX, m_Step5, 0., 1000000.);
	DDX_Text(pDX, IDC_SpindleType, m_SpindleType);
	DDV_MinMaxInt(pDX, m_SpindleType, 0, 1);
	DDX_Text(pDX, IDC_SpindleAxis, m_SpindleAxis);
	DDV_MinMaxInt(pDX, m_SpindleAxis, 0, 7);
	DDX_Text(pDX, IDC_SpindleUpdateTime, m_SpindleUpdateTime);
	DDV_MinMaxDouble(pDX, m_SpindleUpdateTime, 0.001, 100);
	DDX_Text(pDX, IDC_SpindleTau, m_SpindleTau);
	DDV_MinMaxDouble(pDX, m_SpindleTau, 0.000001, 100);
	DDX_Text(pDX, IDC_SpindleCntsPerRev, m_SpindleCntsPerRev);
	DDV_MinMaxDouble(pDX, m_SpindleCntsPerRev, 1, 10000000);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolSetupTPPage, CToolSetupPage)
	ON_BN_CLICKED(IDC_KMotion_HELP, OnIhelp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DegreesA, &CToolSetupTPPage::OnBnClickedDegreesa)
	ON_BN_CLICKED(IDC_DegreesB, &CToolSetupTPPage::OnBnClickedDegreesb)
	ON_BN_CLICKED(IDC_DegreesC, &CToolSetupTPPage::OnBnClickedDegreesc)
END_MESSAGE_MAP()

void CToolSetupTPPage::OnIhelp() 
{
	TheFrame->HelpDlg.Show("KMotionCNC\\ToolSetupScreenTP.htm");
}




BOOL CToolSetupTPPage::OnInitDialog()
{
	CToolSetupPage::OnInitDialog();

	SetStatics();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CToolSetupTPPage::SetStatics()
{
	if (m_DegreesA)
	{
		SetDlgItemText(IDC_StaticUnitsA,"Cnts/deg");
		SetDlgItemText(IDC_StaticVelA,"Vel deg/sec");
		SetDlgItemText(IDC_StaticAccelA,"Accel deg/sec2");
	}
	else
	{
		SetDlgItemText(IDC_StaticUnitsA,"Cnts/inch");
		SetDlgItemText(IDC_StaticVelA,"Vel in/sec");
		SetDlgItemText(IDC_StaticAccelA,"Accel in/sec2");
	}
	if (m_DegreesB)
	{
		SetDlgItemText(IDC_StaticUnitsB,"Cnts/deg");
		SetDlgItemText(IDC_StaticVelB,"Vel deg/sec");
		SetDlgItemText(IDC_StaticAccelB,"Accel deg/sec2");
	}
	else
	{
		SetDlgItemText(IDC_StaticUnitsB,"Cnts/inch");
		SetDlgItemText(IDC_StaticVelB,"Vel in/sec");
		SetDlgItemText(IDC_StaticAccelB,"Accel in/sec2");
	}
	if (m_DegreesC)
	{
		SetDlgItemText(IDC_StaticUnitsC,"Cnts/deg");
		SetDlgItemText(IDC_StaticVelC,"Vel deg/sec");
		SetDlgItemText(IDC_StaticAccelC,"Accel deg/sec2");
	}
	else
	{
		SetDlgItemText(IDC_StaticUnitsC,"Cnts/inch");
		SetDlgItemText(IDC_StaticVelC,"Vel in/sec");
		SetDlgItemText(IDC_StaticAccelC,"Accel in/sec2");
	}
}
void CToolSetupTPPage::OnBnClickedDegreesa()
{
	UpdateData();
	SetStatics();
}

void CToolSetupTPPage::OnBnClickedDegreesb()
{
	UpdateData();
	SetStatics();
}

void CToolSetupTPPage::OnBnClickedDegreesc()
{
	UpdateData();
	SetStatics();
}

BOOL CToolSetupTPPage::OnKillActive()
{
	if (!UpdateData(TRUE)) return FALSE;
	if (InitDialogComplete)
	{
		if (m_ArcsToSegs && m_CollinearTol <=0.0)
		{
			GetDlgItem(IDC_CollinearTol)->SetFocus();
			((CEdit*)GetDlgItem(IDC_CollinearTol))->SetSel(0,-1);		
			AfxMessageBox("Error Arcs To Segs selected with Zero Collinear Tolerance");
			return FALSE;
		}
	}
	return CToolSetupPage::OnKillActive();
}
