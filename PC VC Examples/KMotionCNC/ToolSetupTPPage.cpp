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
	m_MaxAccelU = 0.0;
	m_MaxAccelV = 0.0;
	m_MaxVelC = 0.0;
	m_MaxVelB = 0.0;
	m_MaxVelA = 0.0;
	m_MaxVelX = 0.0;
	m_MaxVelY = 0.0;
	m_MaxVelZ = 0.0;
	m_MaxVelU = 0.0;
	m_MaxVelV = 0.0;
	m_CountsPerInchC = 0.0;
	m_CountsPerInchB = 0.0;
	m_CountsPerInchA = 0.0;
	m_CountsPerInchX = 0.0;
	m_CountsPerInchY = 0.0;
	m_CountsPerInchZ = 0.0;
	m_CountsPerInchU = 0.0;
	m_CountsPerInchV = 0.0;
	m_ReverseRZ = FALSE;
	m_EnableGamePad = TRUE;
	m_ZeroUsingFixtures = FALSE;
	m_ToolLengthImmediately = FALSE;
	m_ToolTableDoM6 = FALSE;
	m_ConfirmExit = TRUE;
	m_AllowConcaveCorners = FALSE;
	m_ArcsToSegs = TRUE;
	m_DisplayEncoder = FALSE;
	m_DegreesA = FALSE;
	m_DegreesB = FALSE;
	m_DegreesC = FALSE;
	m_Lathe = FALSE;
	m_DoRapidsAsFeeds = FALSE;
	m_DiameterMode = FALSE;
	m_XPosFront = FALSE;
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
	m_ThirdOrderTP = FALSE;
	m_TPLogSegs = FALSE;
	m_TPCubicKnots = FALSE;
	m_MaxJerkX = m_MaxJerkY = m_MaxJerkZ = m_MaxJerkA = 0.0;
	m_MaxJerkB = m_MaxJerkC = m_MaxJerkU = m_MaxJerkV = 0.0;
	m_ActuatorLimits = FALSE;
	for (int i = 0; i < MAX_TP_ACTUATORS; i++)
	{
		m_ActScale[i] = m_MaxActVel[i] = m_MaxActAccel[i] = m_MaxActJerk[i] = 0.0;
		m_ActDegrees[i] = FALSE;
	}
	InitDialogComplete=FALSE;
}

CToolSetupTPPage::~CToolSetupTPPage()
{
}

void CToolSetupTPPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CToolFile)
	DDX_Text(pDX, IDC_BreakAngle, m_BreakAngle);
	DDV_MinMaxDouble(pDX, m_BreakAngle, 0., 179.);
	DDX_TextMM(pDX, IDC_CollinearTol, m_CollinearTol, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_CollinearTol, 0., 100.);
	DDX_TextMM(pDX, IDC_CornerTol, m_CornerTol, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_CornerTol, 0., 100.);
	DDX_Text(pDX, IDC_FacetAngle, m_FacetAngle);
	DDV_MinMaxDouble(pDX, m_FacetAngle, 0., 100.);
	DDX_Text(pDX, IDC_TPLookahead, m_TPLookahead);
	DDV_MinMaxDouble(pDX, m_TPLookahead, 0., 10000.);
	DDX_TextMM(pDX, IDC_MaxAccelC, m_MaxAccelC, ConfigUnitsMM && !m_DegreesC);
	DDV_MinMaxDouble(pDX, m_MaxAccelC, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxAccelB, m_MaxAccelB, ConfigUnitsMM && !m_DegreesB);
	DDV_MinMaxDouble(pDX, m_MaxAccelB, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxAccelA, m_MaxAccelA, ConfigUnitsMM && !m_DegreesA);
	DDV_MinMaxDouble(pDX, m_MaxAccelA, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxAccelX, m_MaxAccelX, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxAccelX, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxAccelY, m_MaxAccelY, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxAccelY, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxAccelZ, m_MaxAccelZ, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxAccelZ, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxAccelU, m_MaxAccelU, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxAccelU, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxAccelV, m_MaxAccelV, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxAccelV, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_RadiusC, m_RadiusC, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_RadiusC, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_RadiusB, m_RadiusB, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_RadiusB, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_RadiusA, m_RadiusA, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_RadiusA, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxVelC, m_MaxVelC, ConfigUnitsMM && !m_DegreesC);
	DDV_MinMaxDouble(pDX, m_MaxVelC, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxVelB, m_MaxVelB, ConfigUnitsMM && !m_DegreesB);
	DDV_MinMaxDouble(pDX, m_MaxVelB, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxVelA, m_MaxVelA, ConfigUnitsMM && !m_DegreesA);
	DDV_MinMaxDouble(pDX, m_MaxVelA, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxVelX, m_MaxVelX, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxVelX, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxVelY, m_MaxVelY, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxVelY, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxVelZ, m_MaxVelZ, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxVelZ, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxVelU, m_MaxVelU, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxVelU, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxVelV, m_MaxVelV, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxVelV, 0., 1000000000.);
	DDX_TextMMInv(pDX, IDC_CountsPerInchC, m_CountsPerInchC, ConfigUnitsMM && !m_DegreesC);
	DDV_MinMaxDouble(pDX, m_CountsPerInchC, -1000000000., 1000000000.);
	DDX_TextMMInv(pDX, IDC_CountsPerInchB, m_CountsPerInchB, ConfigUnitsMM && !m_DegreesB);
	DDV_MinMaxDouble(pDX, m_CountsPerInchB, -1000000000., 1000000000.);
	DDX_TextMMInv(pDX, IDC_CountsPerInchA, m_CountsPerInchA, ConfigUnitsMM && !m_DegreesA);
	DDV_MinMaxDouble(pDX, m_CountsPerInchA, -1000000000., 1000000000.);
	DDX_TextMMInv(pDX, IDC_CountsPerInchX, m_CountsPerInchX, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_CountsPerInchX, -1000000000., 1000000000.);
	DDX_TextMMInv(pDX, IDC_CountsPerInchY, m_CountsPerInchY, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_CountsPerInchY, -1000000000., 1000000000.);
	DDX_TextMMInv(pDX, IDC_CountsPerInchZ, m_CountsPerInchZ, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_CountsPerInchZ, -1000000000., 1000000000.);
	DDX_TextMMInv(pDX, IDC_CountsPerInchU, m_CountsPerInchU, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_CountsPerInchU, -1000000000., 1000000000.);
	DDX_TextMMInv(pDX, IDC_CountsPerInchV, m_CountsPerInchV, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_CountsPerInchV, -1000000000., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxJerkX, m_MaxJerkX, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxJerkX, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxJerkY, m_MaxJerkY, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxJerkY, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxJerkZ, m_MaxJerkZ, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxJerkZ, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxJerkA, m_MaxJerkA, ConfigUnitsMM && !m_DegreesA);
	DDV_MinMaxDouble(pDX, m_MaxJerkA, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxJerkB, m_MaxJerkB, ConfigUnitsMM && !m_DegreesB);
	DDV_MinMaxDouble(pDX, m_MaxJerkB, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxJerkC, m_MaxJerkC, ConfigUnitsMM && !m_DegreesC);
	DDV_MinMaxDouble(pDX, m_MaxJerkC, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxJerkU, m_MaxJerkU, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxJerkU, 0., 1000000000.);
	DDX_TextMM(pDX, IDC_MaxJerkV, m_MaxJerkV, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_MaxJerkV, 0., 1000000000.);
	// Actuator table (slots 0-7): texts first, Degrees checks last, so on
	// save the texts are interpreted with the flags they were displayed
	// under; the toggle handlers re-display, preserving the base value.
	for (int i = 0; i < MAX_TP_ACTUATORS; i++)
	{
		BOOL lin = ConfigUnitsMM && !m_ActDegrees[i];
		DDX_TextMMInv(pDX, IDC_ActScale0 + i, m_ActScale[i], lin);
		DDV_MinMaxDouble(pDX, m_ActScale[i], -1000000000., 1000000000.);
		DDX_TextMM(pDX, IDC_ActVel0 + i, m_MaxActVel[i], lin);
		DDV_MinMaxDouble(pDX, m_MaxActVel[i], 0., 1000000000.);
		DDX_TextMM(pDX, IDC_ActAccel0 + i, m_MaxActAccel[i], lin);
		DDV_MinMaxDouble(pDX, m_MaxActAccel[i], 0., 1000000000.);
		DDX_TextMM(pDX, IDC_ActJerk0 + i, m_MaxActJerk[i], lin);
		DDV_MinMaxDouble(pDX, m_MaxActJerk[i], 0., 1000000000.);
	}
	for (int i = 0; i < MAX_TP_ACTUATORS; i++)
		DDX_Check(pDX, IDC_ActDegrees0 + i, m_ActDegrees[i]);
	DDX_Check(pDX, IDC_ThirdOrderTP, m_ThirdOrderTP);
	DDX_Check(pDX, IDC_TPLogSegs, m_TPLogSegs);
	DDX_Check(pDX, IDC_TPCubicKnots, m_TPCubicKnots);
	DDX_Check(pDX, IDC_ActLimitsEnable, m_ActuatorLimits);
	DDX_TextMM(pDX, IDC_JogSpeedV, m_JogSpeedV, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_JogSpeedV, 0., 1000.);
	DDX_TextMM(pDX, IDC_JogSpeedU, m_JogSpeedU, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_JogSpeedU, 0., 1000.);
	DDX_TextMM(pDX, IDC_JogSpeedC, m_JogSpeedC, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_JogSpeedC, 0., 1000.);
	DDX_TextMM(pDX, IDC_JogSpeedB, m_JogSpeedB, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_JogSpeedB, 0., 1000.);
	DDX_TextMM(pDX, IDC_JogSpeedA, m_JogSpeedA, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_JogSpeedA, 0., 1000.);
	DDX_TextMM(pDX, IDC_JogSpeedX, m_JogSpeedX, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_JogSpeedX, 0., 1000.);
	DDX_Text(pDX, IDC_JogSlowPercent, m_JogSlowPercent);
	DDV_MinMaxDouble(pDX, m_JogSlowPercent, 0., 100.);
	DDX_TextMM(pDX, IDC_JogSpeedY, m_JogSpeedY, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_JogSpeedY, 0., 1000.);
	DDX_TextMM(pDX, IDC_JogSpeedZ, m_JogSpeedZ, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_JogSpeedZ, 0., 1000.);
	DDX_Text(pDX, IDC_HardFRORange, m_HardwareFRORange);
	DDV_MinMaxDouble(pDX, m_HardwareFRORange, 0., 100.);
	DDX_Text(pDX, IDC_MaxRapidFRO, m_MaxRapidFRO);
	DDV_MinMaxDouble(pDX, m_MaxRapidFRO, 0., 100.);
	DDX_TextMM(pDX, IDC_ArcRadiusTol, m_ArcRadiusTol, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_ArcRadiusTol, 0., 100.);
	DDX_TextMM(pDX, IDC_ArcRSmallTol, m_ArcRSmallTol, ConfigUnitsMM);
	DDV_MinMaxDouble(pDX, m_ArcRSmallTol, 0., 100.);
	DDX_Check(pDX, IDC_ReverseZA, m_ReverseRZ);
	DDX_Check(pDX, IDC_EnableGamePad, m_EnableGamePad);
	DDX_Check(pDX, IDC_ZeroUsingFixtures, m_ZeroUsingFixtures);
	DDX_Check(pDX, IDC_ToolLengthImmediately, m_ToolLengthImmediately);
	DDX_Check(pDX, IDC_ToolTableDoM6, m_ToolTableDoM6);
	DDX_Check(pDX, IDC_ConfirmExit, m_ConfirmExit);
	DDX_Check(pDX, IDC_AllowConcaveCorners, m_AllowConcaveCorners);
	DDX_Check(pDX, IDC_ArcsToSegs, m_ArcsToSegs);
	DDX_Check(pDX, IDC_DisplayEncoders, m_DisplayEncoder);
	DDX_Check(pDX, IDC_DegreesA, m_DegreesA);
	DDX_Check(pDX, IDC_DegreesB, m_DegreesB);
	DDX_Check(pDX, IDC_DegreesC, m_DegreesC);
	DDX_Check(pDX, IDC_Lathe, m_Lathe);
	DDX_Check(pDX, IDC_DoRapidsAsFeeds, m_DoRapidsAsFeeds);
	DDX_Check(pDX, IDC_DiameterMode, m_DiameterMode);
	DDX_Check(pDX, IDC_XPosFront, m_XPosFront);
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
	ON_BN_CLICKED(IDC_ThirdOrderTP, &CToolSetupTPPage::OnBnClickedThirdOrderTP)
	ON_BN_CLICKED(IDC_ActLimitsEnable, &CToolSetupTPPage::OnBnClickedActLimitsEnable)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ActDegrees0, IDC_ActDegrees0 + MAX_TP_ACTUATORS - 1, OnBnClickedActDegrees)
	ON_BN_CLICKED(IDC_CopyFromCAD, &CToolSetupTPPage::OnBnClickedCopyFromCAD)
	ON_BN_CLICKED(IDC_mm, Onmm)
	ON_BN_CLICKED(IDC_inch, Oninch)

