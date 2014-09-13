// IHelp.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "EXDISP.H"
#include "IHelp.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIHelp dialog


CIHelp::CIHelp(CWnd* pParent /*=NULL*/)
	: CDlgX(pParent)
{
	//{{AFX_DATA_INIT(CIHelp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIHelp::DoDataExchange(CDataExchange* pDX)
{

	CDlgX::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIHelp)
	DDX_Control(pDX, IDC_EXPLORER1, m_Web);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIHelp, CDlgX)
	//{{AFX_MSG_MAP(CIHelp)
	ON_BN_CLICKED(IDC_Back, OnBack)
	ON_BN_CLICKED(IDC_Forward, OnForward)
	ON_BN_CLICKED(IDC_PrintBrowser, OnPrintBrowser)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_Contents, OnContents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIHelp message handlers


void CIHelp::OnBack() 
{
	m_Web.GoBack();
}

void CIHelp::OnForward() 
{
	m_Web.GoForward();
}

void CIHelp::OnPrintBrowser() 
{
	LPDISPATCH lpDispatch = NULL;
	LPOLECOMMANDTARGET lpOleCommandTarget = NULL;

	lpDispatch = m_Web.GetDocument();
	ASSERT(lpDispatch);

	lpDispatch->QueryInterface(IID_IOleCommandTarget, (void**)&lpOleCommandTarget);
	ASSERT(lpOleCommandTarget);

	lpDispatch->Release();

	// Print contents of WebBrowser control.
	lpOleCommandTarget->Exec(NULL, OLECMDID_PRINT, 0, NULL,NULL);
	lpOleCommandTarget->Release();
}

BEGIN_EVENTSINK_MAP(CIHelp, CDlgX)
    //{{AFX_EVENTSINK_MAP(CIHelp)
	ON_EVENT(CIHelp, IDC_EXPLORER1, 105 /* CommandStateChange */, OnCommandStateChangeExplorer1, VTS_I4 VTS_BOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CIHelp::OnCommandStateChangeExplorer1(long Command, BOOL Enable) 
{
	switch(Command)
    {
		case CSC_NAVIGATEFORWARD:
           // m_ctlForward is a CButton type
           m_Forward.EnableWindow(Enable);
           break;

		case CSC_NAVIGATEBACK:
           m_Back.EnableWindow(Enable);
           break;

       default:
           break;
      }   
}

void CIHelp::OnPaint() 
{
	RECT rect;
	
	if (InitDialogComplete)  // save the dialog window position
	{
        GetClientRect(&rect);
		m_Web.MoveWindow( 0, 30, rect.right, rect.bottom-30);
		m_Web.UpdateWindow();
	}
	CDlgX::OnPaint();
}



int CIHelp::Show(CString s)
{
    COleVariant noArg;

	CString Path = TheFrame->MainPath + "\\Help\\" + s;

	if (m_hWnd)
		BringWindowToTop();
	else
		Create(IDD_Help,TheFrame);

	
	// Set To the desired URL.

    m_Web.Navigate(Path,&noArg,&noArg,&noArg,&noArg);


	InvalidateRect(NULL,FALSE);
	return 0;
}

void CIHelp::OnContents() 
{
	Show(Contents);
}

BOOL CIHelp::OnInitDialog() 
{
	CDlgX::OnInitDialog();
	
	m_BUTTON_Close.AutoLoad(IDOK, this);
	m_BUTTON_Print.AutoLoad(IDC_PrintBrowser, this);
	m_Forward.AutoLoad(IDC_Forward, this);
	m_Contents.AutoLoad(IDC_Contents, this);
	m_Back.AutoLoad(IDC_Back, this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
