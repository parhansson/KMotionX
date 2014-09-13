// DlgX.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#pragma once
#include "stdafx.h"
#include "DlgX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgX dialog


CDlgX::CDlgX(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgX::IDD, pParent)
{
	LastMoveX=LastMoveY=100;
	LastSizeX=LastSizeY=500;
	//{{AFX_DATA_INIT(CDlgX)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

UINT NEAR CDlgX::UpdateDataMessage = RegisterWindowMessage("DLGX_UPDATE_DATA");



BEGIN_MESSAGE_MAP(CDlgX, CDialog)
	//{{AFX_MSG_MAP(CDlgX)
    ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )
    ON_REGISTERED_MESSAGE( UpdateDataMessage, ThreadSafeUpdateData )
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)

	ON_WM_INITMENUPOPUP()

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgX message handlers

BOOL CDlgX::OnInitDialog() 
{
	CDialog::OnInitDialog();

	EnableToolTips(TRUE);

	m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_App16));
    SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
	m_hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_App32));
    SendMessage(WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);

	UpdateParameters();

	InitDialogComplete=TRUE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void AFXAPI CDlgX::DDV_MinMaxDouble(CDataExchange* pDX, double const& value, double minVal, double maxVal)
{
		::DDV_MinMaxDouble(pDX, value, minVal, maxVal);
}


void AFXAPI CDlgX::DDX_Text(CDataExchange* pDX, int nIDC, double& value)
{
	::DDX_Text(pDX, nIDC, value);
}

void AFXAPI CDlgX::DDX_Text(CDataExchange* pDX, int nIDC, float& value)
{
	::DDX_Text(pDX, nIDC, value);
}

void AFXAPI CDlgX::DDX_Text(CDataExchange* pDX, int nIDC, int & value)
{
	::DDX_Text(pDX, nIDC, value);
}

void AFXAPI CDlgX::DDX_Text(CDataExchange* pDX, int nIDC, CString &value)
{
	::DDX_Text(pDX, nIDC, value);
}



int CDlgX::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	InitDialogComplete=FALSE;
		
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	
	if (LastMoveX > nScreenWidth  - 30) LastMoveX = nScreenWidth - 30;
	if (LastMoveY > nScreenHeight - 30) LastMoveY = nScreenHeight - 30;


	// For EasySize save original raw dialog size 
	GetWindowRect(OrigWindowRect);
	GetClientRect(OrigClientRect);

	// check if the window has a thick re-sizing border
	// if so, then set the position and size, otherwise just
	// the position

	if ((lpCreateStruct->style & WS_THICKFRAME) == 0)   
		SetWindowPos(NULL,LastMoveX,LastMoveY,0,0,SWP_NOZORDER|SWP_NOSIZE);
	else
		SetWindowPos(NULL,LastMoveX,LastMoveY,LastSizeX,LastSizeY,SWP_NOZORDER);
	
	return 0;
}

void CDlgX::OnOK() 
{
	if (!UpdateData(TRUE)) return;
	CDialog::OnOK();
	InitDialogComplete=FALSE;
    DestroyWindow();
}

void CDlgX::OnClose() 
{
	if (!UpdateData(TRUE)) return;
	CDialog::OnClose();
	InitDialogComplete=FALSE;
    DestroyWindow();
}

void CDlgX::OnCancel() 
{
	if (!UpdateData(TRUE)) return;
	CDialog::OnClose();
	InitDialogComplete=FALSE;
    DestroyWindow();
}

void CDlgX::OnExit() 
{
	if (!UpdateData(TRUE)) return;
	CDialog::OnClose();
	InitDialogComplete=FALSE;
    DestroyWindow();
}

afx_msg LRESULT  CDlgX::ThreadSafeUpdateData(WPARAM w,LPARAM l)
{
	UpdateData(FALSE);
	return 0;
}

void CDlgX::OnMove(int x, int y) 
{
	WINDOWPLACEMENT wp;
	RECT rect;

	CDialog::OnMove(x, y);

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

void CDlgX::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	InvalidateRect(NULL);
}

void CDlgX::OnPaint() 
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
	CDialog::OnPaint();
}

void CDlgX::SaveOnExit(FILE * f)
{
	fprintf(f,"%d %d %d %d\n",LastMoveX,LastMoveY,LastSizeX,LastSizeY);
}

void CDlgX::RestoreOnStart(FILE * f)
{
	fscanf(f,"%d %d %d %d",&LastMoveX,&LastMoveY,
						   &LastSizeX,&LastSizeY);
}


	
	
BOOL CDlgX::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{    
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;    UINT nID =pNMHDR->idFrom;
    
	if (pTTT->uFlags & TTF_IDISHWND)    
	{
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);        
		
		if(nID)        
		{
			if (ToolTipText.LoadString(nID))
			{
                pTTT->lpszText = ToolTipText.GetBuffer(500);
			    return(TRUE);
			}
        }   
	}    
	return(FALSE);
}

int CDlgX::LookForCString(const char * s, const char * c, CString * b)
{
	unsigned int i;

	for (i=0; i<strlen(c); i++)
		if (s[i] != c[i]) return 1;
	
	*b = s+i;

	b->Remove('\r');
	b->Remove('\n');
	return 0;
}


void CDlgX::LookForChars(const char * s, const char * c, char * b)
{
	CString ss;
	unsigned int i;

	for (i=0; i<strlen(c); i++)
		if (s[i] != c[i]) return;
	
	ss = s+i;

	ss.Remove('\r');
	ss.Remove('\n');

	strcpy(b,ss);
}



