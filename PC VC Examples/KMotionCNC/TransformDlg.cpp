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
	m_6AxesSmooth = 0;
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
	DDX_Text(pDX, IDC_6AxesSmooth, m_6AxesSmooth);
	DDV_MinMaxUInt(pDX, m_6AxesSmooth, 0, 12);
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
#define MAX_VALUES 1000000

int CTransformDlg::DoTransform(CString &s)
{
	bool DoAntiGouge = false; //tktk

	double *Values, dummy;
	double PrevValues[8];
	double IJValues[2];
	CString t;
	CString r;

	int k, n, i = 0;

	Values = new double[MAX_VALUES * 6];

	n = 0;
	for (int j = 0; j < 8; j++) PrevValues[j] = 0;

	do
	{
		k = s.Find("\r\n", i);

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

		t = s.Mid(i, k - i);

		if (ProcessVar(t, "X", Values[n * 6 + 0], PrevValues[0], m_ScaleX, m_OffsetX)) { delete[]Values;  return 1; }
		if (ProcessVar(t, "Y", Values[n * 6 + 1], PrevValues[1], m_ScaleY, m_OffsetY)) { delete[]Values;  return 1; }
		if (ProcessVar(t, "Z", Values[n * 6 + 2], PrevValues[2], m_ScaleZ, m_OffsetZ)) { delete[]Values;  return 1; }
		if (ProcessVar(t, "A", Values[n * 6 + 3], PrevValues[3], m_ScaleX, m_OffsetX)) { delete[]Values;  return 1; }
		if (ProcessVar(t, "B", Values[n * 6 + 4], PrevValues[4], m_ScaleX, m_OffsetX)) { delete[]Values;  return 1; }
		if (ProcessVar(t, "C", Values[n * 6 + 5], PrevValues[5], m_ScaleX, m_OffsetX)) { delete[]Values;  return 1; }

		if (m_ScaleIJ)
		{
			if (n == 2)
			{
				if (ProcessVar(t, "I", IJValues[0], PrevValues[6], m_ScaleX, 0.0)) { delete[]Values;  return 1; }
				if (ProcessVar(t, "J", IJValues[1], PrevValues[7], m_ScaleY, 0.0)) { delete[]Values;  return 1; }
			}
			else
			{
				if (ProcessVar(t, "I", dummy, PrevValues[6], m_ScaleX, 0.0)) { delete[]Values;  return 1; }
				if (ProcessVar(t, "J", dummy, PrevValues[7], m_ScaleY, 0.0)) { delete[]Values;  return 1; }
			}
		}

		if (n >= MAX_VALUES-10)
		{
			delete[]Values;
			MessageBox("Max number of Transform Points Exceeded");
			return 1;
		}

		n++;
		r += t;
		i = k;
	} while (i < s.GetLength());

	if (DoAntiGouge)
	{
		if (AntiGouge(Values, IJValues, n, r))
		{
			delete[]Values;
			return 1;
		}
	}
	else if (DoSmooth(Values, n, r))
	{
		delete[]Values;
		return 1;
	}

	s = r;
	delete[]Values;

	return 0;
}

// perform antigouge entry point sequence

