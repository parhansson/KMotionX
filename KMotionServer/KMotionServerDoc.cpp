// KMotionServerDoc.cpp : implementation of the CKMotionServerDoc class
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"
#include "KMotionServer.h"

#include "KMotionServerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerDoc

IMPLEMENT_DYNCREATE(CKMotionServerDoc, CDocument)

BEGIN_MESSAGE_MAP(CKMotionServerDoc, CDocument)
	//{{AFX_MSG_MAP(CKMotionServerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerDoc construction/destruction

CKMotionServerDoc::CKMotionServerDoc()
{
	// TODO: add one-time construction code here

}

CKMotionServerDoc::~CKMotionServerDoc()
{
}

BOOL CKMotionServerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CKMotionServerDoc serialization

void CKMotionServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerDoc diagnostics

#ifdef _DEBUG
void CKMotionServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CKMotionServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CKMotionServerDoc commands
