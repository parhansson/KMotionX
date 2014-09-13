// KMotionServerView.h : interface of the CKMotionServerView class
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined(AFX_KMOTIONSERVERVIEW_H__E0F1EB1A_CF87_4661_BE1B_4CE71882DCA9__INCLUDED_)
#define AFX_KMOTIONSERVERVIEW_H__E0F1EB1A_CF87_4661_BE1B_4CE71882DCA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CKMotionServerView : public CView
{
protected: // create from serialization only
	CKMotionServerView();
	DECLARE_DYNCREATE(CKMotionServerView)

// Attributes
public:
	CKMotionServerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKMotionServerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CKMotionServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CKMotionServerView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in KMotionServerView.cpp
inline CKMotionServerDoc* CKMotionServerView::GetDocument()
   { return (CKMotionServerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KMOTIONSERVERVIEW_H__E0F1EB1A_CF87_4661_BE1B_4CE71882DCA9__INCLUDED_)
