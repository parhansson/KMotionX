// TransformDlg.h
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#if !defined(AFX_TRANSFORMDLG_H__25AC65BF_BF4A_45E2_9212_B08F92BD42BD__INCLUDED_)
#define AFX_TRANSFORMDLG_H__25AC65BF_BF4A_45E2_9212_B08F92BD42BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransformDlg.h : header file
//


struct SEGMENT_STRUCT;

enum SmoothType { Recursive, RunLength, IIRLowPass };

/////////////////////////////////////////////////////////////////////////////
// CTransformDlg dialog

class CTransformDlg : public CDialog
{
// Construction
public:
	int ExtractValues(CString s, CString &r);
	int RoundCorner(double * Values, int n, double R, CString & r);
	int DoSmooth(double *Values, int n, SmoothType Type, CString &r);
	bool CheckSequenceColl(double * p, int i, int k);
	void SetSegmentFromV(SEGMENT_STRUCT *seg, double *p, int i);
	int DoSmooth2(double * Values, int n, CString & r);
	double ComputeLength(double *v);
	int AntiGouge(double *Values, double *IJValues, int n, CString &r);
	CTransformDlg(CWnd* pParent = NULL);   // standard constructor
	int PutValue(double Value, CString &news);

	CString Selection;

// Dialog Data
	//{{AFX_DATA(CTransformDlg)
	enum { IDD = IDD_Transform };
	double	m_OffsetX;
	double	m_OffsetY;
	double	m_OffsetZ;
	double	m_ScaleX;
	double	m_ScaleY;
	double	m_ScaleZ;
	UINT	m_Digits;
	UINT	m_RunLength;
	UINT	m_Recursions;
	double	m_UpSample;
	double	m_ColTolSmooth;
	double	m_TauSmooth;
	double	m_CornerRadius;
	BOOL	m_ScaleIJ;
	//}}AFX_DATA

private:
		double *Values;
		int nValues;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransformDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int ProcessVar(CString &t, CString v, double &value, double &PrevValue, double scale, double offset);
	// Generated message map functions
	//{{AFX_MSG(CTransformDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void Clicked_Transform();
	afx_msg void Clicked_RunningAvg();
	afx_msg void Clicked_3PtRecursive();
	afx_msg void Clicked_IIRLowPass();
	afx_msg void Clicked_RoundCorner();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFORMDLG_H__25AC65BF_BF4A_45E2_9212_B08F92BD42BD__INCLUDED_)
