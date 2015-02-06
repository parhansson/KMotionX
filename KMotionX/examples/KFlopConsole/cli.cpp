// cli.cpp

#include "stdlib.h"
#include <CMutex.h>
#include "../../KMotionDLL/KMotionDLL.h"  // KMOtion DLL Header

CKMotionDLL *KM;                // one instance of the interface class


int console(const char* msg){
	printf("CONSOLE> %s",msg );
	return 0;
}

int main(int argc, char* argv[])
{

	char str[100];

	KM = new CKMotionDLL(0);  // create as board 0
	char response[MAX_LINE];
	if(KM->CheckKMotionVersion()){
		printf("Version check failed\n");
		exit(1);
	}

	if(KM->SetConsoleCallback(console)){
		printf("Faild to register console callback\n");
	}

	if (KM->KMotionLock() == KMOTION_LOCKED)  // see if we can get access
	{
		// Get the firmware date from the KMotion Card which
		// will be in PT (Pacific Time)
		KM->ReleaseToken();
		if(KM->WriteLineReadLine("Version",response)){
			printf("Version check failed\n");
			exit(1);
		} else {
			printf("Version: %s\n",response);

		}
	} else {
		printf("Failed to get lock\n");
		exit(1);
	}

	KM->ServiceConsole();

	printf(">Connected.\n");
	    while(printf("> "), fgets(str, 100, stdin), !feof(stdin)) {
	        if(strncmp(str,"exit",4) == 0){
	        	break;
	        }
	        str[strlen(str) - 1] = '\0';
	    	if(KM->ServiceConsole()){
	    		printf(">ServiceConsole Failed\n");
	    	}
	    	if(strlen(str) > 1){
		        if(strncmp(str,"Echo",4) == 0){
					if(KM->WriteLine(str)){
						printf(">Command failed\n");
					}
		        } else {
		        #if 1
		            KM->WriteLine(str);
		        #else
					if(KM->WriteLineReadLine(str,response)){
						printf(">Command failed\n");
					} else {
						printf("< %s\n",response);

					}
				#endif
		        }
	    	}
	    }
	return 0;
}

