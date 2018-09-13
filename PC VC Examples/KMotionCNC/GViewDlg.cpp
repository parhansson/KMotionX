// GViewDlg.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "GViewDlg.h"
#include "GViewerSetup.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGViewDlg dialog


CGViewDlg::CGViewDlg(CWnd* pParent /*=NULL*/)
	: CDlgX(pParent)
{
	m_GViewTools=NULL;
	m_ShowAxis=false;
	m_ShowTool=false;
	m_ShowBox=false;
	m_Ortho = FALSE;

	m_BoxX=120;
	m_BoxY=120;
	m_BoxZ=120;

	m_BoxOffsetX=0;
	m_BoxOffsetY=0;
	m_BoxOffsetZ=0;


	m_ToolShapeFile = "Tool.wrl";
	m_AxisSize = 1.0;
	m_ToolSize = 5.0;
	m_ToolOffX = 0.0;
	m_ToolOffY = 0.0;
	m_ToolOffZ = 1.25;

	m_IncludeA=FALSE;
	m_IncludeB=FALSE;
	m_IncludeC=FALSE;
	m_IncludeToolAngles=FALSE;


	//{{AFX_DATA_INIT(CGViewDlg)
	//}}AFX_DATA_INIT
}


CGViewDlg::~CGViewDlg()
{
	if (m_GViewTools)
		delete m_GViewTools;
}


void CGViewDlg::OnClose() 
{
	CDlgX::OnClose();
	delete m_GViewTools;
	m_GViewTools=NULL;
}

void CGViewDlg::OnCancel() 
{
	CDlgX::OnCancel();
	delete m_GViewTools;
	m_GViewTools=NULL;
}

void CGViewDlg::OnOK() 
{
	CDlgX::OnOK();
	delete m_GViewTools;
	m_GViewTools=NULL;
}

void CGViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgX::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGViewDlg)
	DDX_Control(pDX, IDC_view, GViewParent.m_view);
	//}}AFX_DATA_MAP
}

void CGViewDlg::SaveOnExit(FILE * f)
{
	SaveConfig();	
	CDlgX::SaveOnExit(f);
}

void CGViewDlg::RestoreOnStart(FILE * f)
{
	LoadConfig();
	CDlgX::RestoreOnStart(f);
}




int CGViewDlg::SaveConfig()
{
	char s[81];
	CString Name = TheFrame->MainPath + "\\data\\GViewer.txt";

	FILE *f=fopen(Name.GetBuffer(0),"wb");
	
	if (!f)
	{
		CString cs;
		cs.Format("Error Opening Configuration File %s", Name.GetBuffer(0));
		TheFrame->MessageBox(cs,"Error",MB_ICONSTOP|MB_OK);
		return 1;
	}

	SaveLoadConfig(f, s, true);

	fclose(f);

	return 0;
}

int CGViewDlg::LoadConfig()
{
	char s[301];
	CString Name = TheFrame->MainPath + "\\data\\GViewer.txt";

	FILE *f=fopen(Name.GetBuffer(0),"rb");
	
	if (!f)
	{
		CString cs;
		cs.Format("Error Opening Configuration File %s", Name.GetBuffer(0));
		TheFrame->MessageBox(cs,"Error",MB_ICONSTOP|MB_OK);
		return 1;
	}

	while (!feof(f))
	{
		fgets(s,300,f);
		if (!feof(f))
		{
			SaveLoadConfig(f, s, false);
		}
	}

	TheFrame->GCodeDlg.GViewControlParent.m_view.m_Ortho = GViewParent.m_view.m_Ortho = m_Ortho!=0;

	fclose(f);

	return 0;
}



int CGViewDlg::SaveLoadConfig(FILE *f, char *s, bool save)
{
	CSTRING(m_ToolShapeFile);
	FLOAT(m_BoxX);
	FLOAT(m_BoxY);
	FLOAT(m_BoxZ);
	FLOAT(m_BoxOffsetX);
	FLOAT(m_BoxOffsetY);
	FLOAT(m_BoxOffsetZ);
	FLOAT(m_ToolSize);
	FLOAT(m_ToolOffX);
	FLOAT(m_ToolOffY);
	FLOAT(m_ToolOffZ);
	FLOAT(m_AxisSize);
	INT(m_ShowAxis);
	INT(m_ShowBox);
	INT(m_ShowTool);
	INT(m_IncludeA);
	INT(m_IncludeB);
	INT(m_IncludeC);
	INT(m_IncludeToolAngles);
	INT(m_Ortho);
	return 0;
}



