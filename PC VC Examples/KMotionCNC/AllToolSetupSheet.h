// AllToolSetupSheet.h : header file
//

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// CAllToolSetupSheet
#pragma once
#include "MySheet.h"
#include "ToolSetupM1Page.h"
#include "ToolSetupM100Page.h"
#include "ToolSetupButtonsPage.h"
#include "ToolSetupFilesPage.h"
#include "ToolSetupTPPage.h"
#include "..\GCodeInterpreter\GCodeInterpreter.h"


class CAllToolSetupSheet : public CMySheet
{
	DECLARE_DYNAMIC(CAllToolSetupSheet)

// Construction
public:
	CAllToolSetupSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CAllToolSetupSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual void RestoreOnStart(FILE *f);
	virtual void SaveOnExit(FILE *f);
	int LastMoveX;
	int LastMoveY;
	int LastSizeX;
	int LastSizeY;
	BOOL InitDialogComplete;

protected:
	int AddControlPages(void);

// Attributes
public:

	CToolSetupM1Page       m_ToolSetupM1Page;
	CToolSetupM100Page     m_ToolSetupM100Page;
	CToolSetupButtonsPage  m_ToolSetupButtonsPage;
	CToolSetupFilesPage    m_ToolSetupFilesPage;
	CToolSetupTPPage       m_ToolSetupTPPage;


// Dialog Data
	//{{AFX_DATA(CToolFile)
	//}}AFX_DATA





// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAllToolSetupSheet)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAllToolSetupSheet();
	virtual BOOL OnInitDialog();

	// Generated message map functions
protected:


	//{{AFX_MSG(CAllToolSetupSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnExit();
	virtual void OnCancel(); 
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL, DWORD dwStyle = (DWORD)-1, DWORD dwExStyle = 0);
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
};

/////////////////////////////////////////////////////////////////////////////
