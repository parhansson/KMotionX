// ImageButton.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"
#include "ImageButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageButton

CImageButton::CImageButton(UINT up, UINT down, UINT disabled, BOOL fill)
{
 this->up = up;
 this->down = down;
 this->disabled = disabled;
 this->m_fill = fill;
}

CImageButton::~CImageButton()
{
}


BEGIN_MESSAGE_MAP(CImageButton, CButton)
	//{{AFX_MSG_MAP(CImageButton)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, &CImageButton::OnBnClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageButton message handlers

/****************************************************************************
*                            CImageButton::SetVPos
* Inputs:
*       DWORD newstyle: Vertical style, one of BS_TOP, BS_BOTTOM, BS_VCENTER
* Result: DWORD
*       Previous style
* Effect: 
*       Sets the style
****************************************************************************/

DWORD CImageButton::SetVPos(DWORD newstyle)
    {
     DWORD style = GetStyle();
     DWORD result = style;
     style &= ~ (BS_TOP | BS_BOTTOM | BS_VCENTER);
     style |= newstyle;
     ::SetWindowLong(m_hWnd, GWL_STYLE, (long)style);
     InvalidateRect(NULL);
     return result & (BS_TOP | BS_BOTTOM | BS_VCENTER);
    } // CImageButton::SetVPos

/****************************************************************************
*                            CImageButton::SetHPos
* Inputs:
*       DWORD newstyle: Horizontal style, one of BS_LEFT, BS_RIGHT, BS_CENTER
* Result: DWORD
*       Previous style
* Effect: 
*       Sets the style
****************************************************************************/

DWORD CImageButton::SetHPos(DWORD newstyle)
{
    DWORD style = GetStyle();
    DWORD result = style;
    style &= ~ (BS_LEFT | BS_RIGHT | BS_CENTER);
    style |= newstyle;
    ::SetWindowLong(m_hWnd, GWL_STYLE, (long)style);
    InvalidateRect(NULL);
    return result & (BS_LEFT | BS_RIGHT | BS_CENTER);
} // CImageButton::SetHPos


