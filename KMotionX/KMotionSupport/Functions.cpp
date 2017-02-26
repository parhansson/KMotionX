/*
 * Functions.cpp
 *
 *  Created on: 19 sep 2014
 *      Author: parhansson
 */
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SECONDS_PER_MONTH 2629743

//currently not used
char *fix_slashes(char *path)
{
    char *p;
    for (p = path; *p; ++p)
        if (*p == '\\')
            *p = '/';
    return path;
}


long GetTickCount()
{
    struct timeval tv;
    if(gettimeofday(&tv, NULL) != 0)
        return 0;

    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

char* _strupr(char* s)
{
  if(s == (void*)0) return s;
  char* p = s;
  while ((*p = toupper( *p ))) p++;
  return s;
}

char* _strlwr(char* s)
{
  if(s == (void*)0) return s;
  char* p = s;
  while ((*p = tolower( *p ))) p++;
  return s;
}


unsigned int timeGetTime()
{
	//TODO Use uptime and the same in GetTickCount
	//http://stackoverflow.com/questions/3070278/uptime-under-linux-in-c

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

