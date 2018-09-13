// ImageButton.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"
#include "ImageButton.h"
#include "Resample.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CList <LPCImageButton, LPCImageButton> CImageButton::ImageButtons;
bool CImageButton::ErrorDisplayed=false;

/////////////////////////////////////////////////////////////////////////////
// CImageButton

CImageButton::CImageButton(UINT up, UINT down, UINT disabled, BOOL fill)
{
	m_up = up;
	m_down = down;
	m_fill = fill;
	m_disabled = disabled;
	ErrorDisplayed = DrawPushed = ForceDisableFocus = false;
	Style = Button;
	img_id_loaded = -1;
	DualButtonState = 0;
	MomentaryResult = 0;
	MomentaryCommandedDown = false;
	CImageButton::ImageButtons.AddTail(this);
}


CImageButton::~CImageButton()
{
}


bool CImageButton::DetermineOwnerDraw(void)
{
	int ButtonStyle = GetStyle();


	if ((ButtonStyle & BS_TYPEMASK) == BS_RADIOBUTTON || (ButtonStyle & BS_TYPEMASK) == BS_AUTORADIOBUTTON)
		Style = Radio;

	if ((ButtonStyle & BS_TYPEMASK) == BS_CHECKBOX || (ButtonStyle & BS_TYPEMASK) == BS_AUTOCHECKBOX)
		Style = CheckBox;

	if (m_up_file != "" || m_down_file != "" || m_up != 0 || m_down != 0 || 
		((mFontDefined || mTextDefined) && Style!=Undefined && Style != Radio && Style != CheckBox))
	{
		// we should draw the control

		if ((ButtonStyle & BS_TYPEMASK) != BS_OWNERDRAW)
		{
			m_OriginalStyle = ButtonStyle;
			m_OriginalStyleValid = true;
			SetButtonStyle((ButtonStyle & ~BS_TYPEMASK) | BS_OWNERDRAW);  //note call only when needed or causes flicker
		}
		return true;
	}
	else
	{
		// Windows should draw the control

		if (m_OriginalStyleValid && (ButtonStyle & BS_TYPEMASK) == BS_OWNERDRAW)
			SetButtonStyle((ButtonStyle & ~BS_TYPEMASK) | (m_OriginalStyle & BS_TYPEMASK));
		return false;
	}
}

BEGIN_MESSAGE_MAP(CImageButton, CButton)
	//{{AFX_MSG_MAP(CImageButton)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONUP()

	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_CONTROL_REFLECT_EX(BN_CLICKED, &CImageButton::OnBnClicked)
	ON_WM_ERASEBKGND()
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
	style &= ~(BS_TOP | BS_BOTTOM | BS_VCENTER);
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
	style &= ~(BS_LEFT | BS_RIGHT | BS_CENTER);
	style |= newstyle;
	::SetWindowLong(m_hWnd, GWL_STYLE, (long)style);
	InvalidateRect(NULL);
	return result & (BS_LEFT | BS_RIGHT | BS_CENTER);
} // CImageButton::SetHPos


