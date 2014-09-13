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

#define FLAG_COLOR 0x808000


/////////////////////////////////////////////////////////////////////////////
// CGViewDlg dialog


CGViewDlg::CGViewDlg(CWnd* pParent /*=NULL*/)
	: CDlgX(pParent)
{
	m_GViewTools=NULL;
	m_ShowAxis=false;
	m_ShowTool=false;
	m_ShowBox=false;

	m_Path = new CPath3d;
	m_view.m_SceneGraph.Add(m_Path);

	for (int i=0; i<NBOXPATHS; i++)
	{
		m_PathBox[i] = new CPath3d;
		m_view.m_SceneGraph.Add(m_PathBox[i]);
	}

	m_BoxX=120;
	m_BoxY=120;
	m_BoxZ=120;

	m_BoxOffsetX=0;
	m_BoxOffsetY=0;
	m_BoxOffsetZ=0;


	m_SceneIsInitialized=false;
	m_FirstScreenDisplay=true;
	
	StartIndexAxis=EndIndexAxis=StartIndexTool=EndIndexTool=0;

	m_ToolShapeFile = "Tool.wrl";
	m_AxisSize = 1.0;
	m_ToolSize = 5.0;
	m_ToolOffX = 0.0;
	m_ToolOffY = 0.0;
	m_ToolOffZ = 1.25;

	m_IncludeA=FALSE;
	m_IncludeB=FALSE;
	m_IncludeC=FALSE;

	//{{AFX_DATA_INIT(CGViewDlg)
	//}}AFX_DATA_INIT
}


CGViewDlg::~CGViewDlg()
{
	if (m_GViewTools)
		delete m_GViewTools;
	
	m_view.m_SceneGraph.Free();
}


//********************************************
// InitGeometry
//********************************************


void CGViewDlg::InitializeScene()
{
	if (!m_SceneIsInitialized)
	{
		AddBox();
		AddAxisToScene();
		AddToolToScene();

		m_SceneIsInitialized=true;
	}
}

void CGViewDlg::DeleteAllScene()
{
	int i;
	for (i=EndIndexTool-1; i>=StartIndexTool; i--)
	{
		delete ((CMesh3d *)m_view.m_SceneGraph.GetAt(i));
		m_view.m_SceneGraph.RemoveAt(i);
	}

	for (i=EndIndexAxis-1; i>=StartIndexAxis; i--)
	{
		delete ((CMesh3d *)m_view.m_SceneGraph.GetAt(i));
		m_view.m_SceneGraph.RemoveAt(i);
	}

	m_SceneIsInitialized=false;
}



void CGViewDlg::AddAxisToScene()
{
	CParserVrml parser;
	CColor color(255,255,00);

	m_AxisShapeFile = TheFrame->MainPathRoot + "\\KMotion\\Data\\Axis.wrl";

	StartIndexAxis = m_view.m_SceneGraph.NbObject();
	
	if(parser.Run(m_AxisShapeFile.GetBuffer(0),&m_view.m_SceneGraph))
	{
		if(m_view.m_Smooth)
		{
			m_view.m_SceneGraph.SetNormalBinding(NORMAL_PER_VERTEX);
			::glShadeModel(GL_SMOOTH);
		}
		else
		{
			m_view.m_SceneGraph.SetNormalBinding(NORMAL_PER_FACE);
			::glShadeModel(GL_FLAT);
		}
		m_view.m_SceneGraph.BuildAdjacency();
		m_view.m_SceneGraph.CalculateNormalPerFace();
		m_view.m_SceneGraph.CalculateNormalPerVertex();
		
		EndIndexAxis = m_view.m_SceneGraph.NbObject();

		CMesh3d *mesh;
		CTransform transform;
		CVector3d OriginalOffset;
		CVector3d scale(m_AxisSize,m_AxisSize,m_AxisSize);
		transform.SetScale(scale);
		for (int i=StartIndexAxis; i<EndIndexAxis; i++)
		{
			mesh = (CMesh3d *)m_view.m_SceneGraph.GetAt(i);
			mesh->Show(m_ShowAxis);
			mesh->SetColor(color);
			mesh->SetTransformOriginal(*mesh->GetTransform());
			OriginalOffset.Copy(mesh->GetTransformOriginal()->GetTranslation());
			OriginalOffset *= m_AxisSize;
			transform.SetTranslation(OriginalOffset);
			mesh->SetTransform(transform);
			mesh->SetModified();
		}
	}
	else
	{
		AfxMessageBox("Unable to load GCode Axis Image file:"+m_AxisShapeFile);
	}
}


