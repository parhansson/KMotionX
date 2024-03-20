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
 * CMutex.cpp
 *
 *  Created on: 12 mar 2014
 *      Author: parhansson
 */

#include "CMutex.h"
#include "KMotionX.h"

#ifdef KMX_DEBUG_MUTEX
#define mutex_debug(M, ...)     \
	if (1)                      \
	{                           \
		debug(M, ##__VA_ARGS__); \
	};
#else
#define mutex_debug(M, ...)
#endif

long int owner;
int lockCount;

CMutex::CMutex()
{
	lock = false;
	name = "unnamed";
	owner = 0;
	lockCount = 0;
}

CMutex::~CMutex()
{
	// delete m1;
	// mutex.~recursive_timed_mutex();
}

/*
bInitiallyOwn
Specifies if the thread creating the CMutex object initially has access to the resource controlled by the mutex.

name
Name of the CMutex object.

lpsaAttribute
Security attributes for the mutex object. Not used in this implemenation
*/
CMutex::CMutex(int initiallyOwn, const char *name, lpsecurityAttr lpsaAttribute)
{
	if (name)
	{
		this->name = name;
	} else {
		this->name = "unnamed";
	}
	if (initiallyOwn)
	{
		Lock();
	}
}

/*
Specifies the amount of time to wait for the synchronization object to be available (signaled).
If INFINITE, Lock will wait until the object is signaled before returning.

DWORD TimeOut_ms = INFINITE = 4294967295

Returns Nonzero if the function was successful; otherwise 0.

*/
int CMutex::Lock(uint32_t TimeOut_ms /* = 4294967295*/)
{

	long int tid = kmx::getThreadId("CMutex:Lock (timeout)");
	mutex_debug("Locking Mutex[%s] waiting max timout: %dms thread: %ld lockCount: %d", name.c_str(), TimeOut_ms, tid, lockCount);
	//if TimeOut_ms = 4294967295 we can use mutex.lock(); instead of try_lock_for. but 49.7 days is enough
	bool success = mutex.try_lock_for(std::chrono::milliseconds(TimeOut_ms));
	if (success)
	{
		owner = tid;
		lockCount++;
		mutex_debug("Locked Mutex[%s] waiting max timout: %dms thread: %ld lockCount: %d", name.c_str(), TimeOut_ms, tid, lockCount);
	} else {
		mutex_debug("Lock failed Mutex[%s] waiting max timout: %dms thread: %ld lockCount: %d", name.c_str(), TimeOut_ms, tid, lockCount);
	}
	return success ? 1 : 0;
}

// Returns Nonzero if the function was successful; otherwise 0.
int CMutex::Unlock()
{
	long int tid = kmx::getThreadId("CMutex::Unlock");
	if (owner == tid && lockCount > 0)
	{
		mutex_debug("Unlocking Mutex[%s] thread: %ld lockCount: %d", name.c_str(), tid, lockCount);
		lockCount--;
		mutex.unlock();
		mutex_debug("Unlocked Mutex[%s] thread: %ld lockCount: %d", name.c_str(), tid, lockCount);
		return 1;
	} else {
		mutex_debug("Faulty unlock Mutex[%s] thread: %ld lockCount: %d", name.c_str(), tid, lockCount);
	}
	return 0;
}
