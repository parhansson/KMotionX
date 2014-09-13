// Display.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#if !defined(AFX_DISPLAY_H__949562A3_C6C0_41E9_9F37_3195B3FCAC86__INCLUDED_)
#define AFX_DISPLAY_H__949562A3_C6C0_41E9_9F37_3195B3FCAC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDisplay window

class CDisplay : public CStatic
{
// Construction
public:
	CDisplay();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplay)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTextColor(int color);
	void SetText(CString OutText);
	virtual ~CDisplay();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDisplay)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_TextColor;
	CString m_OutText;
	void RefreshPlot();
	void DrawText();
	void DrawBackGrnd();
	RECT m_rect;
	CDC *m_MemDC;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAY_H__949562A3_C6C0_41E9_9F37_3195B3FCAC86__INCLUDED_)