void CImageButton::DrawItem(LPDRAWITEMSTRUCT dis)
{
	static int Entry = 0;

	if (Entry > 0)
	{
		if (!ErrorDisplayed)
		{
			ErrorDisplayed = true;
			AfxMessageBox("DrawItem re-entry");
		}
		return;
	}
	Entry++;

	COLORREF background = 0xd8e9ec;

	bool do_alpha = true;
	bool do_focus = true;
	bool do_border = true;
	bool do_base_offset = true;
	bool did_alpha_blend = false;

	bool DoAlphaBlendWithBackGroundImage = ((Style == DualLabel || Style == Label) && TheFrame->GCodeDlg.Screen.BGFile_loaded != "");

	ASSERT(dis->hDC != NULL);

	if (Style == VertBar)
	{
		DrawBarGraph(dis);
		Entry--;
		return;
	}


	// any bitmaps?
	if (m_up_file == "" && m_down_file == "" && m_up == 0 && m_down == 0)
	{
		CUniButton::DrawItem(dis);  // no draw font
		Entry--;
		return;
	}

	if (Style == Label || Style == DROLabel || Style == DualLabel)
	{
		do_focus = false;
		do_border = false;
		do_base_offset = false;
	}

	if (Style == Button)
	{
		do_border = true;
	}

	CDC * dc = CDC::FromHandle(dis->hDC); // Get a CDC we can use
	CRect r(dis->rcItem);		   // Copy the button rectangle
	CBitmap BitmapFile;				// bitmap we are drawing
	CBitmap Bitmap;					// bitmap we are drawing
	BITMAP bmpval;					// Parameters of the bitmap
	CPoint fpos(0, 0);			   // Focus position
	CPoint ipos(0, 0);			   // Image position
	CSize border(0, 0);
	CBitmap BitmapAlpha;           // Bitmap to be created the size of the background to AlphaBlend
	BITMAP bmpvalAlpha;					// Parameters of the bitmap

	if (do_border)
	{
		border.SetSize(::GetSystemMetrics(SM_CXBORDER), ::GetSystemMetrics(SM_CYBORDER));
	}
	// Dimensions of a border line
	// We use ::GetSystemMetrics so we work on
	// hi-res displays
	CPoint baseOffset(border.cx, border.cy); // Offset amount if button-down
	int saved = dc->SaveDC();		   // Save the DC for later restoration
	bool grayout = false;		   // Gray out stock image?
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
	CString idfile;
	bool DrawItDown = (dis->itemState & ODS_SELECTED || ((ToggleType || Style==ToggleButton) && Toggled) || DrawPushed);

	if (Style==Momentary && DrawItDown && !MomentaryCommandedDown)
	{
		HandleButtonDown();
	}

	if (Style == DualButton || Style == DualLabel)
	{
		idfile = DualButtonState == 0 ? m_up_file : m_down_file;
		id = DualButtonState == 0 ? m_up : m_down;
	}
	else
	{
		idfile = DrawItDown && m_down_file != "" ? m_down_file : m_up_file;
		id =  DrawItDown && m_down != 0 ? m_down : m_up;
	}

	
	if (!DrawItDown && (dis->itemState & (ODS_DISABLED | ODS_GRAYED)))
	{ /* grayed */
		if (m_disabled == 0 && m_disabled_file == "")
		{ /* no disabled image */
			grayout = true; // gray out manually
		}
		else
		{ /* use disabled image */
			idfile = m_disabled_file;
			id = m_disabled;
		}
	} 

	if (idfile == "")
	{
		if (id != img_id_loaded)
		{
			BitmapFile.LoadBitmap(id);
			HBITMAP hBmp = img_id; // cImage is still the data owner
			if (hBmp != NULL)
			{
				hBmp = img_id.Detach(); // cImage is "empty" now
				::DeleteObject(hBmp);
			}
			img_id.Attach((HBITMAP)BitmapFile.Detach());  // save/associate with img_id 
			img_id_loaded = id;
		}
		BitmapFile.Attach(img_id);
	}
	else
	{
		// check if there is no path specified, then add in default

		if (idfile.Find(':') == -1 && idfile.Find("\\\\") == -1)
		{
			idfile = TheFrame->MainPathRoot + SCREEN_BITMAPS_DIR + idfile;
		}

		if (DrawItDown)
		{
			if (img_file_loaded_down != idfile)
			{
				if (img_down != NULL) img_down.Detach();
				HRESULT result = img_down.Load(_T(idfile));
				if (result == E_FAIL)
				{
					Entry--;
					if (!ErrorDisplayed)
					{
						ErrorDisplayed = true;
						AfxMessageBox("Can't open button image:" + idfile);
					}
					return;
				}
				img_file_loaded_down = idfile;
	
				Convert24to32(&img_down, img_file_loaded_down);
			}

			BitmapFile.Attach(img_down);
		}
		else
		{
			if (img_file_loaded_up != idfile)
			{
				if (img_up != NULL) img_up.Detach();
				HRESULT result = img_up.Load(_T(idfile));
				if (result == E_FAIL)
				{
					Entry--;
					if (!ErrorDisplayed)
					{
						ErrorDisplayed = true;
						AfxMessageBox("Can't open button image:" + idfile);
					}
					return;
				}
				img_file_loaded_up = idfile;

				Convert24to32(&img_up, img_file_loaded_up);
			}
			BitmapFile.Attach(img_up);
		}
	}

	if (BitmapFile.m_hObject == NULL)
	{
		Entry--;
		return;
	}

	BitmapFile.GetBitmap(&bmpval);
	Bitmap.CreateBitmapIndirect(&bmpval);

	if (Bitmap.m_hObject == NULL)
	{
		Entry--;
		return;
	}


	// replace transparent pixels
	// note do this before resizing as special pixel values may change slightly
	BYTE *bits = new BYTE[bmpval.bmWidthBytes*bmpval.bmHeight];
	if (!bits)
	{
		Entry--;
		return;
	}
	BitmapFile.GetBitmapBits(bmpval.bmWidthBytes*bmpval.bmHeight, bits);
	BitmapFile.Detach();  //so bitmap object wont be deleted when BitmapFile destructs

	// for legacy purposes check if special Grey color instead of alpha was used for transparency
	if (((bits[0] << 16) | (bits[1] << 8) | bits[2]) == background) do_alpha = false;
	
	// replace background color (0xece9d8) with 3D face color
	if (!do_alpha && (bmpval.bmBitsPixel == 24 || bmpval.bmBitsPixel == 32))
	{
		COLORREF gray = ::GetSysColor(COLOR_3DFACE);
		for (int r = 0; r < bmpval.bmHeight; r++)
		{
			for (int c = 0; c < bmpval.bmWidth; c++)
			{
				int i = r*bmpval.bmWidthBytes + c*(bmpval.bmBitsPixel / 8);
				int v = (bits[i] << 16) | (bits[i + 1] << 8) | bits[i + 2];

				if (v == background)
				{
					bits[i] = gray >> 16;
					bits[i + 1] = (gray >> 8) & 0xff;
					bits[i + 2] = (gray) & 0xff;
				}
			}
		}
	}


	// do alpha, when transparent replace with background color (3D face color)/Image
	if (do_alpha && bmpval.bmBitsPixel == 32)
	{
		if (DoAlphaBlendWithBackGroundImage)
		{
			CBrush Br;
			Br.Attach(TheFrame->GCodeDlg.CreateDIBrush(this));
			LOGBRUSH lb;
			Br.GetLogBrush(&lb);
			BITMAPINFOHEADER *bih = (BITMAPINFOHEADER*)lb.lbHatch;

			int BackWidth = bih->biWidth;
			int BackHeight = -bih->biHeight;
			int SourceWidth = bmpval.bmWidth;
			int SourceHeight = bmpval.bmHeight;

			BYTE *BackBits = (BYTE *)lb.lbHatch + sizeof(BITMAPINFOHEADER);
			
			// compute offsets to center the two bitmaps
			int i, is, a, r0, c0, roff = (SourceHeight - BackHeight)/2, coff = (SourceWidth - BackWidth)/2;
			
			// go through the entire Background image
			for (int r = 0; r < BackHeight; r++)
			{
				r0 = r + roff;
				for (int c = 0; c < BackWidth; c++)
				{
					c0 = c + coff;

					// inside source image?
					if (r0 >= 0 && r0 < SourceHeight && c0 >= 0 && c0 < SourceWidth)
					{	// alpha blend
						i = (r0 * SourceWidth + c0) * 4;  // index to source image
						is = (r * BackWidth + c) * 4;  // index to background
						a = bits[i + 3];

						BackBits[is    ] = (a*bits[i    ] + (255 - a)*BackBits[is    ]) / 255;
						BackBits[is + 1] = (a*bits[i + 1] + (255 - a)*BackBits[is + 1]) / 255;
						BackBits[is + 2] = (a*bits[i + 2] + (255 - a)*BackBits[is + 2]) / 255;
					}
				}
			}

			bmpvalAlpha.bmType = 0;
			bmpvalAlpha.bmWidth = BackWidth;
			bmpvalAlpha.bmHeight = BackHeight;  // make it right side up (origin top left corner)
			bmpvalAlpha.bmWidthBytes = BackWidth * 4;
			bmpvalAlpha.bmPlanes = 1;
			bmpvalAlpha.bmBitsPixel = 32;
			bmpvalAlpha.bmBits = BackBits;
			
			// create a new bitmap the size of
			BitmapAlpha.CreateBitmapIndirect(&bmpvalAlpha);

			if (BitmapAlpha.m_hObject == NULL)
			{
				Entry--;
				return;
			}

			did_alpha_blend = true;
		}
		else
		{
			COLORREF gray;
			if (Style == Label || Style == DualLabel)
				gray = TheFrame->GCodeDlg.m_DlgBackgroundColor;
			else
				gray = ::GetSysColor(COLOR_3DFACE);

			for (int r = 0; r < bmpval.bmHeight; r++)
			{
				for (int c = 0; c < bmpval.bmWidth; c++)
				{
					int i = r*bmpval.bmWidthBytes + c*(bmpval.bmBitsPixel / 8);
					int is = (bmpval.bmHeight - 1 - r)*bmpval.bmWidthBytes + c*(bmpval.bmBitsPixel / 8);
					int a = bits[i + 3];

					bits[i]     = (a*bits[i]     + (255 - a)*(gray >> 16))/255;
					bits[i + 1] = (a*bits[i + 1] + (255 - a)*((gray >> 8) & 0xff))/255;
					bits[i + 2] = (a*bits[i + 2] + (255 - a)*((gray) & 0xff))/255;
				}
			}
		}
	}


	// if hovering apply some effect
	if (!DrawItDown && (Style != Undefined) && (Style != Label) && (Style != DualLabel) && (Style != DROLabel) && m_MouseOver)
	{
		for (int r = 0; r < bmpval.bmHeight; r++)
		{
			for (int c = 0; c < bmpval.bmWidth; c++)
			{
				int i = r*bmpval.bmWidthBytes + c*(bmpval.bmBitsPixel / 8);
				int a = bits[i + 3];

				bits[i] = HoverEffect(bits[i]);
				bits[i + 1] = HoverEffect(bits[i + 1]);
				bits[i + 2] = HoverEffect(bits[i + 2]);

				if (bits[i] > 250 && bits[i + 1] > 250 && bits[i + 2] > 250)
				{
					bits[i] = 255;
					bits[i + 1] = 255 *0.92;
					bits[i + 2] = 255 *0.85;
				}
			}
		}
	}



	// If the button was disabled but we don't have a disabled image, gray out
	// the image by replacing every other pixel with a gray pixel
	// This is best done only if the bitmap is small
	if (grayout)
	{ /* gray out */
		COLORREF gray = ::GetSysColor(COLOR_3DFACE);
		for (int r = 0; r < bmpval.bmHeight; r++)
		{
			for (int c0 = 0; c0 < bmpval.bmWidth - 1; c0 += 2)
			{
				int c = c0 + (r & 1);
				int i = r*bmpval.bmWidthBytes + c*(bmpval.bmBitsPixel / 8);
				if (bmpval.bmBitsPixel == 32)
				{
					bits[i] = gray >> 16;
					bits[i + 1] = (gray >> 8) & 0xff;
					bits[i + 2] = (gray) & 0xff;
				}
				else
				{
					bits[i] = ((6 << 10) | (6 << 5) | 6) >> 8;
					bits[i + 1] = ((6 << 10) | (6 << 5) | 6) & 0xff;
				}
			}
		}
	} /* gray out */

	  // Select the bitmap into a DC
	CDC memDC;
	memDC.CreateCompatibleDC(dc);

	if (did_alpha_blend)
	{
		memDC.SelectObject(BitmapAlpha);
		dc->BitBlt(0, 0,		// target x, y
			bmpvalAlpha.bmWidth, // target width
			bmpvalAlpha.bmHeight, // target height
			&memDC,			// source DC
			0, 0,	// source x, y
			SRCCOPY);
	}
	else
	{
		// put pixels back into the bitmap
		Bitmap.SetBitmapBits(bmpval.bmWidthBytes*bmpval.bmHeight, bits);

		// Scale the original file Image
		CResample Resample;
		CBitmap BitmapScaled;

		BitmapScaled.Attach(Resample.CreateResampledBitmap(dis->hDC, Bitmap, (int)(bmpval.bmWidth * m_scale), (int)(bmpval.bmHeight * m_scale), STOCK_FILTER_LANCZOS3));
		memDC.SelectObject(BitmapScaled);


		// Get the bitmap parameters, because we will need width and height
		BitmapScaled.GetBitmap(&bmpval);

		// We compute the desired image size. Because we could offset
		// the image on a button-down, we take that potential into
		// consideration so we don't truncate the image on the right
		// or bottom when we depress the button
		CSize image;

		// Compute the origin of the focus rectangle (fpos) and the
		// bitmap image (ipos)
		// If we are in fill mode, this computation is simple
		// otherwise, we have to take the various alignments into consideration
		if (!m_fill)
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

			switch (style)
			{ /* hstyle */
			case BS_LEFT:
				fpos.x = 2 * border.cx;
				ipos.x = fpos.x + border.cx;
				break;
			case BS_RIGHT:
				fpos.x = clientArea.cx - image.cx;
				if (fpos.x < 2 * border.cx)
				{ /* adjust right */
					bltStart.x = image.cx - clientArea.cx;
					fpos.x = 2 * border.cx;
				} /* adjust right */
				ipos.x = fpos.x + border.cx;
				break;
			case 0:
			case BS_CENTER:
				fpos.x = (clientArea.cx - image.cx) / 2 + border.cx;
				if (fpos.x < 2 * border.cx)
				{ /* adjust center */
					bltStart.x = (image.cx - clientArea.cx) / 2;
					fpos.x = 2 * border.cx;
				} /* adjust center */
				ipos.x = fpos.x + border.cx;
				break;
			} /* hstyle */

			style = GetStyle();
			style &= (BS_TOP | BS_BOTTOM | BS_VCENTER);

			switch (style)
			{ /* vstyle */
			case BS_TOP:
				fpos.y = 2 * border.cy;
				ipos.y = fpos.y + border.cy;
				break;
			case BS_BOTTOM:
				fpos.y = clientArea.cy - image.cy;
				if (fpos.y < 2 * border.cy)
				{ /* adjust to bottom */
					bltStart.y = image.cy - clientArea.cy;
					fpos.y = 2 * border.cy;
				} /* adjust to bottom */
				ipos.y = fpos.y + border.cy;
				break;
			case 0:
			case BS_VCENTER:
				fpos.y = (clientArea.cy - image.cy) / 2 + border.cy;
				if (fpos.y < 2 * border.cy)
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

		CPoint useOffset(0, 0);
		if (do_border)
		{
			// For visual effect, if the button is down, shift the image over and
			// down to make it look "pushed".
			if (DrawItDown)
			{ /* down */
				useOffset = baseOffset;
			} /* down */

			  // Draw the traditional pushbutton edge using DrawEdge
			if (DrawItDown)
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
		}
		else
		{
			CRect rect;

			// draw the colored rectangle portion
			rect.CopyRect(&dis->rcItem);
			rect.DeflateRect(2, 2, 2, 2);

			dc->FillSolidRect(rect, TheFrame->GCodeDlg.m_DlgBackgroundColor);
		}

		if (m_fill)
		{
			CBitmap BitmapFilled;
			BitmapFilled.Attach(Resample.CreateResampledBitmap((HDC)memDC, (HBITMAP)BitmapScaled, image.cx, image.cy, STOCK_FILTER_LANCZOS3));

			memDC.SelectObject(BitmapFilled);

			dc->BitBlt(ipos.x, ipos.y,		// target x, y
				image.cx, // target width
				image.cy, // target height
				&memDC,			// source DC
				bltStart.x, bltStart.y,	// source x, y
				SRCCOPY);
		}
		else
		{
			dc->BitBlt(ipos.x, ipos.y,		// target x, y
				min(bmpval.bmWidth, r.Width() - ipos.x - useOffset.x), // target width
				min(bmpval.bmHeight, r.Height() - ipos.y - useOffset.y), // target height
				&memDC,			// source DC
				bltStart.x, bltStart.y,	// source x, y
				SRCCOPY);
	}


		if (do_focus && (dis->itemState & ODS_FOCUS) && !ForceDisableFocus)
			::DrawFocusRect(dis->hDC, &focus);
	}

	delete bits;


	memDC.DeleteDC();
	dc->RestoreDC(saved);
	Entry--;
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
	if (HandleButtonDown()) return;
	CButton::OnLButtonDown(nFlags, point);
}




void CImageButton::HandleButtonUp()
{
	if (Style == Momentary && Var >=0 && Var <= 4096 && MomentaryResult==0 && MomentaryCommandedDown) // Don't try to clear if there was an error Pushing
	{
		CString s;
		s.Format("ClearBit%d", Var);
		MomentaryResult = TheFrame->KMotionDLL->WriteLine(s);
		if (MomentaryResult==0)
			MomentaryCommandedDown = false;
	}
	DrawPushed = false;
	InvalidateRect(NULL);
}

void CImageButton::OnKillFocus(CWnd* pNewWnd)
{
	// don't do much to allow multiple motion buttons
	HandleButtonUp();
	ForceDisableFocus = true;
	InvalidateRect(NULL);
	CButton::OnKillFocus(pNewWnd);
}

void CImageButton::OnCaptureChanged(CWnd* Wnd)
{
	HandleButtonUp();
	CButton::OnCaptureChanged(Wnd);
}

void CImageButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	HandleButtonUp();
	CButton::OnLButtonUp(nFlags, point);
}



