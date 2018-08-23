// ImageButton.h : header file
//

#pragma once
#include <atlimage.h>
#include "UniButton.h"
class CImageButton;
class CMotionButton;

/////////////////////////////////////////////////////////////////////////////
// CImageButton window

typedef CImageButton *LPCImageButton;

enum StyleTypes
{
	Undefined = -1,
	Label = 0,
	Button = 1,
	MotionButton = 2,
	ToggleButton = 3,
	DROLabel = 4,
	VertBar = 5,
	Radio = 6,
	Momentary = 7,
	DualButton = 8,
	CheckBox = 9,
	DualLabel = 10
};


class CImageButton : public CUniButton
{
// Construction
public:
	CImageButton(UINT up, UINT down = 0, UINT disabled = 0, BOOL fill = FALSE);
	// Default constructor (required for MFC compatibility)
	CImageButton() 
	{
		m_scale=1.0;
		img_id_loaded = -1;
		DualButtonState = 0;
		MomentaryResult = 0;
		m_up = 0; m_down = 0; m_disabled = 0;
		Reset();
		CImageButton::ImageButtons.AddTail(this);
	}

	void Reset()
	{
		m_fill = FALSE; ToggleType = false; Var = -1; ErrorDisplayed = Toggled = DrawPushed = ForceDisableFocus = false;
		m_up_file = ""; m_down_file = ""; m_disabled_file = "";
		Style = Undefined;
		CUniButton::Reset();
	}

	BOOL m_fill; // We want to fill the image
	double m_scale;
	DWORD SetHPos(DWORD style); // Set the horizonatal alignment style
	DWORD SetVPos(DWORD style); // Set the vertical alignment style
	void SetToggled(bool state);

	void DrawBarGraph(LPDRAWITEMSTRUCT dis);

	int dpiX;
	int dpiY;
	int dpi_standard = 96;

	//----------------------
	// LoadBitmaps
	//	UINT up: Resource ID of image to use for up, enabled
	//	UINT down: Resource ID of image to use for down, enabled
	//			if NULL, the up image is used
	//	UINT disabled: Resource ID of image to use for disabled
	//			if NULL, the up image is halftoned gray
	void LoadBitmaps(UINT up, UINT down, UINT disabled)
	{ 
		m_up = up; m_down = down; m_disabled = disabled;
		m_up_file = m_down_file = m_disabled_file = "";
		DetermineOwnerDraw();
		CDC *screen = GetDC();
		dpiX = screen->GetDeviceCaps(LOGPIXELSX);
		dpiY = screen->GetDeviceCaps(LOGPIXELSY);
		ReleaseDC(screen);
		m_fill = FALSE;
		m_scale = dpiX / (double)dpi_standard;
	}

	void GetBitmaps(UINT &up, UINT &down, UINT &disabled)
	{ 
		up = m_up; down = m_down; disabled = m_disabled; 
	}

	void LoadBitmaps(CString up, CString down, CString disabled)
	{ 
		m_up_file = up; m_down_file = down; m_disabled_file = disabled; 
		DetermineOwnerDraw();
	} 



	static bool ErrorDisplayed;

	CImage img_down;
	CString img_file_loaded_down;
	CImage img_up;
	CString img_file_loaded_up;
	CImage img_id;
	int img_id_loaded;


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	virtual bool DetermineOwnerDraw(void);

	int HoverEffect(int i);

// Implementation
public:
	virtual ~CImageButton();

    UINT m_up;
	UINT m_down;
	UINT m_disabled;
    CString m_up_file;
	CString m_down_file;
	CString m_disabled_file;

	int DualButtonState;
	
	//{{AFX_MSG(CImageButton)
	//}}AFX_MSG

	void HandleButtonUp();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void OnKillFocus(CWnd* pNewWnd);
	void OnCaptureChanged(CWnd* Wnd);
	void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg BOOL OnBnClicked();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void Convert24to32(CImage * Img, CString file);

	static CList <LPCImageButton,LPCImageButton> ImageButtons;
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
