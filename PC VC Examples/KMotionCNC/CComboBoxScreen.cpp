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
	Reset();
	CComboBoxScreen::ComboBoxScreens.AddTail(this);
}



CComboBoxScreen::~CComboBoxScreen()
{
	m_font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CComboBoxScreen, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CComboBoxScreen::SetFont(const char *szFaceName, int height, bool Bold, bool Italic)
{
	m_font.DeleteObject();
	m_font.CreateFont(height, 0, 0, 0, Bold ? FW_BOLD : FW_NORMAL, Italic, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, szFaceName);
	if (m_hWnd)	CComboBox::SetFont(&m_font);
}

void CComboBoxScreen::Reset()
{
	ToolTipText = "";
	Var = -1;
	ToolTipText = "";
	Var = -1;
	SetFont("MS Sans Serif", 10, false, false);
}