void CGViewDlg::AddToolToScene()
{
	CParserVrml parser;
	CString file;

	CColor color(100,100,100);

	if (m_ToolShapeFile.IsEmpty()) return;

	// check if there is no path specified, then add in default

	if (m_ToolShapeFile.Find('\\') == -1)
	{
		file = TheFrame->MainPathRoot + "\\KMotion\\Data\\" + m_ToolShapeFile;
	}
	else
	{
		file = m_ToolShapeFile;
	}

	StartIndexTool = m_view.m_SceneGraph.NbObject();
	
	if(parser.Run(file.GetBuffer(0),&m_view.m_SceneGraph))
	{
		if(m_view.m_Smooth)
		{
			m_view.m_SceneGraph.SetNormalBinding(NORMAL_PER_VERTEX);
			::glShadeModel(GL_SMOOTH);
		}
		else
		{
			m_view.m_SceneGraph.SetNormalBinding(NORMAL_PER_FACE);
			::glShadeModel(GL_FLAT);
		}
		m_view.m_SceneGraph.BuildAdjacency();
		m_view.m_SceneGraph.CalculateNormalPerFace();
		m_view.m_SceneGraph.CalculateNormalPerVertex();
		
		EndIndexTool = m_view.m_SceneGraph.NbObject();

		for (int i=StartIndexTool; i<EndIndexTool; i++)
		{
			CMesh3d *mesh = ((CMesh3d *)m_view.m_SceneGraph.GetAt(i));
			mesh->Show(m_ShowTool);
			mesh->SetColor(color);
			mesh->SetTransformOriginal(*mesh->GetTransform());
		}

		ChangeToolPosition();
	}
	else
	{
		AfxMessageBox("Unable to load GCode Tool Image file:"+file);
	}
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
	DDX_Control(pDX, IDC_view, m_view);
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
	return 0;
}



BEGIN_MESSAGE_MAP(CGViewDlg, CDlgX)
	//{{AFX_MSG_MAP(CGViewDlg)
	ON_WM_SIZE()
	ON_COMMAND(ID_XY, OnXy)
	ON_COMMAND(ID_XZ, OnXz)
	ON_COMMAND(ID_YZ, OnYz)
	ON_COMMAND(ID_ClearPaths, OnClearPaths)
	ON_COMMAND(ID_ShowAxis, OnShowAxis)
	ON_COMMAND(ID_Box, OnBox)
	ON_UPDATE_COMMAND_UI(ID_Box, OnUpdateBox)
	ON_UPDATE_COMMAND_UI(ID_ShowAxis, OnUpdateShowAxis)
	ON_COMMAND(ID_RotXY, OnRotXY)
	ON_UPDATE_COMMAND_UI(ID_RotXY, OnUpdateRotXY)
	ON_COMMAND(ID_GViewerSetup, OnGViewerSetup)
	ON_COMMAND(ID_ShowTool, OnShowTool)
	ON_UPDATE_COMMAND_UI(ID_ShowTool, OnUpdateShowTool)
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

	InitializeScene();

	if (m_FirstScreenDisplay)
	{
		m_FirstScreenDisplay=false;
		SetViewDistance();
	}

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
	m_view.MoveWindow(&rect);


	unsigned int size = m_view.m_SceneGraph.NbObject();
	for(unsigned int i=0; i<size; i++)
	{
		CObject3d *pObject3d = m_view.m_SceneGraph.GetAt(i);
		if(pObject3d != NULL)
		{
			pObject3d->SetModified();
			pObject3d->InvalidateDisplayList();
		}
	}

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

	if (m_GViewTools && m_GViewTools->m_hWnd && m_view.m_hWnd)
	{
		m_GViewTools->GetWindowRect(&rect_tools);

		rect.top = rect_tools.bottom - rect_tools.top;
		rect.bottom = cy;
		rect.left = 0;
		rect.right = cx;
		m_view.MoveWindow(&rect);

		// And position the control bars
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	}
}