BOOL CImageButton::OnBnClicked()
{
	DLG_CONTROL Dlg;
	bool NewControl;
	CString s;

	if (Style == Label) return FALSE;

	SetFocus();

	if (ToggleType)  // old mode for KeyJog button
	{
		Toggled = !Toggled;
	}
	else if (Style == ToggleButton)
	{
		if (Var >= 0) // if Var is specified then Toggle an IO Bit
		{
			if (Toggled)
				s.Format("ClearBit%d", Var);
			else
				s.Format("SetBit%d", Var);

			TheFrame->KMotionDLL->WriteLine(s);
			Toggled = !Toggled;  // try to avoid flicker
			return FALSE;
		}
		else // if no Var Specifed then just Toggle the State
		{
			Toggled = !Toggled;
		}
	}


	if (Script != "") TheFrame->GCodeDlg.Screen.Execute(Script, &Dlg, &NewControl);

	Invalidate();
	return FALSE;
}

void CImageButton::Convert24to32(CImage *img, CString file)
{
	if (img->GetBPP() == 32)
	{
		// 32 bit BMP files are actually invalid and do not support Alpha
		// some programs output them.  If so set alpha to 0xFF
		file = file.Right(4);
		file.MakeUpper();
		if (file == ".BMP")
		{
			// set the alph channel to 0xFF
			BYTE *bits32 = (BYTE *)img->GetBits();
			for (int r = 0; r < img->GetHeight(); r++)
			{
				for (int c = 0; c < img->GetWidth(); c++)
				{
					int i = r*img->GetPitch() + c * 4;
					bits32[i + 3] = 0xFF;
				}
			}
		}
	} 
	else if (img->GetBPP() == 24)
	{
		CImage img32;
		img32.CreateEx(img->GetWidth(), img->GetHeight(), 32, BI_RGB, NULL, CImage::createAlphaChannel);

		// Copy the pixels:
		HDC const hdc = img32.GetDC();
		img->AlphaBlend(hdc, 0, 0, 0xff, AC_SRC_OVER);
		img32.ReleaseDC();
		img->Detach();
		img->Attach(img32);
		img32.Detach();

		// we loaded a 24bpp image set the alph channel to 0xFF
		BYTE *bits32 = (BYTE *)img->GetBits();
		for (int r = 0; r < img->GetHeight(); r++)
		{
			for (int c = 0; c < img->GetWidth(); c++)
			{
				int i = r*img->GetPitch() + c * 4;
				bits32[i + 3] = 0xFF;
			}
		}
	}
}

