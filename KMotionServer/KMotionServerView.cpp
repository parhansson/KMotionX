// KMotionServerView.cpp : implementation of the CKMotionServerView class
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"
#include "KMotionServer.h"

#include "KMotionServerDoc.h"
#include "KMotionServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerView

IMPLEMENT_DYNCREATE(CKMotionServerView, CView)

BEGIN_MESSAGE_MAP(CKMotionServerView, CView)
	//{{AFX_MSG_MAP(CKMotionServerView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerView construction/destruction

CKMotionServerView::CKMotionServerView()
{
	// TODO: add construction code here

}

CKMotionServerView::~CKMotionServerView()
{
}

BOOL CKMotionServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerView drawing

void CKMotionServerView::OnDraw(CDC* pDC)
{
	CKMotionServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerView printing

BOOL CKMotionServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKMotionServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKMotionServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerView diagnostics

#ifdef _DEBUG
void CKMotionServerView::AssertValid() const
{
	CView::AssertValid();
}

void CKMotionServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKMotionServerDoc* CKMotionServerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKMotionServerDoc)));
	return (CKMotionServerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerView message handlers
