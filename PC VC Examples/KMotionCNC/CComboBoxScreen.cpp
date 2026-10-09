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

#pragma comment(lib, "comctl32.lib")  // SetWindowSubclass

CList <LPCComboBoxScreen, LPCComboBoxScreen> CComboBoxScreen::ComboBoxScreens;


// CComboBoxScreen

//IMPLEMENT_DYNAMIC(CComboBoxScreen, CButton)
CComboBoxScreen::CComboBoxScreen()
{
	ToolTipText = "";
	CachedID = Var = -1;
	m_CustomColors = m_DarkTheme = false;
	m_TextColor = m_BackColor = m_SelTextColor = m_SelBackColor = CLR_DEFAULT;
	SetFont(L"MS Sans Serif", 10, false, false);

	CComboBoxScreen::ComboBoxScreens.AddTail(this);
}

void CComboBoxScreen::Reset()
{
	ToolTipText = "";
	CachedID = Var = -1;
	SetFont(L"MS Sans Serif", 10, false, false);
	SetColors(CLR_DEFAULT, CLR_DEFAULT, CLR_DEFAULT, CLR_DEFAULT);
	GetPersistText();

	if (m_hWnd != nullptr)
	{
		// Add the WS_TABSTOP style to ensure it can be tabbed to
		ModifyStyle(0, WS_TABSTOP);
	}
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

BEGIN_MESSAGE_MAP(CComboBoxScreen, CComboBox)
END_MESSAGE_MAP()



// this function is called on a WM_MOUSELEAVE

LRESULT CComboBoxScreen::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	CString t;

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
		if (message == WM_CTLCOLOREDIT && m_CustomColors)
			return CtlColor((HDC)wParam);
		break;

	case WM_CTLCOLORLISTBOX:  // the dropdown list
	case WM_CTLCOLORSTATIC:   // disabled
		if (m_CustomColors)
			return CtlColor((HDC)wParam);
		break;

	case WM_DRAWITEM:  // the ComboBoxEx always draws its items with the standard colors
		if (m_CustomColors)
		{
			DrawColoredItem((LPDRAWITEMSTRUCT)lParam);
			return TRUE;
		}
		break;

	case WM_SETFOCUS:
		TheFrame->GCodeDlg.DisableKeyJog();
		break;


	case WM_PARENTNOTIFY:
		if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN)
		{
			TheFrame->GCodeDlg.DisableKeyJog();
		}
		break;
	}
	return CComboBox::WindowProc(message, wParam, lParam);
}

void CComboBoxScreen::FixMyComboboxExTip(CString Text)
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

void CComboBoxScreen::SetFont(const wchar_t *szFaceName, int height, bool Bold, bool Italic)
{
	// Check if the font is already set with the same parameters
	LOGFONT lf = { 0 };
	if (m_font.GetSafeHandle() && m_font.GetLogFont(&lf)) {
		// Compare font attributes
		if (
			lf.lfHeight == height &&
			lf.lfWeight == (Bold ? FW_BOLD : FW_NORMAL) &&
			lf.lfItalic == (BYTE)Italic &&
			wcscmp(lf.lfFaceName, szFaceName) == 0
			)
		{
			// Font is already set, no need to change
			return;
		}
	}
	m_font.DeleteObject();
	m_font.CreateFont(height, 0, 0, 0, Bold ? FW_BOLD : FW_NORMAL, Italic, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, szFaceName);
	if (m_hWnd)	CComboBox::SetFont(&m_font);
}


