// LogSlider.cpp : implementation file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "LogSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogSlider

CLogSlider::CLogSlider()
{
}

CLogSlider::~CLogSlider()
{
}


BEGIN_MESSAGE_MAP(CLogSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CLogSlider)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogSlider message handlers

/****************************************************************************
*                            CLogSlider::SetRange
* Inputs:
*       double low: Low value for range
*       double high: High value for range
*       UINT resolution: Number of points between low and high
* Result: void
*
* Effect:
*       Sets the range
****************************************************************************/

void CLogSlider::SetRange(double L, double H, UINT resolution, int offset, int color)
{
	ASSERT(L < H);
	low = L;
	high = H;
	m_offset=offset;
	m_color=color;
	CSliderCtrl::SetRange(0, (int)resolution);
	SetTic(20);
} // CLogSlider::SetRange

/****************************************************************************
*                             CLogSlider::SetPos
* Inputs:
*       double pos: Desired position
* Result: BOOL
*       TRUE if in range
*       FALSE if out of range
* Effect:
*       Sets the position
****************************************************************************/

BOOL CLogSlider::SetPos(double pos)
{
	double linear;
	int L;
	int H;

	if (!CalcLinear(pos,linear,L,H)) return FALSE;

	// linear is now 1.0/0.16 or 62.5
	// so the actual value is set to 63

	int newpos = (int)(linear + 0.5);
	if(newpos < 0)
		newpos = 0; // roundoff issues may create this

	CSliderCtrl::SetPos( H-newpos );

	return TRUE;
} // CLogSlider::SetPos


BOOL CLogSlider::CalcLinear(double pos, double &linear, int &L, int &H)
{
	CSliderCtrl::GetRange(L, H);

	if(pos < low)
	{
		linear=L;
		return FALSE;
	}
	if(pos > high)
	{
		linear=H;
		return FALSE;
	}

	double loglow = log10(low);
	double loghigh = log10(high);

	double range = loghigh - loglow;

	double ratio = range / (double)(H - L);

	// Now convert the position (linear, 0 based) to a
	// position (linear, scaled and offset)
	// e.g., if low..high is 10e-13..10e3, resolution = 1000
	// then
	// loglow = -13, loghigh = 3
	// 0 is -13, and 1000 is 3
	// the range is 3 - (-13) or 16
	// the ratio is 16/1000, 0.016
	// the value of the offset is loglow
	// the desired position is 10e-10
	// logpos is -10

	double logpos = log10(pos);

	// The desired offset from the low range is
	// 3 units
	// loglow + ? = logpos
	// ? = logpos - loglow

	// Suppose pos is 10e-12
	// logpos is -12.0

	double logoffset = logpos - loglow;

	// logoffset is -12 - (-13) => 1.0

	// We now divide this by the ratio to get the linear
	// position of the slider
	linear = logoffset / ratio;

	return TRUE;
}


/****************************************************************************
*                             CLogSlider::GetPos
* Result: double
*       Floating-point value which is the position
****************************************************************************/

double CLogSlider::GetPos()
{
	// Assume the following:
	// range is 10e-13..10e3
	// resolution is 1000

	int L;
	int H;
	CSliderCtrl::GetRange(L, H);

	double loglow = log10(low);
	double loghigh = log10(high);

	double range = loghigh - loglow;

	double ratio = range / (double)(H - L);

	int linearpos = H - CSliderCtrl::GetPos(); // linear position

	// convert linear position to a logarithmic position

	double pos = (double)linearpos * ratio;

	// Suppose the pos is 63
	// pos is 1.01
	double adjustedpos = loglow + pos;

	// AdjustedPos is
	// -13 + 1.01 = -11.99
	double result = pow(10.0, adjustedpos);
	// result is 1e-11.99, close to 1e-12
	// using finer resolution will give a better result
	return result;
} // CLogSlider::GetPos

/****************************************************************************
*                            CLogSlider::GetRange
* Inputs:
*       double & L:
*       double & H:
* Result: void
*
* Effect:
*       Returns the range via the reference parameters
****************************************************************************/

void CLogSlider::GetRange(double & L, double & H)
{
	L = low;
	H = high;
} // CLogSlider::GetRange

/****************************************************************************
*                         CLogSlider::GetLinearRange
* Inputs:
*       int & low: Place to put low value
*       int & high: Place to put high value
* Result: void
*
* Effect:
*       Extracts the linear control range
* Notes:
*       This is here to facilitate debugging
****************************************************************************/

void CLogSlider::GetLinearRange(int & low, int & high)
{
	CSliderCtrl::GetRange(low, high);
} // CLogSlider::GetLinearRange

/****************************************************************************
*                          CLogSlider::GetLinearPos
* Result: int
*       Linear position
* Notes:
*       This is a debugging aid
****************************************************************************/

int CLogSlider::GetLinearPos()
{
	return CSliderCtrl::GetPos();
} // CLogSlider::GetLinearPos

/****************************************************************************
*                          CLogSlider::SetLinearPos
* Inputs:
*       int pos: Desired linear position
* Result: void
*
* Effect:
*       Sets the linear position
****************************************************************************/

void CLogSlider::SetLinearPos(int pos)
{
	CSliderCtrl::SetPos(pos); 
} // CLogSlider::SetLinearPos


void CLogSlider::PlotInstant(double f)
{
	double linear;
	int L;
	int H;

	CalcLinear(f,linear,L,H);

	RECT CtrlRect;
	CPen *SavePen;
	CPen Pen2(PS_SOLID ,3,m_color);
	CPen Pen(PS_SOLID ,3,RGB(196,196,196));
	CClientDC cdc(this);

	// get the Whole Control size
	GetClientRect(&CtrlRect);

	linear=linear/H;

	double Top=13;
	double Bot=CtrlRect.bottom-13;
	double Mid=Bot+(Top-Bot)*linear;

	CPoint start_point(CtrlRect.right+m_offset,Top);
	CPoint mid_point(CtrlRect.right+m_offset,Mid);
	CPoint end_point(CtrlRect.right+m_offset,Bot);

	SavePen = cdc.SelectObject(&Pen);
	cdc.SetROP2(R2_COPYPEN);

	cdc.MoveTo(start_point);
	cdc.LineTo(mid_point);
	cdc.SelectObject(&Pen2);
	cdc.LineTo(end_point);

	cdc.SelectObject(SavePen);
}

// Gets rid of the focus box
afx_msg void CLogSlider::OnSetFocus(CWnd* pOldWnd)
{
}