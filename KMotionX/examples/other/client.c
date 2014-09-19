#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCK_PATH "kmotionsocket"

int main(void)
{
    char str[100];

    int socket_desc;
    struct sockaddr_un remote;

    if ((socket_desc = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    //prevent inherit file descriptor to Server process
    //http://stackoverflow.com/questions/6945865/process-started-from-system-command-in-c-inherits-parent-fds
    fcntl(socket_desc, F_SETFD, fcntl(socket_desc, F_GETFD) | FD_CLOEXEC);


    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);

    printf("Trying to connect...\n");
#ifdef __APPLE__
    remote.sun_len = sizeof(remote);
    if (connect(socket_desc, (struct sockaddr *)&remote, SUN_LEN(&remote)) == -1) {
#else
    int len;
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(socket_desc, (struct sockaddr *)&remote, len) == -1) {
#endif
        perror("connect failed launching server");
        //exit(1);
#ifndef FORK
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL){
            fprintf(stdout, "Current working dir: %s\n", cwd);
        }
        else{
            perror("getcwd() error");
            exit(1);
        }
        char command[1024];
        sprintf(command, "%s/%s", cwd,"KMotionServer");
        system(command);
		int i;
		for (i=0; i<100; i++) // try for a few secs
		{
			if (connect(socket_desc, (struct sockaddr *)&remote, SUN_LEN(&remote)) == -1) {
				perror("connect");
			 } else {
				break;
			 }
			usleep(50*1000);
		}

		if (i==100)
		{
			/*
			EntryCount--;
			if (ServerMessDisplayed) return 1;
			ServerMessDisplayed=TRUE;
			DoErrMsg("Unable to Connect to KMotion Server");
			PipeMutex->Unlock();
			*/
			exit(1);
		}
#else

		int pid;
		//pid = fork();
		char* program ="/Users/parhansson/KMotion431smake/Server/KMotionServer";
		char* argv[2];
		   argv[0] = "KMotionServer";
		   argv[1] = NULL;
		int pgid = getppid();
		pid = vfork();
		switch(pid)
		{
			case 0:

				//printf ("Am child, my ID: %d", getpid() );
				//printf ("Am child, my Parent id: %d",getppid() );
				//This is done by the child process.
				if(setpgid(0,pgid) == -1){
					perror("setpgid");
				}
				execv(argv[0], argv);

				//If execv returns, it must have failed.

				printf("Unknown command\n");
				_exit(EXIT_SUCCESS);  //exit() is unreliable here, so _exit must be used
				break;
			case -1:
				printf ("The child process has not created\n");
				exit(1);
				break;

			default:
				printf ("In default , process id: %d\n",getpid());
				int i;
				for (i=0; i<100; i++) // try for a few secs
				{
					if (connect(s, (struct sockaddr *)&remote, SUN_LEN(&remote)) == -1) {
						perror("connect");
					 } else {
						break;
					 }
					usleep(50*1000);
				}

				if (i==100)
				{

					//EntryCount--;
					//if (ServerMessDisplayed) return 1;
					//ServerMessDisplayed=TRUE;
					//DoErrMsg("Unable to Connect to KMotion Server");
					//PipeMutex->Unlock();

					exit(1);
				}
			break;
		} // switch case closed

#endif
    }

    printf("Connected.\n");
    int t;
    while(printf("> "), fgets(str, 100, stdin), !feof(stdin)) {
        if(strncmp(str,"exit",4) == 0){
        	close(socket_desc);
        	_exit(0);
        	break;
        }
    	if (send(socket_desc, str, strlen(str), 0) == -1) {
            perror("send");
            exit(1);
        }

        if ((t=recv(socket_desc, str, 100, 0)) > 0) {
            str[t] = '\0';
            printf("echo> %s", str);
        } else {
            if (t < 0) perror("recv");
            else printf("Server closed connection\n");
            exit(1);
        }
    }

    close(socket_desc);

    return 0;
}
