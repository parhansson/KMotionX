// ImageButton.h : header file
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CImageButton window

class CImageButton : public CButton
{
// Construction
public:
	CImageButton(UINT up, UINT down = 0, UINT disabled = 0, BOOL fill = FALSE);
	// Default constructor (required for MFC compatibility)
	CImageButton() {m_up = 0; m_down = 0; m_disabled = 0; m_fill = FALSE; ToggleType=false; Toggled=DrawPushed=ForceDisableFocus=false;}
	BOOL m_fill; // We want to fill the image
	DWORD SetHPos(DWORD style); // Set the horizonatal alignment style
	DWORD SetVPos(DWORD style); // Set the vertical alignment style
	//----------------------
	// LoadBitmaps
	//	UINT up: Resource ID of image to use for up, enabled
	//	UINT down: Resource ID of image to use for down, enabled
	//			if NULL, the up image is used
	//	UINT disabled: Resource ID of image to use for disabled
	//			if NULL, the up image is halftoned gray
	void LoadBitmaps(UINT up, UINT down, UINT disabled)
	      { m_up = up; m_down = down; m_disabled = disabled; } 
	void GetBitmaps(UINT &up, UINT &down, UINT &disabled)
	      { up = m_up; down = m_down; disabled = m_disabled; }

	bool ToggleType;
	bool Toggled;
	bool DrawPushed;
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

	bool ForceDisableFocus;

// Implementation
public:
	virtual ~CImageButton();

	// Generated message map functions
protected:
    UINT m_up;
	UINT m_down;
	UINT m_disabled;
	//{{AFX_MSG(CImageButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnNotifyReflect (NMHDR *  pNotifyStruct, LRESULT*  result); 
	afx_msg BOOL OnBnClicked();
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
