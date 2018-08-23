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
	CString s,r,IDName;
	FILE *f = fopen(TheFrame->MainPathRoot + EDIT_CONTROL_PERSIST_FILE, "rt");
	CScreen *scr = &TheFrame->GCodeDlg.Screen;

	if (!m_hWnd || !scr->CheckIfOKtoChangeText(GetID())) return;

	if (f)
	{
		while (!feof(f))
		{
			fgets(s.GetBufferSetLength(256), 255, f);
			s.ReleaseBuffer();

			int k = s.Find(':');
			if (k == -1) k = s.GetLength();
			IDName = s.Mid(0, k);

			if (IDName == GetIDName())
			{
				s.Delete(0, k);
				if (s[0] == ':') s.Delete(0, 1);
				s.Remove('\r');
				s.Remove('\n');
				SetWindowText(s);
				break;
			}
		}

		fclose(f);	
	}
}



int CEditScreen::SavePersists(void)
{
	CScreen *scr = &TheFrame->GCodeDlg.Screen;
	FILE *f = fopen(TheFrame->MainPathRoot + EDIT_CONTROL_PERSIST_FILE, "wt");

	if (f)
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
					CString IDName = s.Mid(0, j);
					fprintf(f, "%s:%ws\n", IDName.GetBuffer(), E->PrevWindowText.GetBuffer());
				}
			}
		}
		fclose(f);	
	}
	else
	{
		return 1;
	}
	return 0;
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
		// detect whenever the text changes to notify KFLOP somthing changed 
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