void CDlgX::LookForFloat(const char * s, const char * c, float * b)
{
	unsigned int i;

	for (i=0; i<strlen(c); i++)
		if (s[i] != c[i]) return;
	sscanf(s+i,"%f",b);
}

void CDlgX::LookForIIR(const char * s, const char * c, float *f0, float *f1, float *f2, float *f3, float *f4)
{
	unsigned int i;

	for (i=0; i<strlen(c); i++)
		if (s[i] != c[i]) return;
	
	sscanf(s+i,"%f%f%f%f%f",f0,f1,f2,f3,f4);
}

void CDlgX::LookForDouble(const char * s, const char * c, double * b)
{
	unsigned int i;

	for (i=0; i<strlen(c); i++)
		if (s[i] != c[i]) return;
	sscanf(s+i,"%lf",b);
}

void CDlgX::LookForInt(const char * s, const char * c, int * b)
{
	unsigned int i;

	for (i=0; i<strlen(c); i++)
		if (s[i] != c[i]) return;
	sscanf(s+i,"%d",b);
}

void CDlgX::LookForHex(const char * s, const char * c, int * b)
{
	unsigned int i;

	for (i=0; i<strlen(c); i++)
		if (s[i] != c[i]) return;
	sscanf(s+i,"%x",b);
}


// check if the filename contains the specified path
// and no further path, if so strip it from the name


CString CDlgX::StripPathMatch(CString FileName, CString DefaultPath)
{
	CString FileNameUpper;

	DefaultPath.MakeUpper();

	FileNameUpper = FileName;
	FileNameUpper.MakeUpper();

	if (FileNameUpper.Find(DefaultPath) == 0 && 	
		FileNameUpper.Right(FileNameUpper.GetLength() - DefaultPath.GetLength()).Find('\\') == -1)
	{
		FileName.Delete(0,DefaultPath.GetLength());
	}
	return FileName;
}




BOOL CDlgX::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
     ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

     // if there is a top level routing frame then let it handle the message
     if (GetRoutingFrame() != NULL) return FALSE;

     // to be thorough we will need to handle UNICODE versions of the message also !!
     TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
     TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
     TCHAR szFullText[512];
     CString strTipText;
     UINT nID = pNMHDR->idFrom;

     if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
         pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
     {
          // idFrom is actually the HWND of the tool 
          nID = ::GetDlgCtrlID((HWND)nID);
     }

     if (nID != 0) // will be zero on a separator
     {
          AfxLoadString(nID, szFullText);
          strTipText=szFullText;

#ifndef _UNICODE
          if (pNMHDR->code == TTN_NEEDTEXTA)
          {
               lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
          }
          else
          {
               _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
          }
#else
          if (pNMHDR->code == TTN_NEEDTEXTA)
          {
               _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
          }
          else
          {
               lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
          }
#endif

          *pResult = 0;

          // bring the tooltip window above other popup windows
          ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|
               SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER); return TRUE;
     }

	 return FALSE;
} 



void CDlgX::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
    ASSERT(pPopupMenu != NULL);
    // Check the enabled state of various menu items.

    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pParentMenu == NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == NULL indicates that it is secondary popup).
    HMENU hParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
    else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
    {
        CWnd* pParent = this;
           // Child windows don't have menus--need to go to the top!
        if (pParent != NULL &&
           (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
        {
           int nIndexMax = ::GetMenuItemCount(hParentMenu);
           for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
           {
            if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
            {
                // When popup is found, m_pParentMenu is containing menu.
                state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
                break;
            }
           }
        }
    }

    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
      state.m_nIndex++)
    {
        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
        if (state.m_nID == 0)
           continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(state.m_pOther == NULL);
        ASSERT(state.m_pMenu != NULL);
        if (state.m_nID == (UINT)-1)
        {
           // Possibly a popup menu, route to first item of that popup.
           state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
           if (state.m_pSubMenu == NULL ||
            (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
            state.m_nID == (UINT)-1)
           {
            continue;       // First item of popup can't be routed to.
           }
           state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
        }
        else
        {
           // Normal menu item.
           // Auto enable/disable if frame window has m_bAutoMenuEnable
           // set and command is _not_ a system command.
           state.m_pSubMenu = NULL;
           state.DoUpdate(this, FALSE);
        }

        // Adjust for menu deletions and additions.
        UINT nCount = pPopupMenu->GetMenuItemCount();
        if (nCount < state.m_nIndexMax)
        {
           state.m_nIndex -= (state.m_nIndexMax - nCount);
           while (state.m_nIndex < nCount &&
            pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
           {
            state.m_nIndex++;
           }
        }
        state.m_nIndexMax = nCount;
    }
}


CString CDlgX::ExtractDirectory(CString s)
{
	CString sDrive,sDir,sFileName,sExt;

	_splitpath_s(s,sDrive.GetBufferSetLength(_MAX_DRIVE), _MAX_DRIVE, 
		sDir.GetBufferSetLength(_MAX_DIR), _MAX_DIR,
		sFileName.GetBufferSetLength(_MAX_FNAME), _MAX_FNAME, 
		sExt.GetBufferSetLength(_MAX_EXT), _MAX_EXT);
	
	sDrive.ReleaseBuffer();
	sDir.ReleaseBuffer();
	sFileName.ReleaseBuffer();
	sExt.ReleaseBuffer();

	return sDrive + sDir;
}

