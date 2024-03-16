#include "StdAfx.h"
#include <stdio.h>
#include "Ping.h"

void *ScanKFLOPs(void *lpdwParam);

CMutex *KognaListMutex;
CMutex *KFLOPListMutex;

int nKognas = 0;
KOGNA_INFO Kognas[MAX_KOGNAS];  // Adapter List
bool volatile FirstKognasScanComplete = false;

uint8_t nKFLOPs;
KFLOP_INFO KFLOPs[MAX_KFLOPS]; // KFLOP Online list
int FindKognas()
{
    // Create a mutex with no initial owner
    KognaListMutex = new CMutex(
        FALSE,          // initially not owned
        NULL,           // unnamed mutex
        NULL);          // default security attributes
    return 0;
}
int FindKFLOPs()
{
    // Create a mutex with no initial owner
    KFLOPListMutex = new CMutex(
        FALSE,          // initially not owned
        NULL,           // unnamed mutex
        NULL);          // default security attributes

    pthread_t thr;

    if (pthread_create(&thr, NULL, &::ScanKFLOPs, NULL))
    {
        printf("Could not create thread\n");
        return -1;
    }

    // We don't use pthread_join(), so allow system to clean up resources.
    pthread_detach(thr);
    return 0;
}
#ifndef LIB_FTDI
void *ScanKFLOPs(void *lpdwParam)
{
    FT_STATUS ftStatus;
    DWORD numDevs;

    for (;;)
    {
        ftStatus = FT_CreateDeviceInfoList(&numDevs);

        if (ftStatus == FT_OK)
        {
            DWORD dwWaitResult = WaitForSingleObject(KFLOPListMutex, INFINITE); // no time-out interval
            if (dwWaitResult == WAIT_OBJECT_0)
            {
                if (numDevs > 0)
                {
                    ftStatus = FT_GetDeviceInfoList(KFLOPs, &numDevs);

                    nKFLOPs = 0;
                    for (int i = 0; i < (int)numDevs; i++)
                    {
                        if (strstr(KFLOPs[i].Description, "KFLOP") != NULL ||
                            strstr(KFLOPs[i].Description, "KMotion") != NULL ||
                            strstr(KFLOPs[i].Description, "Dynomotion") != NULL)
                        {
                            KFLOPs[nKFLOPs++] = KFLOPs[i];
                        }
                    }
                }
                else
                {
                    nKFLOPs = 0;
                }
                ReleaseMutex(KFLOPListMutex);
            }
        }

        Sleep(1000);
    }
#else
void *ScanKFLOPs(void *lpdwParam)
{
    int ftStatus;
    int i, numDevs;
    struct ftdi_device_list *devlist, *curdev;
    char Manufacturer[128];
    //	char SerialNumber[16];
    char Description[128];
    struct ftdi_context *ftdi;
    //We need to generate a unique id for every process
    //if we have two processes with the same LocId KMotionserver will mix up callbacks
    //for test start two kflpConsole program and se if they mix up console answers
    //They still do, this is not the correct solution
    //Maybe LocId should be the same for different processes then just init randomValue with 0
    uint8_t locIdSeed = 0; //kmx::randomInt();
    for (;;)
    {
        if ((ftdi = ftdi_new()) != 0)
        {
            KFLOPListMutex->Lock();

            numDevs = ftStatus = ftdi_usb_find_all(ftdi, &devlist, VENDOR, PRODUCT);
            //numDevs = ftStatus = ftdi_usb_find_all(ftdi, &devlist, 0x2341, 0x8037);
            nKFLOPs = 0;
            if (numDevs > 0)
            {
                // go through the list and remove any non-dynomotion boards
                i = 0;
                for (curdev = devlist; curdev != NULL && i < MAX_BOARDS; i++)
                {
                    if ((ftStatus = ftdi_usb_get_strings(ftdi, curdev->dev, Manufacturer, sizeof(Manufacturer),
                                                         Description, sizeof(Description), NULL, 0)) < 0)
                    {
                        debug("ftdi_usb_get_strings failed: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
                    }
                    else
                    {
                        debug("%d '%s' '%s'", ftStatus, Manufacturer, Description);
                        if (strstr(Description, "KFLOP") != NULL ||
                            strstr(Description, "KMotion") != NULL ||
                            strstr(Description, "Dynomotion") != NULL)
                        {
                            KFLOPs[nKFLOPs++].LocId = i + locIdSeed;
                        }
                    }
                    curdev = curdev->next;
                }
                ftdi_list_free(&devlist);
            }
            else if (numDevs == 0)
            {
                debug("ftdi_usb_find_all no devices found", ftStatus);
            }
            else
            {
                // this means ftStatus < 0
                debug("ftdi_usb_find_all failed: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
            }
            KFLOPListMutex->Unlock();
            ftdi_free(ftdi);
        }
        else
        {
            debug("ftdi_new failed\n");
        }

        Sleep(1000);
    }
}
#endif
