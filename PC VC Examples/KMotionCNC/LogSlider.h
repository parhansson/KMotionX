// LogSlider.h : header file
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/


#if !defined(AFX_LOGSLIDER_H__C433BC64_687C_4410_A89F_14C89F9A4EE6__INCLUDED_)
#define AFX_LOGSLIDER_H__C433BC64_687C_4410_A89F_14C89F9A4EE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLogSlider window

class CLogSlider : public CSliderCtrl
{
// Construction
public:
        CLogSlider();

// Attributes
public:

// Operations
public:

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CLogSlider)
        //}}AFX_VIRTUAL

// Implementation
public:
        virtual ~CLogSlider();
        void SetRange(double low, double high, UINT resolution, int offset, int color);
        void GetRange(double & low, double & high);
        BOOL SetPos(double pos);
        double GetPos();

        int GetLinearPos();
        void SetLinearPos(int pos);
        void GetLinearRange(int & low, int & high);

		void PlotInstant(double f);
		BOOL CalcLinear(double pos, double &linear, int &L, int &H);

		afx_msg void CLogSlider::OnSetFocus(CWnd* pOldWnd);


        // Generated message map functions
protected:
        double low; // low end of range
        double high; // high end of range

		int m_offset;
		int m_color;

        //{{AFX_MSG(CLogSlider)
                // NOTE - the ClassWizard will add and remove member functions here.
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGSLIDER_H__C433BC64_687C_4410_A89F_14C89F9A4EE6__INCLUDED_)
