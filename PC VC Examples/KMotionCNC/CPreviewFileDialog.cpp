// PreviewFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CPreviewFileDialog.h"
#include <dlgs.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void StaticRender(void *Parent)
{
	((CPreviewFileDialog *)Parent)->ChangeToolPosition();
}


/////////////////////////////////////////////////////////////////////////////
// CPreviewFileDialog

IMPLEMENT_DYNAMIC(CPreviewFileDialog, CFileDialog)


CPreviewFileDialog::CPreviewFileDialog(CAbstractPreview *pPreview,BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd,0,0)
{
	m_ofn.Flags |= OFN_ENABLETEMPLATE;
	m_ofn.hInstance = AfxGetInstanceHandle();
	m_ofn.lpTemplateName = MAKEINTRESOURCE(DIALOG_PREVIEW);
	m_pPreview = pPreview;
	m_bChanged = FALSE;
	m_bPreview = TRUE;


	m_GViewTools=NULL;
	m_ShowAxis=false;
	m_ShowTool=true;


	m_SceneIsInitialized=false;
	m_FirstScreenDisplay=true;
	
	StartIndexAxis=EndIndexAxis=StartIndexTool=EndIndexTool=0;

	m_ToolShapeFile = "Tool.wrl";
	m_AxisSize = 1.0;
	m_ToolSize = 1.0;
	m_ToolOffX = 0.0;
	m_ToolOffY = 0.0;
	m_ToolOffZ = 0.0;

	m_IncludeA=FALSE;
	m_IncludeB=FALSE;
	m_IncludeC=FALSE;

	m_view.RenderCallback = StaticRender;
	m_view.Parent=this;


	//{{AFX_DATA_INIT(CPreviewFileDialog)
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CPreviewFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CPreviewFileDialog)
	ON_BN_CLICKED(IDC_CHECK_PREVIEW,OnClickedPreview)
	ON_COMMAND(IDC_XY, OnXy)
	ON_COMMAND(IDC_XZ, OnXz)
	ON_COMMAND(IDC_YZ, OnYz)
	ON_COMMAND(IDC_ShowAxis, OnShowAxis)
	ON_UPDATE_COMMAND_UI(IDC_ShowAxis, OnUpdateShowAxis)
	ON_COMMAND(IDC_RotXY, OnRotXY)
	ON_UPDATE_COMMAND_UI(IDC_RotXY, OnUpdateRotXY)
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CPreviewFileDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();

	RECT rect,rect_tools,rect_static;
	int result;

	InitializeScene();

	if (m_FirstScreenDisplay)
	{
		m_FirstScreenDisplay=false;
		if (TheFrame->GCodeDlg.m_Lathe)
			OnXz();
		else
			OnXy();
	}

	m_GViewTools = new CDlgToolBar;

	// Create toolbar at the top of the dialog window
	if (m_GViewTools->Create(this))
	{
	    result = m_GViewTools->LoadToolBar(IDR_GViewBarPreView);
	}      
	
	m_GViewTools->SetBarStyle(m_GViewTools->GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);


	// And position the control bars
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	m_GViewTools->GetWindowRect(&rect_tools);
	GetDlgItem(IDC_view)->GetWindowRect(&rect_static);

	m_GViewTools->GetWindowRect(&rect_tools);
	rect.top = 0;
	rect.bottom = 50;
	rect.left = 20 * TheFrame->GCodeDlg.dpiX/ TheFrame->GCodeDlg.dpi_standard;  // fixup for dpi awareness
	rect.right = 400;
	m_GViewTools->MoveWindow(&rect);


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

	FirstFolderChange = true;

	return TRUE;  // return TRUE unless you set the focus to a control
}


BOOL CPreviewFileDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(pResult != NULL);

	// allow message map to override

	OFNOTIFY* pNotify = (OFNOTIFY*)lParam;
	switch(pNotify->hdr.code)
	{
	case CDN_SELCHANGE:
		OnFileNameChange();
		return TRUE;
	case CDN_FOLDERCHANGE:
		OnFolderChange();
		return TRUE;
	case CDN_INITDONE:
		OnInitDone();
		return TRUE;
	}
	return CFileDialog::OnNotify(wParam, lParam, pResult);
}