void CImageButton::DrawItem(LPDRAWITEMSTRUCT dis) 
{
	CDC * dc = CDC::FromHandle(dis->hDC); // Get a CDC we can use
	CRect r(dis->rcItem);		   // Copy the button rectangle
	HBITMAP bitmap;			   // Handle to the bitmap we are drawing
	BITMAP bmpval;			   // Parameters of the bitmap
	CPoint fpos(0,0);			   // Focus position
	CPoint ipos(0,0);			   // Image position
	CSize border(::GetSystemMetrics(SM_CXBORDER), ::GetSystemMetrics(SM_CYBORDER));
	// Dimensions of a border line
	// We use ::GetSystemMetrics so we work on
	// hi-res displays
	CPoint baseOffset(border.cx, border.cy); // Offset amount if button-down
	int saved = dc->SaveDC();		   // Save the DC for later restoration
	BOOL grayout = FALSE;		   // Gray out stock image?
	CPoint bltStart(0, 0);		   // Nominal start of BLT
	
	// The first thing we do is deflate our useful area by the width of a
	// frame, which is twice a border. 
	r.InflateRect(-2 * border.cx, -2 * border.cy);
	
	// Load the bitmap. The three states are
	//  ODS_SELECTED  ODS_DISABLED 
	//       1		n/a		Button is pressed
	//	      0		0		Button is up
	//	      0		1		Button is disabled
	//
	// The possible images supplied 	The images used
	//    ------specified------	----------use----------
	//	   up	down	disabled	up	down	disabled
	//    =====================	========================
	//	   A	B	C		A	B	C
	//	   A	0	C		A	A	C
	//	   A	B	0		A	B	gray(A)
	//	   A	0	0		A	A	gray(A)
	
	UINT id = 0;
	if(dis->itemState & ODS_SELECTED || (ToggleType && Toggled))
	{ /* selected */
		// If the down image is given, use that, else use the
		// up image
		id = down != 0 ? down : up;
	} /* selected */
	else
	{ /* unselected */
		if(dis->itemState & (ODS_DISABLED | ODS_GRAYED))
		{ /* grayed */
			if(disabled == 0)
			{ /* no disabled image */
				id = up;
				grayout = TRUE; // gray out manually
			} /* no disabled image */
			else
			{ /* use disabled image */
				id = disabled;
			} /* use disabled image */
		} /* grayed */
		else
		{ /* enabled */
			id = up;
		} /* enabled */
	} /* unselected */
	
	bitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(id),
		IMAGE_BITMAP,
		0, 0,
		LR_SHARED | LR_LOADMAP3DCOLORS);
	
	// Get the bitmap parameters, because we will need width and height
	::GetObject(bitmap, sizeof(BITMAP), &bmpval);
	
	// We compute the desired image size. Because we could offset
	// the image on a button-down, we take that potential into
	// consideration so we don't truncate the image on the right
	// or bottom when we depress the button
	CSize image;
	
	// Compute the origin of the focus rectangle (fpos) and the
	// bitmap image (ipos)
	// If we are in fill mode, this computation is simple
	// otherwise, we have to take the various alignments into consideration
	if(!m_fill)
	{ /* compute alignment */
		// We need to compute the x,y horizontal coordinates
		// Compute the width and height of the available client area for later convenience
		// This just saves writing r.Width() and r.Height() all the time and
		// having the values recomputed
		CSize clientArea(r.Width(), r.Height());
		
		DWORD style = GetStyle();
		
		image.cx = bmpval.bmWidth;
		image.cy = bmpval.bmHeight;
		style &= (BS_LEFT | BS_RIGHT | BS_CENTER);
		
		switch(style)
		{ /* hstyle */
		case BS_LEFT:
			fpos.x = 2 * border.cx;
			ipos.x = fpos.x + border.cx;
			break;
		case BS_RIGHT:
			fpos.x = clientArea.cx - image.cx;
			if(fpos.x < 2 * border.cx)
			{ /* adjust right */
				bltStart.x = image.cx - clientArea.cx;
				fpos.x = 2 * border.cx;
			} /* adjust right */
			ipos.x = fpos.x + border.cx;
			break;
		case 0:
		case BS_CENTER:
			fpos.x = (clientArea.cx - image.cx) / 2 + border.cx;
			if(fpos.x < 2 * border.cx)
			{ /* adjust center */
				bltStart.x = (image.cx - clientArea.cx) / 2;
				fpos.x = 2 * border.cx;
			} /* adjust center */
			ipos.x = fpos.x + border.cx;
			break;
		} /* hstyle */
		
		style = GetStyle();
		style &= (BS_TOP | BS_BOTTOM | BS_VCENTER);
		
		switch(style)
		{ /* vstyle */
		case BS_TOP:
			fpos.y = 2 * border.cy;
			ipos.y = fpos.y + border.cy;
			break;
		case BS_BOTTOM:
			fpos.y = clientArea.cy - image.cy;
			if(fpos.y < 2 * border.cy)
			{ /* adjust to bottom */
				bltStart.y = image.cy - clientArea.cy;
				fpos.y = 2 * border.cy;
			} /* adjust to bottom */
			ipos.y = fpos.y + border.cy;
			break;
		case 0:
		case BS_VCENTER:
			fpos.y = (clientArea.cy - image.cy) / 2 + border.cy;
			if(fpos.y < 2 * border.cy)
			{ /* adjust top */
				bltStart.y = (image.cy - clientArea.cy) / 2;
				fpos.y = 2 * border.cy;
			} /* adjust top */
			ipos.y = fpos.y + border.cy;
			break;
		} /* vstyle */
	} /* compute alignment */
	else
	{ /* fill alignment */
		image.cx = r.Width() - baseOffset.x;
		image.cy = r.Height() - baseOffset.y;
		fpos.x = 2 * border.cx;
		ipos.x = fpos.x + border.cx;
		fpos.y = 2 * border.cy;
		ipos.y = fpos.y + border.cy;
	} /* fill alignment */
	
	// Compute the focus rectangle area
	CRect focus(fpos.x,
		fpos.y,
		fpos.x + image.cx + 2 * border.cx,
		fpos.y + image.cy + 2 * border.cy);
	
	// For visual effect, if the button is down, shift the image over and
	// down to make it look "pushed".
	CPoint useOffset(0, 0);
	if(dis->itemState & ODS_SELECTED || (ToggleType && Toggled))
	{ /* down */
		useOffset = baseOffset;
	} /* down */
	
	// Draw the traditional pushbutton edge using DrawEdge
	if(dis->itemState & ODS_SELECTED || (ToggleType && Toggled))
	{ /* down */
		dc->DrawEdge(&dis->rcItem, EDGE_SUNKEN, BF_RECT | BF_MIDDLE | BF_SOFT);
	} /* down */
	else
	{ /* up */
		dc->DrawEdge(&dis->rcItem, EDGE_RAISED, BF_RECT | BF_MIDDLE | BF_SOFT);
	} /* up */
	
	// Adjust the focus position and image position by shifting them
	// right-and-down by the desired offset
	focus += useOffset;
	ipos += useOffset;
	
	// Select the bitmap into a DC
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	int savemem = memDC.SaveDC();
	memDC.SelectObject(bitmap);


	// replace background color (0xece9d8) with 3D face color

	COLORREF gray = ::GetSysColor(COLOR_3DFACE);
	COLORREF background = 0xd8e9ec;
	for(int x = 0; x < bmpval.bmWidth; x++)
		for(int y = 0; y < bmpval.bmHeight; y++)
		{
			if (memDC.GetPixel(x,y) == background)
				memDC.SetPixelV(x, y, gray);
		}
	
	// If the button was disabled but we don't have a disabled image, gray out
	// the image by replacing every other pixel with a gray pixel
	// This is best done only if the bitmap is small
	if(grayout)
	{ /* gray out */
		COLORREF gray = ::GetSysColor(COLOR_3DFACE);
		for(int x = 0; x < bmpval.bmWidth; x+=2)
			for(int y = 0; y < bmpval.bmHeight; y++)
				memDC.SetPixelV(x + (y & 1), y, gray);
	} /* gray out */
	
	if(m_fill)
		dc->StretchBlt(ipos.x, ipos.y,		// target x, y
		image.cx, image.cy,	// target width, height
		&memDC,			// source DC
		bltStart.x, bltStart.y,	// source x, y
		bmpval.bmWidth - bltStart.x,
		bmpval.bmHeight - bltStart.y, // source width, height
		SRCCOPY);
	else
		dc->BitBlt(ipos.x, ipos.y,		// target x, y
		min(bmpval.bmWidth, r.Width() - ipos.x - useOffset.x), // target width
		min(bmpval.bmHeight, r.Height() - ipos.y - useOffset.y), // target height
		&memDC,			// source DC
		bltStart.x, bltStart.y,	// source x, y
		SRCCOPY);
	
	if(dis->itemState & ODS_FOCUS)
		::DrawFocusRect(dis->hDC, &focus);
	
	memDC.RestoreDC(savemem);
	
	dc->RestoreDC(saved);
	::DeleteObject(bitmap);
}


BOOL CImageButton::PreTranslateMessage(MSG* pMsg) 
{
	// standard check boxes, push buttons and auto-radio buttons
	// treat a double-click like a single click
	// when the BS_NOTIFY style has not been specified
	if (pMsg->message == WM_LBUTTONDBLCLK)
	{
		pMsg->message = WM_LBUTTONDOWN;
	}
	
	return CButton::PreTranslateMessage(pMsg);
}

void CImageButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDown(nFlags, point);
}


BOOL CImageButton::OnBnClicked()
{
	if (ToggleType)
		Toggled = !Toggled;
	
	return FALSE;
}
