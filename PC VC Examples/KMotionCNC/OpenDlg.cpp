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