void CPreviewFileDialog::OnInitDone()
{
}

void CPreviewFileDialog::OnFileNameChange()
{
	OnFileNameChange(false);
}

void CPreviewFileDialog::OnFileNameChange(bool FolderChanged)
{
	if(m_bPreview)
	{
		CString Path=GetFolderPath();
		CString FName= GetFileName();
		CString FullName= GetPathName();

		CString Comb=Path+"\\"+FName;
		if ((Comb != FullName && FName!="") || FolderChanged)  // must have selected a directory
			m_pPreview->SetPreviewFile("");
		else
			m_pPreview->SetPreviewFile(FullName);
		
		m_view.InvalidateRect(NULL);
		ChangeToolPosition();

		if (TheFrame->GCodeDlg.m_Lathe)
			SetViewDistance(VIEWXZ);
		else
			SetViewDistance(VIEWXY);
	}
	m_bChanged = FALSE;
}


void CPreviewFileDialog::OnFolderChange()
{
	OnFileNameChange(!FirstFolderChange);
	FirstFolderChange = false;
}

void CPreviewFileDialog::OnClickedPreview()
{
	UpdateData();
	m_view.InvalidateRect(NULL);
}

void CPreviewFileDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(m_bPreview)
	{
		m_view.InvalidateRect(NULL);
	}
}


CPreviewFileDialog::~CPreviewFileDialog()
{
	if (m_GViewTools)
		delete m_GViewTools;
	
	m_view.m_SceneGraph.Free();
}


#define FLAG_COLOR 0x808000


/////////////////////////////////////////////////////////////////////////////
// CPreviewFileDialog dialog



//********************************************
// InitGeometry
//********************************************


void CPreviewFileDialog::InitializeScene()
{
	if (!m_SceneIsInitialized)
	{
		AddAxisToScene();
		AddToolToScene();

		m_SceneIsInitialized=true;
	}
}

void CPreviewFileDialog::DeleteAllScene()
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



void CPreviewFileDialog::AddAxisToScene()
{
	CParserVrml parser;
	CColor color(255,255,00);

	if (TheFrame->GCodeDlg.m_Lathe)
	{
		if (TheFrame->GCodeDlg.m_XPosFront)
			m_AxisShapeFile = TheFrame->MainPathRoot + "\\KMotion\\Data\\AxisLatheXFront.wrl";
		else
			m_AxisShapeFile = TheFrame->MainPathRoot + "\\KMotion\\Data\\AxisLathe.wrl";
	}
	else
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
		m_AxisSize=1.0; // originally set scale to 1.
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


CString CPreviewFileDialog::GetToolFileToDisplay()
{
	return m_pPreview->m_csFileName;
}


void CPreviewFileDialog::AddToolToScene()
{
	CParserVrml parser;
	CColor color(100,100,100);

	CString file=GetToolFileToDisplay();

	EndIndexTool = StartIndexTool = m_view.m_SceneGraph.NbObject();

	if (file.IsEmpty()) return;
	
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

		m_ToolFileDisplayed = file;

		FindExtents();
		float sizex = m_maxx-m_minx;
		float sizey = m_maxy-m_miny;
		float sizez = m_maxz-m_minz;
		float max_size = sizex;
		if (sizey > max_size) max_size = sizey; 
		if (sizez > max_size) max_size = sizez; 

		FindAxisExtents();
		sizex = m_Axismaxx-m_Axisminx;
		sizey = m_Axismaxy-m_Axisminy;
		sizez = m_Axismaxz-m_Axisminz;
		float max_axis_size = sizex;
		if (sizey > max_axis_size) max_axis_size = sizey; 
		if (sizez > max_axis_size) max_axis_size = sizez; 

		if (max_axis_size > 0.0)
			m_AxisSize = max_size/max_axis_size;
		else
			m_AxisSize = 1.0;

		CTransform transform;
		CVector3d scale(m_AxisSize,m_AxisSize,m_AxisSize);

		transform.SetScale(scale);
		CVector3d OriginalOffset;


		for (int i=StartIndexAxis; i<EndIndexAxis; i++)
		{
			CMesh3d *mesh = (CMesh3d *)m_view.m_SceneGraph.GetAt(i);
			OriginalOffset.Copy(mesh->GetTransformOriginal()->GetTranslation());
			OriginalOffset *= m_AxisSize;
			transform.SetTranslation(OriginalOffset);
			mesh->SetTransform(transform);
			mesh->SetModified();
		}
	}
	else
	{
		m_ToolFileDisplayed = file;
		AfxMessageBox("Unable to load GCode Tool Image file:"+file);
	}
}

