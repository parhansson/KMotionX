// GViewParent.cpp : implementation file
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


static void StaticRender(void *Parent)
{
	((CGViewParent *)Parent)->ChangeToolPosition();
}

/////////////////////////////////////////////////////////////////////////////
// CGViewParent dialog


CGViewParent::CGViewParent()
{
	m_Path = new CPath3d;
	m_view.m_SceneGraph.Add(m_Path);

	for (int i=0; i<NBOXPATHS; i++)
	{
		m_PathBox[i] = new CPath3d;
		m_view.m_SceneGraph.Add(m_PathBox[i]);
	}

	m_SceneIsInitialized=false;
	m_SceneIsDirty=true;
	m_FirstScreenDisplay=true;
	
	StartIndexAxis=EndIndexAxis=StartIndexTool=EndIndexTool=0;

	m_view.RenderCallback = StaticRender;
	m_view.Parent=this;
}


CGViewParent::~CGViewParent()
{
	m_view.m_SceneGraph.Free();
}


//********************************************
// InitGeometry
//********************************************


void CGViewParent::InitializeScene()
{
	if (!m_SceneIsInitialized)
	{
		AddBox();
		AddAxisToScene();
		AddToolToScene();

		m_SceneIsInitialized=true;
		m_SceneIsDirty=false;
	}
}

void CGViewParent::DeleteAllScene()
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



void CGViewParent::AddAxisToScene()
{
	CParserVrml parser;
	CColor color(255,255,00);
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;

	if (TheFrame->GCodeDlg.m_Lathe)
	{
		if (TheFrame->GCodeDlg.m_XPosFront)
			m_AxisShapeFile = TheFrame->MainPathRoot + "\\KMotion\\Data\\AxisLatheXFront.wrl";
		else
			m_AxisShapeFile = TheFrame->MainPathRoot + "\\KMotion\\Data\\AxisLathe.wrl";
	}
	else
	{
		m_AxisShapeFile = TheFrame->MainPathRoot + "\\KMotion\\Data\\Axis.wrl";
	}

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
		CVector3d scale(GVDlg->m_AxisSize,GVDlg->m_AxisSize,GVDlg->m_AxisSize);
		transform.SetScale(scale);
		for (int i=StartIndexAxis; i<EndIndexAxis; i++)
		{
			mesh = (CMesh3d *)m_view.m_SceneGraph.GetAt(i);
			mesh->Show(GVDlg->m_ShowAxis);
			mesh->SetColor(color);
			mesh->SetTransformOriginal(*mesh->GetTransform());
			OriginalOffset.Copy(mesh->GetTransformOriginal()->GetTranslation());
			OriginalOffset *= GVDlg->m_AxisSize;
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


CString CGViewParent::GetToolFileToDisplay(bool *UsingDefault)
{
	CString file;
	// check if an image is specified in the Tool Table for the tool

	setup_pointer p = TheFrame->GCodeDlg.m_RealTimeSetup;
	file = p->tool_table[p->selected_tool_slot].ToolImage;

	if (file.IsEmpty()) //if none specified use default
	{
		*UsingDefault=true;
		file = TheFrame->GViewDlg.m_ToolShapeFile;
		if (file.IsEmpty()) return "";
	}
	else
	{
		*UsingDefault=false;
	}
	// check if there is no path specified, then add in default

	if (file.Find(':') == -1 && file.Find("\\\\") == -1)
	{
		file = TheFrame->MainPathRoot + TOOL_IMAGE_SUB_DIR + file;
	}
	return file;
}


void CGViewParent::AddToolToScene()
{
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;
	CParserVrml parser;
	bool UsingDefault;
	CColor color(100,100,100);

	CString file=GetToolFileToDisplay(&UsingDefault);

	EndIndexTool = StartIndexTool = m_view.m_SceneGraph.NbObject();

	if (file.IsEmpty()) return;

	m_ToolFileDisplayed = file;

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
			mesh->Show(GVDlg->m_ShowTool);
			mesh->SetColor(color);
			mesh->SetTransformOriginal(*mesh->GetTransform());
		}
	}
	else
	{
		static bool MessDisplayed=false;

		if (!MessDisplayed)
		{
			MessDisplayed=true;
			AfxMessageBox("Unable to load GCode Tool Image file:"+file);
			MessDisplayed=false;
		}
	}
}




