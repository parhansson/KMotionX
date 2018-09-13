////////////////////////////////////////////////////////////////
// MSDN Magazine -- February 2005
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio .NET 2003 on Windows XP. Tab size=3.
//
#include "StdAfx.h"
#include "OpenDlg.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Self-initialization message posted.
const int MYWM_POSTINIT = WM_USER+1;
LPCTSTR PROFILE_SETTINGS = _T("settings");
LPCTSTR PROFILE_VIEWMODE = _T("ViewMode");

IMPLEMENT_DYNAMIC(CPersistOpenDlg, CFileDialog)
BEGIN_MESSAGE_MAP(CPersistOpenDlg, CFileDialog)
	ON_MESSAGE(MYWM_POSTINIT, OnPostInit)
END_MESSAGE_MAP()

//disable because hopefully the this point is just stored and not used until the object is fully constructed
#pragma warning(disable:4355)  
CPersistOpenDlg::CPersistOpenDlg() : CFileDialog(TRUE),m_wndListViewShell(this)
{
	m_bFileNameRestored = false;
	m_ofn.lpstrTitle = _T("Select a file or folder");
}


CPersistOpenDlg::CPersistOpenDlg(
   BOOL bOpenFileDialog,
   LPCTSTR lpszDefExt,
   LPCTSTR lpszFileName,
   DWORD dwFlags,
   LPCTSTR lpszFilter,
   CWnd* pParentWnd,
   DWORD dwSize) : CFileDialog(bOpenFileDialog,lpszDefExt,lpszFileName,dwFlags,lpszFilter,pParentWnd,dwSize),m_wndListViewShell(this)
{
	m_bFileNameRestored = false;
}


////////////////
// dtor saves current open dialog list view mode in profile settings.
//
CPersistOpenDlg::~CPersistOpenDlg()
{
	AfxGetApp()->WriteProfileInt(PROFILE_SETTINGS, PROFILE_VIEWMODE,
		m_wndListViewShell.m_lastViewMode);
}

//////////////////
// Handle WM_INITDIALOG: post myself a special message to set list view mode.
//
BOOL CPersistOpenDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();
	PostMessage(MYWM_POSTINIT,TRUE);
	return TRUE;
}

//////////////////
// Handle MYWN_POSTINIT: not until now is the list view created, so I can set
// it to the remembered value from profile settings. wp is TRUE for
// first-time init; FALSE otherwise.
//
LRESULT CPersistOpenDlg::OnPostInit(WPARAM wp, LPARAM lp)
{
	TRACE(_T("CPersistOpenDlg::OnPostInit\n"));

	// Subclass folder view shell window. Note real dialog is my parent, not me!

	//	Remove this test.  Not sure what is wrong, but everything
	//	seems to work regardless
	//VERIFY(m_wndListViewShell.SubclassDlgItem(lst2, GetParent()));

	if (wp) { // if first-time init:
		// Set view mode based on value saved in profile settings.
		TRACE(_T("CPersistOpenDlg::OnPostInit: first-time, set view mode from profile\n"));
		UINT nMode = AfxGetApp()->GetProfileInt(PROFILE_SETTINGS, PROFILE_VIEWMODE, 0);
		m_wndListViewShell.SetViewMode(nMode);
	}

	return 0;
}

////////////////////////////////////////////////////////////////
// CListViewShellWnd - Used to subclass the "SHELLDLL_DefView" window in the
// file open dialog. This window contain the list view and processses
// commands to set different view modes.
//

/////////////////
// Set the list view to desired mode.
//
BOOL CListViewShellWnd::SetViewMode(int cmd)
{
	if (m_hWnd && cmd) {

		// SHELLDLL_DefView window found: send it the command.
		SendMessage(WM_COMMAND, cmd);

		// Sent parent dialog a Refresh command (F5) to force repaint.
		// The command ID value was obtained by inspection using Spy++.
		//
		const int CMD_REFRESH = 40966; // from Spy++
		GetParent()->SendMessage(WM_COMMAND, CMD_REFRESH);

		return TRUE;
	}
	return FALSE;
}