void CPreviewFileDialog::OnClose() 
{
	CFileDialog::OnClose();
	delete m_GViewTools;
	m_GViewTools=NULL;
}

void CPreviewFileDialog::OnCancel() 
{
	CFileDialog::OnCancel();
	delete m_GViewTools;
	m_GViewTools=NULL;
}

void CPreviewFileDialog::OnOK() 
{
	CFileDialog::OnOK();
	delete m_GViewTools;
	m_GViewTools=NULL;
}

void CPreviewFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreviewFileDialog)
	DDX_Control(pDX, IDC_view, m_view);
	DDX_Check(pDX, IDC_CHECK_PREVIEW, m_bPreview);
	//}}AFX_DATA_MAP
}



// set's the view distance based on tool extents

void CPreviewFileDialog::SetViewDistance(ViewDir View) 
{
	FindExtents();

	float sizex = m_maxx-m_minx;
	float midx = (m_maxx+m_minx)/2;
	float sizey = m_maxy-m_miny;
	float midy = (m_maxy+m_miny)/2;
	float sizez = m_maxz-m_minz;
	float midz = (m_maxz+m_minz)/2;

	float max_size = sizex;
	if (sizey > max_size) max_size = sizey; 
	if (sizez > max_size) max_size = sizez; 

	if (View==VIEWXY)
	{
		m_view.m_xTranslation = -midx;
		m_view.m_yTranslation = -midy;
		m_view.m_zTranslation = -midz -3.0 * max_size;
	}
	else if (View==VIEWYZ)
	{
		m_view.m_xTranslation = -midz;
		m_view.m_yTranslation = -midy;
		m_view.m_zTranslation = -midx -3.0 * max_size;
	}
	else if (View==VIEWXZ)
	{
		m_view.m_xTranslation = -midz;
		m_view.m_yTranslation = -midx;
		m_view.m_zTranslation = -midy -3.0 * max_size;
	}
 
	m_view.m_SpeedTranslation = max_size / 500.0f;
}


void CPreviewFileDialog::OnXy() 
{
	m_view.m_xRotation = 0.0f;
	m_view.m_yRotation = 0.0f;
	m_view.m_zRotation = 0.0f;
	SetViewDistance(VIEWXY);
}

void CPreviewFileDialog::OnXz() 
{
	if (TheFrame->GCodeDlg.m_XPosFront)
	{
		m_view.m_xRotation = 0.0f;
		m_view.m_yRotation = 90.0f;
		m_view.m_zRotation = -90.0f;
	}
	else
	{
		m_view.m_xRotation = 0.0f;
		m_view.m_yRotation = 90.0f;
		m_view.m_zRotation = 90.0f;
	}
	SetViewDistance(VIEWXZ);
}

void CPreviewFileDialog::OnYz() 
{
	m_view.m_xRotation = 0.0f;
	m_view.m_yRotation = 90.0f;
	m_view.m_zRotation = 0.0f;
	SetViewDistance(VIEWYZ);
}

void CPreviewFileDialog::OnClearPaths() 
{
	ClearPaths();
}



void CPreviewFileDialog::ClearPaths() 
{
	m_Path->m_ToolOffsetValid=false;
	((CPath3d *)m_view.m_SceneGraph.GetAt(0))->Free();
}

void CPreviewFileDialog::OnShowAxis() 
{
	m_ShowAxis = !m_ShowAxis;
	
	for (int i=StartIndexAxis; i<EndIndexAxis; i++)
		((CMesh3d *)m_view.m_SceneGraph.GetAt(i))->Show(m_ShowAxis);
}

void CPreviewFileDialog::OnUpdateShowAxis(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ShowAxis);		
}



