// OpenglCtl.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "OpenglCtl.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenglCtl

COpenglCtl::COpenglCtl()
{
	// OpenGL
	m_hGLContext = NULL;
	m_GLPixelIndex = 0;
	
	// Mouse
	m_LeftButtonDown = FALSE;
	m_RightButtonDown = FALSE;

	// Colors
	m_ClearColorRed   = 0.0f;
	m_ClearColorGreen = 0.2f;
	m_ClearColorBlue  = 0.0f;


	m_AddWireframe = 0;
	m_Smooth = 0;
	m_PolygonOffset = -1.0f;
	m_Mode = GL_FILL;

	m_Ortho = false;

	OpenGLMutex = new CMutex(FALSE,L"OpenGL",NULL);

	InitGeometry();

	Parent=RenderCallback=NULL;

	m_cx = m_cy = 100;
}

void COpenglCtl::InitGeometry(void)
{
	m_xRotation = 0.0f;
	m_yRotation = 0.0f;
	m_zRotation = 0.0f;

	m_xTranslation = 0.0f;
	m_yTranslation = 0.0f;
	m_zTranslation = -5.0f;

	m_xScaling = 1.0f;
	m_yScaling = 1.0f;
	m_zScaling = 1.0f;

	m_SpeedRotation = 1.0f / 3.0f;
	m_SpeedTranslation_Ortho = m_SpeedTranslation_Persp = 1.0f / 50.0f;

	m_xyRotation = 0;

	m_aspect = 1.0f;

	m_MeasureValid = false;
	m_MeasureP[0] = m_MeasureP[1] = m_MeasureP[2] = 0.0f;
	m_PanD = 0.0;
}


// screen point -> world units along the view's HORIZONTAL/VERTICAL
// screen axes (pre-rotation view plane; which world axis each one is
// comes from OrthoAxisView).  Same mapping the title-bar display uses.
float COpenglCtl::ScreenToWorldX(CPoint point)
{
	return ((point.x - m_cx / ORTHO_HEIGHT) / ((float)m_cx/m_aspect) *  ORTHO_HEIGHT - m_xTranslation) / m_xScaling;
}

float COpenglCtl::ScreenToWorldY(CPoint point)
{
	return ((point.y - m_cy / ORTHO_HEIGHT) / ((float)m_cy )         * -ORTHO_HEIGHT - m_yTranslation) / m_yScaling;
}

// The draw transform is translate * Rx * Ry * Rz * scale, so screen
// horizontal/vertical correspond to the top two ROWS of R = Rx*Ry*Rz.
// When each of those rows is exactly a signed world axis (true for all
// the XY/XZ/YZ view buttons, mill and lathe alike) the screen<->world
// mapping is a trivial axis pick + sign, done here.  Any hand-rotated
// view fails the test and measure/readout simply turn off.
bool COpenglCtl::OrthoAxisView(int ax[2], float sgn[2])
{
	if (!m_Ortho) return false;

	double r = 3.14159265358979323846 / 180.0;
	double cx = cos(m_xRotation*r), sx = sin(m_xRotation*r);
	double cy = cos(m_yRotation*r), sy = sin(m_yRotation*r);
	double cz = cos(m_zRotation*r), sz = sin(m_zRotation*r);

	double R[2][3];
	R[0][0] = cy*cz;            R[0][1] = -cy*sz;           R[0][2] = sy;
	R[1][0] = cx*sz + sx*sy*cz; R[1][1] = cx*cz - sx*sy*sz; R[1][2] = -sx*cy;

	for (int i = 0; i < 2; i++)
	{
		ax[i] = -1;
		for (int j = 0; j < 3; j++)
		{
			if (fabs(fabs(R[i][j]) - 1.0) < 1e-4)
			{
				ax[i] = j;
				sgn[i] = R[i][j] > 0 ? 1.0f : -1.0f;
			}
			else if (fabs(R[i][j]) > 1e-4)
				return false;
		}
		if (ax[i] < 0) return false;
	}
	return true;
}


