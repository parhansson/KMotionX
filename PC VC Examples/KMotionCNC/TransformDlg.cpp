// TransformDlg.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "TransformDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransformDlg dialog


CTransformDlg::CTransformDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTransformDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransformDlg)
	m_OffsetX = 0.0;
	m_OffsetY = 0.0;
	m_OffsetZ = 0.0;
	m_ScaleX = 1.0;
	m_ScaleY = 1.0;
	m_ScaleZ = 1.0;
	m_Digits = 4;
	m_ScaleIJ = TRUE;
	//}}AFX_DATA_INIT
}


void CTransformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransformDlg)
	DDX_Text(pDX, IDC_OffsetX, m_OffsetX);
	DDX_Text(pDX, IDC_OffsetY, m_OffsetY);
	DDX_Text(pDX, IDC_OffsetZ, m_OffsetZ);
	DDX_Text(pDX, IDC_ScaleX, m_ScaleX);
	DDX_Text(pDX, IDC_ScaleY, m_ScaleY);
	DDX_Text(pDX, IDC_ScaleZ, m_ScaleZ);
	DDX_Text(pDX, IDC_Digits, m_Digits);
	DDV_MinMaxUInt(pDX, m_Digits, 0, 12);
	DDX_Check(pDX, IDC_ScaleIJ, m_ScaleIJ);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate && m_ScaleIJ && (fabs(m_ScaleX) != fabs(m_ScaleY)))
	{
		AfxMessageBox("Scaling I and J with different X Y Scales not allowed");
		pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(CTransformDlg, CDialog)
	//{{AFX_MSG_MAP(CTransformDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransformDlg message handlers

void CTransformDlg::OnOK() 
{


	CDialog::OnOK();
}

int CTransformDlg::DoTransform(CString &s)
{

	CString t;
	CString r;

	int k,i=0;
	
	do 
	{
		k = s.Find("\r\n",i);

		if (k == -1)
		{
			// no end of line found
			// use whatever is left
			k = s.GetLength();
		}
		else
		{
			k += 2;  // include the cr lf
		}

		t = s.Mid(i,k-i);


		if (ProcessVar(t,"X",m_ScaleX,m_OffsetX)) return 1;
		if (ProcessVar(t,"Y",m_ScaleY,m_OffsetY)) return 1;
		if (ProcessVar(t,"Z",m_ScaleZ,m_OffsetZ)) return 1;

		if (m_ScaleIJ)
		{
			if (ProcessVar(t,"I",m_ScaleX,0.0)) return 1;
			if (ProcessVar(t,"J",m_ScaleY,0.0)) return 1;
		}

		r += t;
		i=k;
	}
	while ( i < s.GetLength() );

	s=r;

	return 0;
}

int CTransformDlg::ProcessVar(CString &t, CString v, double scale, double offset)
{
	int length_before_comment;


	if (scale == 1.0 && offset == 0.0) return 0;  // nothing to do

	// see if the string contains the var 
	// and at least one more character

	int comment = t.Find("(*");

	if (comment == -1)
		length_before_comment = t.GetLength();
	else
		length_before_comment = comment;

	CString c = t.Left(length_before_comment);

	c.MakeUpper();

	int i = c.Find(v);  // look for Variable Code

	if (i != -1)
	{
		if (length_before_comment < i+2)
		{
			AfxMessageBox("Invalid " + v + " Value Selected");
			return 1;
		}

		double Value;
		int k;

		int result = sscanf(c.GetBuffer(0)+i+1,"%lf%n",&Value,&k);

		if (result != 1)
		{
			AfxMessageBox("Invalid " + v + " Value Selected");
			return 1;
		}

		CString news;
		CString form;
		form.Format("%%.%df",m_Digits);

		news.Format(form,Value*scale+offset);

		// Strip trailing zeros or decimal point
		// as long as previous character is either another digit or a '.'
		// up until the '.'  is removed

		while (news.GetLength() > 1 && 
			    ((news.GetAt(news.GetLength()-2) >= '0' && news.GetAt(news.GetLength()-2) <= '9') || news.GetAt(news.GetLength()-2) <= '.') &&
				(news.GetAt(news.GetLength()-1) == '0' || news.GetAt(news.GetLength()-1) == '.'))
		{
			char c = news.GetAt(news.GetLength()-1);
			
			news.Delete(news.GetLength()-1,1);

			if (c=='.') break;  // we removed the decimal point, stop
		}

		// repace the value

		t.Delete(i+1,k);
		t.Insert(i+1,news);
	}

	return 0;
}
