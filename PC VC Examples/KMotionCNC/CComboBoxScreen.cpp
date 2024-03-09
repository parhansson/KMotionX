// CComboBoxScreen.cpp : implementation file
//
/*
	Unicode Button
	©2005 Robbert E. Peters

	History:
				Version 1.0 Date: 25/09/2005

	Usage:
		Create an owner-draw button control
		Add a member for it and change CButton 2 CCComboBoxScreen
		Set the Text/Styles

		Link with Usp10.lib

	Copyright:
				You may use this code anyway you like.
*/

#include "stdafx.h"
#include "CComboBoxScreen.h"

CList <LPCComboBoxScreen, LPCComboBoxScreen> CComboBoxScreen::ComboBoxScreens;


// CComboBoxScreen

//IMPLEMENT_DYNAMIC(CComboBoxScreen, CButton)
CComboBoxScreen::CComboBoxScreen()
{
	ToolTipText = "";
	CachedID = Var = -1;
	SetFont("MS Sans Serif", 10, false, false);
	
	CComboBoxScreen::ComboBoxScreens.AddTail(this);
}

void CComboBoxScreen::Reset()
{
	ToolTipText = "";
	CachedID = Var = -1;
	SetFont("MS Sans Serif", 10, false, false);
	GetPersistText();
}

void CComboBoxScreen::GetPersistText(void)
{
	CScreen *scr = &TheFrame->GCodeDlg.Screen;
	if (!m_hWnd || !scr->CheckIfOKtoChangeText(GetID())) return;
	SetWText(scr->GetPersistText(GetIDName()));
}


CComboBoxScreen::~CComboBoxScreen()
{
	m_font.DeleteObject();
}

CString CComboBoxScreen::GetIDName()
{
	CScreen *scr = &TheFrame->GCodeDlg.Screen;
	if (CachedIDName == "")
	{
		CachedIDName = scr->FindResourceName(GetID());
	}
	return CachedIDName;
}

int CComboBoxScreen::GetID()
{
	if (CachedID == -1 && m_hWnd)
	{
		CachedID = GetDlgCtrlID();
	}
	return CachedID;
}

BEGIN_MESSAGE_MAP(CComboBoxScreen, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// this function is called on a WM_MOUSELEAVE

LRESULT CComboBoxScreen::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	CStringW t;

	switch (message)
	{
	case WM_CTLCOLOREDIT:
	case WM_IME_NOTIFY:
	case WM_PAINT:
		// detect whenever the text changes to notify KFLOP something changed 
		// must use this direct call to avoid the conversion from Unicode to ANSI
		::CallWindowProcW(*GetSuperWndProcAddr(), m_hWnd, WM_GETTEXT, 255, (LPARAM)(LPWSTR)t.GetBufferSetLength(256));
		t.ReleaseBuffer();
		if (t != PrevWindowText)
		{
			PrevWindowText = t;
			TheFrame->GCodeDlg.Screen.EditScreenChangesCount++;
			CEditScreen::PersistDirty=true;
		}
		break;
	}
	return CComboBox::WindowProc(message, wParam, lParam);
}

void CComboBoxScreen::FixMyComboboxExTip(CStringW Text)
{
	AFX_MODULE_THREAD_STATE* pThreadState = AfxGetModuleThreadState();
	CToolTipCtrl* pToolTip = pThreadState->m_pToolTip;
	if (pToolTip == NULL || pToolTip->m_hWnd == NULL) return;

	CString sTipText=Text;

	CWnd *ChildControl = GetWindow(GW_CHILD);
	CEdit *EditControl = GetEditCtrl();

	CRect rc; GetWindowRect(&rc);
	ScreenToClient(&rc);

	TOOLINFOW rTI; memset(&rTI, 0, sizeof(TOOLINFOW));
	rTI.cbSize = sizeof(TOOLINFOW);

	rTI.hwnd = m_hWnd;
	rTI.uFlags |= TTF_IDISHWND;

	// set TTF_NOTBUTTON and TTF_CENTERTIP if it isn't a button
	rTI.uFlags |= TTF_NOTBUTTON | TTF_CENTERTIP;

	rTI.lpszText = Text.GetBuffer(); // finish setup

	if (EditControl)
	{
		rTI.uId = (WPARAM)EditControl->m_hWnd;
		pToolTip->SendMessage(TTM_ADDTOOLW, 0, (LPARAM)&rTI);
	}

	rTI.uId = (WPARAM)ChildControl->m_hWnd;
	pToolTip->SendMessage(TTM_ADDTOOLW, 0, (LPARAM)&rTI);
	pToolTip->SendMessage(TTM_ACTIVATE, TRUE);
	Text.ReleaseBuffer();
}

void CComboBoxScreen::SetFont(const char *szFaceName, int height, bool Bold, bool Italic)
{
	m_font.DeleteObject();
	m_font.CreateFont(height, 0, 0, 0, Bold ? FW_BOLD : FW_NORMAL, Italic, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, szFaceName);
	if (m_hWnd)	CComboBox::SetFont(&m_font);
}

void CComboBoxScreen::InsertItemW(CStringW p)
{
	COMBOBOXEXITEMW cbei;
	cbei.mask = CBEIF_TEXT;

	cbei.iItem = -1;
	cbei.pszText = p.GetBuffer();
	::SendMessageW(m_hWnd, CBEM_INSERTITEMW, 0, LPARAM(&cbei));
}


void CComboBoxScreen::SetTextAndDropDown(CStringW s)
{
	// Delete every item from the combo box.
	ResetAll();

	// create list.  Strings are separated by '/' 
	// if first one is specified set as text

	int i = 0;
	bool Done;

	CStringW a = Part(i++, s, Done);

	if (a != "")
	{
		CStringW w = GetWText();
		if (w == "")
			SetWText(a);
	}

	while (!Done)
	{
		InsertItemW(Part(i++, s, Done));
	}
}


void CComboBoxScreen::ResetAll()
{
	// Delete every item from the combo box.
	for (int i = GetCount() - 1; i >= 0; i--)
		DeleteString(i);
}


// Return control text as a wide string
CStringW CComboBoxScreen::GetWText()
{
	CStringW w;
	::CallWindowProcW(*GetSuperWndProcAddr(), m_hWnd, WM_GETTEXT, 2000, (LPARAM)(LPWSTR)w.GetBufferSetLength(2001));
	w.ReleaseBuffer();
	return w;
}

// Set control text as a wide string
void CComboBoxScreen::SetWText(CStringW w)
{
	::CallWindowProcW(*GetSuperWndProcAddr(), m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCWSTR)w);
}


// extract a string into parts separated by semicolons
CStringW CComboBoxScreen::Part(int n, CStringW p, bool &Done)
{
	CStringW s = "";
	for (int k = 0; k <= n; k++)
	{
		int i = p.Find(';');

		Done = i < 0;

		if (Done) i = p.GetLength();

		s = p.Mid(0, i);

		if (k<n && p.GetLength() > 0) p = p.Mid(i + 1, p.GetLength() - i - 1);
	}
	return s;
}