// Change the Ortho zoom (m_zTranslation drives the scale computed at
// draw time) while keeping the CENTER of the view fixed instead of the
// world origin: the view transform is translate-then-scale, so a scale
// change alone leaves the world origin anchored on screen.  Scaling the
// x/y translation by the same zoom ratio anchors whatever is at the
// view center instead.
//
// The zoom is MULTIPLICATIVE: each increment changes the zoom by a
// constant RATIO (exp of the increment), so it feels the same at any
// zoom level - a linear change in the zoom distance is violent when
// zoomed in and glacial when zoomed out.
void COpenglCtl::OrthoZoomAboutCenter(float dz)
{
	float Dold = m_zTranslation - m_zTranslation0_Ortho + ORTHO_HEIGHT;
	if (Dold < 1e-6f) Dold = 1e-6f;

	float Dnew = Dold * expf(dz / (float)ORTHO_HEIGHT);
	if (Dnew < 1e-6f) Dnew = 1e-6f;         // zoom-in limit (precision)
	if (Dnew > 1e6f)  Dnew = 1e6f;          // zoom-out limit

	m_zTranslation = Dnew - (float)ORTHO_HEIGHT + m_zTranslation0_Ortho;

	float f = Dold / Dnew;                  // = NewScale / OldScale
	m_xTranslation *= f;
	m_yTranslation *= f;
}


// Eye-space distance of the scene under a client point, from the depth
// buffer of the last render (nearest hit in a ~7x7 pixel window).  This
// is the shared "what is the user viewing" answer for perspective zoom
// AND pan: the surface under the cursor is the target, so several
// things at different depths resolve by pointing at the one you want.
// Over background the world origin's depth is used so the gestures
// still feel proportional.  Optionally returns the projection
// near/far planes (exactly as SetupOpenGL configures them).
// smallest (nearest) depth value in a viewport rectangle of the last
// rendered frame; 1.0 = nothing there.  Rect is clamped to the window.
float COpenglCtl::DepthRectMin(int x0, int y0, int w, int h)
{
	float dmin = 1.0f;
	HWND hWnd = GetSafeHwnd();
	if (!hWnd || m_cx <= 0 || m_cy <= 0) return dmin;

	if (x0 < 0) { w += x0; x0 = 0; }
	if (y0 < 0) { h += y0; y0 = 0; }
	if (x0 + w > m_cx) w = m_cx - x0;
	if (y0 + h > m_cy) h = m_cy - y0;
	if (w <= 0 || h <= 0) return dmin;

	float *d = new float[(size_t)w*h];
	HDC hDC = ::GetDC(hWnd);
	OpenGLMutex->Lock();
	wglMakeCurrent(hDC, m_hGLContext);
	glReadPixels(x0, y0, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, d);
	wglMakeCurrent(hDC, NULL);
	OpenGLMutex->Unlock();
	::ReleaseDC(hWnd, hDC);

	for (int i = 0; i < w*h; i++) if (d[i] < dmin) dmin = d[i];
	delete [] d;
	return dmin;
}


double COpenglCtl::PerspEyeDepthAt(CPoint point, double *zn_out, double *zf_out)
{
	double msize = max(TheFrame->GViewDlg.m_BoxX, TheFrame->GViewDlg.m_BoxY);
	msize = max(msize, TheFrame->GViewDlg.m_BoxZ);
	double zn = msize*20*1e-5, zf = msize*20;
	if (zn_out) *zn_out = zn;
	if (zf_out) *zf_out = zf;
	if (zn <= 0.0) return 0.0;

	float dmin = DepthRectMin(point.x - 3, m_cy - 1 - point.y - 3, 7, 7);

	double D;
	if (dmin < 0.9999f)
		D = zn*zf / (zf - (double)dmin*(zf - zn));  // eye distance of the hit
	else
		D = -m_zTranslation;                        // background: world origin
	if (D < 2.0*zn) D = 2.0*zn;
	if (D > zf) D = zf;
	return D;
}


