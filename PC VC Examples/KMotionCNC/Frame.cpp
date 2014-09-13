// Frame.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"
#include "kmotioncnc.h"
#include "Frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFrame *TheFrame;


/////////////////////////////////////////////////////////////////////////////
// CFrame dialog


CFrame::CFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CFrame::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	HelpDlg.Contents = "KMotionCNC\\KMotionCNC.htm"; 

	KMotionDLL = new CKMotionDLL(0);
	CCoordMotion *CM = new CCoordMotion(KMotionDLL);
    GCodeDlg.Interpreter = new CGCodeInterpreter(CM);
}

CFrame::~CFrame()
{
	delete GCodeDlg.Interpreter->CoordMotion;
	delete GCodeDlg.Interpreter;
	delete KMotionDLL;
}


void CFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrame)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrame, CDialog)
	//{{AFX_MSG_MAP(CFrame)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()






/////////////////////////////////////////////////////////////////////////////
// CFrame message handlers


void CFrame::OnClose() 
{
	DestroyWindow ();
	delete this;
}


void CFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	
	// TODO: Add your message handler code here
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
		m_hIcon = AfxGetApp()->LoadIcon(IDI_KMotionCNC16);
		SetIcon(m_hIcon,FALSE);
		m_hIcon = AfxGetApp()->LoadIcon(IDI_KMotionCNC32);
		SetIcon(m_hIcon,TRUE);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}

BOOL CFrame::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_hIcon = AfxGetApp()->LoadIcon(IDI_KMotionCNC16);
	SetIcon(m_hIcon,FALSE);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_KMotionCNC32);
	SetIcon(m_hIcon,TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
