// KMotionServerDoc.h : interface of the CKMotionServerDoc class
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined(AFX_KMOTIONSERVERDOC_H__D896A501_9B01_4AD5_8C96_77ADC23B9C1C__INCLUDED_)
#define AFX_KMOTIONSERVERDOC_H__D896A501_9B01_4AD5_8C96_77ADC23B9C1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CKMotionServerDoc : public CDocument
{
protected: // create from serialization only
	CKMotionServerDoc();
	DECLARE_DYNCREATE(CKMotionServerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKMotionServerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CKMotionServerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CKMotionServerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KMOTIONSERVERDOC_H__D896A501_9B01_4AD5_8C96_77ADC23B9C1C__INCLUDED_)
