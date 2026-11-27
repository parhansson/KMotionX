// PreviewFileDialog.h : header file
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPreviewFileDialog dialog



class CAbstractPreview
{
public:
	virtual void SetPreviewFile(const CString& csFileName)=0;
	virtual void DrawPreview(CDC *pDC,int x,int y,int width,int height)=0;
	CString m_csFileName;
};





class CPreviewFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CPreviewFileDialog)

public:
	CPreviewFileDialog(CAbstractPreview *pPreview,BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
		COpenglCtl	m_view;

	~CPreviewFileDialog(); 

	void ChangeToolPosition();

protected:
	//{{AFX_MSG(CPreviewFileDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedPreview();
	afx_msg void OnPaint();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnFileNameChange();
	void OnFileNameChange(bool FolderChanged);
	virtual void OnFolderChange();
	virtual void OnInitDone();

	BOOL m_bPreview;
	CAbstractPreview *m_pPreview;

	BOOL m_bChanged;
	bool FirstFolderChange;

	int FindExtents();
	int FindAxisExtents();
	
	CString m_ToolShapeFile;
	CString m_AxisShapeFile;
	CString m_ToolFileDisplayed;

	float m_minx;
	float m_maxx;
	float m_miny;
	float m_maxy;
	float m_minz;
	float m_maxz;
	
	float m_Axisminx;
	float m_Axismaxx;
	float m_Axisminy;
	float m_Axismaxy;
	float m_Axisminz;
	float m_Axismaxz;
	
	float m_AxisSize;
	float m_ToolSize;
	float m_ToolOffX;
	float m_ToolOffY;
	float m_ToolOffZ;

	BOOL m_IncludeA;
	BOOL m_IncludeB;
	BOOL m_IncludeC;

	void SetViewDistance(ViewDir View);

	CColor m_ColorBox;

    CDlgToolBar *m_GViewTools;

	bool m_SceneIsInitialized;
	bool m_FirstScreenDisplay;


	void ClearPaths();
	void AddAxisToScene();
	void AddToolToScene();
	
	CPath3d *m_Path;

	void InitializeScene();
	void DeleteAllScene();
	int StartIndexAxis,EndIndexAxis;
	int StartIndexTool,EndIndexTool;


	BOOL m_ShowAxis,m_ShowTool;

protected:

	// Generated message map functions
	//{{AFX_MSG(CGViewDlg)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnXy();
	afx_msg void OnXz();
	afx_msg void OnYz();
	afx_msg void OnClearPaths();
	afx_msg void OnShowAxis();
	afx_msg void OnUpdateShowAxis(CCmdUI* pCmdUI);
	afx_msg void OnRotXY();
	afx_msg void OnUpdateRotXY(CCmdUI* pCmdUI);
	afx_msg void OnShowTool();
	afx_msg void OnUpdateShowTool(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	//}}AFX_MSG
private:
	CString GetToolFileToDisplay();
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMove(int x, int y);
};


