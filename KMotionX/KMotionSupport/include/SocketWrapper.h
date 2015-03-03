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
 * SocketWrapper.h
 *
 *  Created on: 19 mar 2014
 *      Author: parhansson
 */

#ifndef SOCKETWRAPPER_H_
#define SOCKETWRAPPER_H_
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <fcntl.h>
#include <string>
#include <exception>
#include <system_error>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/tcp.h>



namespace CFile {

const int modeReadWrite = 0;

} /* namespace CFile */



class SocketWrapper {
public:
	SocketWrapper();
	virtual ~SocketWrapper();

	bool Open(const char* name, int mode);              // For local pipe
	bool Open(unsigned int port, const char* hostname); // For TCP socket

	//int Write(void* buffer, int size);
	int Write(const void* buffer, int size);

	int Read(char* buffer, int size);
	int Close();

private:
	int socketDesc;

};



#endif /* SOCKETWRAPPER_H_ */