BEGIN_MESSAGE_MAP(CGViewDlg, CDlgX)
	//{{AFX_MSG_MAP(CGViewDlg)
	ON_WM_SIZE()
	ON_COMMAND(IDC_XY, OnXy)
	ON_COMMAND(IDC_XZ, OnXz)
	ON_COMMAND(IDC_YZ, OnYz)
	ON_COMMAND(IDC_ClearPaths, OnClearPaths)
	ON_COMMAND(IDC_ShowAxis, OnShowAxis)
	ON_COMMAND(IDC_Ortho, OnOrtho)
	ON_COMMAND(IDC_Box, OnBox)
	ON_UPDATE_COMMAND_UI(IDC_Box, OnUpdateBox)
	ON_UPDATE_COMMAND_UI(IDC_ShowAxis, OnUpdateShowAxis)
	ON_UPDATE_COMMAND_UI(IDC_Ortho, OnUpdateOrtho)
	ON_COMMAND(IDC_RotXY, OnRotXY)
	ON_UPDATE_COMMAND_UI(IDC_RotXY, OnUpdateRotXY)
	ON_COMMAND(IDC_GViewerSetup, OnGViewerSetup)
	ON_COMMAND(IDC_ShowTool, OnShowTool)
	ON_UPDATE_COMMAND_UI(IDC_ShowTool, OnUpdateShowTool)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
	ON_WM_MOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGViewClass message handlers





BOOL CGViewDlg::OnInitDialog() 
{
	RECT rect,rect_tools,rect_dlg;;
	int result;

	CDlgX::OnInitDialog();

	GViewParent.OnInitDialog();

	m_GViewTools = new CDlgToolBar;

	// Create toolbar at the top of the dialog window
	if (m_GViewTools->Create(this))
	{
	    result = m_GViewTools->LoadToolBar(IDR_GViewBar);
	}      
	
	m_GViewTools->SetBarStyle(m_GViewTools->GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);


	// And position the control bars
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	
	RefreshTitle();


	m_GViewTools->GetWindowRect(&rect_tools);

	GetClientRect(&rect_dlg);

	rect.top = rect_tools.bottom - rect_tools.top;
	rect.bottom = rect_dlg.bottom;
	rect.left = 0;
	rect.right = rect_dlg.right;
	GViewParent.m_view.MoveWindow(&rect);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGViewDlg::RefreshTitle() 
{
	SetWindowText("G Code Viewer");
}


void CGViewDlg::OnSize(UINT nType, int cx, int cy) 
{
	RECT rect,rect_tools;;

	CDlgX::OnSize(nType, cx, cy);

	if (m_GViewTools && m_GViewTools->m_hWnd && GViewParent.m_view.m_hWnd)
	{
		m_GViewTools->GetWindowRect(&rect_tools);

		rect.top = rect_tools.bottom - rect_tools.top;
		rect.bottom = cy;
		rect.left = 0;
		rect.right = cx;
		GViewParent.m_view.MoveWindow(&rect);

		// And position the control bars
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	}
}


void CGViewDlg::OnMove(int x, int y)
{
	RECT rect,rect_tools;

	CDlgX::OnMove(x, y);

	if (m_GViewTools && m_GViewTools->m_hWnd && GViewParent.m_view.m_hWnd)
	{
		m_GViewTools->GetWindowRect(&rect_tools);
		rect.top = rect_tools.bottom - rect_tools.top;
		rect.left = 0;
		GViewParent.m_view.SendMessage(WM_MOVE,0,rect.left+(rect.top<<16));
	}
}




void CGViewDlg::OnXy() 
{
	GViewParent.OnXy();
}

void CGViewDlg::OnXz() 
{
	GViewParent.OnXz();
}

void CGViewDlg::OnYz() 
{
	GViewParent.OnYz();
}

void CGViewDlg::OnClearPaths() 
{
	GViewParent.OnClearPaths();
}


void CGViewDlg::OnShowAxis() 
{
	GViewParent.OnShowAxis();
}

void CGViewDlg::OnOrtho() 
{
	GViewParent.OnOrtho();
}

void CGViewDlg::OnUpdateShowAxis(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_ShowAxis);
}

void CGViewDlg::OnUpdateOrtho(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_Ortho);
}

void CGViewDlg::OnShowTool()
{
	GViewParent.OnShowTool();
}

void CGViewDlg::OnUpdateShowTool(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ShowTool);		
}


void CGViewDlg::OnBox() 
{
	GViewParent.OnBox();
}

void CGViewDlg::OnUpdateBox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ShowBox);		
}


void CGViewDlg::OnRotXY() 
{
	GViewParent.OnRotXY();
}

void CGViewDlg::OnUpdateRotXY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GViewParent.m_view.m_xyRotation);
}

void CGViewDlg::OnGViewerSetup() 
{
	GViewParent.OnGViewerSetup();
}




BOOL CGViewDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	return CDlgX::OnMouseWheel(nFlags, zDelta, pt);
}

