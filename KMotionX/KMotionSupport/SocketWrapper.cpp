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

//namespace CFile {
//static int modeReadWrite = 0;
//} /* namespace CFile */
SocketWrapper::SocketWrapper() {

	socketDesc = -1;

}

SocketWrapper::~SocketWrapper() {
	if (socketDesc >= 0)
	    Close();
}

bool SocketWrapper::Open(const char* name, int mode){

	if(socketDesc == -1){

		if ((socketDesc = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
			perror("socket");
			exit(1);
		}

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
#endif
	if(connectResult == -1){
		//perror("connect failed:");
		return false;
	} else {
		return true;
	}
}


bool SocketWrapper::Open(unsigned int port, const char* hostname){

	if(socketDesc == -1){

		if ((socketDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("tcp socket");
			exit(1);
		}

	}

    char port_s[10];
    sprintf(port_s, "%u", port);

	struct addrinfo hints, *result;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;  // IPv4 only for now
	hints.ai_socktype = SOCK_STREAM;
	
	int rc = getaddrinfo(hostname, port_s, &hints, &result);
	if (rc) {
	    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
	    exit(1);
	}
	if (!result)
	    return false;

	rc = connect(socketDesc, result->ai_addr, result->ai_addrlen);
	freeaddrinfo(result);
	return rc >= 0;
}

#if 0
int SocketWrapper::Write(void* buffer, int size){
	int written = send(socketDesc, buffer, size, 0);
	if(written == -1){
		throw CFileException();
	} else {
		return written;
	}
}
#endif

int SocketWrapper::Write(const void* buffer, int size){
	int written = send(socketDesc, buffer, size, 0);
	if(written == -1){
		throw CFileException();
	} else {
		return written;
	}
}

int SocketWrapper::Read(char* buffer, int size){
	int result = recv(socketDesc,buffer,size,0);
	if(result < 0){
		throw CFileException();
	}
	return result;
}

int SocketWrapper::Close(){
	shutdown(socketDesc,SHUT_RDWR);
	int rc = close(socketDesc);
	socketDesc = -1;
	return rc;
}


