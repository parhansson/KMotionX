// KMotionCNC.cpp : Defines the class behaviors for the application.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"
#include "KMotionCNC.h"
#include "Frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKMotionCNCApp

BEGIN_MESSAGE_MAP(CKMotionCNCApp, CWinApp)
	//{{AFX_MSG_MAP(CKMotionCNCApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKMotionCNCApp construction

CKMotionCNCApp::CKMotionCNCApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CKMotionCNCApp object

CKMotionCNCApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CKMotionCNCApp initialization

BOOL CKMotionCNCApp::InitInstance()
{
	//Set up date and time defaults so they're the same as system defaults
	setlocale(LC_ALL, "");
	setlocale( LC_NUMERIC, "English" );

	AfxEnableControlContainer();
	InitCommonControls();    // initialize common control library
	CWinApp::InitInstance(); // call parent class method
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


	m_pMainWnd = TheFrame = new CFrame;


	TheFrame->Create(IDD_Frame);

	TheFrame->GCodeDlg.ExternalRestore();

	int n = TheFrame->GCodeDlg.m_DialogFace;

	TheFrame->GCodeDlg.Create(IDD_KMOTIONCNC_0_ORIGINAL+n);  // put up the real main window

	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	
	// no, we are amodal to be hidden, so return true 

	return TRUE;
}

BOOL CKMotionCNCApp::OnIdle(LONG lCount) 
{
	if (TheFrame->GViewDlg.m_hWnd != NULL)
	{
		TheFrame->GViewDlg.m_GViewTools->SendMessage(WM_IDLEUPDATECMDUI);
	}

	if (TheFrame->GCodeDlg.m_hWnd != NULL)
	{
		TheFrame->GCodeDlg.m_GCodeTools->SendMessage(WM_IDLEUPDATECMDUI);
	}
	
	return CWinApp::OnIdle(lCount);
}