void CGViewDlg::OnMove(int x, int y)
{
	RECT rect,rect_tools;

	CDlgX::OnMove(x, y);

	if (m_GViewTools && m_GViewTools->m_hWnd && m_view.m_hWnd)
	{
		m_GViewTools->GetWindowRect(&rect_tools);
		rect.top = rect_tools.bottom - rect_tools.top;
		rect.left = 0;
		m_view.SendMessage(WM_MOVE,0,rect.left+(rect.top<<16));
	}
}


// set's the view distance based on extents
// or Box whichever is bigger

void CGViewDlg::SetViewDistance() 
{
	FindExtents();

	if (m_BoxOffsetX-m_BoxX/2 < m_minx) m_minx = m_BoxOffsetX-m_BoxX/2;
	if (m_BoxOffsetY-m_BoxY/2 < m_miny) m_miny = m_BoxOffsetY-m_BoxY/2;
	if (m_BoxOffsetZ-m_BoxZ/2 < m_minz) m_minz = m_BoxOffsetZ-m_BoxZ/2;

	if (m_BoxOffsetX+m_BoxX/2 > m_maxx) m_maxx = m_BoxOffsetX+m_BoxX/2;
	if (m_BoxOffsetY+m_BoxY/2 > m_maxy) m_maxy = m_BoxOffsetY+m_BoxY/2;
	if (m_BoxOffsetZ+m_BoxZ/2 > m_maxz) m_maxz = m_BoxOffsetZ+m_BoxZ/2;

	float sizex = m_maxx-m_minx;
	float midx = (m_maxx+m_minx)/2;
	float sizey = m_maxy-m_miny;
	float midy = (m_maxy+m_miny)/2;
	float sizez = m_maxz-m_minz;
	float midz = (m_maxz+m_minz)/2;

	float max_size = sizex;
	if (sizey > max_size) max_size = sizey; 
	if (sizez > max_size) max_size = sizez; 

	m_view.m_xTranslation = -midx;
	m_view.m_yTranslation = -midy;
	m_view.m_zTranslation = -midz -1.75 * max_size;

	m_view.m_SpeedTranslation = max_size / 500.0f;
}


void CGViewDlg::OnXy() 
{
	m_view.m_xRotation = 0.0f;
	m_view.m_yRotation = 0.0f;
	m_view.m_zRotation = 0.0f;
	SetViewDistance();
}

void CGViewDlg::OnXz() 
{
	m_view.m_xRotation = -90.0f;
	m_view.m_yRotation = 0.0f;
	m_view.m_zRotation = 0.0f;
	SetViewDistance();
}

void CGViewDlg::OnYz() 
{
	m_view.m_xRotation = 0.0f;
	m_view.m_yRotation = -90.0f;
	m_view.m_zRotation = 0.0f;
	SetViewDistance();
}

void CGViewDlg::OnClearPaths() 
{
	ClearPaths();
	AddBox();
}


