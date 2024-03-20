// cli.cpp
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <CMutex.h>
#include "KMotionDLL.h" // KMOtion DLL Header

CKMotionDLL *KM; // one instance of the interface class

int WaitForReady(int waittime);
int consoleCallback(const char *msg);
void errorCallback(const char *msg);
void ctrlc_handler(int signal);
void cleanup();
void cleanupAndExit(int exitcode);

int main(int argc, char *argv[])
{

    // Install Ctrl+C signal handler
    if (signal(SIGINT, ctrlc_handler) == SIG_ERR) {
        perror("Error setting up signal handler");
        return 1;
    }


  char str[100];

  KM = new CKMotionDLL(0); // create as board 0
  int locations;
  int list[64];
  KM->ListLocations(&locations, list);
  printf("locations %d locid %d\n", locations, list[0]);

  char response[MAX_LINE];
  if (KM->CheckKMotionVersion())
  {
    printf("Version check failed\n");
    cleanupAndExit(1);
  }

  if (KM->SetErrMsgCallback(errorCallback))
  {
    printf("Failed to register error callback\n");
  }

  if (KM->SetConsoleCallback(consoleCallback))
  {
    printf("Failed to register console callback\n");
  }

  if (KM->WaitToken(true, 1000) == KMOTION_LOCKED) // see if we can get access
  {
    // Get the firmware date from the KMotion Card which
    // will be in PT (Pacific Time)
    KM->ReleaseToken();
    if (KM->WriteLineReadLine("Version", response))
    {
      printf("Version check failed\n");
      cleanupAndExit(1);
    }
    else
    {
      printf("Version: %s\n", response);
    }
  }
  else
  {
    printf("Failed to get lock\n");
    cleanupAndExit(1);
  }

  KM->ServiceConsole();

  printf("Connected.\n");

  while (printf(">"), fgets(str, 100, stdin), !feof(stdin))
  {
    if (strncmp(str, "exit", 4) == 0)
    {
      break;
    }
    
    str[strlen(str) - 1] = '\0';

    if (KM->ServiceConsole())
    {
      printf(">ServiceConsole Failed\n");
    }

    if (strlen(str) > 1)
    {
      if (KM->WaitToken(true, 1000) == KMOTION_LOCKED) // see if we can get access
      {
        //we need to set console callbacks for every call
        //if we start two kflopConsoles simultanously the last one setting the callbacks
        //will get all responses since they talk with the same board.
        if (KM->SetErrMsgCallback(errorCallback))
        {
          printf("Failed to register error callback\n");
        }

        if (KM->SetConsoleCallback(consoleCallback))
        {
          printf("Failed to register console callback\n");
        }
        // send command
        if (KM->WriteLineWithEcho(str))
        {
          KM->ReleaseToken();
          printf(">Command failed\n");
          cleanupAndExit(1);
        }

        // print all responses until we receive "Ready"
        if (WaitForReady(5000))
        {
          KM->ReleaseToken();
          printf("Timeout waiting for Ready\n");
          cleanupAndExit(1);
        }
        KM->ReleaseToken();
      }
      else
      {
        printf("Failed to get lock\n");
        // exit(1);
      }
    }
  }
  //disconnect before exiting
  cleanup();
  return 0;
}

int WaitForReady(int waittime)
{
  int timeout;
  char buf[300];

  do
  {
    timeout = KM->ReadLineTimeOut(buf, waittime);
    if (!timeout)
      printf("%s", buf);
  } while (!timeout && strncmp(buf, "Ready\r\n", 7) != 0);

  return timeout;
}

int consoleCallback(const char *msg)
{
  printf("CONSOLE> %s", msg);
  return 0;
}
void errorCallback(const char *msg)
{
  printf("ERROR> %s\n", msg);
}

// Function to handle Ctrl+C signal
void ctrlc_handler(int signal) {
    printf("\nCtrl+C received. Cleaning up and exiting.\n");
    cleanupAndExit(0);
}
void cleanup() {
    // Perform cleanup tasks here
    // Example: Closing files, freeing memory, etc.

    KM->Disconnect();
    delete KM;
}
void cleanupAndExit(int exitcode) {

    cleanup();
    exit(exitcode);
}
