// UniButton.cpp : implementation file
//
/*
	Unicode Button
	©2005 Robbert E. Peters

	History:
				Version 1.0 Date: 25/09/2005

	Usage:
		Create an owner-draw button control
		Add a member for it and change CButton 2 CUniButton
		Set the Text/Styles

		Link with Usp10.lib

	Copyright:
				You may use this code anyway you like.
*/

#include "stdafx.h"
#include "UniButton.h"

#include "Usp10.h"

// CUniButton

//IMPLEMENT_DYNAMIC(CUniButton, CButton)
CUniButton::CUniButton()
{
	Reset();
}

void CUniButton::Reset()
{
	m_bUseRTL = 0;	//dont use right to left reading order
	m_dwDefaultSSAflags = 0;

	SetDefaultTextColors();

	m_HorzAlign = H_CENTER;
	m_VertAlign = V_CENTER;
	
	//remove old font
	m_font.DeleteObject();

	int m_fheight = 11;
	LOGFONT lfont;
	memset(&lfont, 0, sizeof(LOGFONT));

	lfont.lfHeight = 11;
	lfont.lfWeight = 400;
	lfont.lfItalic = false;
	lfont.lfOutPrecision = OUT_TT_PRECIS;
	lfont.lfQuality = ANTIALIASED_QUALITY;
	strcpy(lfont.lfFaceName, "MS Sans Serif");


	m_font.CreateFontIndirect(&lfont);
	mTextDefined = mFontDefined = false;
	ToolTipText = "";
	Script = "";
	Value = 0;
	m_szText[0]=0;
	m_HotKey = -1;
	m_MouseOver = false;
	m_OriginalStyle = BS_OWNERDRAW;
	m_OriginalStyleValid = false;
	DrawPushed = MomentaryCommandedDown = false;
}

void CUniButton::SetDefaultTextColors()
{
	m_clrHilightBkgnd = m_clrBkgnd = GetGlobalData()->GetColor(COLOR_3DLIGHT); // Closest thing I can find to normal button face

	m_clrNormalText = GetSysColor(COLOR_BTNTEXT);
	m_clrHilightText = GetSysColor(COLOR_BTNTEXT);
}

int CUniButton::HandleButtonDown()
{
	if (Style == Label) return 0;

	ForceDisableFocus = false;
	SetFocus();

	if (Style == Momentary)
	{
		if (Var >= 0 && Var <= 4096)
	{
		CString s;
		s.Format("SetBit%d", Var);
		MomentaryResult = TheFrame->KMotionDLL->WriteLine(s);
		if (MomentaryResult) return 1; // don't push button down if it failed
		}
		DrawPushed = MomentaryCommandedDown = true;
		Invalidate();
	}
	return 0;
}

CUniButton::~CUniButton()
{
	m_font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CUniButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// this function is called on a WM_MOUSELEAVE
LRESULT CUniButton::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	if (!DetermineOwnerDraw())
	{
		CButton::OnMouseLeave();
		return 0;
	}
	// write message to debug output window
	// the mouse has left the window
	m_MouseOver = false;
	Invalidate();
	return 0;
}

LRESULT CUniButton::OnMouseHover(WPARAM wp, LPARAM lp)
{
	// write message to debug output window
	// each time a WM_MOUSEHOVER is generated it cancels previous calls to TrackMouseEvent()
	// so we must recall it everytime a WM_MOUSEHOVER or WM_MOUSELEAVE is generated.The
	// logic that calls it in WM_MOUSELEAVE is actually in WM_MOUSEMOVE. When the cursor
	// is detected as moving back into our window TrackMouseEvent() gets called.
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_HOVER | TME_LEAVE;
	tme.hwndTrack = m_hWnd;
	tme.dwHoverTime = HOVER_DEFAULT;
	::TrackMouseEvent(&tme);
	return 0;
}

void CUniButton::OnMouseMove(UINT flags, CPoint point)
{
	if (!DetermineOwnerDraw())
	{
		CButton::OnMouseMove(flags, point);
		return;
	}

	// see large comment in previous function
	if (!m_MouseOver && Style != Label && Style != DROLabel)
	{
		m_MouseOver = true;
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		::TrackMouseEvent(&tme);
		Invalidate();
	}
}