int CTransformDlg::AntiGouge(double *Values, double *IJValues, int n, CString &r)
{
	double Xs, Ys, Xe, Ye, Xf, Yf, Xcc, Ycc, Rf, Rc, Rcc, dx, dy, D, descrim, Root, 
		L, dxp, dyp, Xa,Ya, Xb,Yb, X1, Y1, X2, Y2, dr, dr2, Xc, Yc, signdy, I0, J0, I1, J1;

	// Verify there are 3 lines of code
	if (n != 3)
	{
		MessageBox("Anti Gouge function should have 3 lines: G0 the G1 then G3");
		return 1;
	}

	Xs = Values[0 * 6 + 0];
	Ys = Values[0 * 6 + 1];
	Xe = Values[1 * 6 + 0];
	Ye = Values[1 * 6 + 1];
	Xf = Values[2 * 6 + 0];
	Yf = Values[2 * 6 + 1];
	Xcc = Xe + IJValues[0];
	Ycc = Ye + IJValues[1];
	Rf = sqrt(IJValues[0] * IJValues[0] + IJValues[1] * IJValues[1]);


	// Rc is to be slightly larger than largest tool
	Rc = 0.1;
	Rcc = Rf - Rc;

	// Solve for offset parallel line defined by two points 
	dx = (Xe - Xs);
	dy = (Ye - Ys);
	L = sqrt(dx*dx + dy*dy);
	dxp = Rc / L * -dy;
	dyp = Rc / L * dx;
	Xa = Xs + dxp - Xcc;  // also shift origin to Xcc Ycc
	Ya = Ys + dyp - Ycc;
	Xb = Xe + dxp - Xcc;
	Yb = Ye + dyp - Xcc;
	
	// Solve for line circle intersection.  From:
	//http://mathworld.wolfram.com/Circle-LineIntersection.html
	dx = Xb - Xa;
	dy = Yb - Ya;
	dr2 = dx*dx + dy*dy;
	dr = sqrt(dr2);
	D = Xa * Yb - Xb * Ya;
	descrim = Rcc * Rcc * dr2 - D*D;
		
	if (descrim <= 0.0)
	{
		MessageBox("Anti Gouge function No Circle Line Intersection");
		return 1;
	}

	Root = sqrt(descrim);
	signdy = dy < 0 ? -1.0 : 1.0;
	Xc = (D * dy + signdy * dx * Root)/dr2;
	Yc = (-D * dx + fabs(dy) * Root)/dr2;
	Xc += Xcc;
	Yc += Ycc;

	// now compute necessary points
	X1 = Xc - dxp;  // offset back
	Y1 = Yc - dyp;

	X2 = Xcc + (Xc - Xcc) * Rf / Rcc;
	Y2 = Ycc + (Yc - Ycc) * Rf / Rcc;

	I0 = Xc - X1;
	J0 = Yc - Y1;

	I1 = Xcc - X2;
	J1 = Ycc - Y2;


	CString news;
	//Create Code
	r = "";
	PutValue(Xs, news);	r += "G0X" + news;
	PutValue(Ys, news);	r += "Y" + news;
	PutValue(Values[0 * 6 + 2], news);	r += "Z" + news;
	r += "\r\n";
	PutValue(X1, news);	r += "G41 D1 G1 X" + news;
	PutValue(Y1, news);	r += "Y" + news;
	PutValue(Values[1 * 6 + 2], news);	r += "Z" + news;
	r += "\r\n";
	PutValue(X2, news);	r += "G3 X" + news;
	PutValue(Y2, news);	r += "Y" + news;
	PutValue(Values[1 * 6 + 2], news);	r += "Z" + news;
	PutValue(I0, news);	r += "I" + news;
	PutValue(J0, news);	r += "J" + news;
	r += "\r\n";
	PutValue(Xf, news);	r += "G3 X" + news;
	PutValue(Yf, news);	r += "Y" + news;
	PutValue(Values[2 * 6 + 2], news);	r += "Z" + news;
	PutValue(I1, news);	r += "I" + news;
	PutValue(J1, news);	r += "J" + news;
	r += "\r\n";

	return 0;
}

