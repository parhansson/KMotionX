// ToolSetupFilesPage.cpp : implementation file
//

#include "stdafx.h"
#include "ToolSetupFilesPage.h"
#include "EditToolFile.h"
#include "MainFrm.h"
#include "OpenDlg.h"

// CToolSetupFilesPage dialog

IMPLEMENT_DYNAMIC(CToolSetupFilesPage, CToolSetupPage)

CToolSetupFilesPage::CToolSetupFilesPage()
	: CToolSetupPage(CToolSetupFilesPage::IDD)
{
	m_ToolFile = _T("");
	m_SetupFile = _T("");
	m_GeoFile = _T("");
	m_DialogFace=0;
}

CToolSetupFilesPage::~CToolSetupFilesPage()
{
}

void CToolSetupFilesPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CToolSetupFilesPage)
	DDX_Text(pDX, IDC_ToolFile, m_ToolFile);
	DDX_Text(pDX, IDC_SetupFile, m_SetupFile);
	DDX_Text(pDX, IDC_GeoFile, m_GeoFile);
	DDX_Text(pDX, IDC_VarsFile, m_VarsFile);
	DDX_CBIndex(pDX, IDC_DialogFace, m_DialogFace);
}


BEGIN_MESSAGE_MAP(CToolSetupFilesPage, CToolSetupPage)
	ON_BN_CLICKED(IDC_KMotion_HELP, OnIhelp)
	ON_BN_CLICKED(IDC_BrowseToolFile, OnBrowseToolFile)
	ON_BN_CLICKED(IDC_BrowseSetupFile, OnBrowseSetupFile)
	ON_BN_CLICKED(IDC_BrowseGeoFile, OnBrowseGeoFile)
	ON_BN_CLICKED(IDC_BrowseVarsFile, OnBrowseVarsFile)
	ON_BN_CLICKED(IDC_EditToolFile, &OnBnClickedEdittoolfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CToolSetupFilesPage::OnIhelp() 
{
	TheFrame->HelpDlg.Show("KMotionCNC\\ToolSetupScreenFiles.htm");
}

void CToolSetupFilesPage::OnBrowseToolFile() 
{
	CPersistOpenDlg FileDlg (TRUE, ".tbl", NULL, 
		OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"Tool Files (*.tbl)|*.tbl|All Files (*.*)|*.*||");
	
	if (FileDlg.DoModal() == IDOK)
	{
		m_ToolFile = FileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CToolSetupFilesPage::OnBrowseSetupFile() 
{
	CPersistOpenDlg FileDlg (TRUE, ".set", NULL, 
		OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"Setup Files (*.set)|*.set|All Files (*.*)|*.*||");
	
	if (FileDlg.DoModal() == IDOK)
	{
		m_SetupFile = FileDlg.GetPathName();
		UpdateData(FALSE);
	}
}


void CToolSetupFilesPage::OnBrowseGeoFile() 
{
	CPersistOpenDlg FileDlg (TRUE, ".txt", NULL, 
		OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"Geo Files (*.txt)|*.txt|All Files (*.*)|*.*||");
	
	if (FileDlg.DoModal() == IDOK)
	{
		m_GeoFile = FileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CToolSetupFilesPage::OnBrowseVarsFile() 
{
	CPersistOpenDlg FileDlg (TRUE, ".var", NULL, 
		OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"Variables Files (*.var)|*.var|All Files (*.*)|*.*||");
	
	if (FileDlg.DoModal() == IDOK)
	{
		m_VarsFile = FileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CToolSetupFilesPage::OnBnClickedEdittoolfile()
{
	CEditToolFile EditToolFile;

	if (EditToolFile.LoadFile(m_ToolFile)) return;

	if (EditToolFile.DoModal() == IDOK)
	{
		EditToolFile.SaveFile(m_ToolFile);
		TheFrame->GCodeDlg.Interpreter->m_ReadToolFile=true;
	}
}