void CUniButton::SetFont(const char *szFaceName, int height, bool Bold, bool Italic)
{
	mFontDefined = szFaceName[0] != 0;
	//remove old font
	m_font.DeleteObject();

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
	if (!DetermineOwnerDraw())
		CButton::SetFont(&m_font);
}


// may be overridden
bool CUniButton::DetermineOwnerDraw(void)
{
	if (mFontDefined && Style!=Undefined)
	{
		int ButtonStyle = GetStyle();
		if ((ButtonStyle & BS_TYPEMASK) != BS_OWNERDRAW)
		{
			m_OriginalStyle = ButtonStyle;
			m_OriginalStyleValid = true;
		}
		SetButtonStyle((ButtonStyle & ~BS_TYPEMASK) | BS_OWNERDRAW);
		return true;
	}
	else
	{
		int ButtonStyle = GetStyle();
		if (m_OriginalStyleValid && (ButtonStyle & BS_TYPEMASK) == BS_OWNERDRAW)
			SetButtonStyle((ButtonStyle & ~BS_TYPEMASK) | (m_OriginalStyle & BS_TYPEMASK));
		return false;
	}
}


int CUniButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
// CUniButton message handlers

void CUniButton::PlotTXTUnicode(
	HDC      hDC,               // In   Device
	int		xpos,
	int		ypos,
	RECT    *prc,               // In   Clipping rectange
	WCHAR	*szText,
	int      iLen)				//len of string
{
	HRESULT  hr;
	int      iClipWidth;
	int      iFrom;
	int      iTo;
	DWORD    dwFlags;

	SCRIPT_STRING_ANALYSIS ssa;


	if (iLen < 1)
		return;

	// There's at least one character to display

	dwFlags = m_dwDefaultSSAflags | SSA_GLYPHS;    // Require glyph generation for ScriptStringOut

	if (m_bUseRTL)
		dwFlags |= SSA_RTL;

	// Draw a line to indicate clipping position

	iClipWidth = prc->right - prc->left - 50;


	// Analyse the string.
	//
	// ScriptStringAnalyse calls the ScriptItemize, ScriptShape,
	// ScriptPlace and ScriptBreak APIs and returns a pointer to a block
	// of memory containing all these results.

	hr = ScriptStringAnalyse(
		hDC,
		szText,
		iLen, 0, -1,
		dwFlags,
		iClipWidth,
		NULL,
		NULL,
		NULL,
		NULL, NULL, &ssa);

	if (SUCCEEDED(hr)) 
	{
		// Display this line
		// Support selection range specified in either direction ifrom,ito
		iFrom = 1;	//no selection
		iTo = 0;
		hr = ScriptStringOut(
			ssa,
			xpos,
			ypos,
			ETO_CLIPPED,
			prc,
			iFrom,   // ScriptStringOut will ignore From/To that are outside this line
			iTo,
			FALSE);


		ScriptStringFree(&ssa);
	}
}

SIZE CUniButton::CalcTXTUnicode(HDC hDC, WCHAR *Sstr)
{
	SIZE tsize;
	tsize.cx = 0;
	tsize.cy = 0;

	HRESULT  hr;
	DWORD    dwFlags;

	SCRIPT_STRING_ANALYSIS ssa;

	size_t iLen = wcslen(Sstr);

	if (iLen < 1)
		return tsize;

	// There's at least one character to display

	dwFlags = m_dwDefaultSSAflags | SSA_GLYPHS;    // Require glyph generation for ScriptStringOut

	if (m_bUseRTL)
		dwFlags |= SSA_RTL;

	// Analyse the string.
	//
	// ScriptStringAnalyse calls the ScriptItemize, ScriptShape,
	// ScriptPlace and ScriptBreak APIs and returns a pointer to a block
	// of memory containing all these results.

	hr = ScriptStringAnalyse(
		hDC,
		Sstr,
		(int)iLen, 0, -1,
		dwFlags,
		0,
		NULL,
		NULL,
		NULL,
		NULL, NULL, &ssa);

	const SIZE *rsize = ScriptString_pSize(ssa);
	if (rsize != NULL)
	{
		tsize.cx = rsize->cx;
		tsize.cy = rsize->cy;
	}

	ScriptStringFree(&ssa);

	return tsize;
}

void CUniButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect, focusrect;

	// draw the colored rectangle portion
	rect.CopyRect(&lpDrawItemStruct->rcItem);
	focusrect = rect;
	focusrect.DeflateRect(2, 2, 2, 2);

	COLORREF tColor = RGB(200, 200, 200);	//disabled

	if (Style == Label || Style == DROLabel)
	{
		tColor= m_clrNormalText;
	}
	else if (lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		tColor = GetSysColor(COLOR_GRAYTEXT);
	}
	else if ((lpDrawItemStruct->itemState & ODS_SELECTED) || (Style == ToggleButton && Toggled) || DrawPushed)
	{
		// need this to allow space bar to work
		if (Style == Momentary && !MomentaryCommandedDown)
		{
			HandleButtonDown();
		}

		pDC->FillSolidRect(rect, m_clrHilightBkgnd);
		tColor = m_clrHilightText;
		pDC->Draw3dRect(rect,
			GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNSHADOW));
	}
	else
	{
		if (m_MouseOver)
		{
			COLORREF HoverColor;
			if (fabs(GetRValue(m_clrBkgnd) - GetRValue(m_clrHilightBkgnd) > 63) || 
				fabs(GetGValue(m_clrBkgnd) - GetGValue(m_clrHilightBkgnd) > 63) || 
				fabs(GetBValue(m_clrBkgnd) - GetBValue(m_clrHilightBkgnd) > 63))
			{
				// if "pushed" color is different go half way there
				HoverColor = RGB((GetRValue(m_clrBkgnd) + GetRValue(m_clrHilightBkgnd)) / 2,
								(GetGValue(m_clrBkgnd) + GetGValue(m_clrHilightBkgnd)) / 2,
								(GetBValue(m_clrBkgnd) + GetBValue(m_clrHilightBkgnd)) / 2);	//Hover
			}
			else
			{
				// otherwise if light in color ( large reduce R and G values (make bluish)
				if (GetRValue(m_clrBkgnd) > 128 && GetGValue(m_clrBkgnd) > 128)
				{
					HoverColor = RGB(229,241,251);
				}
				else
				{
					HoverColor = RGB(0,0,200);
				}
			}
			pDC->FillSolidRect(rect, HoverColor);
		}
		else
			pDC->FillSolidRect(rect, m_clrBkgnd);

		tColor = m_clrNormalText;
		pDC->Draw3dRect(rect,
			GetSysColor(COLOR_BTNHIGHLIGHT), GetSysColor(COLOR_BTNSHADOW));
	}

	if ((Style != Label && Style != DROLabel) && lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&focusrect);
	}

	int oldbkmode = pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(tColor);

	CFont *oldfont = pDC->SelectObject(&m_font);


	SIZE txtext;
	txtext = CalcTXTUnicode(pDC->m_hDC, m_szText);
	//	rect.right=txtext.cx;
	//	rect.bottom=txtext.cy;

	int xoffset, yoffset;
	switch (m_HorzAlign)
	{
	case H_LEFT:
		xoffset = rect.right - txtext.cx - 2;
		break;
	case H_CENTER:
		xoffset = (rect.right - txtext.cx) / 2;
		break;
	case H_RIGHT:
		xoffset = 2;
		break;
	}

	switch (m_VertAlign)
	{
	case V_TOP:
		yoffset = 2;
		break;
	case V_CENTER:
		yoffset = (rect.bottom - txtext.cy) / 2;
		break;
	case V_BOTTOM:
		yoffset = rect.bottom - txtext.cy - 2;
		break;
	}

	PlotTXTUnicode(pDC->m_hDC, xoffset, yoffset, &rect, m_szText, (int)wcslen(m_szText));

	pDC->SelectObject(oldfont);
	pDC->SetBkMode(oldbkmode);
}
