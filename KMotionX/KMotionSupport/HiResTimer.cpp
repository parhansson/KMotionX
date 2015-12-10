/*
Copyright (c) 2014, P.Hansson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
 */
// HiResTimer.cpp: implementation of the CHiResTimer class.

#include <HiResTimer.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "MessageBox.h"

#ifdef __MACH__
#include <mach/mach_time.h>
static double conversionFactor;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHiResTimer::CHiResTimer()
{
	Freq=0;
	nSplit=0;
	MessageDisplayed=false;
#ifdef __MACH__
	mach_timebase_info_data_t timebase;
	mach_timebase_info(&timebase);
	conversionFactor = (double)timebase.numer / (double)timebase.denom;
#endif
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
	int64_t T0,T1,F;
	double result;

	if (Freq==0)
		QueryPerformanceFrequency(&Freq);

	T0=Times[t0];
	T1=Times[t1];
	F=Freq;

#ifdef __MACH__
	result = (T1-T0) * conversionFactor / ((double) F);
#else
  result = (T1-T0) / ((double) F);
#endif

	return result;
}

// grab a split time measurement

void CHiResTimer::Split()
{
	//VERIFY (nSplit<MAX_TIMES);

	QueryPerformanceCounter(&Times[nSplit]);
	Desc[nSplit][0]='\0';
	nSplit++;
}

// grab a split time measurement

void CHiResTimer::Split(const char * s)
{
	if (nSplit>=MAX_TIMES) return; 

	QueryPerformanceCounter(&Times[nSplit]);
	strcpy(Desc[nSplit],s);
	nSplit++;
}


// elapsed time from start in seconds

double CHiResTimer::Elapsed_Seconds()
{
	int64_t t1;
	QueryPerformanceCounter(&t1);

	int64_t T0,T1,F;
	double result;

	if (Freq==0)
		QueryPerformanceFrequency(&Freq);

	T0=Times[0];
	T1=t1;
	F=Freq;

#ifdef __MACH__
	result = (T1-T0) * conversionFactor / ((double) F);
#else
	result = (T1-T0) / ((double) F);
#endif

	//debug("Result %lf\n",result);
	//debug("Elapsed_Seconds \n\tusec=%lld\n\tdecimalusec%f\n", (long long)(T1-T0), result);
	//printf("Elapsed_Time \n\tsec=%lf\n", result);
	return result;

}


void CHiResTimer::DisplayDiff(int t1, int t0)
{
    //SJH - function not used
	char s[128];

	sprintf(s, "From sample %d to %d was %f us",t1,t0,Diff_us(t1,t0));

	if (MessageDisplayed) return;

	MessageDisplayed=true;
	MessageBox(0/*NULL*/,s,"Timer",MB_OK|MB_SYSTEMMODAL);
	MessageDisplayed=false;
}


void CHiResTimer::DisplaySplit()
{
    //SJH - function not used
	char s[MAX_TEXT + 96];

	for (int i=0; i<nSplit-1; i++)
	{
		sprintf(s,"From sample %d to %d was %10.2f us %s\n",i+1,i,Diff_us(i+1,i),Desc[i+1]);
	}

	if (MessageDisplayed) return;

	MessageDisplayed=true;
	MessageBox(0,s,"Timer",MB_OK|MB_SYSTEMMODAL);
	MessageDisplayed=false;
}

int CHiResTimer::QueryPerformanceCounter(int64_t *lpPerformanceCount){
	//printf("%s:%d\n", __FILE__, __LINE__);

#ifdef __MACH__
    *lpPerformanceCount = mach_absolute_time();
#else
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	*lpPerformanceCount = (int64_t)now.tv_sec*1000000000 + now.tv_nsec;
	//printf("QueryPerformanceCounter time=%u value=%lld\n", time, (long long)value);
#endif

	return 1;
}

int CHiResTimer::QueryPerformanceFrequency(int64_t *lpPerformanceCount){
	//printf("%s:%d\n", __FILE__, __LINE__);
	//printf("QueryPerformanceFrequency value=%lld\n", (long long)value);
	*lpPerformanceCount = 1000000000;
	return 1;
}