void CGViewDlg::AddBox() 
{
	// creat the Paths to draw a cube

	m_ColorBox.Set(0x80,0x80,0x80);

	m_PathBox[0]->Free();
	m_PathBox[1]->Free();
	m_PathBox[2]->Free();
	m_PathBox[3]->Free();

	m_PathBox[0]->Show(m_ShowBox);
	m_PathBox[1]->Show(m_ShowBox);
	m_PathBox[2]->Show(m_ShowBox);
	m_PathBox[3]->Show(m_ShowBox);

	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX-m_BoxX/2,m_BoxOffsetY-m_BoxY/2,m_BoxOffsetZ-m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX+m_BoxX/2,m_BoxOffsetY-m_BoxY/2,m_BoxOffsetZ-m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX+m_BoxX/2,m_BoxOffsetY+m_BoxY/2,m_BoxOffsetZ-m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX-m_BoxX/2,m_BoxOffsetY+m_BoxY/2,m_BoxOffsetZ-m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX-m_BoxX/2,m_BoxOffsetY-m_BoxY/2,m_BoxOffsetZ-m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX-m_BoxX/2,m_BoxOffsetY-m_BoxY/2,m_BoxOffsetZ+m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX+m_BoxX/2,m_BoxOffsetY-m_BoxY/2,m_BoxOffsetZ+m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX+m_BoxX/2,m_BoxOffsetY+m_BoxY/2,m_BoxOffsetZ+m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX-m_BoxX/2,m_BoxOffsetY+m_BoxY/2,m_BoxOffsetZ+m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3d(m_BoxOffsetX-m_BoxX/2,m_BoxOffsetY-m_BoxY/2,m_BoxOffsetZ+m_BoxZ/2,m_ColorBox));

	m_PathBox[1]->AddVertex(new CVertex3d(m_BoxOffsetX-m_BoxX/2,m_BoxOffsetY+m_BoxY/2,m_BoxOffsetZ-m_BoxZ/2,m_ColorBox));
	m_PathBox[1]->AddVertex(new CVertex3d(m_BoxOffsetX-m_BoxX/2,m_BoxOffsetY+m_BoxY/2,m_BoxOffsetZ+m_BoxZ/2,m_ColorBox));

	m_PathBox[2]->AddVertex(new CVertex3d(m_BoxOffsetX+m_BoxX/2,m_BoxOffsetY+m_BoxY/2,m_BoxOffsetZ-m_BoxZ/2,m_ColorBox));
	m_PathBox[2]->AddVertex(new CVertex3d(m_BoxOffsetX+m_BoxX/2,m_BoxOffsetY+m_BoxY/2,m_BoxOffsetZ+m_BoxZ/2,m_ColorBox));

	m_PathBox[3]->AddVertex(new CVertex3d(m_BoxOffsetX+m_BoxX/2,m_BoxOffsetY-m_BoxY/2,m_BoxOffsetZ-m_BoxZ/2,m_ColorBox));
	m_PathBox[3]->AddVertex(new CVertex3d(m_BoxOffsetX+m_BoxX/2,m_BoxOffsetY-m_BoxY/2,m_BoxOffsetZ+m_BoxZ/2,m_ColorBox));
}


void CGViewDlg::ClearPaths() 
{
	((CPath3d *)m_view.m_SceneGraph.GetAt(0))->Free();
}

void CGViewDlg::OnShowAxis() 
{
	m_ShowAxis = !m_ShowAxis;
	
	for (int i=StartIndexAxis; i<EndIndexAxis; i++)
		((CMesh3d *)m_view.m_SceneGraph.GetAt(i))->Show(m_ShowAxis);
}

void CGViewDlg::OnUpdateShowAxis(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ShowAxis);		
}

void CGViewDlg::OnShowTool() 
{
	m_ShowTool = !m_ShowTool;

	if (m_ToolShapeFile.IsEmpty())
	{
		AfxMessageBox("Tool Image VRML file is blank.  Please specify a valid VRML file in the G Viewer Setup");
	}
	
	for (int i=StartIndexTool; i<EndIndexTool; i++)
		((CMesh3d *)m_view.m_SceneGraph.GetAt(i))->Show(m_ShowTool);
}

void CGViewDlg::OnUpdateShowTool(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ShowTool);		
}


void CGViewDlg::OnBox() 
{
	m_ShowBox = !m_ShowBox;

	m_PathBox[0]->Show(m_ShowBox);
	m_PathBox[1]->Show(m_ShowBox);
	m_PathBox[2]->Show(m_ShowBox);
	m_PathBox[3]->Show(m_ShowBox);
}