BOOL CGViewParent::OnInitDialog() 
{
	InitializeScene();

	if (m_FirstScreenDisplay)
	{
		m_FirstScreenDisplay=false;
		if (TheFrame->GCodeDlg.m_Lathe)
			OnXz();
		else
			OnXy();
	}


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




// set's the view distance based on extents
// or Box whichever is bigger

void CGViewParent::SetViewDistance(ViewDir View) 
{
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;

	FindExtents();

	if (GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2 < m_minx) m_minx = GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2;
	if (GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2 < m_miny) m_miny = GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2;
	if (GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2 < m_minz) m_minz = GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2;

	if (GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2 > m_maxx) m_maxx = GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2;
	if (GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2 > m_maxy) m_maxy = GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2;
	if (GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2 > m_maxz) m_maxz = GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2;

	float sizex = m_maxx-m_minx;
	float midx = (m_maxx+m_minx)/2;
	float sizey = m_maxy-m_miny;
	float midy = (m_maxy+m_miny)/2;
	float sizez = m_maxz-m_minz;
	float midz = (m_maxz+m_minz)/2;

	float max_size = sizex;
	if (sizey > max_size) max_size = sizey; 
	if (sizez > max_size) max_size = sizez; 

	float xScaling = 0.85f * (2.0f * m_view.m_aspect) / sizex;  // do this for ortho mode
	float yScaling = 0.85f * 2.0f / sizey;

	if (xScaling > yScaling)
		m_view.m_Scaling0 = yScaling;
	else
		m_view.m_Scaling0 = xScaling;


	if (View==VIEWXY)
	{
		m_view.m_xTranslation = -midx;
		m_view.m_yTranslation = -midy;
	}
	else if (View==VIEWYZ)
	{
		m_view.m_xTranslation = -midz;
		m_view.m_yTranslation = -midy;
	}
	else if (View==VIEWXZ)
	{
		m_view.m_xTranslation = -midz;
		m_view.m_yTranslation = -midx;
	}

	m_view.m_zTranslation0 = m_view.m_zTranslation = -midz -1.75 * max_size;
 
	m_view.m_SpeedTranslation = max_size / 500.0f;;
}


void CGViewParent::OnXy() 
{
	m_view.m_xRotation = 0.0f;
	m_view.m_yRotation = 0.0f;
	m_view.m_zRotation = 0.0f;
	SetViewDistance(VIEWXY);
}

void CGViewParent::OnXz() 
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

void CGViewParent::OnYz() 
{
	m_view.m_xRotation = 0.0f;
	m_view.m_yRotation = 90.0f;
	m_view.m_zRotation = 0.0f;
	SetViewDistance(VIEWYZ);
}

void CGViewParent::OnClearPaths() 
{
	ClearPaths();
	AddBox();
}


// creat the Paths to draw a cube
void CGViewParent::AddBox()
{
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;

	m_ColorBox.Set(0x80,0x80,0x80);

	m_PathBox[0]->Free();
	m_PathBox[1]->Free();
	m_PathBox[2]->Free();
	m_PathBox[3]->Free();

	m_PathBox[0]->Show(GVDlg->m_ShowBox);
	m_PathBox[1]->Show(GVDlg->m_ShowBox);
	m_PathBox[2]->Show(GVDlg->m_ShowBox);
	m_PathBox[3]->Show(GVDlg->m_ShowBox);

	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[0]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2,m_ColorBox));

	m_PathBox[1]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[1]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX-GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2,m_ColorBox));

	m_PathBox[2]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[2]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY+GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2,m_ColorBox));

	m_PathBox[3]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ-GVDlg->m_BoxZ/2,m_ColorBox));
	m_PathBox[3]->AddVertex(new CVertex3dFast(GVDlg->m_BoxOffsetX+GVDlg->m_BoxX/2,GVDlg->m_BoxOffsetY-GVDlg->m_BoxY/2,GVDlg->m_BoxOffsetZ+GVDlg->m_BoxZ/2,m_ColorBox));
}


