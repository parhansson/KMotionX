/*
 * incX.cpp
 *
 *  Created on: 19 sep 2014
 *      Author: parhansson
 */


#include "incX.h"

#define SECONDS_PER_MONTH 2629743

char* _strupr(char* s)
{
  assert(s != (void*)0);
  char* p = s;
  while ((*p = toupper( *p ))) p++;
  return s;
}

unsigned int timeGetTime()
{

	//this is an ugly beast. cut from windows API doc

	//The timeGetTime function retrieves the system time, in milliseconds.
	//The system time is the time elapsed since Windows was started.


	//Note that the value returned by the timeGetTime function is a DWORD value.
	//The return value wraps around to 0 every 2^32 milliseconds, which is about 49.71 days.
	//This can cause problems in code that directly uses the timeGetTime return value in computations,
	//particularly where the value is used to control code execution.
	//You should always use the difference between two timeGetTime return values in computations.

	//We normalize to current month which is about 30 days and will fit in unsigned int.
	struct timeval now;
	gettimeofday(&now, NULL);
	unsigned long epoch = (now.tv_sec);
	unsigned int monthSinceEpoch = epoch/SECONDS_PER_MONTH;
	unsigned int sThisMonth = epoch - (SECONDS_PER_MONTH * monthSinceEpoch);
	unsigned int msThisMonth = sThisMonth*1000 + now.tv_usec/1000;
	//printf("Timestamp: monthSinceEpoch=%d sThisMonth=%d msThisMonth=%d\n", monthSinceEpoch,sThisMonth, msThisMonth);

	return msThisMonth;
}