void CGViewDlg::OnUpdateBox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ShowBox);		
}

int CGViewDlg::FindExtents()
{
	m_minx = m_BoxOffsetX - m_BoxX/2;
	m_maxx = m_BoxOffsetX + m_BoxX/2;
	m_miny = m_BoxOffsetY - m_BoxY/2;
	m_maxy = m_BoxOffsetY + m_BoxY/2;
	m_minz = m_BoxOffsetZ - m_BoxZ/2;
	m_maxz = m_BoxOffsetZ + m_BoxZ/2;
	
	m_Path->Range(0,&m_minx,&m_maxx);
	m_Path->Range(1,&m_miny,&m_maxy);
	m_Path->Range(2,&m_minz,&m_maxz);

	return 0;
}

void CGViewDlg::OnRotXY() 
{
	m_view.m_xyRotation = !m_view.m_xyRotation;
}

void CGViewDlg::OnUpdateRotXY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_view.m_xyRotation);		
}

void CGViewDlg::OnGViewerSetup() 
{
	CGViewerSetup GViewerSetup;

	GViewerSetup.m_ToolShapeFile = m_ToolShapeFile;
	GViewerSetup.m_BoxX = m_BoxX;
	GViewerSetup.m_BoxY = m_BoxY;
	GViewerSetup.m_BoxZ = m_BoxZ;
	GViewerSetup.m_BoxOffsetX = m_BoxOffsetX;
	GViewerSetup.m_BoxOffsetY = m_BoxOffsetY;
	GViewerSetup.m_BoxOffsetZ = m_BoxOffsetZ;
	GViewerSetup.m_ToolSize = m_ToolSize;
	GViewerSetup.m_AxisSize = m_AxisSize;
	GViewerSetup.m_ToolOffX = m_ToolOffX;
	GViewerSetup.m_ToolOffY = m_ToolOffY;
	GViewerSetup.m_ToolOffZ = m_ToolOffZ;
	GViewerSetup.m_IncludeA = m_IncludeA;
	GViewerSetup.m_IncludeB = m_IncludeB;
	GViewerSetup.m_IncludeC = m_IncludeC;

	if (GViewerSetup.DoModal() == IDOK)
	{
		m_ToolShapeFile = GViewerSetup.m_ToolShapeFile;
		m_BoxX = GViewerSetup.m_BoxX;
		m_BoxY = GViewerSetup.m_BoxY;
		m_BoxZ = GViewerSetup.m_BoxZ;
		m_BoxOffsetX = GViewerSetup.m_BoxOffsetX;
		m_BoxOffsetY = GViewerSetup.m_BoxOffsetY;
		m_BoxOffsetZ = GViewerSetup.m_BoxOffsetZ;
		m_ToolSize = GViewerSetup.m_ToolSize;
		m_AxisSize = GViewerSetup.m_AxisSize;
		m_ToolOffX = GViewerSetup.m_ToolOffX;
		m_ToolOffY = GViewerSetup.m_ToolOffY;
		m_ToolOffZ = GViewerSetup.m_ToolOffZ;
		m_IncludeA = GViewerSetup.m_IncludeA;
		m_IncludeB = GViewerSetup.m_IncludeB;
		m_IncludeC = GViewerSetup.m_IncludeC;

		DeleteAllScene();
		InitializeScene();
		SaveConfig();
	}
}