// Colors from the Screen Script.  Highlight colors are for the selected item.  On a dark background
// the text and selection default to dark colors and the arrow button, border and scroll bar use
// Windows' dark theme (Windows 10 1809 and later)
void CComboBoxScreen::SetColors(COLORREF Text, COLORREF Back, COLORREF SelText, COLORREF SelBack)
{
	bool Custom = Text != CLR_DEFAULT || Back != CLR_DEFAULT || SelText != CLR_DEFAULT || SelBack != CLR_DEFAULT;
	bool Dark = Back != CLR_DEFAULT && CScreen::IsDarkColor(Back);

	if (Back == CLR_DEFAULT) Back = GetSysColor(COLOR_WINDOW);
	if (Text == CLR_DEFAULT) Text = Dark ? DARK_TEXT_COLOR : GetSysColor(COLOR_WINDOWTEXT);
	if (SelBack == CLR_DEFAULT) SelBack = Dark ? DARK_SEL_COLOR : GetSysColor(COLOR_HIGHLIGHT);
	if (SelText == CLR_DEFAULT) SelText = Dark ? Text : GetSysColor(COLOR_HIGHLIGHTTEXT);

	CComboBox *Combo = m_hWnd ? GetComboBoxCtrl() : NULL;

	// with an edit control the combo box paints around the edit with the standard color
	if (Combo && GetEditCtrl())
		::SetWindowSubclass(Combo->m_hWnd, InnerComboProc, 0, (DWORD_PTR)this);

	if (Custom != m_CustomColors || Text != m_TextColor || Back != m_BackColor || SelText != m_SelTextColor || SelBack != m_SelBackColor)
	{
		m_CustomColors = Custom;
		m_TextColor = Text;
		m_BackColor = Back;
		m_SelTextColor = SelText;
		m_SelBackColor = SelBack;
		m_BackBrush.DeleteObject();
		m_BackBrush.CreateSolidBrush(Back);
		if (m_hWnd) RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
	}

	if (Dark != m_DarkTheme)
	{
		m_DarkTheme = Dark;
		if (Combo)
		{
			::SetWindowTheme(Combo->m_hWnd, Dark ? L"DarkMode_CFD" : NULL, NULL);

			COMBOBOXINFO cbi = { sizeof(cbi) };
			if (::GetComboBoxInfo(Combo->m_hWnd, &cbi) && cbi.hwndList)
				::SetWindowTheme(cbi.hwndList, Dark ? L"DarkMode_Explorer" : NULL, NULL);
		}
	}
}

LRESULT CComboBoxScreen::CtlColor(HDC hDC)
{
	::SetTextColor(hDC, m_TextColor);
	::SetBkColor(hDC, m_BackColor);
	return (LRESULT)m_BackBrush.GetSafeHandle();
}

// After painting, the combo box inside a ComboBoxEx fills the space around the edit control
// with the system window color, so repaint it with the background color
LRESULT CALLBACK CComboBoxScreen::InnerComboProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	LRESULT result = ::DefSubclassProc(hWnd, message, wParam, lParam);
	CComboBoxScreen *C = (CComboBoxScreen *)dwRefData;

	if (message == WM_PAINT && C->m_CustomColors)
	{
		HWND Edit = (HWND)::SendMessage(C->m_hWnd, CBEM_GETEDITCONTROL, 0, 0);
		COMBOBOXINFO cbi = { sizeof(cbi) };
		if (Edit && ::GetComboBoxInfo(hWnd, &cbi))
		{
			RECT e;
			::GetWindowRect(Edit, &e);
			::MapWindowPoints(NULL, hWnd, (POINT *)&e, 2);
			HDC hDC = ::GetDC(hWnd);
			::ExcludeClipRect(hDC, e.left, e.top, e.right, e.bottom);
			::FillRect(hDC, &cbi.rcItem, (HBRUSH)C->m_BackBrush.GetSafeHandle());
			::ReleaseDC(hWnd, hDC);
		}
	}
	else if (message == WM_NCDESTROY)
	{
		::RemoveWindowSubclass(hWnd, InnerComboProc, uIdSubclass);
	}
	return result;
}

