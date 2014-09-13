#pragma once
#include "ToolSetupPage.h"

// CToolSetupFilesPage dialog

class CToolSetupFilesPage : public CToolSetupPage
{
	DECLARE_DYNAMIC(CToolSetupFilesPage)

public:
	CToolSetupFilesPage();
	virtual ~CToolSetupFilesPage();

// Dialog Data
	enum { IDD = IDD_ToolSetupFilesPage };
	CString	m_ToolFile;
	CString	m_SetupFile;
	CString	m_GeoFile;
	CString m_VarsFile;
	int m_DialogFace;	// Select how the main dialog should look

protected:

	afx_msg void OnIhelp();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg void OnBrowseToolFile();
	afx_msg void OnBrowseSetupFile();
	afx_msg void OnBrowseGeoFile();
	afx_msg void OnBrowseVarsFile();
	afx_msg void OnBnClickedEdittoolfile();

	DECLARE_MESSAGE_MAP()
};