void CGViewParent::ClearPaths() 
{
	m_Path->m_ToolOffsetValid=false;
	((CPath3d *)m_view.m_SceneGraph.GetAt(0))->Free();
}

void CGViewParent::OnShowAxis() 
{
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;

	GVDlg->m_ShowAxis = !GVDlg->m_ShowAxis;
	
	for (int i=StartIndexAxis; i<EndIndexAxis; i++)
		((CMesh3d *)m_view.m_SceneGraph.GetAt(i))->Show(GVDlg->m_ShowAxis);
}

void CGViewParent::OnOrtho() 
{
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;
	GVDlg->m_Ortho = !GVDlg->m_Ortho;
	m_view.m_Ortho = GVDlg->m_Ortho!=0;
	m_view.SetupOpenGL();
	m_view.Invalidate();
}

void CGViewParent::OnShowTool()
{
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;
	GVDlg->m_ShowTool = !GVDlg->m_ShowTool;

	if (GVDlg->m_ToolShapeFile.IsEmpty())
	{
		AfxMessageBox("Tool Image VRML file is blank.  Please specify a valid VRML file in the G Viewer Setup");
	}
	
	for (int i=StartIndexTool; i<EndIndexTool; i++)
		((CMesh3d *)m_view.m_SceneGraph.GetAt(i))->Show(GVDlg->m_ShowTool);
}



void CGViewParent::OnBox() 
{
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;
	GVDlg->m_ShowBox = !GVDlg->m_ShowBox;

	m_PathBox[0]->Show(GVDlg->m_ShowBox);
	m_PathBox[1]->Show(GVDlg->m_ShowBox);
	m_PathBox[2]->Show(GVDlg->m_ShowBox);
	m_PathBox[3]->Show(GVDlg->m_ShowBox);
}


int CGViewParent::FindExtents()
{
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;
	m_minx = GVDlg->m_BoxOffsetX - GVDlg->m_BoxX/2;
	m_maxx = GVDlg->m_BoxOffsetX + GVDlg->m_BoxX/2;
	m_miny = GVDlg->m_BoxOffsetY - GVDlg->m_BoxY/2;
	m_maxy = GVDlg->m_BoxOffsetY + GVDlg->m_BoxY/2;
	m_minz = GVDlg->m_BoxOffsetZ - GVDlg->m_BoxZ/2;
	m_maxz = GVDlg->m_BoxOffsetZ + GVDlg->m_BoxZ/2;

	m_view.OpenGLMutex->Lock();
	m_Path->Range(0,&m_minx,&m_maxx);
	m_Path->Range(1,&m_miny,&m_maxy);
	m_Path->Range(2,&m_minz,&m_maxz);
	m_view.OpenGLMutex->Unlock();

	return 0;
}

void CGViewParent::OnRotXY() 
{
	m_view.m_xyRotation = !m_view.m_xyRotation;
}