// Draw a dropdown list item or the selection field with the Screen Script colors,
// laid out like the ComboBoxEx draws them
void CComboBoxScreen::DrawColoredItem(LPDRAWITEMSTRUCT d)
{
	bool Field = (d->itemState & ODS_COMBOBOXEDIT) != 0;
	RECT r = d->rcItem;
	if (!Field) r.left++;  // list item highlights start a pixel in

	if (d->itemAction == ODA_FOCUS)  // just toggle the focus rectangle
	{
		if (!(d->itemState & ODS_NOFOCUSRECT)) ::DrawFocusRect(d->hDC, &r);
		return;
	}

	wchar_t Text[1024] = L"";
	int Item = d->itemID != (UINT)-1 ? (int)d->itemID : GetCurSel();
	if (Item >= 0)
	{
		COMBOBOXEXITEMW cbei = {};
		cbei.mask = CBEIF_TEXT;
		cbei.iItem = Item;
		cbei.pszText = Text;
		cbei.cchTextMax = sizeof(Text) / sizeof(Text[0]);
		::SendMessageW(m_hWnd, CBEM_GETITEMW, 0, (LPARAM)&cbei);
	}

	bool Selected = (d->itemState & ODS_SELECTED) != 0;
	COLORREF TextColor = Selected ? m_SelTextColor : m_TextColor;
	if (d->itemState & ODS_DISABLED) TextColor = CScreen::BlendColor(m_TextColor, m_BackColor, 50);

	int OldBkMode = ::GetBkMode(d->hDC);
	COLORREF OldBkColor = ::SetBkColor(d->hDC, Selected ? m_SelBackColor : m_BackColor);
	COLORREF OldTextColor = ::SetTextColor(d->hDC, TextColor);

	if (Field && !Selected)
		::SetBkMode(d->hDC, TRANSPARENT);  // the field shows the combo box's themed face
	else
		::ExtTextOutW(d->hDC, 0, 0, ETO_OPAQUE, &r, NULL, 0, NULL);  // fill the background

	RECT t = r;
	if (!Field) t.left += 2;
	::DrawTextW(d->hDC, Text, -1, &t, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

	if ((d->itemState & ODS_FOCUS) && !(d->itemState & ODS_NOFOCUSRECT))
		::DrawFocusRect(d->hDC, &r);

	::SetBkMode(d->hDC, OldBkMode);
	::SetBkColor(d->hDC, OldBkColor);
	::SetTextColor(d->hDC, OldTextColor);
}

void CComboBoxScreen::InsertItem(CString p)
{
	COMBOBOXEXITEMW cbei;
	cbei.mask = CBEIF_TEXT;

	cbei.iItem = -1;
	cbei.pszText = p.GetBuffer();
	::SendMessageW(m_hWnd, CBEM_INSERTITEMW, 0, LPARAM(&cbei));
}


void CComboBoxScreen::SetTextAndDropDown(CString s)
{
	// Delete every item from the combo box.
	ResetAll();

	// create list.  Strings are separated by '/' 
	// if first one is specified set as text

	int i = 0;
	bool Done;

	CString a = Part(i++, s, Done);

	if (a != "")
	{
		CString w = GetWText();
		if (w == "")
			SetWText(a);
	}

	while (!Done)
	{
		InsertItem(Part(i++, s, Done));
	}
}


void CComboBoxScreen::ResetAll()
{
	// Delete every item from the combo box.
	for (int i = GetCount() - 1; i >= 0; i--)
		DeleteString(i);
}


// Return control text as a wide string
CString CComboBoxScreen::GetWText()
{
	CString w;
	::CallWindowProcW(*GetSuperWndProcAddr(), m_hWnd, WM_GETTEXT, 2000, (LPARAM)(LPWSTR)w.GetBufferSetLength(2001));
	w.ReleaseBuffer();
	return w;
}

// Set control text as a wide string
void CComboBoxScreen::SetWText(CString w)
{
	::CallWindowProcW(*GetSuperWndProcAddr(), m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCWSTR)w);
}


// extract a string into parts separated by semicolons
CString CComboBoxScreen::Part(int n, CString p, bool &Done)
{
	CString s = "";
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



