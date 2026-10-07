// IHelp.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "EXDISP.H"
#include "IHelp.h"
#include "MainFrm.h"
#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>

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
	FirstTime = EdgeHelpClient = true;

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


struct handle_data {
	unsigned long process_id;
	HWND window_handle;
};

BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !is_main_window(handle))
		return TRUE;
	data.window_handle = handle;
	return FALSE;
}

HWND find_main_window(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.window_handle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return data.window_handle;
}

HWND CIHelp::FindEdgeHelpWindow()
{
	HWND hlc = NULL;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (stricmp(entry.szExeFile, "DynoWebHelp.exe") == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

				hlc = find_main_window(entry.th32ProcessID);

				CloseHandle(hProcess);
				break;
			}
		}
	}

	CloseHandle(snapshot);

	return hlc;
}


int CIHelp::Show(CString s)
{
	if (FirstTime)
	{
		FirstTime = false;
		CString Path = TheFrame->MainPath + "\\Help\\" + "UseIEHelp.txt";
		
		FILE *f = fopen(Path, "rt");
		if (f)
		{
			fclose(f);
			EdgeHelpClient = false;
		}
	}

	CString Path = TheFrame->MainPath + "\\Help\\" + s;

	if (EdgeHelpClient)  // should we use the Edge Based Help Client?
	{
		HWND hlc = FindEdgeHelpWindow();
		if (!hlc)
		{
			// not found launch it
			CString DynoHelp = "\"" + TheFrame->MainPathRoot + "\\DynoWebHelp\\bin\\x86\\Release\\net462-windows\\DynoWebHelp.exe\"";
			int result = TheFrame->GCodeDlg.Interpreter->ExecutePC(DynoHelp, true);

			if (result)
			{
				MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Can't Execute DynoWebHelp at:\r\r" + DynoHelp),
					L"KMotion", MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);

				EdgeHelpClient = false;
			}
			else
			{
				int i;
				for (i=0; i<50; i++)
				{
					hlc = FindEdgeHelpWindow();
					if (hlc) break;
					Sleep(50);
				}

				Sleep(2500);  // Seems sending message too quickly causes it to crash

				if (!hlc)
				{
					MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Can't Find DynoWebHelp at:\r\r" + DynoHelp),
						L"KMotion", MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);

					EdgeHelpClient = false;
				}
			}
		}


		CStringW strDataToSend = Path;
		COPYDATASTRUCT cpd;
		cpd.dwData = 0;
		cpd.cbData = (strDataToSend.GetLength() + 1) * 2;
		cpd.lpData = (PVOID)strDataToSend.GetBuffer(cpd.cbData);
		::SendMessage(hlc, WM_COPYDATA, (WPARAM)::GetDesktopWindow(), (LPARAM)&cpd);
	}

	if (!EdgeHelpClient)  // should we use the Edge Based Help Client?
	{
		COleVariant noArg;

		if (m_hWnd)
			BringWindowToTop();
		else
			Create(IDD_Help, TheFrame);


		// Set To the desired URL.

		m_Web.Navigate(Path, &noArg, &noArg, &noArg, &noArg);


		InvalidateRect(NULL, FALSE);
	}
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
			if (!Silent) MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Can't Set IE Emulation Version for Help to work properly - Access Denied"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			return 1;
		}

		if (dwErrorCode != ERROR_SUCCESS)
		{
			if (!Silent) MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Can't Set IE Emulation Version for Help to work properly"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
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
				if (!Silent) MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Can't Set IE Emulation Version for Help to work properly"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
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
				if (!Silent) MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Can't Set IE Emulation Version for Help to work properly"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
				return 1;
			}

			s = s.Right(s.GetLength()-LastSlash-1);
			
			if (ERROR_SUCCESS != RegSetValueEx(key, s, 0, REG_DWORD, (const BYTE*)&nValue, sizeof(nValue)))
			{ 
				if (!Silent) MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Can't Set IE Emulation Version for Help to work properly"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
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
