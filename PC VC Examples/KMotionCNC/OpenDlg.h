////////////////////////////////////////////////////////////////
// MSDN Magazine -- February 2005
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio .NET 2003 on Windows XP. Tab size=3.
//
// NOTE: This code was tested with WinXP, all bets are off for other versions
// of Windows. In theory, it should work.

// These are the WM_COMMAND IDs to send the shell window to set the
// different folder views. These were obtained using Spy++
//
#pragma once
#include "commctrl.h"

enum {
	ODM_UNDEFINED = 0,
	ODM_VIEW_ICONS = 0x7029,
	ODM_VIEW_LIST  = 0x702b,
	ODM_VIEW_DETAIL= 0x702c,
	ODM_VIEW_THUMBS= 0x702d,
	ODM_VIEW_TILES = 0x702e,
};

//////////////////
// This class is used to subclass the "SHELLDLL_DefView" window in the file
// open dialog.
//
class CListViewShellWnd : public CWnd
{
public:
	CWnd* m_pDialog;
	DWORD m_lastViewMode;

	CListViewShellWnd(CWnd* pDlg) : m_lastViewMode(0), m_pDialog(pDlg)
	{
	}

	~CListViewShellWnd()
	{
	}

	int  GetViewMode();
	BOOL SetViewMode(int cmd);

protected:
	virtual LRESULT WindowProc(UINT msg, WPARAM wParam, LPARAM lParam);
};

/////////////////
// Customized file open dialog remembers state of folder view.
//
class CPersistOpenDlg : public CFileDialog {
public:
	CPersistOpenDlg();

	CPersistOpenDlg(
		BOOL bOpenFileDialog,
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0);



	~CPersistOpenDlg();

protected:
	CListViewShellWnd m_wndListViewShell;
	BOOL m_bFirstTime;

	virtual BOOL OnInitDialog();			 // handle init dialog
	afx_msg LRESULT OnPostInit(WPARAM wp, LPARAM lp); // the REAL initialization

	DECLARE_DYNAMIC(CPersistOpenDlg)
	DECLARE_MESSAGE_MAP()
};