int CTransformDlg::DoSmooth(double *Values, int n, CString &r)
{
	double *NewValues, *SmoothValues;
	NewValues = new double[MAX_VALUES * 6];
	SmoothValues = new double[MAX_VALUES * 6];

	for (int z = 0; z < (int)m_6AxesSmooth; z++)
	{
		// sub divide segments
		for (int i = 0; i < n; i++)
		{
			for (int v = 0; v < 6; v++)
			{
				NewValues[i * 2 * 6 + v] = Values[i * 6 + v];
				if (i < n - 1)
					NewValues[(i * 2 + 1) * 6 + v] = (Values[(i + 1) * 6 + v] + Values[i * 6 + v]) / 2;
			}
		}
		n = n * 2 - 1;

		if (n > MAX_VALUES)
		{
			delete[] NewValues;
			delete[] SmoothValues;
			MessageBox("Too many blocks to smooth.  Reduce number of blocks to smooth or smooth count");
			return 1;
		}

		//Smooth segments
		for (int i = 0; i < n; i++)
		{
			for (int v = 0; v < 6; v++)
			{
				if (i == 0 || i == n - 1)
					SmoothValues[i * 6 + v] = NewValues[i * 6 + v];
				else
					SmoothValues[i * 6 + v] = (NewValues[(i + 1) * 6 + v] + 2 * NewValues[i * 6 + v] + NewValues[(i - 1) * 6 + v]) / 4;
			}
		}

		if (z < (int)m_6AxesSmooth - 1) 
		{
			for (int i = 0; i < n; i++)
			{
				for (int v = 0; v < 6; v++)
				{
						Values[i * 6 + v] = SmoothValues[i * 6 + v];
				}
			}

		}
	}

	CString news;
	//Create Code
	if (m_6AxesSmooth > 0)
	{
		r = "";
		for (int i = 0; i < n; i++)
		{
			PutValue(SmoothValues[i * 6 + 0], news);	r += "X" + news;
			PutValue(SmoothValues[i * 6 + 1], news);	r += "Y" + news;
			PutValue(SmoothValues[i * 6 + 2], news);	r += "Z" + news;
			PutValue(SmoothValues[i * 6 + 3], news);	r += "A" + news;
			PutValue(SmoothValues[i * 6 + 4], news);	r += "B" + news;
			PutValue(SmoothValues[i * 6 + 5], news);	r += "C" + news;
			r += "\r\n";
		}
	}

	delete[] NewValues;
	delete[] SmoothValues;

	return 0;
}

int CTransformDlg::ProcessVar(CString &t, CString v, double &Value, double &PrevValue, double scale, double offset)
{
	CString news;
	int length_before_comment;


	if (scale == 1.0 && offset == 0.0 && m_6AxesSmooth == 0) return 0;  // nothing to do

	// see if the string contains the var 
	// and at least one more character

	int comment = t.Find("(");

	if (comment == -1)
		length_before_comment = t.GetLength();
	else
		length_before_comment = comment;

	CString c = t.Left(length_before_comment);

	c.MakeUpper();

	int i = c.Find(v);  // look for Variable Code

	if (i != -1)
	{
		if (length_before_comment < i + 2)
		{
			AfxMessageBox("Invalid " + v + " Value Selected");
			return 1;
		}

		int k;

		int result = sscanf(c.GetBuffer(0) + i + 1, "%lf%n", &Value, &k);

		if (result != 1)
		{
			AfxMessageBox("Invalid " + v + " Value Selected");
			return 1;
		}

		PrevValue = Value;

		PutValue(Value*scale + offset, news);

		// repace the value

		t.Delete(i + 1, k);
		t.Insert(i + 1, news);
	}
	else
	{
		Value = PrevValue;
	}

	return 0;
}


int CTransformDlg::PutValue(double Value, CString &news)
{

	CString form;
	form.Format("%%.%df", m_Digits);

	news.Format(form, Value);

	// Strip trailing zeros or decimal point
	// as long as previous character is either another digit or a '.'
	// up until the '.'  is removed

	while (news.GetLength() > 1 &&
		((news.GetAt(news.GetLength() - 2) >= '0' && news.GetAt(news.GetLength() - 2) <= '9') || news.GetAt(news.GetLength() - 2) <= '.') &&
		(news.GetAt(news.GetLength() - 1) == '0' || news.GetAt(news.GetLength() - 1) == '.'))
	{
		char c = news.GetAt(news.GetLength() - 1);

		news.Delete(news.GetLength() - 1, 1);

		if (c == '.') break;  // we removed the decimal point, stop
	}

	return 0;
}