void CGViewDlg::ChangeToolPosition()
{
	CCoordMotion *CM = TheFrame->GCodeDlg.Interpreter->CoordMotion;
	double x=CM->current_x;
	double y=CM->current_y;
	double z=CM->current_z;
	double a=CM->current_a;
	double b=CM->current_b;
	double c=CM->current_c;
	
	if (!TheFrame->GCodeDlg.m_Simulate)
	{
		if (TheFrame->KMotionDLL->WaitToken(FALSE)==KMOTION_LOCKED)
		{
			TheFrame->KMotionDLL->ReleaseToken();

			if (CM->ReadCurAbsPosition(&x,&y,&z,&a,&b,&c)) return;
		}
	}


	CTransform transform;
	CVector3d OriginalOffset,RotOffset;
	CVector3d ToolOffset(m_ToolOffX*m_ToolSize,m_ToolOffY*m_ToolSize,m_ToolOffZ*m_ToolSize);
	CVector3d scale(m_ToolSize,m_ToolSize,m_ToolSize);


	MOTION_PARAMS *MP=&CM->Kinematics->m_MotionParams;

	// rotation about X axis changes Y and Z Axis as a function of A
	bool doA = TheFrame->GViewDlg.m_IncludeA && MP->DegreesA && MP->RadiusA>0.0;
	if (doA)
	{
		double Theta=a*PI/180.0;
		double y0 = y+MP->RadiusA;
		y = y0 * cos(Theta) - z * sin(Theta);
		z = y0 * sin(Theta) + z * cos(Theta);
		transform.SetValueRotationYZ(a);
	}

	// rotation about Y axis changes Z and X Axis as a function of B
	bool doB = TheFrame->GViewDlg.m_IncludeB && MP->DegreesB && MP->RadiusB>0.0;
	if (doB)
	{
		double Theta=b*PI/180.0;
		double z0 = z+MP->RadiusB;
		z = z0 * cos(Theta) - x * sin(Theta);
		x = z0 * sin(Theta) + x * cos(Theta);
		transform.SetValueRotationZX(b);
	}

	// rotation about Z axis changes X and Y Axis as a function of C
	bool doC = TheFrame->GViewDlg.m_IncludeC && MP->DegreesC && MP->RadiusC>0.0;
	if (doC)
	{
		double Theta=c*PI/180.0;
		double x0 = x+MP->RadiusC;
		x = x0 * cos(Theta) - y * sin(Theta);
		y = x0 * sin(Theta) + y * cos(Theta);
		transform.SetValueRotationXY(c);
	}

	m_view.OpenGLMutex->Lock();
	
	CMesh3d *mesh;
	transform.SetScale(scale);
	CVector3d offset(x,y,z);


	for (int i=StartIndexTool; i<EndIndexTool; i++)
	{
		mesh = (CMesh3d *)m_view.m_SceneGraph.GetAt(i);
		OriginalOffset.Copy(mesh->GetTransformOriginal()->GetTranslation());
		OriginalOffset *= m_ToolSize;
		OriginalOffset += &ToolOffset;

		if (doA) OriginalOffset.RotateYZ(a*PI/180.0);
		if (doB) OriginalOffset.RotateZX(b*PI/180.0);
		if (doC) OriginalOffset.RotateXY(c*PI/180.0);

		OriginalOffset += &offset;

		transform.SetTranslation(OriginalOffset);

		mesh->SetTransform(transform);
		mesh->SetModified();
	}

	CVector3d scaleAxis(m_AxisSize,m_AxisSize,m_AxisSize);
	transform.SetScale(scaleAxis);


	if (doA || doB || doC)
	{
		for (int i=StartIndexAxis; i<EndIndexAxis; i++)
		{
			mesh = (CMesh3d *)m_view.m_SceneGraph.GetAt(i);
			OriginalOffset.Copy(mesh->GetTransformOriginal()->GetTranslation());
			OriginalOffset *= m_AxisSize;
			if (doA) OriginalOffset.RotateYZ(a*PI/180.0);
			if (doB) OriginalOffset.RotateZX(b*PI/180.0);
			if (doC) OriginalOffset.RotateXY(c*PI/180.0);
			transform.SetTranslation(OriginalOffset);
			mesh->SetTransform(transform);
			mesh->SetModified();
		}
	}

	m_view.OpenGLMutex->Unlock();
}


BOOL CGViewDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	return CDlgX::OnMouseWheel(nFlags, zDelta, pt);
}

