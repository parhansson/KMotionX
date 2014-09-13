// BsDocTemplate.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#include "stdafx.h"

#include "afxwin.h"

#include "BsDocTemplate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BsDocTemplate

//IMPLEMENT_DYNCREATE(BsDocTemplate, CSingleDocTemplate)

BsDocTemplate::BsDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
							 CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass):
				CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}


BsDocTemplate::~BsDocTemplate()
{
}


BEGIN_MESSAGE_MAP(BsDocTemplate, CSingleDocTemplate)
	//{{AFX_MSG_MAP(BsDocTemplate)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BsDocTemplate diagnostics

#ifdef _DEBUG
void BsDocTemplate::AssertValid() const
{
	CSingleDocTemplate::AssertValid();
}

void BsDocTemplate::Dump(CDumpContext& dc) const
{
	CSingleDocTemplate::Dump(dc);
}
#endif //_DEBUG


CDocument* BsDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName,
	BOOL bMakeVisible)
	// if lpszPathName == NULL => create new file of this type
{
	CDocument* pDocument = NULL;
	CFrameWnd* pFrame = NULL;
	BOOL bCreated = FALSE;      // => doc and frame created
	BOOL bWasModified = FALSE;

	if (m_pOnlyDoc != NULL)
	{
		// already have a document - reinit it
		pDocument = m_pOnlyDoc;
		if (!pDocument->SaveModified())
			return NULL;        // leave the original one

		pFrame = (CFrameWnd*)AfxGetMainWnd();
		ASSERT(pFrame != NULL);
		ASSERT_KINDOF(CFrameWnd, pFrame);
		ASSERT_VALID(pFrame);
	}
	else
	{
		// create a new document
		pDocument = CreateNewDocument();
		ASSERT(pFrame == NULL);     // will be created below
		bCreated = TRUE;
	}

	if (pDocument == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC,MB_OK|MB_SYSTEMMODAL);
		return NULL;
	}
	ASSERT(pDocument == m_pOnlyDoc);

	if (pFrame == NULL)
	{
		ASSERT(bCreated);

		// create frame - set as main document frame
		BOOL bAutoDelete = pDocument->m_bAutoDelete;
		pDocument->m_bAutoDelete = FALSE;
					// don't destroy if something goes wrong
		pFrame = CreateNewFrame(pDocument, NULL);
		pDocument->m_bAutoDelete = bAutoDelete;
		if (pFrame == NULL)
		{
			AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC,MB_OK|MB_SYSTEMMODAL);
			delete pDocument;       // explicit delete on error
			return NULL;
		}
	}

	if (lpszPathName == NULL)
	{
		// create a new document
		SetDefaultTitle(pDocument);

		// avoid creating temporary compound file when starting up invisible
		if (!bMakeVisible)
			pDocument->m_bEmbedded = TRUE;

		if (!pDocument->OnNewDocument())
		{
			// user has been alerted to what failed in OnNewDocument
			TRACE0("CDocument::OnNewDocument returned FALSE.\n");
			if (bCreated)
				pFrame->DestroyWindow();    // will destroy document
			return NULL;
		}
	}
	else
	{
		CWaitCursor wait;

		// open an existing document
		bWasModified = pDocument->IsModified();
		pDocument->SetModifiedFlag(FALSE);  // not dirty for open

		if (!pDocument->OnOpenDocument(lpszPathName))
		{
			// user has been alerted to what failed in OnOpenDocument
			TRACE0("CDocument::OnOpenDocument returned FALSE.\n");
			if (bCreated)
			{
				pFrame->DestroyWindow();    // will destroy document
			}
			else if (!pDocument->IsModified())
			{
				// original document is untouched
				pDocument->SetModifiedFlag(bWasModified);
			}
			else
			{
				// we corrupted the original document
				SetDefaultTitle(pDocument);

				if (!pDocument->OnNewDocument())
				{
					TRACE0("Error: OnNewDocument failed after trying to open a document - trying to continue.\n");
					// assume we can continue
				}
			}
			return NULL;        // open failed
		}
		pDocument->SetPathName(lpszPathName);
	}

	CWinThread* pThread = AfxGetThread();
	if (bCreated && pThread->m_pMainWnd == NULL)
	{
		// set as main frame (InitialUpdateFrame will show the window)
		pThread->m_pMainWnd = pFrame;
	}
//AR
//	InitialUpdateFrame(pFrame, pDocument, bMakeVisible);
	InitialUpdateFrame(pFrame, pDocument, FALSE);

	return pDocument;
}


