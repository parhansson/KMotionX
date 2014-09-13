// ResumeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResumeDlg.h"


// CResumeDlg dialog

IMPLEMENT_DYNAMIC(CResumeDlg, CDialog)

CResumeDlg::CResumeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResumeDlg::IDD, pParent)
	, m_SafeZ(0)
	, m_MoveToSafeZ(FALSE)
	, m_SafeRelAbs(FALSE)
	, m_TraverseXY(FALSE)
	, m_TraverseSafeX(0)
	, m_TraverseSafeY(0)
	, m_SafeStartSpindle(FALSE)
	, m_SafeSpindleCWCCW(0)
	, m_DoSafeFeedZ(FALSE)
	, m_FeedSafeZ(0)
	, m_ZFeedRate(0)
	, m_RestoreFeedRate(FALSE)
	, m_ResumeFeedRate(0)
	, m_Metric(false)
{

}

CResumeDlg::~CResumeDlg()
{
}

void CResumeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SafeZ, m_SafeZ);
	DDX_Check(pDX, IDC_MoveToSafeZ, m_MoveToSafeZ);
	DDX_Radio(pDX, IDC_RelativeZMove, m_SafeRelAbs);
	DDX_Check(pDX, IDC_TraverseXY, m_TraverseXY);
	DDX_Text(pDX, IDC_TraverseSafeX, m_TraverseSafeX);
	DDX_Text(pDX, IDC_TraverseSafeY, m_TraverseSafeY);
	DDX_Check(pDX, IDC_SafeStartSpindle, m_SafeStartSpindle);
	DDX_Radio(pDX, IDC_SafeSpindleCW, m_SafeSpindleCWCCW);
	DDX_Check(pDX, IDC_DoSafeFeedZ, m_DoSafeFeedZ);
	DDX_Text(pDX, IDC_FeedSafeZ, m_FeedSafeZ);
	DDX_Text(pDX, IDC_ZFeedRate, m_ZFeedRate);
	DDX_Check(pDX, IDC_RestoreFeedRate, m_RestoreFeedRate);
	DDX_Text(pDX, IDC_ResumeFeedRate, m_ResumeFeedRate);
}

BEGIN_MESSAGE_MAP(CResumeDlg, CDialog)
END_MESSAGE_MAP()


BOOL CResumeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DoMetric(IDC_StaticInch0);
	DoMetric(IDC_StaticInch1);
	DoMetric(IDC_StaticInch2);
	DoMetric(IDC_StaticInch3);
	DoMetric(IDC_StaticInch4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CResumeDlg::DoMetric(int ID)
{
	CString s;

	if (m_Metric)
	{
		GetDlgItemTextA(ID,s);
		s.Replace("inches","mm");
		s.Replace("in/min","mm/min");
		SetDlgItemText(ID,s);
	}
}