void CGViewParent::OnGViewerSetup() 
{
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;
	CGViewerSetup GViewerSetup;

	GViewerSetup.m_ToolShapeFile = GVDlg->m_ToolShapeFile;
	GViewerSetup.m_BoxX = GVDlg->m_BoxX;
	GViewerSetup.m_BoxY = GVDlg->m_BoxY;
	GViewerSetup.m_BoxZ = GVDlg->m_BoxZ;
	GViewerSetup.m_BoxOffsetX = GVDlg->m_BoxOffsetX;
	GViewerSetup.m_BoxOffsetY = GVDlg->m_BoxOffsetY;
	GViewerSetup.m_BoxOffsetZ = GVDlg->m_BoxOffsetZ;
	GViewerSetup.m_ToolSize = GVDlg->m_ToolSize;
	GViewerSetup.m_AxisSize = GVDlg->m_AxisSize;
	GViewerSetup.m_ToolOffX = GVDlg->m_ToolOffX;
	GViewerSetup.m_ToolOffY = GVDlg->m_ToolOffY;
	GViewerSetup.m_ToolOffZ = GVDlg->m_ToolOffZ;
	GViewerSetup.m_IncludeA = GVDlg->m_IncludeA;
	GViewerSetup.m_IncludeB = GVDlg->m_IncludeB;
	GViewerSetup.m_IncludeC = GVDlg->m_IncludeC;
	GViewerSetup.m_IncludeToolAngles = GVDlg->m_IncludeToolAngles;

	if (GViewerSetup.DoModal() == IDOK)
	{
		GVDlg->m_ToolShapeFile = GViewerSetup.m_ToolShapeFile;
		GVDlg->m_BoxX = GViewerSetup.m_BoxX;
		GVDlg->m_BoxY = GViewerSetup.m_BoxY;
		GVDlg->m_BoxZ = GViewerSetup.m_BoxZ;
		GVDlg->m_BoxOffsetX = GViewerSetup.m_BoxOffsetX;
		GVDlg->m_BoxOffsetY = GViewerSetup.m_BoxOffsetY;
		GVDlg->m_BoxOffsetZ = GViewerSetup.m_BoxOffsetZ;
		GVDlg->m_ToolSize = GViewerSetup.m_ToolSize;
		GVDlg->m_AxisSize = GViewerSetup.m_AxisSize;
		GVDlg->m_ToolOffX = GViewerSetup.m_ToolOffX;
		GVDlg->m_ToolOffY = GViewerSetup.m_ToolOffY;
		GVDlg->m_ToolOffZ = GViewerSetup.m_ToolOffZ;
		GVDlg->m_IncludeA = GViewerSetup.m_IncludeA;
		GVDlg->m_IncludeB = GViewerSetup.m_IncludeB;
		GVDlg->m_IncludeC = GViewerSetup.m_IncludeC;
		GVDlg->m_IncludeToolAngles = GViewerSetup.m_IncludeToolAngles;

		DeleteAllScene();
		InitializeScene();
		GVDlg->SaveConfig();
	}
}


