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
	void OrthoZoomAboutCenter(float dz);
	void PerspZoomAtCursor(float zDelta, CPoint point);
	double PerspEyeDepthAt(CPoint point, double *zn_out = NULL, double *zf_out = NULL);
	float DepthRectMin(int x0, int y0, int w, int h);
	void SwitchProjection(bool ortho);
	double m_PanD;   // eye depth captured at left-button-down (perspective
	                 // 1:1 grab pan; see PerspEyeDepthAt)

	// Measure mode (any axis-aligned Ortho view: XY, XZ, YZ, mill or
	// lathe): double-click remembers a reference position (marked with
	// a red X); the title then shows the cursor position, the
	// reference, the delta to the cursor, and the diagonal length.
	// Double-clicking on (or near) the marker clears it.
	bool  m_MeasureValid;
	float m_MeasureP[3];                 // world coords (inches)

	// True when the current Ortho view maps both screen axes onto
	// (signed) world axes - all the XY/XZ/YZ view buttons do, mill and
	// lathe alike.  ax[0]/sgn[0] = world axis index (0=X 1=Y 2=Z) and
	// sign on screen horizontal; ax[1]/sgn[1] same for screen vertical.
	bool  OrthoAxisView(int ax[2], float sgn[2]);
	float ScreenToWorldX(CPoint point);
	float ScreenToWorldY(CPoint point);

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
	void OpenGLInit();
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
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

