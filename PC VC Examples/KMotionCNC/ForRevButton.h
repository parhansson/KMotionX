// ForRevButton.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/
#pragma once
#include "ImageButton.h"

class CKMotionCNCDlg;

/////////////////////////////////////////////////////////////////////////////
// CForRevButton window

class CForRevButton : public CImageButton
{
// Construction
public:
	CForRevButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CForRevButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Init(CKMotionCNCDlg *Dlg, int dir, double mag);
	virtual ~CForRevButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CForRevButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	bool m_Moving;
	CKMotionCNCDlg *Dlg;
	int m_dir;
	double m_mag;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void HandleButtonDown(void);
	void HandleButtonUp(void);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