// Perspective zoom: move a FRACTION of the distance to whatever is
// under the cursor per wheel notch, along the cursor ray.  Constant
// steps sized by the view box either crawl or leap right past a small
// feature; a fractional step can never pass the target, only approach
// it, and it self-scales from full-machine views down to tiny detail.
// zDelta is in wheel units (120 = one notch ~ 20%).
void COpenglCtl::PerspZoomAtCursor(float zDelta, CPoint point)   // client coords
{
	if (m_cx <= 0 || m_cy <= 0) return;

	double zn, zf;
	double D = PerspEyeDepthAt(point, &zn, &zf);
	if (D <= 0.0) return;

	// multiplicative step: ~20% of the remaining distance per notch
	double f = exp(-(double)zDelta / 120.0 * 0.22);
	double Dnew = D * f;
	if (Dnew < 2.0*zn) Dnew = 2.0*zn;               // never reach/clip the target
	if (Dnew > zf)     Dnew = zf;
	f = Dnew / D;

	// keep the point under the cursor ON the cursor: scale its view-space
	// x,y by the same ratio as its depth (45 deg vertical field of view)
	double t = tan(45.0/2.0 * 3.14159265358979323846/180.0);
	double px = (2.0*point.x/m_cx - 1.0) * t * m_aspect * D;
	double py = (1.0 - 2.0*point.y/m_cy) * t * D;
	m_xTranslation += (float)(px*(f - 1.0));
	m_yTranslation += (float)(py*(f - 1.0));
	m_zTranslation += (float)(D - Dnew);
}


// Switch Ortho <-> Perspective keeping WHAT IS ON SCREEN the same:
// same view center, same visible size.  The two modes express zoom in
// different currencies (Ortho: a scale factor derived from the z
// translation; Perspective: actual camera distance) and pan in
// different units (view units vs world units), so a raw switch from a
// deep zoom lands "inside" the scene with the wheel nearly inert.
// The depth buffer of the last render answers what is being viewed:
// geometry near the view center is preferred, else the nearest hit
// anywhere on screen, else the world origin.  The visible height at
// that depth is matched across the modes.
void COpenglCtl::SwitchProjection(bool ortho)
{
	if (ortho == m_Ortho) return;

	double t = tan(45.0/2.0 * 3.14159265358979323846/180.0);

	// perspective near/far as SetupOpenGL configures them
	double msize = max(TheFrame->GViewDlg.m_BoxX, TheFrame->GViewDlg.m_BoxY);
	msize = max(msize, TheFrame->GViewDlg.m_BoxZ);
	double zn = msize*20*1e-5, zf = msize*20;

	// depth of the viewed object from the CURRENT mode's last render
	float d = DepthRectMin(m_cx*3/8, m_cy*3/8, m_cx/4, m_cy/4);
	if (d >= 0.9999f) d = DepthRectMin(0, 0, m_cx, m_cy);

	double tx = m_xTranslation, ty = m_yTranslation;

	if (ortho)
	{
		// Perspective -> Ortho.  Eye distance of the viewed object:
		double D;
		if (d < 0.9999f && zn > 0.0)
			D = zn*zf / (zf - (double)d*(zf - zn));
		else
			D = -m_zTranslation;                 // world origin depth
		if (D < 2.0*zn) D = 2.0*zn;
		if (D > zf) D = zf;

		// Ortho scale with the same visible height (2*D*t world units
		// spanned the window), then invert the draw-time scale formula
		// Scale = Scaling0*H/(z - z0 + H) for the z that produces it
		double s = 1.0/(D*t);
		if (m_Scaling0 > 0.0 && s > 0.0)
			m_zTranslation = (float)(m_zTranslation0_Ortho - ORTHO_HEIGHT
				+ m_Scaling0*(double)ORTHO_HEIGHT/s);
		else
			m_zTranslation = m_zTranslation0_Ortho;

		// same world point at the view center (view units = s * world)
		m_xTranslation = (float)(s*tx);
		m_yTranslation = (float)(s*ty);
	}
	else
	{
		// Ortho -> Perspective.  Current effective ortho scale:
		double denom = m_zTranslation - m_zTranslation0_Ortho + ORTHO_HEIGHT;
		double s = (denom > 0.0) ? m_Scaling0*(double)ORTHO_HEIGHT/denom
		                         : m_Scaling0*10000.0;
		if (s <= 0.0) s = 1.0;

		// camera distance with the same visible height
		double D = 1.0/(s*t);
		if (D < 2.0*zn) D = 2.0*zn;
		if (D > 0.9*zf) D = 0.9*zf;              // stay inside the far plane

		// view z of the viewed object in the ortho frame
		// (glOrtho -1000..1000: depth d -> view z = 1000 - 2000*d)
		double rz = 0.0;                         // (R*w)_z of the object
		if (d < 0.9999f)
			rz = (1000.0 - 2000.0*(double)d - m_zTranslation)/s;

		m_xTranslation = (float)(tx/s);
		m_yTranslation = (float)(ty/s);
		m_zTranslation = (float)(-D - rz);
	}

	m_Ortho = ortho;
	m_PanD = 0.0;                                // stale grab depth
}