int CPreviewFileDialog::FindExtents()
{
	m_minx = 1e30f;
	m_maxx = -1e30f;
	m_miny = 1e30f;
	m_maxy = -1e30f;
	m_minz = 1e30f;
	m_maxz = -1e30f;

	float minx,maxx,miny,maxy,minz,maxz;

	HWND hWnd = m_view.GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	// Activate view, set active OpenGL rendering context
	wglMakeCurrent(hDC,m_view.m_hGLContext);


	for (int i=StartIndexTool; i<EndIndexTool; i++)
	{
		CMesh3d *mesh = ((CMesh3d *)m_view.m_SceneGraph.GetAt(i));
		mesh->Range(0,&minx,&maxx);
		mesh->Range(1,&miny,&maxy);
		mesh->Range(2,&minz,&maxz);

		if (minx < m_minx) m_minx=minx;
		if (maxx > m_maxx) m_maxx=maxx;
		if (miny < m_miny) m_miny=miny;
		if (maxy > m_maxy) m_maxy=maxy;
		if (minz < m_minz) m_minz=minz;
		if (maxz > m_maxz) m_maxz=maxz;
	}

	return 0;
}


int CPreviewFileDialog::FindAxisExtents()
{
	m_Axisminx = 1e30f;
	m_Axismaxx = -1e30f;
	m_Axisminy = 1e30f;
	m_Axismaxy = -1e30f;
	m_Axisminz = 1e30f;
	m_Axismaxz = -1e30f;

	float minx,maxx,miny,maxy,minz,maxz;

	for (int i=StartIndexAxis; i<EndIndexAxis; i++)
	{
		CMesh3d *mesh = ((CMesh3d *)m_view.m_SceneGraph.GetAt(i));
		mesh->Range(0,&minx,&maxx);
		mesh->Range(1,&miny,&maxy);
		mesh->Range(2,&minz,&maxz);

		if (minx < m_Axisminx) m_Axisminx=minx;
		if (maxx > m_Axismaxx) m_Axismaxx=maxx;
		if (miny < m_Axisminy) m_Axisminy=miny;
		if (maxy > m_Axismaxy) m_Axismaxy=maxy;
		if (minz < m_Axisminz) m_Axisminz=minz;
		if (maxz > m_Axismaxz) m_Axismaxz=maxz;
	}

	return 0;
}



void CPreviewFileDialog::OnRotXY() 
{
	m_view.m_xyRotation = !m_view.m_xyRotation;
}

void CPreviewFileDialog::OnUpdateRotXY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_view.m_xyRotation);		
}



void CPreviewFileDialog::ChangeToolPosition()
{
	// check if tool changed
	if (GetToolFileToDisplay().CompareNoCase(m_ToolFileDisplayed))
	{
		DeleteAllScene();
		InitializeScene();
	}

	double x=0;
	double y=0;
	double z=0;
	double a=0;
	double b=0;
	double c=0;
	
	CTransform transform;
	CVector3d OriginalOffset,RotOffset;
	CVector3d ToolOffset(m_ToolOffX*m_ToolSize,m_ToolOffY*m_ToolSize,m_ToolOffZ*m_ToolSize);
	CVector3d scale(m_ToolSize,m_ToolSize,m_ToolSize);

	CMesh3d *mesh;
	transform.SetScale(scale);
	CVector3d offset(x,y,z);


	for (int i=StartIndexTool; i<EndIndexTool; i++)
	{
		mesh = (CMesh3d *)m_view.m_SceneGraph.GetAt(i);
		OriginalOffset.Copy(mesh->GetTransformOriginal()->GetTranslation());
		OriginalOffset *= m_ToolSize;
		OriginalOffset += &ToolOffset;

		OriginalOffset += &offset;

		transform.SetTranslation(OriginalOffset);

		mesh->SetTransform(transform);
		mesh->SetModified();
	}

	CVector3d scaleAxis(m_AxisSize,m_AxisSize,m_AxisSize);
	transform.SetScale(scaleAxis);
}


BOOL CPreviewFileDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	return CFileDialog::OnMouseWheel(nFlags, zDelta, pt);
}

