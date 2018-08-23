// GViewDlg.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#pragma once
#include "DlgX.h"
#include "dlgbars.h"
#include "OpenglCtl.h"
#include "GViewParent.h"




/////////////////////////////////////////////////////////////////////////////
// CGViewDlg dialog

class CGViewDlg : public CDlgX
{
// Construction
public:
	CGViewParent GViewParent;
	void ChangeToolPosition();
	int FindExtents();
	
	CString m_ToolShapeFile;
	float m_BoxX;
	float m_BoxY;
	float m_BoxZ;
	float m_BoxOffsetX;
	float m_BoxOffsetY;
	float m_BoxOffsetZ;
	float m_ToolSize;
	float m_ToolOffX;
	float m_ToolOffY;
	float m_ToolOffZ;
	float m_AxisSize;
	BOOL m_ShowAxis, m_ShowBox, m_ShowTool, m_Ortho;
	BOOL m_IncludeA;
	BOOL m_IncludeB;
	BOOL m_IncludeC;
	BOOL m_IncludeToolAngles;

	~CGViewDlg(); 
	void SaveOnExit(FILE * f);
	void RestoreOnStart(FILE * f);
	int SaveLoadConfig(FILE *f, char *s, bool save);
	int LoadConfig();
	int SaveConfig();
	void RefreshTitle(); 

    CDlgToolBar *m_GViewTools;


	CGViewDlg(CWnd* pParent = NULL);   // standard constructor
	

	
// Dialog Data
	//{{AFX_DATA(CGViewDlg)
	enum { IDD = IDD_GView };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGViewDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation



protected:

	// Generated message map functions
	//{{AFX_MSG(CGViewDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnXy();
	afx_msg void OnXz();
	afx_msg void OnYz();
	afx_msg void OnClearPaths();
	afx_msg void OnShowAxis();
	afx_msg void OnOrtho();
	afx_msg void OnBox();
	afx_msg void OnUpdateBox(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowAxis(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOrtho(CCmdUI* pCmdUI);
	afx_msg void OnRotXY();
	afx_msg void OnUpdateRotXY(CCmdUI* pCmdUI);
	afx_msg void OnGViewerSetup();
	afx_msg void OnShowTool();
	afx_msg void OnUpdateShowTool(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString GetToolFileToDisplay(bool *UsingDefault);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMove(int x, int y);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

