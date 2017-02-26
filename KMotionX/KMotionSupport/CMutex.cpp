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

long int owner;
int lockCount;

CMutex::CMutex() {
	lock = false;
	name = 0;
	owner = 0;
	lockCount = 0;
}

CMutex::~CMutex() {
 //delete m1;
	//mutex.~recursive_timed_mutex();
}


//CMutex(FALSE,"KMotionPipe",NULL)
CMutex::CMutex(int initiallyOwn,const char *name ,int n){
	//TODO malloc and strcpy name
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

//		printf("Thread %.8x %.8x: Waiting to get lock \n", t);

		long int tid = ::getThreadId("CMutex:Lock (timeout)");
		//printf("%s:%d Mutex: %s waiting max timout %d thread %ld lockCount %d", __FILE__, __LINE__, name,TimeOut_ms,tid,lockCount);
		//using Ms = std::chrono::milliseconds;
		bool success = mutex.try_lock_for(std::chrono::milliseconds(TimeOut_ms));
		if(success){
			owner = tid;
			lockCount++;
		}

		//printf(" %s\n",success?"Success":"Failed");
		//printf(" %s lockCount %d \n",success?"Success":"Failed",lockCount);
		return success?1:0;
	}

	void CMutex::Lock(){
		long int tid = ::getThreadId("CMutex:Lock");
		//return Lock(4294967295);
		//printf("%s:%d Mutex: %s thread %ld lockCount %d waiting", __FILE__, __LINE__, name,tid,lockCount);
		mutex.lock();
		owner = tid;
		lockCount++;

		//printf(" %s lockCount %d \n","Success",lockCount);
	}

	void CMutex::Unlock(){
		long int tid = ::getThreadId("CMutex::Unlock");

		if(owner == tid && lockCount > 0){
			//printf("%s:%d Mutex: %s thread %ld lockCount %d unlock.....", __FILE__, __LINE__, name, tid,lockCount);
			lockCount--;
			mutex.unlock();
			//printf("unlocked\n");
		}

	}

	long int getThreadId(const char *callerId){
		long int tid;
		//tid = syscall(SYS_gettid/*224*/);
	#ifdef __APPLE__
		
		pthread_t t;
		t = pthread_self();
		//unsigned int
		mach_port_t mt;
		mt = pthread_mach_thread_np(t);

		//tid = t->__sig;
		tid = mt;
	#else 
		//assume linux. do syscall
		tid = syscall(SYS_gettid/*224*/);
		if(tid < 0){
			//perror("syscall");
		}

		//pthread_id_np_t   tid;
		//tid = pthread_getthreadid_np();

	#endif
		// if(callerId){
		// 		printf("%s wants to know thread id: %lu\n",callerId, tid);
		// } else {
		// 	printf("Thread id: %lu\n", tid);
		// }
		return tid;
	}


