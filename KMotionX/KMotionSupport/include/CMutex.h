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
/*
 * CMutex
 *
 *  Created on: 15 apr 2012
 *      Author: parhansson
 */


#ifndef CMUTEX_
#define CMUTEX_
#include <mutex>
#include <chrono>


long int getThreadId(const char *callerId);


#define TMUTEX
class CMutex
{


public:
	CMutex();
	CMutex(int initiallyOwn,const char *name ,int n);

	//Specifies the amount of time to wait for the synchronization object to be available (signaled).
	//If INFINITE, Lock will wait until the object is signaled before returning.
	//DWORD TimeOut_ms = INFINITE = 4294967295
	int Lock(int TimeOut_ms/* = 4294967295*/);
	void Lock();
	void Unlock();
	//CMutex(FALSE,"KMotionPipe",NULL)
	virtual ~CMutex();

private:
//	string value;
//	char *buffer;
	const char *name;
#if defined(TMUTEX)
	std::recursive_timed_mutex  mutex;
	//std::timed_mutex  mutex;
#else
#endif
	bool lock;


};

#endif /* CMUTEX_ */