int CImageButton::HoverEffect(int i)
{
	int r = (i - 128) * 2 + 128;
	if (r < 0) r = 0;
	if (r > 255) r = 255;
	return r;
}

BOOL CImageButton::OnEraseBkgnd(CDC* pDC)
{
	// any bitmaps? (don't erase to avoid fliker)
	if (m_up_file == "" && m_down_file == "" && m_up == 0 && m_down == 0)
		return CWnd::OnEraseBkgnd(pDC);
	else
		return FALSE;
}

void CImageButton::SetToggled(bool state)
{
	if (Toggled != state)
	{
		Toggled = state;
		InvalidateRect(NULL);
	}
}

void CImageButton::DrawBarGraph(LPDRAWITEMSTRUCT dis)
{
	CDC * dc = CDC::FromHandle(dis->hDC);	// Get a CDC we can use
	int saved = dc->SaveDC();				// Save the DC for later restoration
	CRect r(dis->rcItem);					 // Copy the button rectangle
	CSize clientArea(r.Width(), r.Height());

	CRect Top(r);
	CRect Bot(r);

	int y = r.Height() * (1.0-Value);

	if (y < 0) y = 0;
	if (y > r.Height()) y = r.Height();

	Top.bottom = y;
	Bot.top = y;

	dc->FillSolidRect(Top, m_clrBkgnd);
	dc->FillSolidRect(Bot, m_clrNormalText);

	dc->RestoreDC(saved);
}