COpenglCtl::~COpenglCtl()
{
	delete OpenGLMutex;
}


BEGIN_MESSAGE_MAP(COpenglCtl, CStatic)
	//{{AFX_MSG_MAP(COpenglCtl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
	ON_WM_MOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenglCtl message handlers

void COpenglCtl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LeftButtonDown = TRUE;
	m_LeftDownPos = point;
	// perspective pan: capture the depth of what is being grabbed ONCE
	// per drag so the pan rate can't change mid-drag as the cursor
	// slides across other geometry
	if (!m_Ortho) m_PanD = PerspEyeDepthAt(point);
	SetCapture();
	SetFocus(); // set focus so scroll wheel works
	MouseTimer.Start();
	CStatic::OnLButtonDown(nFlags, point);
}

void COpenglCtl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	
	CStatic::OnLButtonUp(nFlags, point);
}

void COpenglCtl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = TRUE;
	m_RightDownPos = point;
	SetCapture();
	SetFocus(); // set focus so scroll wheel works
	MouseTimer.Start();
	
	CStatic::OnRButtonDown(nFlags, point);
}

void COpenglCtl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	
	CStatic::OnRButtonUp(nFlags, point);
}

// inches -> user units for a WORLD axis (X has its own conversion:
// lathe diameter mode)
static float GViewUserUnits(int axis, float v)
{
	return axis == 0 ? TheFrame->GCodeDlg.Interpreter->InchesToUserUnitsX(v)
	                 : TheFrame->GCodeDlg.Interpreter->InchesToUserUnits(v);
}