//////////////////
// Get current list view mode in the form of WM_COMMAND code that can be
// passed to SetViewMode.
//
int CListViewShellWnd::GetViewMode()
{
	if (!m_hWnd)
		return 0;

	// Get the child list view window.
	HWND hlc = ::FindWindowEx(m_hWnd, NULL, _T("SysListView32"), NULL);
	VERIFY(hlc);
	CListCtrl* plc = 	(CListCtrl*)CWnd::FromHandle(hlc);
	VERIFY(plc);

	#pragma warning(disable:4996)	
	#pragma warning(disable:4244)	
	DWORD dwView = plc->GetView();
	if (dwView==LV_VIEW_ICON) {
		// If list view is in ICON mode, look at icon spacing to determine
		// whether Thumbnail or Icon mode.
		//
		CSize sz = CSize(plc->SendMessage(LVM_GETITEMSPACING));
		if (sz.cx > GetSystemMetrics(SM_CXICONSPACING)) {
			return ODM_VIEW_THUMBS;
		} else {
			return ODM_VIEW_ICONS;
		}
		
	} else if (dwView==LV_VIEW_DETAILS) {
		return ODM_VIEW_DETAIL;

	} else if (dwView==LV_VIEW_LIST) {
		return ODM_VIEW_LIST;

	} else if (dwView==LV_VIEW_TILE) {
		return ODM_VIEW_TILES;

	}
	return 0;
}

/////////////////
// Process window message from ListViewShellWnd. Call default to bypass MFC
// entirely because MFC has all sorts of ASSERT checks that will bomb
// (ListViewShellWnd is not written in MFC). The only message I care about is
// WM_DESTROY, to re-initialize myself.
//
// The original version in MSDN only overrode OnCommand; it's necessary to
// override WindowProc to completely bypass MFC.
//
LRESULT CListViewShellWnd::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg==WM_DESTROY) {
		m_lastViewMode = GetViewMode();			// save current view mode
		m_pDialog->PostMessage(MYWM_POSTINIT); // post re-initialization message
		UnsubclassWindow();							// unsubclass myself
	}
	return Default(); // all message: pass to default WndProc, avoid MFC/CWnd
}


// Unmanaged Code Workaround :

// The problem is that the initial size of edit control for FileName is too small to accommodate the string, 
// resulting in truncation.The initial size of the OpenFileDialog and child edit controls occurs before the 
// dialog is visually displayed on screen.After the edit control gets resized, the string position doesn't get updated. 
// Dev found the following workaround for unmanaged code (C++). The workaround is to set the FileName text after the 
// OpenFileDialog and child edit controls have been resized. One method is to use an event hook for the TextSelectionChanged 
// event and set the FileName text. The FileName text can be the same as the text set initially."

// By using this information from Microsoft we came to this solution for C++ : We subclass CFileSelect, add a flag 
// m_bFileNameRestored which is initialized with false, and override OnFileNameChange as follows :

typedef struct _COMDLG_FILTERSPEC
{
	LPCWSTR pszName;
	LPCWSTR pszSpec;
} 	COMDLG_FILTERSPEC;

enum _FILEOPENDIALOGOPTIONS
{
	FOS_OVERWRITEPROMPT = 0x2,
	FOS_STRICTFILETYPES = 0x4,
	FOS_NOCHANGEDIR = 0x8,
	FOS_PICKFOLDERS = 0x20,
	FOS_FORCEFILESYSTEM = 0x40,
	FOS_ALLNONSTORAGEITEMS = 0x80,
	FOS_NOVALIDATE = 0x100,
	FOS_ALLOWMULTISELECT = 0x200,
	FOS_PATHMUSTEXIST = 0x800,
	FOS_FILEMUSTEXIST = 0x1000,
	FOS_CREATEPROMPT = 0x2000,
	FOS_SHAREAWARE = 0x4000,
	FOS_NOREADONLYRETURN = 0x8000,
	FOS_NOTESTFILECREATE = 0x10000,
	FOS_HIDEMRUPLACES = 0x20000,
	FOS_HIDEPINNEDPLACES = 0x40000,
	FOS_NODEREFERENCELINKS = 0x100000,
	FOS_DONTADDTORECENT = 0x2000000,
	FOS_FORCESHOWHIDDEN = 0x10000000,
	FOS_DEFAULTNOMINIMODE = 0x20000000,
	FOS_FORCEPREVIEWPANEON = 0x40000000
};
typedef DWORD FILEOPENDIALOGOPTIONS;

