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
 * SocketWrapper.cpp
 *
 *  Created on: 19 mar 2014
 *      Author: parhansson
 */

#include "SocketWrapper.h"
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>

SocketWrapper::SocketWrapper()
{
	socketDesc = -1;
}

SocketWrapper::~SocketWrapper() 
{
    Close();
}

bool SocketWrapper::Open(const char* name, int mode)
{

	if(socketDesc == -1){

		if ((socketDesc = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
            throw std::system_error(errno, std::system_category(), "Create local socket");

		//prevent inherit file descriptor to Server process
		//http://stackoverflow.com/questions/6945865/process-started-from-system-command-in-c-inherits-parent-fds
		fcntl(socketDesc, F_SETFD, fcntl(socketDesc, F_GETFD) | FD_CLOEXEC);
	}


	struct sockaddr_un remote;

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, name);
	//char buffer[512];
	//getcwd(buffer, 512);
	//debug("%s:%d Trying to connect socket %s\n",__FILE__,__LINE__, buffer);
	int connectResult;
#ifdef __APPLE__
	remote.sun_len = sizeof(remote);
	connectResult = connect(socketDesc, (struct sockaddr *)&remote, SUN_LEN(&remote));
#else
	int len;
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	connectResult = connect(socketDesc, (struct sockaddr *)&remote, len);
	//if (connectResult < 0)
    //        throw std::system_error(errno, std::system_category(), remote.sun_path);

#endif
	return connectResult >= 0;
}


struct gai_category_impl : public std::error_category {
  const char *name() const noexcept { return "DNS"; }
  std::string message(int err) const { return gai_strerror(err); }
};
 
const std::error_category &
gai_category()
{
  static gai_category_impl cat;
  return cat;
}

bool SocketWrapper::Open(unsigned int port, const char* hostname)
{

	if(socketDesc == -1) {
		if ((socketDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            throw std::system_error(errno, std::system_category(), "Create tcp socket");
	}

    char port_s[10];
    sprintf(port_s, "%u", port);

	struct addrinfo hints, *result;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;  // IPv4 only for now
	hints.ai_socktype = SOCK_STREAM;
	
	int rc = getaddrinfo(hostname, port_s, &hints, &result);
	if (rc) {
	    throw std::system_error(rc, gai_category(), hostname);
	}
	if (!result)
	    return false;

	rc = connect(socketDesc, result->ai_addr, result->ai_addrlen);
	if (rc >= 0) {
        int flag = 1;
        setsockopt(socketDesc, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
	}
	freeaddrinfo(result);
	return rc >= 0;
}

int SocketWrapper::Write(const void* buffer, int size)
{
	int written = send(socketDesc, buffer, size, 0);
	if(written < 0)
		throw std::system_error(errno, std::system_category(), "Write");
	return written;
}

int SocketWrapper::Read(char* buffer, int size)
{
	int result = recv(socketDesc,buffer,size,0);
	if(result < 0)
		throw std::system_error(errno, std::system_category(), "Read");
	return result;
}

int SocketWrapper::Close()
{
    if (socketDesc < 0)
        return 0;
	shutdown(socketDesc,SHUT_RDWR);
	int rc = close(socketDesc);
	socketDesc = -1;
	return rc;
}


