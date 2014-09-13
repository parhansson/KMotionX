#pragma once

#define NFIXTURES 9
#define NFIXAXES 6

// CEditFixtures dialog

class CEditFixtures : public CDialog
{
	DECLARE_DYNAMIC(CEditFixtures)

public:
	CEditFixtures(CWnd* pParent = NULL,	CGCodeInterpreter *Interp = NULL);   // standard constructor
	virtual ~CEditFixtures();

// Dialog Data
	enum { IDD = IDD_EditFixtures };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_MESSAGE_MAP()
public:
	double m_Fixture[NFIXAXES][NFIXTURES];
	double m_Global[NFIXAXES];
	CGCodeInterpreter *m_Interpreter;
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedClearall();
	afx_msg void OnBnClickedHelpfixture();
	afx_msg void OnBnClickedClear0();
	afx_msg void OnBnClickedClear1();
	afx_msg void OnBnClickedClear2();
	afx_msg void OnBnClickedClear3();
	afx_msg void OnBnClickedClear4();
	afx_msg void OnBnClickedClear5();
	afx_msg void OnBnClickedClear6();
	afx_msg void OnBnClickedClear7();
	afx_msg void OnBnClickedClear8();
	afx_msg void OnBnClickedClear9();
	afx_msg void OnBnClickedClearG();
	virtual BOOL OnInitDialog();
};
