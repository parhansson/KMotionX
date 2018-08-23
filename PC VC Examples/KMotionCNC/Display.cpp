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

CList <LPCDisplay, LPCDisplay> CDisplay::Displays;


/////////////////////////////////////////////////////////////////////////////
// CDisplay

CDisplay::CDisplay()
{
	Reset();
	CDisplay::Displays.AddTail(this);
}

void CDisplay::Reset()
{
	m_OutText = "";
	m_BackColor = m_TextColor = 0;
	m_fheight = 36;
	LOGFONT lfont;
	memset(&lfont, 0, sizeof(LOGFONT));
	m_FaceName = "Courier New";

	lfont.lfHeight = 36;
	lfont.lfWeight = 700;
	m_Bold = true;
	m_Italic = lfont.lfItalic = false;
	lfont.lfOutPrecision = OUT_TT_PRECIS;
	lfont.lfQuality = ANTIALIASED_QUALITY;
	strcpy(lfont.lfFaceName, m_FaceName);

	//remove old font
	m_font.DeleteObject();
	if (m_hWnd) m_font.CreateFontIndirect(&lfont);
}

CDisplay::~CDisplay()
{
}

void CDisplay::SetFont(const char *szFaceName, int height, bool Bold, bool Italic)
{
	//remove old font
	m_font.DeleteObject();

	m_FaceName = szFaceName;
	m_fheight = height;
	m_Bold = Bold;
	m_Italic = Italic;

	//create new font
	LOGFONT lfont;
	memset(&lfont, 0, sizeof(LOGFONT));
	lfont.lfHeight = height;
	lfont.lfWeight = Bold ? 700 : 400;
	lfont.lfItalic = Italic;
	lfont.lfOutPrecision = OUT_TT_PRECIS;
	lfont.lfQuality = ANTIALIASED_QUALITY;
	strcpy(lfont.lfFaceName, szFaceName);
	m_font.CreateFontIndirect(&lfont);
}

void CDisplay::SetBackColor(int Color)
{
	m_BackColor = Color;
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
	CBrush BackGrndBrush(m_BackColor), *SaveBrush;
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
	CFont *SaveFont;
	COLORREF OldTextColor;


	SaveBackMode = m_MemDC->SetBkMode(TRANSPARENT);


//	fontDisplay.CreateFont(36, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0,
//		FIXED_PITCH, "Courier New");

	CDC *screen = GetDC();
	int dpiX = screen->GetDeviceCaps(LOGPIXELSX);
	ReleaseDC(screen);
	int dpi_standard = 96;
	double m_scale = dpiX / (double)dpi_standard;


//	fontDisplay.CreateFont((int)(m_rect.bottom+3* m_scale+0.5), 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0,
//		FIXED_PITCH, "Courier New");


	
	SaveFont = m_MemDC->SelectObject(&m_font);
	OldTextColor = m_MemDC->SetTextColor(m_TextColor);
	SaveTextAlign = m_MemDC->SetTextAlign(TA_CENTER|TA_BASELINE);

//	m_MemDC->TextOut(m_rect.right/2+1, (int)(m_rect.bottom-(7 * m_scale + 0.5)), m_OutText);
	m_MemDC->TextOut(m_rect.right/2+1, (int)(m_rect.bottom * 0.83), m_OutText);
	
	//Reset grapic variables
	m_MemDC->SetTextAlign(SaveTextAlign);
	m_MemDC->SetTextColor(OldTextColor);
	m_MemDC->SelectObject(SaveFont);
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