void COpenglCtl::OnMouseMove(UINT nFlags, CPoint point)
{
	float SpeedTranslation, TimeFactor=1.0f;

	// After 2 seconds of holding the same button reduce the speed
	if (MouseTimer.Elapsed_Seconds() > 2.0) TimeFactor=0.05f;


	if (m_Ortho)
		SpeedTranslation = m_SpeedTranslation_Ortho;
	else
		SpeedTranslation = m_SpeedTranslation_Persp;

	// Both : rotation
	if(m_LeftButtonDown && m_RightButtonDown)
	{
		if(!m_xyRotation)
		{
			m_yRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation * TimeFactor;
			m_xRotation -= (float)(m_LeftDownPos.y - point.y) * m_SpeedRotation * TimeFactor;
		}
		else
		{
			m_zRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation * TimeFactor;
		}
		m_LeftDownPos = point;
		m_RightDownPos = point;
		InvalidateRect(NULL,FALSE);
	}

	else

	// Left : x / y translation
	if(m_LeftButtonDown)
	{
		if (!m_Ortho && m_PanD > 0.0 && m_cy > 0)
		{
			// perspective 1:1 grab pan: world-per-pixel at the depth
			// captured at button-down, so the scene under the cursor
			// TRACKS the cursor at any zoom.  No TimeFactor - a true
			// grab must not change speed mid-drag.
			float wpp = (float)(2.0*m_PanD
				*tan(45.0/2.0*3.14159265358979323846/180.0)/m_cy);
			m_xTranslation -= (float)(m_LeftDownPos.x - point.x) * wpp;
			m_yTranslation += (float)(m_LeftDownPos.y - point.y) * wpp;
		}
		else
		{
			m_xTranslation -= (float)(m_LeftDownPos.x - point.x) * SpeedTranslation * TimeFactor;
			m_yTranslation += (float)(m_LeftDownPos.y - point.y) * SpeedTranslation * TimeFactor;
		}
		m_LeftDownPos = point;
		InvalidateRect(NULL,FALSE);
	}

	else

	// Right : z translation
	if(m_RightButtonDown)
	{
		if (m_Ortho)
			OrthoZoomAboutCenter((float)(m_RightDownPos.y - point.y) * SpeedTranslation * TimeFactor);
		else
			// proportional zoom toward the cursor, ~30 drag pixels per
			// wheel-notch equivalent (see PerspZoomAtCursor)
			PerspZoomAtCursor((float)(point.y - m_RightDownPos.y) * 4.0f * TimeFactor, point);

		m_RightDownPos = point;
		InvalidateRect(NULL,FALSE);
	}

	CString r;		// the readout, without the window title prefix
	int ax[2]; float sgn[2];
	if (OrthoAxisView(ax, sgn))
	{
		static const wchar_t AxName[3] = { L'x', L'y', L'z' };

		// cursor position along the two world axes on screen
		float h = GViewUserUnits(ax[0], sgn[0]*ScreenToWorldX(point));
		float v = GViewUserUnits(ax[1], sgn[1]*ScreenToWorldY(point));

		if (m_MeasureValid)
		{
			// measure mode: show the reference, delta, and diagonal
			float hr = GViewUserUnits(ax[0], m_MeasureP[ax[0]]);
			float vr = GViewUserUnits(ax[1], m_MeasureP[ax[1]]);
			float dh = h - hr, dv = v - vr;
			r.Format(L"%c%c = %.4f %.4f  ref = %.4f %.4f  del = %.4f %.4f  len = %.4f",
				AxName[ax[0]], AxName[ax[1]],
				h, v, hr, vr, dh, dv, sqrtf(dh*dh + dv*dv));
		}
		else
			r.Format(L"%c%c = %.4f %.4f",
				AxName[ax[0]], AxName[ax[1]], h, v);
	}
	CString s = r.IsEmpty() ? CString(L"G Code Viewer") : L"G Code Viewer  " + r;
	if (TheFrame->GViewDlg.m_hWnd) TheFrame->GViewDlg.SetWindowText(s);

	// screen-script text controls whose text is "$GViewPos$" show the same readout -
	// the only place it is visible when the viewer is embedded in the screen
	POSITION pos = CImageButton::ImageButtons.GetHeadPosition();
	while (pos)
	{
		LPCImageButton b = CImageButton::ImageButtons.GetNext(pos);
		if (b->m_GViewPos && b->m_hWnd && wcscmp(b->m_szText, r) != 0) b->SetText(r);
	}

	
	CStatic::OnMouseMove(nFlags, point);
}

void COpenglCtl::OpenGLInit()
{
	HWND hWnd = GetSafeHwnd();

	if (hWnd == NULL) return; // not yet created?

	HDC hDC = ::GetDC(hWnd);

	TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Lock();

	if (SetWindowPixelFormat(hDC) == FALSE)
		return;

	if (CreateViewGLContext(hDC) == FALSE)
		return;


	glClearDepth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// Perspective
	CRect rect;
	GetClientRect(&rect);
	double aspect = (rect.Height() == 0) ? rect.Width() : (double)rect.Width() / (double)rect.Height();
	gluPerspective(45, aspect, 0.01, 1000.0);


	GLfloat position[] = { 1000.0f, 1000.0f, -1000.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, position);



	// Default : blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

	// Modulate : texture lighting
	glEnable(GL_TEXTURE_2D);
	TRACE("Texture parameters...\n");

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	SetupOpenGL();
	TheFrame->GCodeDlg.ActualGViewParent->m_view.OpenGLMutex->Unlock();
}

void COpenglCtl::PreSubclassWindow() 
{
	OpenGLInit();
	
	SetTimer(1,150,NULL);

	CStatic::PreSubclassWindow();
}


