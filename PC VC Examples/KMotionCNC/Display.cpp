// Display.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#include "stdafx.h"
#include "Display.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplay

CDisplay::CDisplay()
{
	m_OutText = "";
	m_TextColor=0;
}

CDisplay::~CDisplay()
{
}


BEGIN_MESSAGE_MAP(CDisplay, CStatic)
	//{{AFX_MSG_MAP(CDisplay)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplay message handlers

void CDisplay::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC MemDC;
	BOOL result;
	CBitmap Bitmap;


	m_MemDC = &MemDC;
	GetClientRect(&m_rect);
	

	result = m_MemDC->CreateCompatibleDC(&dc);
	Bitmap.CreateCompatibleBitmap(&dc, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top);
	m_MemDC->SelectObject((HBITMAP)Bitmap);

	DrawBackGrnd();
	DrawText();

	dc.BitBlt(1, 1, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, m_MemDC, 1, 1, SRCCOPY);

	m_MemDC->DeleteDC();
	Bitmap.DeleteObject();

	// Do not call CStatic::OnPaint() for painting messages
}

void CDisplay::DrawBackGrnd()
{
	CBrush BackGrndBrush(RGB(0,0,0)), *SaveBrush;
	CPen BackGrndPen(PS_SOLID, 1, RGB(0,0,0)), *SavePen;


	SaveBrush = m_MemDC->SelectObject(&BackGrndBrush);
	SavePen = m_MemDC->SelectObject(&BackGrndPen);

	m_MemDC->Rectangle(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);	

	// Reset graphic variables
	(m_MemDC->SelectObject(SavePen))->DeleteObject();
	(m_MemDC->SelectObject(SaveBrush))->DeleteObject();
}

void CDisplay::DrawText()
{

	int SaveBackMode;
	UINT SaveTextAlign;
	CString val;
	CFont fontDisplay, *SaveFont;
	COLORREF OldTextColor;


	SaveBackMode = m_MemDC->SetBkMode(TRANSPARENT);
   	fontDisplay.CreateFont(36, 0, 0, 0,       FW_BOLD, 0, 0, 0, 0, 0, 0, 0,
                          FIXED_PITCH, "Courier New");
	

	
	SaveFont = m_MemDC->SelectObject(&fontDisplay);
	OldTextColor = m_MemDC->SetTextColor(m_TextColor);
	SaveTextAlign = m_MemDC->SetTextAlign(TA_CENTER|TA_BASELINE);

	m_MemDC->TextOut(m_rect.right/2+1, m_rect.bottom-7, m_OutText);
	
	//Reset grapic variables
	m_MemDC->SetTextAlign(SaveTextAlign);
	m_MemDC->SetTextColor(OldTextColor);
	(m_MemDC->SelectObject(SaveFont))->DeleteObject();
	m_MemDC->SetBkMode(SaveBackMode);
}

void CDisplay::RefreshPlot()
{
	InvalidateRect(NULL, false);
}

void CDisplay::SetText(CString OutText)
{
	m_OutText = OutText;
	RefreshPlot();
}


void CDisplay::SetTextColor(int color)
{
	m_TextColor=color;
}
