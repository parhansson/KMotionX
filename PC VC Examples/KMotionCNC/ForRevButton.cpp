// ForRevButton.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"
#include "ForRevButton.h"
#include "..\GCodeInterpreter\GCodeInterpreter.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CForRevButton

CForRevButton::CForRevButton()
{
	m_Moving=false;
}

CForRevButton::~CForRevButton()
{
}


BEGIN_MESSAGE_MAP(CForRevButton, CImageButton)
	//{{AFX_MSG_MAP(CForRevButton)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CForRevButton message handlers


void CForRevButton::OnMouseMove(UINT nFlags, CPoint point) 
{
}

void CForRevButton::Init(CKMotionCNCDlg *pDlg, int dir, double mag)
{
	Dlg = pDlg;
	m_dir=dir;
	m_mag=mag;
}



void CForRevButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		if (lpDrawItemStruct->itemState & ODS_SELECTED || DrawPushed)
				HandleButtonDown();
		else
				HandleButtonUp();
	}
	CImageButton::DrawItem(lpDrawItemStruct);
}

void CForRevButton::HandleButtonDown(void)
{
	CString s;
	CCoordMotion *CM= Dlg->Interpreter->CoordMotion;

	// FRO in direction

	if (!Dlg->m_Simulate)
	{
		s.Format("SetFROTemp %.4f",m_dir * m_mag);
		if (TheFrame->KMotionDLL->WriteLine(s)) return;
		m_Moving=true;
	}
}

void CForRevButton::HandleButtonUp(void)
{
	CString s;

	if (m_Moving)
	{
		s.Format("SetFROTemp %.4f",0.0);
		if (TheFrame->KMotionDLL->WriteLine(s)) return;
		m_Moving=false;
	}
}