typedef /* [v1_enum] */
enum FDAP
{
	FDAP_BOTTOM = 0,
	FDAP_TOP = 1
} 	FDAP;

//MIDL_INTERFACE("42f85136-db7e-439c-85f1-e4075d135fc8")
class IFileDialogVista2 : public IModalWindow
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetFileTypes(
		/* [in] */ UINT cFileTypes,
		/* [size_is][in] */ __RPC__in_ecount_full(cFileTypes) const COMDLG_FILTERSPEC *rgFilterSpec) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex(
		/* [in] */ UINT iFileType) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex(
		/* [out] */ __RPC__out UINT *piFileType) = 0;

	virtual HRESULT STDMETHODCALLTYPE Advise(
		/* [in] */ __RPC__in_opt IFileDialogEvents *pfde,
		/* [out] */ __RPC__out DWORD *pdwCookie) = 0;

	virtual HRESULT STDMETHODCALLTYPE Unadvise(
		/* [in] */ DWORD dwCookie) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetOptions(
		/* [in] */ FILEOPENDIALOGOPTIONS fos) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetOptions(
		/* [out] */ __RPC__out FILEOPENDIALOGOPTIONS *pfos) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder(
		/* [in] */ __RPC__in_opt IShellItem *psi) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetFolder(
		/* [in] */ __RPC__in_opt IShellItem *psi) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetFolder(
		/* [out] */ __RPC__deref_out_opt IShellItem **ppsi) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection(
		/* [out] */ __RPC__deref_out_opt IShellItem **ppsi) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetFileName(
		/* [string][in] */ __RPC__in_string LPCWSTR pszName) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetFileName(
		/* [string][out] */ __RPC__deref_out_opt_string LPWSTR *pszName) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetTitle(
		/* [string][in] */ __RPC__in_string LPCWSTR pszTitle) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel(
		/* [string][in] */ __RPC__in_string LPCWSTR pszText) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel(
		/* [string][in] */ __RPC__in_string LPCWSTR pszLabel) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetResult(
		/* [out] */ __RPC__deref_out_opt IShellItem **ppsi) = 0;

	virtual HRESULT STDMETHODCALLTYPE AddPlace(
		/* [in] */ __RPC__in_opt IShellItem *psi,
		/* [in] */ FDAP fdap) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension(
		/* [string][in] */ __RPC__in_string LPCWSTR pszDefaultExtension) = 0;

	virtual HRESULT STDMETHODCALLTYPE Close(
		/* [in] */ HRESULT hr) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetClientGuid(
		/* [in] */ __RPC__in REFGUID guid) = 0;

	virtual HRESULT STDMETHODCALLTYPE ClearClientData(void) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetFilter(
		/* [in] */ __RPC__in_opt IShellItemFilter *pFilter) = 0;

};

BOOL CPersistOpenDlg::Is_Version_Greater_Then_WinXP_SP3()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;
	int op = VER_GREATER;

	// Initialize the OSVERSIONINFOEX structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 1;
	osvi.wServicePackMajor = 3;
	osvi.wServicePackMinor = 0;

	// Initialize the condition mask.

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);

	// Perform the test.

	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION |
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
		dwlConditionMask);
}

void CPersistOpenDlg::OnFileNameChange()
{
	CFileDialog::OnFileNameChange();
	// Do not call Default() if you override
	// no default processing needed

	// Reset FileName only the first time after opening the FileOpenDialog.
	if (!m_bFileNameRestored)
	{
		CString strFileName = GetFileName();
		if (Is_Version_Greater_Then_WinXP_SP3())
		{
			IFileDialogVista2* pIFileDialog = (IFileDialogVista2*)m_pIFileDialog;

			// First set filename to empty, then setting it to the original name will do it.
			pIFileDialog->SetFileName(L"");
			pIFileDialog->SetFileName(CStringW(strFileName));
			m_bFileNameRestored = true;
		}
	}
}