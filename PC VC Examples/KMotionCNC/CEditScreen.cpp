// CEditScreen.cpp : implementation file
//
/*
	Unicode Button
	©2005 Robbert E. Peters

	History:
				Version 1.0 Date: 25/09/2005

	Usage:
		Create an owner-draw button control
		Add a member for it and change CButton 2 CCEditScreen
		Set the Text/Styles

		Link with Usp10.lib

	Copyright:
				You may use this code anyway you like.
*/

#include "stdafx.h"
#include "CEditScreen.h"


CList <LPCEditScreen, LPCEditScreen> CEditScreen::EditScreens;
bool CEditScreen::PersistDirty=false;

// CEditScreen

//IMPLEMENT_DYNAMIC(CEditScreen, CButton)
CEditScreen::CEditScreen()
{
	ToolTipText = "";
	CachedID = Var = -1;
	m_TextColor = m_BackColor = CLR_DEFAULT;
	m_DarkTheme = false;
	SetFont(L"MS Sans Serif", 10, false, false);

	CEditScreen::EditScreens.AddTail(this);
}

void CEditScreen::Reset()
{
	ToolTipText = "";
	CachedID = Var = -1;
	SetFont(L"MS Sans Serif", 10, false, false);
	SetColors(CLR_DEFAULT, CLR_DEFAULT);
	GetPersistText();

	if (m_hWnd != nullptr)
	{
		// Add the WS_TABSTOP style to ensure it can be tabbed to
		ModifyStyle(0, WS_TABSTOP);
	}
}

void CEditScreen::GetPersistText(void)
{
	CScreen *scr = &TheFrame->GCodeDlg.Screen;
	if (!m_hWnd || !scr->CheckIfOKtoChangeText(GetID())) return;
	SetWText(scr->GetPersistText(GetIDName()));
}

CEditScreen::~CEditScreen()
{
	m_font.DeleteObject();
}


CString CEditScreen::GetIDName()
{
	CScreen *scr = &TheFrame->GCodeDlg.Screen;
	if (CachedIDName == "")
	{
		CachedIDName = scr->FindResourceName(GetID());
	}
	return CachedIDName;
}

int CEditScreen::GetID()
{
	if (CachedID == -1 && m_hWnd)
	{
		CachedID = GetDlgCtrlID();
	}
	return CachedID;
}

BEGIN_MESSAGE_MAP(CEditScreen, CEdit)
	//{{AFX_MSG_MAP(CColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// this function is called on a WM_MOUSELEAVE

LRESULT CEditScreen::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	CString t;

	switch (message)
	{
	case WM_IME_NOTIFY:
	case WM_PAINT:
		// detect whenever the text changes to notify KFLOP something changed 
		::GetWindowTextW(m_hWnd,t.GetBufferSetLength(256),255);
		t.ReleaseBuffer();
		if (t != PrevWindowText)
		{
			PrevWindowText = t;
			TheFrame->GCodeDlg.Screen.EditScreenChangesCount++;
			PersistDirty=true;
		}
		break;

	case WM_SETFOCUS:
		TheFrame->GCodeDlg.DisableKeyJog();
		break;
	}
	return CEdit::WindowProc(message, wParam, lParam);
}


void CEditScreen::SetFont(const wchar_t *szFaceName, int height, bool Bold, bool Italic)
{
    // Check if the font is already set with the same parameters
    LOGFONT lf = {0};
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
    if (m_hWnd) CEdit::SetFont(&m_font);
}


// Colors from the Screen Script.  On a dark background the text defaults to light
// and the border uses Windows' dark theme (Windows 10 1809 and later)
void CEditScreen::SetColors(COLORREF Text, COLORREF Back)
{
	bool Dark = Back != CLR_DEFAULT && CScreen::IsDarkColor(Back);
	if (Dark && Text == CLR_DEFAULT) Text = DARK_TEXT_COLOR;

	if (Text != m_TextColor || Back != m_BackColor)
	{
		m_TextColor = Text;
		m_BackColor = Back;
		m_BackBrush.DeleteObject();
		if (Back != CLR_DEFAULT) m_BackBrush.CreateSolidBrush(Back);
		if (m_hWnd) Invalidate();
	}

	if (Dark != m_DarkTheme)
	{
		m_DarkTheme = Dark;
		if (m_hWnd) ::SetWindowTheme(m_hWnd, Dark ? L"DarkMode_CFD" : NULL, NULL);
	}
}

// Called by the Dialog for WM_CTLCOLOREDIT and WM_CTLCOLORSTATIC (read only), returns
// the background brush, or NULL for the standard background
HBRUSH CEditScreen::CtlColor(CDC *pDC)
{
	if (m_TextColor != CLR_DEFAULT) pDC->SetTextColor(m_TextColor);
	if (m_BackColor == CLR_DEFAULT) return NULL;
	pDC->SetBkColor(m_BackColor);
	return m_BackBrush;
}



int CEditScreen::SavePersists(void)
{
	CScreen *scr = &TheFrame->GCodeDlg.Screen;
	CString file = TheFrame->MainPathRoot + EDIT_CONTROL_PERSIST_FILE;

	// Open the file with the specified encoding
	FILE *fStream;
	
	_tfopen_s(&fStream, file, _T("w,ccs=UTF-8"));
		
	if (!fStream)  // failed
	{
		::MessageBox(NULL, TheFrame->KMotionDLL->Translate("Unable to open Screen Script file:\r\r") + file, L"KMotionCNC", MB_ICONSTOP | MB_OK);
		return 1;
	}

	if (fStream)
	{
		POSITION p = scr->Defines.GetHeadPosition();
		for (int i = 0; i < scr->Defines.GetSize(); i++)
		{
			int j;
			CString s = scr->Defines.GetNext(p);
			for (j = 0; j < s.GetLength(); j++)
			{
				if (s[j] == ' ' || s[j] == '\t') break; // find next space
			}

			int ID;
			swscanf((const wchar_t *)s + j + 1, L"%d", &ID);

			if (scr->CheckIfOKtoChangeText(ID))
			{
				CEditScreen *E;
				E = scr->FindEditScreen(ID);
				if (E)  // EditScreen Control?
				{
					CString w, IDName = s.Mid(0, j);
					CString t = E->GetWText();
					swprintf(w.GetBuffer(2048), 2048, L"%ls:%ls\n", IDName.GetBuffer(), t.GetBuffer());
					w.ReleaseBuffer();
					fputws(w, fStream);
				}

				CComboBoxScreen *C;
				C = scr->FindComboBoxScreen(ID);
				if (C)  // ComboBoxScreen Control?
				{
					CString w, IDName = s.Mid(0, j);
					swprintf(w.GetBuffer(2048), 2048, L"%ls:%ls\n", IDName.GetBuffer(), C->PrevWindowText.GetBuffer());
					w.ReleaseBuffer();
					fputws(w, fStream);
				}
			}
		}
		fclose(fStream);
	}
	else
	{
		return 1;
	}
	return 0;
}


// Return control text as a wide string
CString CEditScreen::GetWText()
{
	CString w;
	::CallWindowProcW(*GetSuperWndProcAddr(), m_hWnd, WM_GETTEXT, 2000, (LPARAM)(LPWSTR)w.GetBufferSetLength(200));
	w.ReleaseBuffer();
	return w;
}

// Set control text as a wide string
void CEditScreen::SetWText(CString w)
{
	::CallWindowProcW(*GetSuperWndProcAddr(), m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCWSTR)w);
}
