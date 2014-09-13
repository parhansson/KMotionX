// HiResTimer.cpp: implementation of the CHiResTimer class.
/*********************************************************************/
/*         Copyright (c) 2003-2006  DynoMotion Incorporated          */
/*********************************************************************/

#include "stdafx.h"
#include "HiResTimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHiResTimer::CHiResTimer()
{
	Freq.QuadPart=0;	
	nSplit=0;
	MessageDisplayed=false;
}

CHiResTimer::~CHiResTimer()
{

}

void CHiResTimer::Start()                
{
	QueryPerformanceCounter(&Times[0]);
	nSplit=1;
}

double CHiResTimer::Diff_us(int t1, int t0)
{
	return DiffSecs(t1, t0) * 1e6;
}

double CHiResTimer::DiffSecs(int t1, int t0)
{
	__int64 T0,T1,F;
	double result;

	if (Freq.QuadPart==0)
		QueryPerformanceFrequency(&Freq);

	T0=Times[t0].QuadPart;
	T1=Times[t1].QuadPart;
	F=Freq.QuadPart;

	result = (T1-T0) / ((double) F);

	return result;
}

// grab a split time measurement

void CHiResTimer::Split()
{
	VERIFY (nSplit<MAX_TIMES); 

	QueryPerformanceCounter(&Times[nSplit]);
	Desc[nSplit]="";
	nSplit++;
}

// grab a split time measurement

void CHiResTimer::Split(CString s)
{
	if (nSplit>=MAX_TIMES) return; 

	QueryPerformanceCounter(&Times[nSplit]);
	Desc[nSplit]=s;
	nSplit++;
}


// elapsed time from start in seconds

double CHiResTimer::Elapsed_Seconds()
{
	LARGE_INTEGER t1;
	QueryPerformanceCounter(&t1);

	__int64 T0,T1,F;
	double result;

	if (Freq.QuadPart==0)
		QueryPerformanceFrequency(&Freq);

	T0=Times[0].QuadPart;
	T1=t1.QuadPart;
	F=Freq.QuadPart;

	result = (T1-T0) / ((double) F);

	return result;

}


void CHiResTimer::DisplayDiff(int t1, int t0)
{
	CString s;

	s.Format("From sample %d to %d was %f us",t1,t0,Diff_us(t1,t0));

	if (MessageDisplayed) return;

	MessageDisplayed=true;
	MessageBox(NULL,s,"Timer",MB_OK|MB_SYSTEMMODAL);
	MessageDisplayed=false;
}


void CHiResTimer::DisplaySplit()
{
	CString s,s2;

	for (int i=0; i<nSplit-1; i++)
	{
		s.Format("From sample %d to %d was %10.2f us %s",i+1,i,Diff_us(i+1,i),Desc[i+1].GetBuffer());

		s2= s2+s+"\r";
	}

	if (MessageDisplayed) return;

	MessageDisplayed=true;
	MessageBox(NULL,s2,"Timer",MB_OK|MB_SYSTEMMODAL);
	MessageDisplayed=false;
}
