#pragma once
#include "ToolSetupPage.h"

// CToolSetupTPPage dialog

class CToolSetupTPPage : public CToolSetupPage
{
	DECLARE_DYNAMIC(CToolSetupTPPage)

public:
	CToolSetupTPPage();
	virtual ~CToolSetupTPPage();

// Dialog Data
	enum { IDD = IDD_ToolSetupTPPage };
	double	m_BreakAngle;
	double	m_CollinearTol;
	double	m_CornerTol;
	double	m_FacetAngle;
	double	m_TPLookahead;
	double	m_RadiusC;
	double	m_RadiusB;
	double	m_RadiusA;
	double	m_MaxAccelC;
	double	m_MaxAccelB;
	double	m_MaxAccelA;
	double	m_MaxAccelX;
	double	m_MaxAccelY;
	double	m_MaxAccelZ;
	double	m_MaxVelC;
	double	m_MaxVelB;
	double	m_MaxVelA;
	double	m_MaxVelX;
	double	m_MaxVelY;
	double	m_MaxVelZ;
	double	m_CountsPerInchC;
	double	m_CountsPerInchB;
	double	m_CountsPerInchA;
	double	m_CountsPerInchX;
	double	m_CountsPerInchY;
	double	m_CountsPerInchZ;
	double	m_JogSpeedC;
	double	m_JogSpeedB;
	double	m_JogSpeedA;
	double	m_JogSpeedX;
	double	m_JogSpeedY;
	double	m_JogSpeedZ;
	double	m_JogSlowPercent;
	BOOL	m_ReverseRZ;
	BOOL	m_EnableGamePad;
	BOOL	m_ZeroUsingFixtures;
	BOOL	m_ArcsToSegs;
	BOOL	m_DisplayEncoder;
	BOOL	m_DegreesA;
	BOOL	m_DegreesB;
	BOOL	m_DegreesC;
	BOOL	m_Lathe;
	double	m_Step0;
	double	m_Step1;
	double	m_Step2;
	double	m_Step3;
	double	m_Step4;
	double	m_Step5;
	int m_SpindleType;
	int m_SpindleAxis;
	double m_SpindleUpdateTime;
	double m_SpindleTau;
	double m_SpindleCntsPerRev;
	BOOL InitDialogComplete;


protected:

	afx_msg void OnIhelp();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDegreesa();
	afx_msg void OnBnClickedDegreesb();
	afx_msg void OnBnClickedDegreesc();
	void SetStatics();
	virtual BOOL OnKillActive();
};
