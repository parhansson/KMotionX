// DlgX.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#if !defined(AFX_DLGX_H__ABAADC21_01DC_11D2_810A_004033E07FBE__INCLUDED_)
#define AFX_DLGX_H__ABAADC21_01DC_11D2_810A_004033E07FBE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#define FLOAT(x)					\
	if (save)						\
	{								\
		fprintf(f,#x "=%23.15e\n",x);  \
	}								\
	else							\
	{								\
		LookForFloat(s,#x"=",&x);	\
	}							

#define DOUBLE(x)					\
	if (save)						\
	{								\
		fprintf(f,#x "=%23.15e\n",x);  \
	}								\
	else							\
	{								\
		LookForDouble(s,#x"=",&x);	\
	}							

#define INT(x)					\
	if (save)						\
	{								\
		fprintf(f,#x "=%d\n",x);  \
	}								\
	else							\
	{								\
		LookForInt(s,#x"=",&x);	\
	}							


#define CHAN_DOUBLE(x)			\
		DOUBLE(x##[0]);			\
		DOUBLE(x##[1]);			\
		DOUBLE(x##[2]);			\
		DOUBLE(x##[3]);			\
		DOUBLE(x##[4]);			\
		DOUBLE(x##[5]);			\
		DOUBLE(x##[6]);			\
		DOUBLE(x##[7]);				

#define CHAN_FLOAT(x)			\
		FLOAT(x##[0]);			\
		FLOAT(x##[1]);			\
		FLOAT(x##[2]);			\
		FLOAT(x##[3]);			\
		FLOAT(x##[4]);			\
		FLOAT(x##[5]);			\
		FLOAT(x##[6]);			\
		FLOAT(x##[7]);				

#define CHAN_INT(x)				\
		INT(x##[0]);			\
		INT(x##[1]);			\
		INT(x##[2]);			\
		INT(x##[3]);			\
		INT(x##[4]);			\
		INT(x##[5]);			\
		INT(x##[6]);			\
		INT(x##[7]);				


#define CSTRING(x)					\
	if (save)						\
	{								\
		fprintf(f,#x "=%s\n",x.GetBuffer(0));  \
	}								\
	else							\
	{								\
		LookForCString(s,#x"=",&x);	\
	}							

// Write a filename string to a file
// check if the filename contains the specified default path
// if so, strip it off
// on loading, if there is no path at all then
// add in the default path  (unless blank) 

#define CSTRING_PATH(x,p)					\
	if (save)						\
	{								\
		fprintf(f,#x "=%s\n",StripPathMatch(x,p));  \
	}								\
	else							\
	{								\
		if (LookForCString(s,#x"=",&x) ==0)	\
		{                                           \
			if (!x.IsEmpty() && x.Find('\\') == -1)                 \
				x = p + x;                          \
		}											\
	}							

#define CHARS(x)					\
	if (save)						\
	{								\
		fprintf(f,#x "=%s\n",x);    \
	}								\
	else							\
	{								\
		LookForChars(s,#x"=",x);	\
	}							



/////////////////////////////////////////////////////////////////////////////
// CDlgX dialog

class CDlgX : public CDialog
{
// Construction
public:
	static UINT NEAR  UpdateDataMessage;
	afx_msg LRESULT  ThreadSafeUpdateData(WPARAM w,LPARAM l);
	virtual void UpdateParameters() {return;};

	virtual void RestoreOnStart(FILE *f);
	virtual void SaveOnExit(FILE *f);

	void AFXAPI DDV_MinMaxDouble(CDataExchange* pDX, double const& value, double minVal, double maxVal);	
	void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, double& value);
	void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, int & value);
	void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, CString &value);
	void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, float& value);
	
	CString CDlgX::StripPathMatch(CString FileName, CString DefaultPath);

	int LastMoveX;
	int LastMoveY;
	int LastSizeX;
	int LastSizeY;
	BOOL InitDialogComplete;
	CDlgX(CWnd* pParent = NULL);   // standard constructor
	CString ToolTipText;
	void LookForFloat(const char * s, const char * c, float * b);
	void LookForDouble(const char * s, const char * c, double * b);
	void LookForInt(const char * s, const char * c, int * b);
	void LookForHex(const char * s, const char * c, int * b);
	int LookForCString(const char * s, const char * c, CString * b);
	void LookForChars(const char * s, const char * c, char *b);
	void LookForIIR(const char * s, const char * c, float *f0, float *f1, float *f2, float *f3, float *f4);

// Dialog Data
	//{{AFX_DATA(CDlgX)
	enum { IDD = 0 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgX)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgX)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnExit();
	virtual void OnCancel(); 
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);
	//}}AFX_MSG
	virtual BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	CString ExtractDirectory(CString s);
	CRect OrigClientRect;
	CRect OrigWindowRect;
private:
	HICON m_hIcon;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGX_H__ABAADC21_01DC_11D2_810A_004033E07FBE__INCLUDED_)
