// IHelp.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "EXDISP.H"
#include "IHelp.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIHelp dialog


CIHelp::CIHelp(CWnd* pParent /*=NULL*/)
	: CDlgX(pParent)
{
	//{{AFX_DATA_INIT(CIHelp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIHelp::DoDataExchange(CDataExchange* pDX)
{

	CDlgX::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIHelp)
	DDX_Control(pDX, IDC_EXPLORER1, m_Web);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIHelp, CDlgX)
	//{{AFX_MSG_MAP(CIHelp)
	ON_BN_CLICKED(IDC_Back, OnBack)
	ON_BN_CLICKED(IDC_Forward, OnForward)
	ON_BN_CLICKED(IDC_PrintBrowser, OnPrintBrowser)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_Contents, OnContents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIHelp message handlers


void CIHelp::OnBack() 
{
	m_Web.GoBack();
}

void CIHelp::OnForward() 
{
	m_Web.GoForward();
}

void CIHelp::OnPrintBrowser() 
{
	LPDISPATCH lpDispatch = NULL;
	LPOLECOMMANDTARGET lpOleCommandTarget = NULL;

	lpDispatch = m_Web.GetDocument();
	ASSERT(lpDispatch);

	lpDispatch->QueryInterface(IID_IOleCommandTarget, (void**)&lpOleCommandTarget);
	ASSERT(lpOleCommandTarget);

	lpDispatch->Release();

	// Print contents of WebBrowser control.
	lpOleCommandTarget->Exec(NULL, OLECMDID_PRINT, 0, NULL,NULL);
	lpOleCommandTarget->Release();
}

BEGIN_EVENTSINK_MAP(CIHelp, CDlgX)
    //{{AFX_EVENTSINK_MAP(CIHelp)
	ON_EVENT(CIHelp, IDC_EXPLORER1, 105 /* CommandStateChange */, OnCommandStateChangeExplorer1, VTS_I4 VTS_BOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CIHelp::OnCommandStateChangeExplorer1(long Command, BOOL Enable) 
{
	switch(Command)
    {
		case CSC_NAVIGATEFORWARD:
           // m_ctlForward is a CButton type
           m_Forward.EnableWindow(Enable);
           break;

		case CSC_NAVIGATEBACK:
           m_Back.EnableWindow(Enable);
           break;

       default:
           break;
      }   
}

void CIHelp::OnPaint() 
{
	RECT rect;
	
	if (InitDialogComplete)  // save the dialog window position
	{
        GetClientRect(&rect);
		m_Web.MoveWindow( 0, 30, rect.right, rect.bottom-30);
		m_Web.UpdateWindow();
	}
	CDlgX::OnPaint();
}



int CIHelp::Show(CString s)
{
    COleVariant noArg;

	CString Path = TheFrame->MainPath + "\\Help\\" + s;

	if (m_hWnd)
		BringWindowToTop();
	else
		Create(IDD_Help,TheFrame);

	
	// Set To the desired URL.

    m_Web.Navigate(Path,&noArg,&noArg,&noArg,&noArg);


	InvalidateRect(NULL,FALSE);
	return 0;
}

void CIHelp::OnContents() 
{
	Show(Contents);
}

#define ACCESS  (KEY_WRITE | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS)
#define Name "Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"

int CIHelp::TryToSetIEVersionToUse(int Version, bool Silent)
{
	static bool FirstTime = true;

	if (FirstTime)  // Only try this once
	{
		FirstTime = false;

		HKEY key;
		long dwErrorCode = ::RegOpenKeyEx(HKEY_CURRENT_USER, Name, 0, ACCESS | KEY_WOW64_32KEY, &key);

		if (dwErrorCode == ERROR_ACCESS_DENIED)
		{
			if (!Silent) AfxMessageBox("Can't Set IE Emulation Version for Help to work properly - Access Denied");
			return 1;
		}

		if (dwErrorCode != ERROR_SUCCESS)
		{
			if (!Silent) AfxMessageBox("Can't Set IE Emulation Version for Help to work properly");
			return 1;
		}

		DWORD nValue = Version;
		DWORD dwBufferSize;
		DWORD nResult=0;
		LONG nError = ::RegQueryValueEx(key, "KMotionCNC.exe", 0, NULL, (LPBYTE)&nResult, &dwBufferSize);

		if (ERROR_SUCCESS == nError)
		{
			// it exists - leave it alone
			nValue = nResult;
		}
		else
		{
			// doesn't exist try to add it
			if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, (CString)Name, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WOW64_32KEY, NULL, &key, NULL))
			{
				if (!Silent) AfxMessageBox("Can't Set IE Emulation Version for Help to work properly");
				return 1;
			}

			CString Executable;
			GetModuleFileName(NULL, Executable.GetBuffer(MAX_PATH), MAX_PATH);
			Executable.ReleaseBuffer();

			// Figure out our executable name to make the key with

			// Avoid strange relative paths
			CString s;
			_fullpath(s.GetBufferSetLength(MAX_PATH), Executable, MAX_PATH);
			s.ReleaseBuffer();

			s.Replace("\"", "");  // remove quotes
			s.TrimRight();
			s.TrimLeft();

			int LastSlash = s.ReverseFind('\\');

			if (LastSlash == -1)
			{
				if (!Silent) AfxMessageBox("Can't Set IE Emulation Version for Help to work properly");
				return 1;
			}

			s = s.Right(s.GetLength()-LastSlash-1);
			
			if (ERROR_SUCCESS != RegSetValueEx(key, s, 0, REG_DWORD, (const BYTE*)&nValue, sizeof(nValue)))
			{ 
				if (!Silent) AfxMessageBox("Can't Set IE Emulation Version for Help to work properly");
				return 1;
			}
		}

		RegCloseKey(key);
	}
	return 0;
}

BOOL CIHelp::OnInitDialog() 
{
	CDlgX::OnInitDialog();
	
	TryToSetIEVersionToUse(0x9000, true);  // try and set the Version to use silently

	m_BUTTON_Close.AutoLoad(IDOK, this);
	m_BUTTON_Print.AutoLoad(IDC_PrintBrowser, this);
	m_Forward.AutoLoad(IDC_Forward, this);
	m_Contents.AutoLoad(IDC_Contents, this);
	m_Back.AutoLoad(IDC_Back, this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
