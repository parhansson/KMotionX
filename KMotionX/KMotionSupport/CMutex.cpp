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

#include <CMutex.h>
#include <unistd.h>
#include <sys/syscall.h>

CMutex::CMutex() {
	lock = false;
	name = 0;

}

CMutex::~CMutex() {
 //delete m1;
	//mutex.~recursive_timed_mutex();
}


//CMutex(FALSE,"KMotionPipe",NULL)
CMutex::CMutex(int initiallyOwn,const char *name ,int n){
	this->name = name;
	if(initiallyOwn){
		Lock(/*n*/);
	}
}

	//Specifies the amount of time to wait for the synchronization object to be available (signaled).
	//If INFINITE, Lock will wait until the object is signaled before returning.
	//DWORD TimeOut_ms = INFINITE = 4294967295
	int CMutex::Lock(int TimeOut_ms/* = 4294967295*/){
		//std::condition_variable_any;
		//printf("tid = %d\n", syscall(SYS_gettid));
//		pthread_t t = pthread_self();
//		printf("Thread %.8x %.8x: Waiting to get lock \n", t);

		//printf("%s:%d Mutex: %s waiting max timout %d", __FILE__, __LINE__, name,TimeOut_ms);
		int locked = 0;
#if defined(TMUTEX)
		using Ms = std::chrono::milliseconds;
		bool success = mutex.try_lock_for(Ms(TimeOut_ms));
		locked = success?1:0;
		if(success){
			lock = true;
		}
#else
		if(lock){
			//locked failed return false;
			locked = 0;
		} else {
			lock = 1;
			locked = 1;
		}
#endif

		//printf(" %s\n",locked==1?"Success":"Failed");
		return locked;
	}

	void CMutex::Lock(){
		//return Lock(4294967295);
		//printf("%s:%d Mutex: %s waiting", __FILE__, __LINE__, name);
		int locked = 1;
#if defined(TMUTEX)
		mutex.lock();
		lock = true;
#else
		if(lock){
			//locked failed return false;
			locked = 0;
		} else {
			lock = 1;
			locked = 1;
		}
#endif

		//printf(" %s\n",locked==1?"Success":"Failed");
	}

	void CMutex::Unlock(){
		//printf("%s:%d Mutex: %s unlock.....", __FILE__, __LINE__, name);
#if defined(TMUTEX)
		if(lock){
			mutex.unlock();
			lock = false;
		}
#else
		lock = 0;
#endif
		//printf("unlocked\n");
	}
