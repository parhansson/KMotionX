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
	SetFont("MS Sans Serif", 10, false, false);

	CEditScreen::EditScreens.AddTail(this);
}

void CEditScreen::Reset()
{
	ToolTipText = "";
	CachedID = Var = -1;
	SetFont("MS Sans Serif", 10, false, false);
	GetPersistText();
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

BEGIN_MESSAGE_MAP(CEditScreen, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// this function is called on a WM_MOUSELEAVE

LRESULT CEditScreen::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	CStringW t;

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
	}
	return CEdit::WindowProc(message, wParam, lParam);
}


void CEditScreen::SetFont(const char *szFaceName, int height, bool Bold, bool Italic)
{
	m_font.DeleteObject();
	m_font.CreateFont(height, 0, 0, 0, Bold ? FW_BOLD : FW_NORMAL, Italic, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, szFaceName);
	if (m_hWnd) CEdit::SetFont(&m_font);
}



int CEditScreen::SavePersists(void)
{
	CScreen *scr = &TheFrame->GCodeDlg.Screen;
	CString file = TheFrame->MainPathRoot + EDIT_CONTROL_PERSIST_FILE;

	// Open the file with the specified encoding
	FILE *fStream;
	
	errno_t e = _tfopen_s(&fStream, file, _T("w,ccs=UNICODE"));
		
	if (e != 0)  // failed..CString sRead;
	{
		::MessageBoxW(NULL, TheFrame->KMotionDLL->Translate("Unable to open Screen Script file:\r\r") + (CStringW)file, L"KMotionCNC", MB_ICONSTOP | MB_OK);
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
			sscanf((const char *)s + j + 1, "%d", &ID);

			if (scr->CheckIfOKtoChangeText(ID))
			{
				CEditScreen *E;
				E = scr->FindEditScreen(ID);
				if (E)  // EditScreen Control?
				{
					CStringW w, IDName = s.Mid(0, j);
					CStringW t;
					::GetWindowTextW(E->m_hWnd, t.GetBufferSetLength(256), 255);
					t.ReleaseBuffer();
					swprintf(w.GetBuffer(2048), 2048, L"%s:%s\n", IDName.GetBuffer(), t.GetBuffer());
					w.ReleaseBuffer();
					fputws(w, fStream);
				}

				CComboBoxScreen *C;
				C = scr->FindComboBoxScreen(ID);
				if (C)  // ComboBoxScreen Control?
				{
					CStringW w, IDName = s.Mid(0, j);
					swprintf(w.GetBuffer(2048), 2048, L"%s:%s\n", IDName.GetBuffer(), C->PrevWindowText.GetBuffer());
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
CStringW CEditScreen::GetWText()
{
	CStringW w;
	::CallWindowProcW(*GetSuperWndProcAddr(), m_hWnd, WM_GETTEXT, 2000, (LPARAM)(LPWSTR)w.GetBufferSetLength(200));
	w.ReleaseBuffer();
	return w;
}

// Set control text as a wide string
void CEditScreen::SetWText(CStringW w)
{
	::CallWindowProcW(*GetSuperWndProcAddr(), m_hWnd, WM_SETTEXT, 0, (LPARAM)(LPCWSTR)w);
}
