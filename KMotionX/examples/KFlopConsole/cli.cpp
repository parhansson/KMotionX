// cli.cpp

#include "stdlib.h"
#include <CMutex.h>
#include "KMotionDLL.h"  // KMOtion DLL Header

CKMotionDLL *KM;                // one instance of the interface class

int WaitForReady(int waittime);

int consoleCallback(const char* msg){
  printf("CONSOLE> %s",msg );
  return 0;
}
void errorCallback(const char* msg){
  printf("ERROR> %s\n",msg );
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

  if(KM->SetErrMsgCallback(errorCallback)){
    printf("Faild to register error callback\n");
  }

  if(KM->SetConsoleCallback(consoleCallback)){
    printf("Faild to register console callback\n");
  }

  if (KM->WaitToken(true, 1000) == KMOTION_LOCKED)  // see if we can get access
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

  printf("Connected.\n");

  while(printf(">"), fgets(str, 100, stdin), !feof(stdin)) {
    if(strncmp(str,"exit",4) == 0){
            break;
          }
          str[strlen(str) - 1] = '\0';


    if (KM->ServiceConsole()) {
      printf(">ServiceConsole Failed\n");
    }

    if (strlen(str) > 1) {
      if (KM->WaitToken(true, 1000) == KMOTION_LOCKED) // see if we can get access
          {
        // send command
        if (KM->WriteLineWithEcho(str)) {
          KM->ReleaseToken();
          printf(">Command failed\n");
          exit(1);
        }

        // print all responses until we receive "Ready"
        if (WaitForReady(5000)) {
          KM->ReleaseToken();
          printf("Timeout waiting for Ready\n");
          exit(1);
        }
        KM->ReleaseToken();
      } else {
        printf("Failed to get lock\n");
        //exit(1);
      }
    }
  }
  return 0;
}

int WaitForReady(int waittime)
{
  int timeout;
  char buf[300];

  do
  {
    timeout = KM->ReadLineTimeOut(buf,waittime);
    if (!timeout) printf("%s",buf);
  }
  while (!timeout && strncmp(buf,"Ready\r\n",7)!=0);

  return timeout;
}