//********************************************
// SetWindowPixelFormat
//********************************************
BOOL COpenglCtl::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;

	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE;
	
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	m_GLPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);
	if(m_GLPixelIndex == 0) // Choose default
	{
		m_GLPixelIndex = 1;
		if(DescribePixelFormat(hDC,m_GLPixelIndex,
			sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
			return FALSE;
	}
	
	if(!SetPixelFormat(hDC,m_GLPixelIndex,&pixelDesc))
		return FALSE;
	
	return TRUE;
}



//********************************************
// CreateViewGLContext
// Create an OpenGL rendering context
//********************************************
BOOL COpenglCtl::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);
	
	if(m_hGLContext==NULL)
		return FALSE;
	
	if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
		return FALSE;
	
	return TRUE;
}


void COpenglCtl::OnPaint() 
{
	static CHiResTimer Timer;
	static bool FirstTime=true;
	static bool DisplayError = true;
	static double LastLockTime,LastUnlockTime;
	double drawtime;

	CPaintDC dc(this); // device context for painting

	if (FirstTime)
	{
		// (glGetString returns NULL without a current context)
		const char *pVer = (const char *)glGetString(GL_VERSION);
		const char *pVendor = (const char *)glGetString(GL_VENDOR);
		const char *pRenderer = (const char *)glGetString(GL_RENDERER);
		CStringA Ver = pVer ? pVer : "(none)";
		CStringA Vendor = pVendor ? pVendor : "(none)";
		CStringA Renderer = pRenderer ? pRenderer : "(none)";
		int v0, v1, v2, r;

		r = sscanf(Ver.GetBuffer(), "%d.%d.%d", &v0, &v1, &v2);

		if (r < 2 || v0*10+v1 < 33)
		{
			if (DisplayError)
			{
				DisplayError = false;
				// OpenGL version is too low, warn user.  The vendor/renderer tell
				// a missing graphics driver (Windows' built in "GDI Generic"
				// software OpenGL 1.1) from hardware that is really too old.
				CString msg, hint;

				if (Vendor.Find("Microsoft") >= 0 || Renderer.Find("GDI Generic") >= 0)
					hint = L"Windows is using its built in software OpenGL (version 1.1): no graphics driver is installed "
						   L"for the display adapter, or this is a Remote Desktop session or a virtual machine without 3D "
						   L"acceleration.  Installing the graphics card maker's driver normally fixes this.";
				else
					hint = L"The graphics adapter or its driver does not support OpenGL 3.3.  Update the graphics driver from "
						   L"the graphics card maker.  If the hardware itself is too old a newer graphics card (or the Mesa3D "
						   L"software renderer) is needed.";

				msg.Format(L"OpenGL version too low.  3.3 or higher is required.\n\n"
						   L"Version:   %s\nVendor:    %s\nRenderer:  %s\n\n%s",
						   (CStringW)Ver, (CStringW)Vendor, (CStringW)Renderer, hint);
				AfxMessageBox(msg);
			}
			return;
		}


		Timer.Start();
		FirstTime=false;
	}
	else
	{
		drawtime = LastUnlockTime - LastLockTime;

		if (drawtime > 10.0) drawtime = 10.0;
		// never keep the mutex locked for more than 50% of the time
		if (Timer.Elapsed_Seconds() - LastUnlockTime < drawtime) 
			return;
	}

	LastLockTime = Timer.Elapsed_Seconds();
	OpenGLMutex->Lock();
	
	wglMakeCurrent(dc,m_hGLContext);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	
	if (m_Ortho)
	{
		// Position / rotation / scale
		glTranslated(m_xTranslation, m_yTranslation, m_zTranslation);
		glRotatef(m_xRotation, 1.0, 0.0, 0.0);
		glRotatef(m_yRotation, 0.0, 1.0, 0.0);
		glRotatef(m_zRotation, 0.0, 0.0, 1.0);

		if (m_zTranslation - m_zTranslation0_Ortho + ORTHO_HEIGHT > 0.0)
			m_xScaling = m_yScaling = m_zScaling = m_Scaling0 * ORTHO_HEIGHT / (m_zTranslation - m_zTranslation0_Ortho + ORTHO_HEIGHT);
		else
			m_xScaling = m_yScaling = m_zScaling = m_Scaling0 * 10000.0;

		glScalef(m_xScaling, m_yScaling, m_zScaling);
	}
	else
	{
		// Position / rotation / scale
		glTranslated(m_xTranslation, m_yTranslation, m_zTranslation);
		glRotatef(m_xRotation, 1.0, 0.0, 0.0);
		glRotatef(m_yRotation, 0.0, 1.0, 0.0);
		glRotatef(m_zRotation, 0.0, 0.0, 1.0);
	}

	// Start rendering...
	RenderScene();

	// measure-mode reference marker (axis-aligned Ortho views): red X
	// of fixed ~8 pixel size at the remembered position, drawn in the
	// plane of the two ON-SCREEN world axes so it faces the camera in
	// the XY, XZ and YZ views alike (drawn inside the view transform so
	// it tracks pan/zoom; size divided by the scale so it stays
	// constant on screen)
	int vax[2]; float vsgn[2];
	if (m_MeasureValid && m_cx > 0 && m_xScaling > 0.0f && OrthoAxisView(vax, vsgn))
	{
		float sz = 8.0f * (float)ORTHO_HEIGHT * m_aspect / (m_cx * m_xScaling);
		float eh[3] = {0,0,0}, ev[3] = {0,0,0};   // in-plane world basis
		eh[vax[0]] = vsgn[0];
		ev[vax[1]] = vsgn[1];
		const float *P = m_MeasureP;
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glLineWidth(2.0f);
		glColor3f(1.0f, 0.2f, 0.2f);
		glBegin(GL_LINES);
		glVertex3f(P[0]-sz*(eh[0]+ev[0]), P[1]-sz*(eh[1]+ev[1]), P[2]-sz*(eh[2]+ev[2]));
		glVertex3f(P[0]+sz*(eh[0]+ev[0]), P[1]+sz*(eh[1]+ev[1]), P[2]+sz*(eh[2]+ev[2]));
		glVertex3f(P[0]-sz*(eh[0]-ev[0]), P[1]-sz*(eh[1]-ev[1]), P[2]-sz*(eh[2]-ev[2]));
		glVertex3f(P[0]+sz*(eh[0]-ev[0]), P[1]+sz*(eh[1]-ev[1]), P[2]+sz*(eh[2]-ev[2]));
		glEnd();
		glPopAttrib();
	}

	glPopMatrix();

	// Double buffer
	SwapBuffers(dc);
	glFlush();

	OpenGLMutex->Unlock();
	LastUnlockTime = Timer.Elapsed_Seconds();

	// Release
	wglMakeCurrent(dc,NULL);

	LastUnlockTime = Timer.Elapsed_Seconds();  // record last time screen was finished being updated
	HiResTimer.Start();  // record last time screen was finished being updated
}

