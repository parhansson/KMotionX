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

	OpenGLMutex = new CMutex(FALSE,"OpenGL",NULL);

	InitGeometry();
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
	m_SpeedTranslation = 1.0f / 50.0f;

	m_xyRotation = 0;
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenglCtl message handlers

void COpenglCtl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_LeftButtonDown = TRUE;
	m_LeftDownPos = point;
	SetCapture();
	
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
	
	CStatic::OnRButtonDown(nFlags, point);
}

void COpenglCtl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	
	CStatic::OnRButtonUp(nFlags, point);
}

void COpenglCtl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Both : rotation
	if(m_LeftButtonDown && m_RightButtonDown)
	{
		if(!m_xyRotation)
		{
			m_yRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation;
			m_xRotation -= (float)(m_LeftDownPos.y - point.y) * m_SpeedRotation;
		}
		else
		{
			m_zRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation;
//			m_xRotation -= (float)(m_LeftDownPos.y - point.y) * m_SpeedRotation;
		}
		m_LeftDownPos = point;
		m_RightDownPos = point;
		InvalidateRect(NULL,FALSE);
	}

	else

	// Left : x / y translation
	if(m_LeftButtonDown)
	{
		m_xTranslation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedTranslation;
		m_yTranslation += (float)(m_LeftDownPos.y - point.y) * m_SpeedTranslation;
		m_LeftDownPos = point;
		InvalidateRect(NULL,FALSE);
	}

	else

	// Right : z translation
	if(m_RightButtonDown)
	{
		m_zTranslation += (float)(m_RightDownPos.y - point.y) * m_SpeedTranslation;
		m_RightDownPos = point;
		InvalidateRect(NULL,FALSE);
	}


	
	CStatic::OnMouseMove(nFlags, point);
}

void COpenglCtl::PreSubclassWindow() 
{
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	TheFrame->GViewDlg.m_view.OpenGLMutex->Lock();

	if(SetWindowPixelFormat(hDC)==FALSE)
		return;
	
	if(CreateViewGLContext(hDC)==FALSE)
		return;
	
	//::ReleaseDC(hWnd,hDC);
	
	// Default mode
	glPolygonMode(GL_FRONT,GL_FILL);
	glPolygonMode(GL_BACK,GL_FILL);
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	
	// Lights properties
	float	ambientProperties[]  = {0.7f, 0.7f, 0.7f, 1.0f};
	float	diffuseProperties[]  = {0.8f, 0.8f, 0.8f, 1.0f};
	float	specularProperties[] = {1.0f, 1.0f, 1.0f, 1.0f};
	
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientProperties);
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
	glLightfv( GL_LIGHT0, GL_SPECULAR, specularProperties);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
	
	glClearColor(m_ClearColorRed,m_ClearColorGreen,m_ClearColorBlue,1.0f);
	glClearDepth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	// Perspective
	CRect rect;
	GetClientRect(&rect);
	double aspect = (rect.Height() == 0) ? rect.Width() : (double)rect.Width()/(double)rect.Height();
	gluPerspective(45,aspect,0.01,1000.0);
	

	float position[] = { 1000.0f, 1000.0f, -1000.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, position);


	// Default : lighting
	glEnable(GL_LIGHT0);


	
	// Default : blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	// Default : material
	float	MatAmbient[]	= {0.0f, 0.33f, 0.50f, 1.0f};
	float	MatDiffuse[]	= {0.5f, 0.5f, 0.5f, 1.0f};
	float	MatSpecular[]	= {0.1f, 0.1f, 0.1f, 1.0f};
	float	MatShininess[]  = { 84 };
	float	MatEmission[]	= {0.0f, 0.0f, 0.0f, 1.0f};
	
	// Back : green
	float MatAmbientBack[]  = {0.0f, 0.5f, 0.0f, 1.0f};
	
	glEnable(GL_DEPTH_TEST);
	
	// Modulate : texture lighting
	glEnable(GL_TEXTURE_2D);
	TRACE("Texture parameters...\n");
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	
	TheFrame->GViewDlg.m_view.OpenGLMutex->Unlock();

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
	static double LastLockTime,LastUnlockTime;
	double drawtime;

	CPaintDC dc(this); // device context for painting

	if (FirstTime)
	{
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
	
	// Position / translation / scale
	glTranslated(m_xTranslation,m_yTranslation,m_zTranslation);
	glRotatef(m_xRotation, 1.0, 0.0, 0.0);
	glRotatef(m_yRotation, 0.0, 1.0, 0.0);
	glRotatef(m_zRotation, 0.0, 0.0, 1.0);
	glScalef(m_xScaling,m_yScaling,m_zScaling);

	// Start rendering...
	RenderScene();
	
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
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	OpenGLMutex->Lock();
	// Activate view, set active OpenGL rendering context
	wglMakeCurrent(hDC,m_hGLContext);

	// Set OpenGL perspective, viewport and mode

	double aspect = (cy == 0) ? cx : (double)cx/(double)cy;
	
	glViewport(0,0,cx,cy);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45,aspect,0.01,1000.0);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glDrawBuffer(GL_BACK);

	// Release
	::ReleaseDC(hWnd,hDC);
	OpenGLMutex->Unlock();

	CStatic::OnSize(nType, cx, cy);
}


void COpenglCtl::OnMove(int x, int y)
{
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	OpenGLMutex->Lock();
	// Activate view, set active OpenGL rendering context
	wglMakeCurrent(hDC,m_hGLContext);

	RECT rect;

	GetClientRect(&rect);

	int cx = rect.right;
	int cy = rect.bottom;

	// Set OpenGL perspective, viewport and mode

	double aspect = (cy == 0) ? cx : (double)cx/(double)cy;
	
	glViewport(0,0,cx,cy);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45,aspect,0.01,1000.0);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glDrawBuffer(GL_BACK);

	// Release
	::ReleaseDC(hWnd,hDC);
	OpenGLMutex->Unlock();

	CStatic::OnMove(x, y);
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

	TheFrame->GViewDlg.ChangeToolPosition();

	m_SceneGraph.glDraw();
}


void COpenglCtl::OnTimer(UINT nIDEvent) 
{
	if (HiResTimer.nSplit < 1 || HiResTimer.Elapsed_Seconds() > .15)
	{
		InvalidateRect(NULL);
	}
	CStatic::OnTimer(nIDEvent);
}

BOOL COpenglCtl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_zTranslation += (float)(zDelta) * m_SpeedTranslation;
	InvalidateRect(NULL,FALSE);

	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}

