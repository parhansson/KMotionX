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
	m_Digits = 5;
	m_UpSample = 0.0003;
	m_ColTolSmooth = 0.0003;
	m_TauSmooth = 0.005;
	m_CornerRadius = 0.1;
	m_RunLength = 10;
	m_Recursions = 10;
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
	DDX_Text(pDX, IDC_UpSample, m_UpSample);
	DDV_MinMaxDouble(pDX, m_UpSample, 1e-6, 3000);
	DDX_Text(pDX, IDC_ColTolSmooth, m_ColTolSmooth);
	DDV_MinMaxDouble(pDX, m_ColTolSmooth, 0, 3000);
	DDX_Text(pDX, IDC_TauSmooth, m_TauSmooth);
	DDV_MinMaxDouble(pDX, m_TauSmooth, 1e-6, 30);
	DDX_Text(pDX, IDC_CornerRadius, m_CornerRadius);
	DDV_MinMaxDouble(pDX, m_CornerRadius, 1e-6, 1000.0);
	DDV_MinMaxUInt(pDX, m_RunLength, 0, 3000);
	DDX_Text(pDX, IDC_RunLength, m_RunLength);
	DDV_MinMaxUInt(pDX, m_Recursions, 0, 3000);
	DDX_Text(pDX, IDC_Recursions, m_Recursions);
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
	ON_BN_CLICKED(ID_Transform, &CTransformDlg::Clicked_Transform)
	ON_BN_CLICKED(ID_RunningAvg, &CTransformDlg::Clicked_RunningAvg)
	ON_BN_CLICKED(ID_3PtRecursive, &CTransformDlg::Clicked_3PtRecursive)
	ON_BN_CLICKED(ID_IIRLowPass, &CTransformDlg::Clicked_IIRLowPass)
	ON_BN_CLICKED(ID_RoundG1, &CTransformDlg::Clicked_RoundCorner)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransformDlg message handlers

#define MAX_VALUES 1000000

int CTransformDlg::ExtractValues(CString s, CString  &r)
{
	double dummy;
	double PrevValues[8];
	double IJValues[2];
	CString t;

	int k, n, i = 0;

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

		if (n >= MAX_VALUES - 10)
		{
			delete[]Values;
			MessageBox("Max number of Transform Points Exceeded");
			return 1;
		}

		n++;
		r += t;
		i = k;
	} while (i < s.GetLength());

	s = r;
	nValues = n;

	return 0;
}

// round corner to avoid gouge.  Assumes 2 consecutive G1 moves.  Inserts radius R

