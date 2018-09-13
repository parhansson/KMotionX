// MotionButton.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/
#pragma once
#include "ImageButton.h"

class CKMotionCNCDlg;
class CMotionButton;
typedef CMotionButton *LPCMotionButton;


/////////////////////////////////////////////////////////////////////////////
// CMotionButton window

class CMotionButton : public CImageButton
{
// Construction
public:
	CMotionButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMotionButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Init(CKMotionCNCDlg *Dlg, int axis, int dir, double *mag, bool StepMode, int HotKey);
	virtual ~CMotionButton();
	double m_RawVel;
	bool m_SimulateMotion;
	bool m_Moving;
	bool m_StepMode;
	double *m_mag;
	int m_axis;
	int m_dir;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMotionButton)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd*);

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	double m_SimulateDelta;
	double *m_pSimulatePos;

	bool m_HasMouseCapture;
	CKMotionCNCDlg *Dlg;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);


public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	int HandleButtonDown(void);
	void HandleButtonUp(void);
	void PutRawV(double *v);

	typedef CMotionButton *LPCMotionButton;

	int DoExecute();
	int LaunchExecution();
	int m_exitcode;
	double stepx, stepy, stepz, stepa, stepb, stepc, stepu, stepv;

    static CList <LPCMotionButton,LPCMotionButton> AxisGroup;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
