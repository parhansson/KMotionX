// OpenglCtl.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/
#pragma once
#include "HiResTimer.h"


typedef void RENDERCALLBACK(void *);

#define ORTHO_CAMERA_Z -200.0  // Distance Camera is back in ortho mode
#define ORTHO_HEIGHT 2.0 // OpenGL Height of View (+/- 1 units)



/////////////////////////////////////////////////////////////////////////////
// COpenglCtl window

class COpenglCtl : public CStatic
{
// Construction
public:
	COpenglCtl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenglCtl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	CHiResTimer HiResTimer;
	CHiResTimer MouseTimer;

	CMutex *OpenGLMutex;
	virtual ~COpenglCtl();
	// OpenGL specific
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL CreateViewGLContext(HDC hDC);
	HGLRC m_hGLContext;
	int m_GLPixelIndex;
	void SetClearColor(void) {	glClearColor(m_ClearColorRed,m_ClearColorGreen,m_ClearColorBlue,1.0f); }
	unsigned char *SnapClient(CSize *pSize);

	// Mouse 
	BOOL m_LeftButtonDown;
	BOOL m_RightButtonDown;
	CPoint m_LeftDownPos;
	CPoint m_RightDownPos;
	HCURSOR m_CursorRotation;

	bool m_Ortho;

	// Position, rotation ,scaling
	void InitGeometry(void);

	float m_xRotation;
	float m_yRotation;
	float m_zRotation;
	BOOL m_xyRotation;

	float m_xTranslation;
	float m_yTranslation;
	float m_zTranslation;
	float m_zTranslation0_Ortho;  // nominal distance where Ortho should be at Scale0
	float m_zTranslation0_Persp;  // nominal distance for persp

	float m_xScaling;
	float m_yScaling;
	float m_zScaling;
	float m_Scaling0;  // nominal distance where Ortho should be at Scale=1.0

	float m_aspect;
	int m_cx, m_cy;

	float m_SpeedTranslation_Ortho;
	float m_SpeedTranslation_Persp;
	float m_SpeedRotation;

	// Colors
	float m_ClearColorRed;
	float m_ClearColorGreen;
	float m_ClearColorBlue;

	// Animation
	float m_StepRotationX;
	float m_StepRotationY;
	float m_StepRotationZ;


	// The scene
	CSceneGraph3d m_SceneGraph;

	// Options
	BOOL m_AddWireframe;
	BOOL m_Smooth;
	float m_PolygonOffset;
	GLenum m_Mode;

	void RenderScene();

	void *Parent;
	RENDERCALLBACK *RenderCallback;
	void SetupOpenGL();


	// Generated message map functions
protected:
	//{{AFX_MSG(COpenglCtl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMove(int x, int y);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

