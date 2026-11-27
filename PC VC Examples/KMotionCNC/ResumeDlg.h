#pragma once


// CResumeDlg dialog

class CResumeDlg : public CDialog
{
	DECLARE_DYNAMIC(CResumeDlg)

public:
	CResumeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CResumeDlg();

// Dialog Data
	enum { IDD = IDD_Resume };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_SafeZ;
	BOOL m_MoveToSafeZ;
	BOOL m_SafeRelAbs;
	BOOL m_TraverseXY;
	double m_TraverseSafeX;
	double m_TraverseSafeY;
	BOOL m_SafeStartSpindle;
	int m_SafeSpindleCWCCW;
	BOOL m_DoSafeFeedZ;
	double m_FeedSafeZ;
	double m_ZFeedRate;
	BOOL m_RestoreFeedRate;
	double m_ResumeFeedRate;
	virtual BOOL OnInitDialog();
	void DoMetric(int ID);
	bool m_Metric;
};