int CTransformDlg::RoundCorner(double *Values, int n, double R, CString &r)
{
	double Xs, Ys, Xe, Ye, Xf, Yf;
	CString news;


	// Verify there are 3 lines of code
	if (n < 3)
	{
		MessageBox("Round function should have 3 or more lines: Gx then G1 then G1");
		return 1;
	}

	Xs = Values[0 * 6 + 0];
	Ys = Values[0 * 6 + 1];
	Xe = Values[1 * 6 + 0];
	Ye = Values[1 * 6 + 1];
	Xf = Values[2 * 6 + 0];
	Yf = Values[2 * 6 + 1];

	for (int i = 0; i < n - 2; i++)
	{
		double adx = Xe - Xs;
		double ady = Ye - Ys;
		double bdx = Xe - Xf;
		double bdy = Ye - Yf;

		//	double dot = adx * bdx + ady * bdy;

		double amag = sqrt(adx*adx + ady*ady);
		double bmag = sqrt(bdx*bdx + bdy*bdy);

		//	double B = acos(dot / (amag*bmag));

		double B = atan2(adx*bdy - ady*bdx, adx*bdx + ady*bdy);  // signed angle between vectors

		double A = R / tan(fabs(B) / 2.0);

		if (A > amag)
		{
			CString s;
			s.Format("Segment %d too short X=%f Y=%f", i + 1, Xe, Ye);
			MessageBox(s);
			// combine the two segments together
			Xe = Xf;
			Ye = Yf;
			Xf = Values[(3 + i) * 6 + 0];
			Yf = Values[(3 + i) * 6 + 1];
			continue;
		}

		if (A > bmag)
		{
			CString s;
			s.Format("Segment %d too short X=%f Y=%f", i + 2, Xe, Ye);
			MessageBox(s);
			// combine the two segments together
			Xf = Values[(3 + i) * 6 + 0];
			Yf = Values[(3 + i) * 6 + 1];
			continue;
		}


		double Pax = Xe - A / amag*adx;
		double Pay = Ye - A / amag*ady;

		double Pbx = Xe - A / bmag*bdx;
		double Pby = Ye - A / bmag*bdy;

		double I = -ady / amag*R;
		double J = +adx / amag*R;

		if (B > 0)
		{
			I = -I;
			J = -J;
		}

		//Create Code
		if (i == 0)  // put out starting point first time
		{
			r = "";  // clear result
			PutValue(Xs, news);	r += "G1X" + news;
			PutValue(Ys, news);	r += "Y" + news;
			PutValue(Values[0 * 6 + 2], news);	r += "Z" + news;
			r += "\r\n";
		}
		PutValue(Pax, news); r += "G1X" + news;
		PutValue(Pay, news); r += "Y" + news;
		PutValue(Values[1 * 6 + 2], news);	r += "Z" + news;
		r += "\r\n";
		if (B > 0)
		{
			PutValue(Pbx, news);	r += "G2X" + news;
		}
		else
		{
			PutValue(Pbx, news);	r += "G3X" + news;
		}
		PutValue(Pby, news);	r += "Y" + news;
		PutValue(Values[1 * 6 + 2], news);	r += "Z" + news;
		PutValue(I, news);	r += "I" + news;
		PutValue(J, news);	r += "J" + news;
		r += "\r\n";
		if (i < n - 3)  // advance if not last
		{
			// advance to next set of 3 points
			Xs = Pbx;
			Ys = Pby;
			Xe = Xf;
			Ye = Yf;
			Xf = Values[(3+i) * 6 + 0];
			Yf = Values[(3+i) * 6 + 1];
		}
	}
	// output last point
	PutValue(Xf, news);	r += "G1X" + news;
	PutValue(Yf, news);	r += "Y" + news;
	PutValue(Values[2 * 6 + 2], news);	r += "Z" + news;
	r += "\r\n";
	return 0;
}


// perform antigouge entry point sequence

int CTransformDlg::AntiGouge(double *Values, double *IJValues, int n, CString &r)
{
	double Xs, Ys, Xe, Ye, Xf, Yf, Xcc, Ycc, Rf, Rc, Rcc, dx, dy, D, descrim, Root,
		L, dxp, dyp, Xa, Ya, Xb, Yb, X1, Y1, X2, Y2, dr, dr2, Xc, Yc, signdy, I0, J0, I1, J1;

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
		MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Anti Gouge function No Circle Line Intersection"), L"KMotion", MB_ICONSTOP | MB_OK);
		return 1;
	}

	Root = sqrt(descrim);
	signdy = dy < 0 ? -1.0 : 1.0;
	Xc = (D * dy + signdy * dx * Root) / dr2;
	Yc = (-D * dx + fabs(dy) * Root) / dr2;
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

#define sqr(x) ((x)*(x))

// compute length between 2 consecutive 6 axis points
double CTransformDlg::ComputeLength(double *v)
{
	return sqrt(sqr(v[1] - v[6 + 1]) + sqr(v[1] - v[6 + 1]) + sqr(v[1] - v[6 + 1]));
}