void COpenglCtl::OnSize(UINT nType, int cx, int cy) 
{
	SetupOpenGL();

	CStatic::OnSize(nType, cx, cy);
}


void COpenglCtl::OnMove(int x, int y)
{
	SetupOpenGL();

	CStatic::OnMove(x, y);
}

void COpenglCtl::SetupOpenGL()
{
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	OpenGLMutex->Lock();
	// Activate view, set active OpenGL rendering context
	wglMakeCurrent(hDC,m_hGLContext);

	// background from the Screen Script if specified
	COLORREF Back = CLR_DEFAULT;
	if (TheFrame->GCodeDlg.m_DialogFaceInUse == CUSTOM_DLG_FACE)
		Back = TheFrame->GCodeDlg.Screen.ViewBackColor;

	if (Back != CLR_DEFAULT)
		glClearColor(GetRValue(Back) / 255.0f, GetGValue(Back) / 255.0f, GetBValue(Back) / 255.0f, 1.0f);
	else
		glClearColor(m_ClearColorRed, m_ClearColorGreen, m_ClearColorBlue, 1.0f);

	RECT rect;
	GetClientRect(&rect);

	m_cx = rect.right;
	m_cy = rect.bottom;

	// Set OpenGL perspective, viewport and mode

	m_aspect = (m_cy == 0) ? m_cx : (double)m_cx/(double)m_cy;
	
	glViewport(0,0,m_cx,m_cy);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (m_Ortho)
		glOrtho(-1 * m_aspect, 1 * m_aspect, -1, 1, -1000.0, 1000.0);
	else
	{
		// find maximum dimension of "box"
		double msize = max(TheFrame->GViewDlg.m_BoxX, TheFrame->GViewDlg.m_BoxY);
		msize = max(msize, TheFrame->GViewDlg.m_BoxZ);
		gluPerspective(45, m_aspect, msize*20*1e-5, msize*20); // configure near and far Z clip planes
	}

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glDrawBuffer(GL_BACK);

	// Release
	::ReleaseDC(hWnd,hDC);
	OpenGLMutex->Unlock();
}