END_MESSAGE_MAP()

void CToolSetupTPPage::OnIhelp() 
{
	TheFrame->HelpDlg.Show("KMotionCNC\\ToolSetupScreenTP.htm");
}




BOOL CToolSetupTPPage::OnInitDialog()
{
	CToolSetupPage::OnInitDialog();

	SetStatics();
	UpdateAxisParamView();

	if (ConfigUnitsMM)
		CheckRadioButton(IDC_mm, IDC_inch, IDC_mm);
	else
		CheckRadioButton(IDC_mm, IDC_inch, IDC_inch);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CToolSetupTPPage::SetStatics()
{
	if (ConfigUnitsMM)
	{
		SetDlgItemText(IDC_CntsUnit, L"Counts/mm");
		SetDlgItemText(IDC_StaticVel, L"Vel mm/sec");
		SetDlgItemText(IDC_StaticAccel, L"Accel mm/sec");
		SetDlgItemText(IDC_StaticJerk, L"Jerk mm/sec3");
		SetDlgItemText(IDC_Unit1, L"mm");
		SetDlgItemText(IDC_Unit2, L"mm");
		SetDlgItemText(IDC_Unit3Full, L"mm");
		SetDlgItemText(IDC_Unit4Full, L"mm");
		SetDlgItemText(IDC_UnitPerSec, L"mm/sec");
		SetDlgItemText(IDC_StaticRadiusA, L"Radius mm");
		SetDlgItemText(IDC_StaticRadiusB, L"Radius mm");
		SetDlgItemText(IDC_StaticRadiusC, L"Radius mm");
	}
	else
	{
		SetDlgItemText(IDC_CntsUnit, L"Counts/inch");
		SetDlgItemText(IDC_StaticVel, L"Vel in/sec");
		SetDlgItemText(IDC_StaticAccel, L"Accel in/sec");
		SetDlgItemText(IDC_StaticJerk, L"Jerk in/sec3");
		SetDlgItemText(IDC_Unit1, L"in");
		SetDlgItemText(IDC_Unit2, L"in");
		SetDlgItemText(IDC_Unit3Full, L"inch");
		SetDlgItemText(IDC_Unit4Full, L"inch");
		SetDlgItemText(IDC_UnitPerSec, L"in/sec");
		SetDlgItemText(IDC_StaticRadiusA, L"Radius inches");
		SetDlgItemText(IDC_StaticRadiusB, L"Radius inches");
		SetDlgItemText(IDC_StaticRadiusC, L"Radius inches");
	}

		
	if (m_DegreesA)
	{
		SetDlgItemText(IDC_StaticUnitsA,L"Cnts/deg");
		SetDlgItemText(IDC_StaticVelA,L"Vel deg/sec");
		SetDlgItemText(IDC_StaticAccelA,L"Accel deg/sec2");
		SetDlgItemText(IDC_StaticJerkA,L"Jerk deg/sec3");
	}
	else
	{
		if (ConfigUnitsMM)
		{
			SetDlgItemText(IDC_StaticUnitsA, L"Cnts/mm");
			SetDlgItemText(IDC_StaticVelA, L"Vel mm/sec");
			SetDlgItemText(IDC_StaticAccelA, L"Accel mm/sec2");
			SetDlgItemText(IDC_StaticJerkA, L"Jerk mm/sec3");
		}
		else
		{
			SetDlgItemText(IDC_StaticUnitsA, L"Cnts/inch");
			SetDlgItemText(IDC_StaticVelA, L"Vel in/sec");
			SetDlgItemText(IDC_StaticAccelA, L"Accel in/sec2");
			SetDlgItemText(IDC_StaticJerkA, L"Jerk in/sec3");
		}
	}
	if (m_DegreesB)
	{
		SetDlgItemText(IDC_StaticUnitsB,L"Cnts/deg");
		SetDlgItemText(IDC_StaticVelB,L"Vel deg/sec");
		SetDlgItemText(IDC_StaticAccelB,L"Accel deg/sec2");
		SetDlgItemText(IDC_StaticJerkB,L"Jerk deg/sec3");
	}
	else
	{
		if (ConfigUnitsMM)
		{
			SetDlgItemText(IDC_StaticUnitsB, L"Cnts/mm");
			SetDlgItemText(IDC_StaticVelB, L"Vel mm/sec");
			SetDlgItemText(IDC_StaticAccelB, L"Accel mm/sec2");
			SetDlgItemText(IDC_StaticJerkB, L"Jerk mm/sec3");
		}
		else
		{
			SetDlgItemText(IDC_StaticUnitsB, L"Cnts/inch");
			SetDlgItemText(IDC_StaticVelB, L"Vel in/sec");
			SetDlgItemText(IDC_StaticAccelB, L"Accel in/sec2");
			SetDlgItemText(IDC_StaticJerkB, L"Jerk in/sec3");
		}
	}
	if (m_DegreesC)
	{
		SetDlgItemText(IDC_StaticUnitsC,L"Cnts/deg");
		SetDlgItemText(IDC_StaticVelC,L"Vel deg/sec");
		SetDlgItemText(IDC_StaticAccelC,L"Accel deg/sec2");
		SetDlgItemText(IDC_StaticJerkC,L"Jerk deg/sec3");
	}
	else
	{
		if (ConfigUnitsMM)
		{
			SetDlgItemText(IDC_StaticUnitsC, L"Cnts/mm");
			SetDlgItemText(IDC_StaticVelC, L"Vel mm/sec");
			SetDlgItemText(IDC_StaticAccelC, L"Accel mm/sec2");
			SetDlgItemText(IDC_StaticJerkC, L"Jerk mm/sec3");
		}
		else
		{
			SetDlgItemText(IDC_StaticUnitsC, L"Cnts/inch");
			SetDlgItemText(IDC_StaticVelC, L"Vel in/sec");
			SetDlgItemText(IDC_StaticAccelC, L"Accel in/sec2");
			SetDlgItemText(IDC_StaticJerkC, L"Jerk in/sec3");
		}
	}
}
void CToolSetupTPPage::OnBnClickedDegreesa()
{
	UpdateData();
	SetStatics();
	UpdateData(FALSE);
}

void CToolSetupTPPage::OnBnClickedDegreesb()
{
	UpdateData();
	SetStatics();
	UpdateData(FALSE);
}

void CToolSetupTPPage::OnBnClickedDegreesc()
{
	UpdateData();
	SetStatics();
	UpdateData(FALSE);
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
			MessageBox( /*TRAN*/TheFrame->KMotionDLL->Translate("Error Arcs To Segs selected with Zero Collinear Tolerance"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			return FALSE;
		}
	}
	return CToolSetupPage::OnKillActive();
}

void CToolSetupTPPage::Onmm()
{
	UpdateData();
	ConfigUnitsMM = TRUE;
	CheckRadioButton(IDC_mm, IDC_inch, IDC_mm);
	UpdateData(FALSE);
	SetStatics();
}

void CToolSetupTPPage::Oninch()
{
	UpdateData();
	ConfigUnitsMM = FALSE;
	CheckRadioButton(IDC_mm, IDC_inch, IDC_inch);
	UpdateData(FALSE);
	SetStatics();
}

// Show either the CAD Axis Parameters or the per-slot Actuator Limits
// table inside the Axis Parameters group, per the Actuator Space Limits
// mode; the Jerk column (in whichever view is active) shows only when
// the 3rd Order Planner is selected.
void CToolSetupTPPage::UpdateAxisParamView()
{
	int i;
	BOOL act = IsDlgButtonChecked(IDC_ActLimitsEnable) == BST_CHECKED;
	BOOL third = IsDlgButtonChecked(IDC_ThirdOrderTP) == BST_CHECKED;
	int cad = act ? SW_HIDE : SW_SHOW;
	int slot = act ? SW_SHOW : SW_HIDE;

	// CAD view: row labels, Counts/Vel/Accel edits, headers, ABC extras
	static const int CADIds[] = {
		IDC_CADRowX, IDC_CADRowY, IDC_CADRowZ, IDC_CADRowA,
		IDC_CADRowB, IDC_CADRowC, IDC_CADRowU, IDC_CADRowV,
		IDC_CountsPerInchX, IDC_CountsPerInchY, IDC_CountsPerInchZ,
		IDC_CountsPerInchA, IDC_CountsPerInchB, IDC_CountsPerInchC,
		IDC_CountsPerInchU, IDC_CountsPerInchV,
		IDC_MaxVelX, IDC_MaxVelY, IDC_MaxVelZ, IDC_MaxVelA,
		IDC_MaxVelB, IDC_MaxVelC, IDC_MaxVelU, IDC_MaxVelV,
		IDC_MaxAccelX, IDC_MaxAccelY, IDC_MaxAccelZ, IDC_MaxAccelA,
		IDC_MaxAccelB, IDC_MaxAccelC, IDC_MaxAccelU, IDC_MaxAccelV,
		IDC_CntsUnit, IDC_StaticVel, IDC_StaticAccel,
		IDC_StaticUnitsA, IDC_StaticVelA, IDC_StaticAccelA,
		IDC_StaticUnitsB, IDC_StaticVelB, IDC_StaticAccelB,
		IDC_StaticUnitsC, IDC_StaticVelC, IDC_StaticAccelC,
		IDC_DegreesA, IDC_DegreesB, IDC_DegreesC,
		IDC_RadiusA, IDC_RadiusB, IDC_RadiusC,
		IDC_StaticRadiusA, IDC_StaticRadiusB, IDC_StaticRadiusC };
	static const int CADJerkIds[] = {
		IDC_MaxJerkX, IDC_MaxJerkY, IDC_MaxJerkZ, IDC_MaxJerkA,
		IDC_MaxJerkB, IDC_MaxJerkC, IDC_MaxJerkU, IDC_MaxJerkV,
		IDC_StaticJerk, IDC_StaticJerkA, IDC_StaticJerkB, IDC_StaticJerkC };

	for (i = 0; i < (int)(sizeof(CADIds)/sizeof(CADIds[0])); i++)
		GetDlgItem(CADIds[i])->ShowWindow(cad);
	for (i = 0; i < (int)(sizeof(CADJerkIds)/sizeof(CADJerkIds[0])); i++)
		GetDlgItem(CADJerkIds[i])->ShowWindow((!act && third) ? SW_SHOW : SW_HIDE);

	// Actuator view: slot labels, grid, Degrees, note, Copy button
	for (i = 0; i < MAX_TP_ACTUATORS; i++)
	{
		GetDlgItem(IDC_ActRow0     + i)->ShowWindow(slot);
		GetDlgItem(IDC_ActScale0   + i)->ShowWindow(slot);
		GetDlgItem(IDC_ActVel0     + i)->ShowWindow(slot);
		GetDlgItem(IDC_ActAccel0   + i)->ShowWindow(slot);
		GetDlgItem(IDC_ActJerk0    + i)->ShowWindow((act && third) ? SW_SHOW : SW_HIDE);
		GetDlgItem(IDC_ActDegrees0 + i)->ShowWindow(slot);
	}
	GetDlgItem(IDC_ActHdrScale)->ShowWindow(slot);
	GetDlgItem(IDC_ActHdrVel)->ShowWindow(slot);
	GetDlgItem(IDC_ActHdrAccel)->ShowWindow(slot);
	GetDlgItem(IDC_ActHdrJerk)->ShowWindow((act && third) ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_ActNote)->ShowWindow(slot);
	GetDlgItem(IDC_CopyFromCAD)->ShowWindow(slot);

	SetDlgItemText(IDC_AxisParamGroup,
		act ? L"Actuator Limits (slots 0-7)" : L"Axis Parameters");

	// The 3rd Order planner always streams rapids as feeds (option hidden)
	// and always facets arcs to segments (shown checked but disabled;
	// Collinear Tolerance governs the faceting)
	GetDlgItem(IDC_DoRapidsAsFeeds)->ShowWindow(third ? SW_HIDE : SW_SHOW);
	if (third) CheckDlgButton(IDC_ArcsToSegs, BST_CHECKED);
	GetDlgItem(IDC_ArcsToSegs)->EnableWindow(!third);
}

void CToolSetupTPPage::OnBnClickedThirdOrderTP()
{
	UpdateAxisParamView();
}

void CToolSetupTPPage::OnBnClickedActLimitsEnable()
{
	UpdateAxisParamView();
}

// A Degrees toggle changes that slot's display units: re-read with the
// old flags, then re-display with the new, preserving the base value.
void CToolSetupTPPage::OnBnClickedActDegrees(UINT nID)
{
	UpdateData();
	UpdateData(FALSE);
}

// Seed the actuator table from the CAD axis parameters currently on the
// screen (exact for the standard linear 1:1 machine; a starting point
// for nonlinear ones).  Degrees flags follow the CAD A/B/C settings.
void CToolSetupTPPage::OnBnClickedCopyFromCAD()
{
	if (!UpdateData(TRUE)) return;      // capture current CAD fields

	double cadScale[MAX_TP_ACTUATORS] = { 0 }, cadVel[MAX_TP_ACTUATORS] = { 0 };
	double cadAccel[MAX_TP_ACTUATORS] = { 0 }, cadJerk[MAX_TP_ACTUATORS] = { 0 };
	BOOL   cadDeg[MAX_TP_ACTUATORS] = { 0 };

	cadScale[0] = m_CountsPerInchX; cadVel[0] = m_MaxVelX; cadAccel[0] = m_MaxAccelX; cadJerk[0] = m_MaxJerkX;
	cadScale[1] = m_CountsPerInchY; cadVel[1] = m_MaxVelY; cadAccel[1] = m_MaxAccelY; cadJerk[1] = m_MaxJerkY;
	cadScale[2] = m_CountsPerInchZ; cadVel[2] = m_MaxVelZ; cadAccel[2] = m_MaxAccelZ; cadJerk[2] = m_MaxJerkZ;
	cadScale[3] = m_CountsPerInchA; cadVel[3] = m_MaxVelA; cadAccel[3] = m_MaxAccelA; cadJerk[3] = m_MaxJerkA; cadDeg[3] = m_DegreesA;
	cadScale[4] = m_CountsPerInchB; cadVel[4] = m_MaxVelB; cadAccel[4] = m_MaxAccelB; cadJerk[4] = m_MaxJerkB; cadDeg[4] = m_DegreesB;
	cadScale[5] = m_CountsPerInchC; cadVel[5] = m_MaxVelC; cadAccel[5] = m_MaxAccelC; cadJerk[5] = m_MaxJerkC; cadDeg[5] = m_DegreesC;
	cadScale[6] = m_CountsPerInchU; cadVel[6] = m_MaxVelU; cadAccel[6] = m_MaxAccelU; cadJerk[6] = m_MaxJerkU;
	cadScale[7] = m_CountsPerInchV; cadVel[7] = m_MaxVelV; cadAccel[7] = m_MaxAccelV; cadJerk[7] = m_MaxJerkV;

	for (int i = 0; i < MAX_TP_ACTUATORS; i++)
	{
		m_ActScale[i]    = cadScale[i];
		m_MaxActVel[i]   = cadVel[i];
		m_MaxActAccel[i] = cadAccel[i];
		if (cadJerk[i] != 0.0) m_MaxActJerk[i] = cadJerk[i];
		m_ActDegrees[i]  = cadDeg[i];
	}

	UpdateData(FALSE);
}