int CTransformDlg::DoSmooth(double *Values, int n, SmoothType Type, CString &r)
{
	double *NewValues, *SmoothValues;
	NewValues = new double[MAX_VALUES * 6];
	SmoothValues = new double[MAX_VALUES * 6];

	double DesiredLength = 0.0003;

	// sub divide segments
	int i, v, newn = 0;

	for (i = 0; i < n - 1; i++)  // loop every segment
	{
		// compute segment length

		double length = ComputeLength(Values + i * 6);

		int k = (int)(length / DesiredLength) + 1;

		for (int m = 0; m < k; m++)
		{
			for (int v = 0; v < 6; v++)
			{
				NewValues[newn * 6 + v] = Values[i * 6 + v] + (Values[(i + 1) * 6 + v] - Values[i * 6 + v]) * (double)m / (double)k;
			}
			newn++;
			if (newn > MAX_VALUES - 1)
			{
				delete[] NewValues;
				delete[] SmoothValues;
				MessageBox("Too many blocks to smooth.  Reduce number of blocks to smooth or smooth count");
				return 1;
			}
		}
	}

	// copy last point
	for (v = 0; v < 6; v++)
		NewValues[newn * 6 + v] = Values[i * 6 + v];
	newn++;

	if (Type == RunLength) // Do running average Smoothing 
	{
		int d;
		double Sums[6];
		// set sums to first point
		for (v = 0; v < 6; v++) Sums[v] = NewValues[v];

		//Smooth segments replace each point with a running average
		for (i = 0; i < newn; i++)
		{
			if (i < (int)m_RunLength)
			{
				d = i;  // d is number of points on both sides
				for (v = 0; v < 6; v++) SmoothValues[i * 6 + v] = Sums[v] / (2 * d + 1);
				// add in 2 more points
				for (v = 0; v < 6; v++) Sums[v] += NewValues[(i + d + 1) * 6 + v] + NewValues[(i + d + 2) * 6 + v];
			}
			else if (i > newn - (int)m_RunLength - 2)
			{
				d = (newn - i - 1); // d is number of points on both sides
				for (v = 0; v < 6; v++) SmoothValues[i * 6 + v] = Sums[v] / (2 * d + 1);
				// subtract out 2 more points, except on last point
				if (i < newn - 1)
					for (v = 0; v < 6; v++) Sums[v] -= NewValues[(i - d) * 6 + v] + NewValues[(i - d + 1) * 6 + v];
			}
			else
			{
				d = m_RunLength;
				for (v = 0; v < 6; v++) SmoothValues[i * 6 + v] = Sums[v] / (2 * d + 1);
				// subtract old value, add in new
				for (v = 0; v < 6; v++) Sums[v] += -NewValues[(i - d) * 6 + v] + NewValues[(i + d + 1) * 6 + v];
			}
		}
	}
	else if (Type == Recursive) // do 3 point recursion smoothing
	{
		for (int z = 0; z < (int)m_Recursions; z++)
		{
			//Smooth segments
			for (i = 0; i < newn; i++)
			{
				for (int v = 0; v < 6; v++)
				{
					if (i == 0 || i == newn - 1)
						SmoothValues[i * 6 + v] = NewValues[i * 6 + v];
					else
						SmoothValues[i * 6 + v] = (NewValues[(i + 1) * 6 + v] + 2 * NewValues[i * 6 + v] + NewValues[(i - 1) * 6 + v]) / 4;
				}
			}

			if (z < (int)m_Recursions - 1)
			{
				for (int i = 0; i < newn; i++)
				{
					for (int v = 0; v < 6; v++)
					{
						NewValues[i * 6 + v] = SmoothValues[i * 6 + v];
					}
				}
			}
		}
	}
	else  // do IIR Low pass
	{
		double KLP = exp(-m_UpSample / m_TauSmooth);
		double KLPM1 = !.0 - KLP;
		//Smooth segments
		for (i = 0; i < newn; i++)
		{
			for (int v = 0; v < 6; v++)
			{
				if (i == 0)
					SmoothValues[i * 6 + v] = NewValues[i * 6 + v];  // copy first point
				else
					SmoothValues[i * 6 + v] = SmoothValues[(i - 1) * 6 + v] * KLP + NewValues[i * 6 + v] * KLPM1;
			}
		}
	}

	//remove collinear points (in place)
	int w0max = 0, w0, w1, k = 1;  // number of output points

	for (i = 0; i < newn - 2;)
	{
		w0 = 1;
		w1 = 2;

		// Upto w0 is valid, check upto w1
		while (CheckSequenceColl(SmoothValues, i, i + w1))
		{
			// valid
			w0 = w1;
			w1 *= 2;  // double trial distance forward

			if (i + w1 >= newn) 
				w1 = newn - i - 1;  // don't go beyond end
			if (i + w0 >= newn - 1) break;  // can't go any further
		}

		//w0 valid, w1 invalid, check half way between, binary search

		while (w1 - w0 > 1)
		{
			int d = w0 + (w1 - w0) / 2;
			if (CheckSequenceColl(SmoothValues, i, i + d))
				w0 = d;
			else
				w1 = d;
		}

		// w0 is the furthest valid

		i += w0; // move i to end of chain

		if (w0 > w0max) w0max = w0;

		 // move it to the list
		for (v = 0; v < 6; v++)	SmoothValues[k * 6 + v] = SmoothValues[i * 6 + v];
		
		k++;

		if (i > newn - 1 - 2) // less than 2 points left??
		{
			if (i == newn - 1 - 1) // yes, one point left??
			{
				i++;  // point to it
				// move it to the list
				for (v = 0; v < 6; v++)	SmoothValues[k * 6 + v] = SmoothValues[i * 6 + v];
				k++;
			}
			break;
		}
	}

	newn = k;

	CString news;
	//Create Code
	if (m_RunLength > 0)
	{
		r = "";
		for (int i = 0; i < newn; i++)
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


bool CTransformDlg::CheckSequenceColl(double *p, int b, int e)
{
	CCoordMotion *CM = TheFrame->GCodeDlg.Interpreter->CoordMotion;
	SEGMENT beg, mid, end;
	int i;

	SetSegmentFromV(&beg, p, b);
	SetSegmentFromV(&end, p, e);

	for (i = b + 1; i <= e - 1; i++)  // check all points between
	{
		SetSegmentFromV(&mid, p, i);
		if (!CM->CheckCollinear(&beg, &mid, &end, 0.0003)) return false;
	}
	return true;
}

void CTransformDlg::SetSegmentFromV(SEGMENT_STRUCT *seg, double *p, int i)
{
	seg->x0 = p[i * 6];
	seg->y0 = p[i * 6 + 1];
	seg->z0 = p[i * 6 + 2];
	seg->a0 = p[i * 6 + 3];
	seg->b0 = p[i * 6 + 4];
	seg->c0 = p[i * 6 + 5];
	seg->u0 = 0.0;
	seg->v0 = 0.0;
}



int CTransformDlg::ProcessVar(CString &t, CString v, double &Value, double &PrevValue, double scale, double offset)
{
	CString news;
	int length_before_comment;

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
			MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid ") + (CStringW) v + " Value Selected", L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
			return 1;
		}

		int k;

		int result = sscanf(c.GetBuffer(0) + i + 1, "%lf%n", &Value, &k);

		if (result != 1)
		{
			MessageBoxW(NULL, /*TRAN*/TheFrame->KMotionDLL->Translate("Invalid ") + (CStringW) v + " Value Selected", L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
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


void CTransformDlg::Clicked_Transform()
{
	CString r;

	if (!UpdateData()) return;

	Values = new double[MAX_VALUES * 6];

	if (!ExtractValues(Selection, r))
	{
		TheFrame->GCodeDlg.m_Editor.ReplaceSel(r);
	}

	delete[]Values;
	EndModalLoop(0);
}


void CTransformDlg::Clicked_RunningAvg()
{
	CString r;

	if (!UpdateData()) return;

	Values = new double[MAX_VALUES * 6];

	ExtractValues(Selection, r);

	if (!DoSmooth(Values, nValues, RunLength, r))
	{
		TheFrame->GCodeDlg.m_Editor.ReplaceSel(r);
	}

	delete[]Values;
	EndModalLoop(0);
}


void CTransformDlg::Clicked_3PtRecursive()
{
	CString r;

	if (!UpdateData()) return;

	Values = new double[MAX_VALUES * 6];

	ExtractValues(Selection, r);

	if (!DoSmooth(Values, nValues, Recursive, r))
		TheFrame->GCodeDlg.m_Editor.ReplaceSel(r);

	delete[]Values;
	EndModalLoop(0);
}


void CTransformDlg::Clicked_IIRLowPass()
{
	CString r;

	if (!UpdateData()) return;

	Values = new double[MAX_VALUES * 6];

	ExtractValues(Selection, r);

	if (!DoSmooth(Values, nValues, IIRLowPass, r))
	{
		TheFrame->GCodeDlg.m_Editor.ReplaceSel(r);
	}

	delete[]Values;
	EndModalLoop(0);
}


void CTransformDlg::Clicked_RoundCorner()
{
	CString r;

	if (!UpdateData()) return;

	Values = new double[MAX_VALUES * 6];

	ExtractValues(Selection, r);

	if (!RoundCorner(Values, nValues, m_CornerRadius, r))
		TheFrame->GCodeDlg.m_Editor.ReplaceSel(r);

	delete[]Values;
	EndModalLoop(0);
}
