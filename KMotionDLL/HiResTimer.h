// HiResTimer.h: interface for the CHiResTimer class.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/



#if !defined(AFX_HIRESTIMER_H__B6D03E55_83BB_49F8_8FCC_2C75A4C1BB3C__INCLUDED_)
#define AFX_HIRESTIMER_H__B6D03E55_83BB_49F8_8FCC_2C75A4C1BB3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_TIMES 100

class CHiResTimer  
{
public:
	CHiResTimer();
	virtual ~CHiResTimer();
	void Start();                 
	double DiffSecs(int t1, int t0);
	double Diff_us(int t1, int t0);
	void DisplayDiff(int t1, int t0);
	double Elapsed_Seconds();
	void Split();
	void Split(CString s);
	int nSplit;
	void DisplaySplit();
	LARGE_INTEGER Times[MAX_TIMES];
	LARGE_INTEGER Freq;
	CString Desc[MAX_TIMES];
	bool MessageDisplayed;
};

#endif // !defined(AFX_HIRESTIMER_H__B6D03E55_83BB_49F8_8FCC_2C75A4C1BB3C__INCLUDED_)