void COpenglCtl::OnDestroy() 
{
	OpenGLMutex->Lock();
	if(wglGetCurrentContext() != NULL)
		wglMakeCurrent(NULL,NULL);
	
	if(m_hGLContext != NULL)
	{
		wglDeleteContext(m_hGLContext);
		m_hGLContext = NULL;
//tktk		glDeleteProgram(CMesh3d::shaderProgram);
//tktk		glDeleteProgram(CPath3d::shaderProgram);
//tktk		CMesh3d::shaderProgram = 0;
//tktk		CPath3d::shaderProgram = 0;
	}
	OpenGLMutex->Unlock();

	CStatic::OnDestroy();
}

BOOL COpenglCtl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void COpenglCtl::RenderScene()
{
	// Main drawing
	glPolygonMode(GL_FRONT,m_Mode);

	if (RenderCallback) RenderCallback(Parent);

	m_SceneGraph.glDraw();
}


void COpenglCtl::OnTimer(UINT_PTR nIDEvent) 
{
	if (HiResTimer.nSplit < 1 || HiResTimer.Elapsed_Seconds() > .15)
	{
		InvalidateRect(NULL);
	}
	CStatic::OnTimer(nIDEvent);
}

BOOL COpenglCtl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_Ortho)
		OrthoZoomAboutCenter((float)(zDelta) * -m_SpeedTranslation_Ortho);
	else
	{
		ScreenToClient(&pt);           // wheel points arrive in screen coords
		PerspZoomAtCursor((float)zDelta, pt);
	}


	InvalidateRect(NULL,FALSE);

	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}


// Measure mode: in any axis-aligned Ortho view (XY, XZ, YZ - mill or
// lathe) a double-click remembers the position as the reference (red X
// marker); double-clicking on/near the marker clears it.  The title
// bar then shows the cursor position, reference, delta and diagonal
// length (see OnMouseMove).  The marker's out-of-view-plane coordinate
// is zero; switching views measures against its projection there.
void COpenglCtl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int ax[2]; float sgn[2];
	if (m_cx > 0 && m_xScaling > 0.0f && OrthoAxisView(ax, sgn))
	{
		float h = sgn[0]*ScreenToWorldX(point);   // along world axis ax[0]
		float v = sgn[1]*ScreenToWorldY(point);   // along world axis ax[1]

		// world size of ~6 pixels for the clear-hit test (in-plane)
		float wpp = (float)ORTHO_HEIGHT * m_aspect / (m_cx * m_xScaling);
		if (m_MeasureValid &&
			fabsf(h - m_MeasureP[ax[0]]) < 6.0f*wpp &&
			fabsf(v - m_MeasureP[ax[1]]) < 6.0f*wpp)
		{
			m_MeasureValid = false;
		}
		else
		{
			m_MeasureP[0] = m_MeasureP[1] = m_MeasureP[2] = 0.0f;
			m_MeasureP[ax[0]] = h;
			m_MeasureP[ax[1]] = v;
			m_MeasureValid = true;
		}

		InvalidateRect(NULL,FALSE);
		OnMouseMove(nFlags, point);   // refresh the title immediately
	}

	CStatic::OnLButtonDblClk(nFlags, point);
}

