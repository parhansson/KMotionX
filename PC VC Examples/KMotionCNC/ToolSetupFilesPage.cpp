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

BOOL CToolSetupFilesPage::OnInitDialog()
{
	CToolSetupPage::OnInitDialog();

	SetAllShowHide();
	return TRUE;
}

void CToolSetupFilesPage::SetAllShowHide()
{
	if (m_DialogFace == CUSTOM_DLG_FACE)
	{
		GetDlgItem(IDC_StaticScreenScript)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BrowseScreenScriptFile)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ScreenScriptFile)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_StaticScreenScript)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BrowseScreenScriptFile)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ScreenScriptFile)->ShowWindow(SW_HIDE);
	}
}


void CToolSetupFilesPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CToolSetupFilesPage)
	DDX_Text(pDX, IDC_ToolFile, m_ToolFile);
	DDX_Text(pDX, IDC_SetupFile, m_SetupFile);
	DDX_Text(pDX, IDC_GeoFile, m_GeoFile);
	DDX_Text(pDX, IDC_VarsFile, m_VarsFile);
	DDX_Text(pDX, IDC_ScreenScriptFile, m_ScreenScriptFile);
	DDX_CBIndex(pDX, IDC_DialogFace, m_DialogFace);
}


BEGIN_MESSAGE_MAP(CToolSetupFilesPage, CToolSetupPage)
	ON_BN_CLICKED(IDC_KMotion_HELP, OnIhelp)
	ON_BN_CLICKED(IDC_BrowseToolFile, OnBrowseToolFile)
	ON_BN_CLICKED(IDC_BrowseSetupFile, OnBrowseSetupFile)
	ON_BN_CLICKED(IDC_BrowseGeoFile, OnBrowseGeoFile)
	ON_BN_CLICKED(IDC_BrowseVarsFile, OnBrowseVarsFile)
	ON_BN_CLICKED(IDC_BrowseScreenScriptFile, OnBrowseScreenScriptFile)
	ON_BN_CLICKED(IDC_EditToolFile, &OnBnClickedEdittoolfile)
	ON_CBN_CLOSEUP(IDC_DialogFace, OnCloseupDialogFace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CToolSetupFilesPage::OnCloseupDialogFace()
{
	if (!UpdateData()) return;
	SetAllShowHide();
}

void CToolSetupFilesPage::OnIhelp() 
{
	TheFrame->HelpDlg.Show("KMotionCNC\\ToolSetupScreenFiles.htm");
}

void CToolSetupFilesPage::OnBrowseToolFile() 
{
	CPersistOpenDlg FileDlg (TRUE, ".tbl", 
		TheFrame->GCodeDlg.InitialFile(m_ToolFile, DATA_SUB_DIR, "Default.tbl"),
		OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"Tool Files (*.tbl)|*.tbl|All Files (*.*)|*.*||");
	
	if (FileDlg.DoModal() == IDOK)
	{
		m_ToolFile = TheFrame->GCodeDlg.StripPathMatch(FileDlg.GetPathName(), DATA_SUB_DIR);
		UpdateData(FALSE);
	}
}

void CToolSetupFilesPage::OnBrowseSetupFile() 
{
	CPersistOpenDlg FileDlg (TRUE, ".set", 
		TheFrame->GCodeDlg.InitialFile(m_SetupFile, DATA_SUB_DIR, "Default.set"),
		OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"Setup Files (*.set)|*.set|All Files (*.*)|*.*||");
	
	if (FileDlg.DoModal() == IDOK)
	{
		m_SetupFile = TheFrame->GCodeDlg.StripPathMatch(FileDlg.GetPathName(), DATA_SUB_DIR);
		UpdateData(FALSE);
	}
}


void CToolSetupFilesPage::OnBrowseGeoFile() 
{
	CPersistOpenDlg FileDlg (TRUE, ".txt", 
		TheFrame->GCodeDlg.InitialFile(m_GeoFile, DATA_SUB_DIR, "Measurements.txt"),
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
	CPersistOpenDlg FileDlg (TRUE, ".var", 
		TheFrame->GCodeDlg.InitialFile(m_VarsFile, DATA_SUB_DIR, "emc.var"),
				OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"Variables Files (*.var)|*.var|All Files (*.*)|*.*||");
	
	if (FileDlg.DoModal() == IDOK)
	{
		m_VarsFile = FileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CToolSetupFilesPage::OnBrowseScreenScriptFile()
{
	CPersistOpenDlg FileDlg (TRUE, ".scr", 
		TheFrame->GCodeDlg.InitialFile(m_ScreenScriptFile, SCREEN_SCRIPTS_DIR, "Default.scr"),
		OFN_FILEMUSTEXIST | OFN_ENABLESIZING, 
		"ScreenScript Files (*.scr)|*.scr|All Files (*.*)|*.*||");

	
	if (FileDlg.DoModal() == IDOK)
	{
		m_ScreenScriptFile = TheFrame->GCodeDlg.StripPathMatch(FileDlg.GetPathName(), SCREEN_SCRIPTS_DIR);
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
		TheFrame->GCodeDlg.HandleToolTableClose();
	}
}

