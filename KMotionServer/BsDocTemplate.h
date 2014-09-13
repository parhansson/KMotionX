// BsDocTemplate.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#if !defined(AFX_BSDOCTEMPLATE_H__95B37176_CB54_421D_BF87_E1104702108D__INCLUDED_)
#define AFX_BSDOCTEMPLATE_H__95B37176_CB54_421D_BF87_E1104702108D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// BsDocTemplate document

class BsDocTemplate : public CSingleDocTemplate
{
public:
//	BsDocTemplate();
	BsDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);           // protected constructor used by dynamic creation
//	DECLARE_DYNCREATE(BsDocTemplate)


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BsDocTemplate)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~BsDocTemplate();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(BsDocTemplate)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BSDOCTEMPLATE_H__95B37176_CB54_421D_BF87_E1104702108D__INCLUDED_)
