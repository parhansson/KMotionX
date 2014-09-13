// AllToolSetupSheet.cpp : implementation file
//

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "AllToolSetupSheet.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAllToolSetupSheet

IMPLEMENT_DYNAMIC(CAllToolSetupSheet, CMySheet)

CAllToolSetupSheet::CAllToolSetupSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMySheet(nIDCaption, pParentWnd, iSelectPage)
{
	LastMoveX=LastMoveY=100;
	LastSizeX=LastSizeY=500;
	AddControlPages();
}

CAllToolSetupSheet::CAllToolSetupSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMySheet(pszCaption, pParentWnd, iSelectPage)
{
	//{{AFX_DATA_INIT(CToolFile)
	//}}AFX_DATA_INIT
	LastMoveX=LastMoveY=100;
	LastSizeX=LastSizeY=500;
	AddControlPages();
}

CAllToolSetupSheet::~CAllToolSetupSheet()
{
}


int CAllToolSetupSheet::AddControlPages()
{
	while (GetPageCount()) 
		RemovePage(0);

	AddPage(&m_ToolSetupM1Page);
	AddPage(&m_ToolSetupM100Page);
	AddPage(&m_ToolSetupButtonsPage);
	AddPage(&m_ToolSetupFilesPage);
	AddPage(&m_ToolSetupTPPage);
	
	if (GetPageCount() == 0)
	{
		AfxMessageBox("No Tool Setup Tabs");
		return 1;
	}

	m_KeepCancelButton=true;

	return 0;
}

BEGIN_MESSAGE_MAP(CAllToolSetupSheet, CMySheet)
	//{{AFX_MSG_MAP(CAllToolSetupSheet)
    ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllToolSetupSheet message handlers

BOOL CAllToolSetupSheet::OnInitDialog()
{
	CMySheet::OnInitDialog();
	SetWindowPos(NULL,LastMoveX,LastMoveY,0,0,SWP_NOZORDER|SWP_NOSIZE);
	InitDialogComplete=TRUE;
	m_ToolSetupM1Page.InitDialogComplete=TRUE;
	m_ToolSetupM100Page.InitDialogComplete=TRUE;
	m_ToolSetupButtonsPage.InitDialogComplete=TRUE;
	m_ToolSetupTPPage.InitDialogComplete=TRUE;
	return TRUE;
}

int CAllToolSetupSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	InitDialogComplete=FALSE;
		
	if (CMySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	
	if (LastMoveX > nScreenWidth  - 30) LastMoveX = nScreenWidth - 30;
	if (LastMoveY > nScreenHeight - 30) LastMoveY = nScreenHeight - 30;

	// check if the window has a thick re-sizing border
	// if so, then set the position and size, otherwise just
	// the position

	if ((lpCreateStruct->style & WS_THICKFRAME) == 0)   
		SetWindowPos(NULL,LastMoveX,LastMoveY,0,0,SWP_NOZORDER|SWP_NOSIZE);
	else
		SetWindowPos(NULL,LastMoveX,LastMoveY,LastSizeX,LastSizeY,SWP_NOZORDER);
	
	return 0;
}

void CAllToolSetupSheet::OnClose() 
{
	CMySheet::OnClose();
	InitDialogComplete=FALSE;
    DestroyWindow();
}

void CAllToolSetupSheet::OnCancel() 
{
	CMySheet::OnClose();
	InitDialogComplete=FALSE;
    DestroyWindow();
}

void CAllToolSetupSheet::OnExit() 
{
	CMySheet::OnClose();
	InitDialogComplete=FALSE;
    DestroyWindow();
}


void CAllToolSetupSheet::OnMove(int x, int y) 
{
	WINDOWPLACEMENT wp;
	RECT rect;

	CMySheet::OnMove(x, y);

	wp.length = sizeof(WINDOWPLACEMENT);  //not sure if this is still required, but what the hey 
	GetWindowPlacement(&wp);

	if (InitDialogComplete && wp.showCmd!=SW_SHOWMAXIMIZED && wp.showCmd!=SW_SHOWMINIMIZED)  // save the dialog window position
	{
        GetWindowRect(&rect);
		LastMoveX = rect.left;
		LastMoveY = rect.top;
		LastSizeX = rect.right - rect.left;
		LastSizeY = rect.bottom - rect.top;
	}
}

void CAllToolSetupSheet::OnSize(UINT nType, int cx, int cy) 
{
	CMySheet::OnSize(nType, cx, cy);
	InvalidateRect(NULL);
}

void CAllToolSetupSheet::OnPaint() 
{
	WINDOWPLACEMENT wp;
	RECT rect;

	wp.length = sizeof(WINDOWPLACEMENT);  //not sure if this is still required, but what the hey 
	GetWindowPlacement(&wp);

	if (InitDialogComplete && wp.showCmd!=SW_SHOWMAXIMIZED && wp.showCmd!=SW_SHOWMINIMIZED)  // save the dialog window position
	{
        GetWindowRect(&rect);
		LastMoveX = rect.left;
		LastMoveY = rect.top;
		LastSizeX = rect.right - rect.left;
		LastSizeY = rect.bottom - rect.top;
	}
	CMySheet::OnPaint();
}

void CAllToolSetupSheet::SaveOnExit(FILE * f)
{
	fprintf(f,"%d %d %d %d\n",LastMoveX,LastMoveY,LastSizeX,LastSizeY);
}

void CAllToolSetupSheet::RestoreOnStart(FILE * f)
{
	fscanf(f,"%d %d %d %d",&LastMoveX,&LastMoveY,
						   &LastSizeX,&LastSizeY);
}


BOOL CAllToolSetupSheet::Create(CWnd* pParentWnd , DWORD dwStyle , DWORD dwExStyle)
{
	if (AddControlPages()) return false;
	return CMySheet::Create(pParentWnd, dwStyle, dwExStyle);
}

BOOL CAllToolSetupSheet::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	return CMySheet::DestroyWindow();
}

void CAllToolSetupSheet::DoDataExchange(CDataExchange* pDX)
{
	CMySheet::DoDataExchange(pDX);
}