void CGViewParent::ChangeToolPosition()
{
	CKMotionCNCDlg *Dlg = &TheFrame->GCodeDlg;
	CGViewDlg *GVDlg = &TheFrame->GViewDlg;
	// check if tool changed

	bool UsingDefault, SceneWasDirty=m_SceneIsDirty;

	if (GetToolFileToDisplay(&UsingDefault).CompareNoCase(m_ToolFileDisplayed) || m_SceneIsDirty)
	{
		DeleteAllScene();
		InitializeScene();
	}

	CGCodeInterpreter *GC = Dlg->Interpreter;
	CCoordMotion *CM = GC->CoordMotion;
	double x=CM->current_x;
	double y=CM->current_y;
	double z=CM->current_z;
	double a=CM->current_a;
	double b=CM->current_b;
	double c=CM->current_c;
	
	if (!Dlg->m_Simulate)
	{
		if (Dlg->m_ConnectedForStatus)
		{
			if (CM->ReadCurAbsPosition(&x,&y,&z,&a,&b,&c))
			{
				return;
			}
		}
	}

	if (CM->Kinematics->RemapForNonStandardAxes(&x, &y, &z, &a, &b, &c)) return;

	setup_pointer p = Dlg->m_RealTimeSetup;

	// Tool Table is in User Units so do any necessary conversion
	x -= GC->UserUnitsToInchesX(p->tool_table[p->selected_tool_slot].xoffset);  // X is special because it handles Diam/Radius mode
	y -= GC->UserUnitsToInches(p->tool_table[p->selected_tool_slot].yoffset);
	z -= GC->UserUnitsToInches(p->tool_table[p->selected_tool_slot].length);

	CTransform TransformTool,TransformAxis;
	CVector3d OriginalOffset,RotOffset;
	CVector3d ToolOffset;
	CVector3d scale(1.0f,1.0f,1.0f);

	if (UsingDefault)
	{
		ToolOffset.Set(GVDlg->m_ToolOffX*GVDlg->m_ToolSize, GVDlg->m_ToolOffY*GVDlg->m_ToolSize, GVDlg->m_ToolOffZ*GVDlg->m_ToolSize);
		scale.Set(GVDlg->m_ToolSize,GVDlg->m_ToolSize,GVDlg->m_ToolSize);
	}

	MOTION_PARAMS *MP=&CM->Kinematics->m_MotionParams;

	// rotation about X axis changes Y and Z Axis as a function of A
	bool doA = GVDlg->m_IncludeA && MP->DegreesA && MP->RadiusA>0.0;
	if (doA)
	{
		double Theta=a*PI/180.0;
		double y0 = y+MP->RadiusA;
		y = y0 * cos(Theta) - z * sin(Theta);
		z = y0 * sin(Theta) + z * cos(Theta);
		TransformAxis.SetValueRotationYZ(a);
	}
	else
	{
		TransformAxis.SetValueRotationYZ(0.0);
	}


	// rotation about Y axis changes Z and X Axis as a function of B
	bool doB = GVDlg->m_IncludeB && MP->DegreesB && MP->RadiusB>0.0;
	if (doB)
	{
		double Theta=b*PI/180.0;
		double z0 = z+MP->RadiusB;
		z = z0 * cos(Theta) - x * sin(Theta);
		x = z0 * sin(Theta) + x * cos(Theta);
		TransformAxis.SetValueRotationZX(b);
	}
	else
	{
		TransformAxis.SetValueRotationZX(0.0);
	}


	// rotation about Z axis changes X and Y Axis as a function of C
	bool doC = GVDlg->m_IncludeC && MP->DegreesC && MP->RadiusC>0.0;
	if (doC)
	{
		double Theta=c*PI/180.0;
		double x0 = x+MP->RadiusC;
		x = x0 * cos(Theta) - y * sin(Theta);
		y = x0 * sin(Theta) + y * cos(Theta);
		TransformAxis.SetValueRotationXY(c);
	}
	else
	{
		TransformAxis.SetValueRotationXY(0.0);
	}

	CVector3d offset(x,y,z);

	if (GVDlg->m_IncludeToolAngles)
	{
		TransformTool.SetValueRotationYZ(a);
		TransformTool.SetValueRotationZX(b);
		TransformTool.SetValueRotationXY(c);
	}

	m_view.OpenGLMutex->Lock();
	
	CMesh3d *mesh;

	if (GVDlg->m_IncludeToolAngles)
		TransformTool.SetScale(scale);
	else
		TransformAxis.SetScale(scale);


	for (int i=StartIndexTool; i<EndIndexTool; i++)
	{
		mesh = (CMesh3d *)m_view.m_SceneGraph.GetAt(i);
		OriginalOffset.Copy(mesh->GetTransformOriginal()->GetTranslation());
		if (UsingDefault) OriginalOffset *= GVDlg->m_ToolSize;
		
		OriginalOffset += &ToolOffset;

		if (doA || GVDlg->m_IncludeToolAngles) OriginalOffset.RotateYZ(a*PI/180.0);
		if (doB || GVDlg->m_IncludeToolAngles) OriginalOffset.RotateZX(b*PI/180.0);
		if (doC || GVDlg->m_IncludeToolAngles) OriginalOffset.RotateXY(c*PI/180.0);

		OriginalOffset += &offset;

		if (GVDlg->m_IncludeToolAngles)
		{
			TransformTool.SetTranslation(OriginalOffset);
			mesh->SetTransform(TransformTool);
		}
		else
		{
			TransformAxis.SetTranslation(OriginalOffset);
			mesh->SetTransform(TransformAxis);
		}

		mesh->SetModified();
	}

	CVector3d scaleAxis(GVDlg->m_AxisSize,GVDlg->m_AxisSize,GVDlg->m_AxisSize);
	TransformAxis.SetScale(scaleAxis);


	if (doA || doB || doC || SceneWasDirty)
	{
		for (int i=StartIndexAxis; i<EndIndexAxis; i++)
		{
			mesh = (CMesh3d *)m_view.m_SceneGraph.GetAt(i);
			OriginalOffset.Copy(mesh->GetTransformOriginal()->GetTranslation());
			OriginalOffset *= GVDlg->m_AxisSize;
			if (doA) OriginalOffset.RotateYZ(a*PI / 180.0);
			if (doB) OriginalOffset.RotateZX(b*PI / 180.0);
			if (doC) OriginalOffset.RotateXY(c*PI / 180.0);
			TransformAxis.SetTranslation(OriginalOffset);
			mesh->SetTransform(TransformAxis);
			mesh->SetModified();
		}
	}

	m_view.OpenGLMutex->Unlock();
}

