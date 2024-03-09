// GViewParent.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#pragma once
#include "DlgX.h"
#include "dlgbars.h"
#include "OpenglCtl.h"


#define NBOXPATHS 4

enum ViewDir { VIEWXY, VIEWYZ, VIEWXZ };


/////////////////////////////////////////////////////////////////////////////
// CGViewParent to be used by a dialog

class CGViewParent
{
// Construction
public:
	void ChangeToolPosition();
	int FindExtents();
	
	CString m_AxisShapeFile;
	CString m_ToolFileDisplayed;

	float m_minx;
	float m_maxx;
	float m_miny;
	float m_maxy;
	float m_minz;
	float m_maxz;
	

	~CGViewParent(); 
	void SetViewDistance(ViewDir View);

	CColor m_ColorBox;

	bool m_SceneIsInitialized;
	bool m_SceneIsDirty;
	bool m_FirstScreenDisplay;


	CGViewParent();   // standard constructor
	void ClearPaths();
	void AddBox();
	void AddAxisToScene();
	void AddToolToScene();
	
	CPath3d *m_Path;
	CPath3d *m_PathBox[NBOXPATHS];

	void InitializeScene();
	void DeleteAllScene();
	int StartIndexAxis,EndIndexAxis;
	int StartIndexTool,EndIndexTool;

	COpenglCtl	m_view;



// Implementation

	 BOOL OnInitDialog();
	 void OnXy();
	 void OnXz();
	 void OnYz();
	 void OnClearPaths();
	 void OnShowAxis();
	 void OnOrtho();
	 void OnBox();
	 void OnRotXY();
	 void OnGViewerSetup();
	 void OnShowTool();
	CString GetToolFileToDisplay(bool *UsingDefault);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

