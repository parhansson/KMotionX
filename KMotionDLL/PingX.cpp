#include "StdAfx.h"
#include <stdio.h>
#include "Ping.h"
#include <libusb-1.0/libusb.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#ifndef _KMOTIONX

DWORD ScanAdapters(LPDWORD lpdwParam);
int ScanAdaptersErr();
DWORD ReceiveResponses(LPDWORD lpdwParam);
int JoinMulticastGroup(SOCKET s, struct addrinfo *group, struct addrinfo *iface);
int SetSendInterface(SOCKET s, struct addrinfo *iface);
int SetMulticastTtl(SOCKET s, int af, int ttl);
struct addrinfo *ResolveAddress(const char *addr, const char *port, int af, int type, int proto);
int FindIPAddr(unsigned long *IP, unsigned long *Subnet, int max_addr, int *nfound);
#endif

void *ScanKFLOPs(void *lpdwParam);

void CleanupFindKFLOPs()
{
    if (KFLOPListMutex != NULL)
    {
        pthread_mutex_destroy(KFLOPListMutex);
    }
    free(KFLOPListMutex);
    KFLOPListMutex = NULL;
}

pthread_mutex_t *KognaListMutex = NULL;
pthread_mutex_t *KFLOPListMutex = NULL;

int nKognas = 0;
KOGNA_INFO Kognas[MAX_KOGNAS]; // Adapter List
bool volatile FirstKognasScanComplete = false;

uint8_t nKFLOPs = 0;
KFLOP_INFO KFLOPs[MAX_KFLOPS]; // KFLOP Online list
int FindKognas()
{
    // Create a mutex with no initial owner
    KognaListMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    memset(KognaListMutex, 0, sizeof(pthread_mutex_t));
    // Initialize the mutex
    pthread_mutex_init(KognaListMutex, NULL);
    return 0;
}
int FindKFLOPs()
{
    // Create a mutex with no initial owner
    // KFLOPListMutex = new CMutex(
    //     FALSE,       // initially not owned
    //     "KFLOPList", // mutex name
    //     NULL);       // default security attributes
    log_info("FindKFLOPs");
    KFLOPListMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (KFLOPListMutex == NULL)
    {
        log_err("Failed to allocate memory for mutex");
        exit(EXIT_FAILURE);
    }
    memset(KFLOPListMutex, 0, sizeof(pthread_mutex_t));
    // Initialize the mutex
    pthread_mutex_init(KFLOPListMutex, NULL);

    pthread_attr_t attr;                                         // Thread attribute
    pthread_attr_init(&attr);                                    // Creating thread attributes
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);              // FIFO scheduling for threads
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Don't want threads (particualrly main)
                                                                 // waiting on each other
    pthread_t thr;

    // Create a thread to scan for KFLOPs
    if (pthread_create(&thr, &attr, &::ScanKFLOPs, NULL))
    {
        log_err("Could not create thread");
        return -1;
    }

    // We don't use pthread_join(), so allow system to clean up resources.
    // since we are using pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)
    // we should not need this
    // pthread_detach(thr);
    return 0;
}

void *ScanKFLOPs(void *lpdwParam)
{
    int ftStatus;
    int i, numDevs;
    struct ftdi_device_list *devlist, *curdev;
    char Manufacturer[128];
    char SerialNumber[16];
    char Description[64];
    struct ftdi_context *ftdi;
    uint8_t nDevsBefore;

    if (!(ftdi = ftdi_new()))
    {
        log_err("ftdi_new failed");
        pthread_exit(0);
        // exit(1);
    }
    // We need to generate a unique id for every process
    // if we have two processes with the same LocId KMotionserver will mix up callbacks
    // for test start two kflpConsole program and se if they mix up console answers
    // They still do, this is not the correct solution
    // Maybe LocId should be the same for different processes then just init randomValue with 0

    for (;;)
    {
        //log_info("Scanning for KFLOPs");

        int dwWaitResult = pthread_mutex_lock(KFLOPListMutex); // no time-out interval
        if (dwWaitResult == 0) // Equivalent to WAIT_OBJECT_0
        { 
            //numDevs = ftStatus = ftdi_usb_find_all(ftdi, &devlist, 0, 0);
            numDevs = ftStatus = ftdi_usb_find_all(ftdi, &devlist, VENDOR, PRODUCT);
            nDevsBefore = nKFLOPs;
            nKFLOPs = 0;
            if (numDevs > 0)
            {
                // go through the list and remove any non-dynomotion boards
                i = 0;
                for (curdev = devlist; curdev != NULL && i < MAX_BOARDS; i++)
                {
                    if ((ftStatus = ftdi_usb_get_strings(ftdi, curdev->dev, Manufacturer, sizeof(Manufacturer),
                                                         Description, sizeof(Description), SerialNumber, sizeof(SerialNumber))) < 0)
                    {
                        log_err("ftdi_usb_get_strings failed: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
                    }
                    else
                    {
                        if (strstr(Description, "KFLOP") != NULL ||
                            strstr(Description, "KMotion") != NULL ||
                            strstr(Description, "Dynomotion") != NULL)
                        {
                            uint8_t bus_number = libusb_get_bus_number(curdev->dev);
                            uint8_t device_address = libusb_get_device_address(curdev->dev);
                            KFLOPs[nKFLOPs].bus_number = bus_number;
                            KFLOPs[nKFLOPs].device_address = device_address;
                            KFLOPs[nKFLOPs].LocId = (bus_number << 8) | device_address;
                            strncpy(KFLOPs[nKFLOPs].Description, Description, 64);
                            strncpy(KFLOPs[nKFLOPs].SerialNumber, SerialNumber, 16);

                            // log_info("Device %d: LocId=%d, Bus=%u, Address=%u, SerialNumber=%s, Description=%s",
                            //         i, KFLOPs[nKFLOPs].LocId,
                            //         KFLOPs[nKFLOPs].bus_number, KFLOPs[nKFLOPs].device_address, KFLOPs[nKFLOPs].SerialNumber, KFLOPs[nKFLOPs].Description);

                            nKFLOPs++;
                        }
                    }
                    curdev = curdev->next;
                }
                ftdi_list_free(&devlist);
                //log_info("Found %d KFLOP(s)", nKFLOPs);
            }
            else if (numDevs == 0)
            {
                debug("ftdi_usb_find_all no devices found. status");
            }
            else
            {
                // this means ftStatus < 0
                log_err("ftdi_usb_find_all failed: %d (%s)", ftStatus, ftdi_get_error_string(ftdi));
            }
            //  Unlock the mutex after updating
            pthread_mutex_unlock(KFLOPListMutex);
            if(nDevsBefore != nKFLOPs)
            {
                // Notify the main thread that the list has changed
                // This is a placeholder for actual notification logic
                log_info("KFLOP list updated: %d devices found", nKFLOPs);
                for (size_t i = 0; i < 16; i++)
                {
                    // Notify the main thread or perform any necessary actions
                    // For example, you could use a condition variable or an event
                    // to signal that the list has changed.
                    log_info("Device %d: Bus %d, Address %d, LocId=%d, SerialNumber=%s, Description=%s", i, KFLOPs[i].bus_number, KFLOPs[i].device_address, KFLOPs[i].LocId, KFLOPs[i].SerialNumber, KFLOPs[i].Description);
                }
                
            }
        } else {
            log_err("pthread_mutex_lock failed: %d", dwWaitResult);
        }
        Sleep(1000);
    }
    ftdi_free(ftdi);
}

#ifndef _KMOTIONX

DWORD ScanAdapters(LPDWORD lpdwParam)
{
    for (;;)  // if some error, try again
    {
        int result = ScanAdaptersErr();
        if (result)
        {
            CStringW s;

            s.Format(Translate("Scanning for Kogna's Failed - Error %d"), result);
            MessageBoxW(NULL, s, L"KMotion", MB_ICONSTOP | MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_SYSTEMMODAL);

            WSACleanup();
            if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
                MessageBoxW(NULL, Translate("Winsock Startup Failed"), L"KMotion", MB_ICONSTOP|MB_OK|MB_TOPMOST|MB_SETFOREGROUND|MB_SYSTEMMODAL);
        }
        Sleep(1000);
    }
    return 0;
}

int ScanAdaptersErr()
{
    int i;
    char buf[BUF_SIZE];

    for (;;)
    {
        if (FindIPAddr(IPs, Subnet, MAX_ADAPTERS, &nfound)) return 1;

        // filter non-local adapters out
        int k = 0;
        for (i = 0; i < nfound; i++)
        {
            if ((IPs[i] & 0xff) == 192 || (IPs[i] & 0xff) == 172 || (IPs[i] & 0xff) == 10)
            {
                params[k].AdapterIP = IPs[i];
                params[k].result = -1;
                params[k].n = 0;
                k++;
            }
        }
        nfound = k;


        for (i = 0; i < nfound; i++)
        {
            // create a worker Thread to handle responses
            HANDLE Thread = CreateThread(
                NULL,                        /* no security attributes        */
                100000,                      /* stack size 100K        */
                (LPTHREAD_START_ROUTINE) ::ReceiveResponses, /* thread function       */
                &params[i],	    			 /* argument to thread function   */
                0,                           /* use default creation flags    */
                NULL);

            if (Thread == NULL) return -1;

            // multicast on every adapter
            SOCKET s;

            struct addrinfo* resmulti = NULL, * resbind = NULL, * resif = NULL;

            const char* gBindAddr = NULL,   // Address to bind socket to (default is 0.0.0.0 or ::)
                * gMulticast = "239.81.92.240",   // Multicast group to join
                * gPort = "25000";          // Port number to use

            int   gSocketType = SOCK_DGRAM,   // datagram
                gProtocol = IPPROTO_UDP,    // UDP
                gLoopBack = 0,              // Disable loopback?
                gTtl = 8;         // Multicast TTL value

            char gInterface[40];          // Interface to join the multicast group on

            sprintf(gInterface,"%d.%d.%d.%d", params[i].AdapterIP & 0xff, (params[i].AdapterIP >> 8) & 0xff,
                    (params[i].AdapterIP >> 16) & 0xff, (params[i].AdapterIP >> 24) & 0xff);

            // Resolve the multicast address
            resmulti = ResolveAddress(gMulticast, gPort, AF_UNSPEC, gSocketType, gProtocol);
            if (resmulti == NULL) return -2;
            
            // Resolve the binding address
            resbind = ResolveAddress(gBindAddr, "0", resmulti->ai_family, resmulti->ai_socktype, resmulti->ai_protocol);
            if (resbind == NULL) return -3;

            // Resolve the multicast interface
            resif = ResolveAddress(gInterface, "0", resmulti->ai_family, resmulti->ai_socktype, resmulti->ai_protocol);
            if (resif == NULL) return -4;

            // Create the socket. In Winsock 1 you don't need any special
            // flags to indicate multicasting.
            s = socket(resmulti->ai_family, resmulti->ai_socktype, resmulti->ai_protocol);
            if (s == INVALID_SOCKET) return -5;

            // Bind the socket to the local interface. This is done so we can receive data
            int rc = bind(s, resbind->ai_addr, (int)resbind->ai_addrlen);
            if (rc == SOCKET_ERROR) return -6;

            // Join the multicast group 
            rc = JoinMulticastGroup(s, resmulti, resif);
            if (rc == SOCKET_ERROR) return -7;

            // Set the send (outgoing) interface
            rc = SetSendInterface(s, resif);
            if (rc == SOCKET_ERROR) return -8;

            // Set the TTL to something else. The default TTL is one.
            rc = SetMulticastTtl(s, resmulti->ai_family, gTtl);
            if (rc == SOCKET_ERROR) return -9;

            memset(buf, 0, BUF_SIZE);
            sprintf(buf, "Kogna?");

            // Send some data
            rc = sendto(s, buf, BUF_SIZE, 0, resmulti->ai_addr, (int)resmulti->ai_addrlen);
            if (rc == SOCKET_ERROR) return -11;

            closesocket(s);

        }
        // wait for all adapters to finish
        for (i = 0; i < nfound; i++)
        {
            while (params[i].result == -1)
                Sleep(10);
        }

        // process every Kogna found
        DWORD dwWaitResult = WaitForSingleObject(KognaListMutex, INFINITE);  // no time-out interval
        if (dwWaitResult != WAIT_OBJECT_0) return -12;

        int n = 0;
        for (i = 0; i < nfound; i++)  // loop through Adapters
        {
            for (k = 0; k < params[i].n; k++)
            {
                // put it in the list
                Kognas[n].KognaIP = ntohl(params[i].KognaIP[k]);
                Kognas[n].AdapterIP = ntohl(params[i].AdapterIP);
                Kognas[n].KognaSerialNumber = params[i].KognaSerialNumber[k];
                n++;
            }
        }
        nKognas = n;
        ReleaseMutex(KognaListMutex);
        FirstKognasScanComplete = true;
    }
}


DWORD ReceiveResponses(LPDWORD lpdwParam)
{
    char* buf = NULL;
    INVOKE_PARAMS *p = (INVOKE_PARAMS*)lpdwParam;

    sockaddr_in socketAddress = { 0 };
    socketAddress.sin_family = PF_INET;
    socketAddress.sin_port = htons(25001);
    socketAddress.sin_addr.s_addr = p->AdapterIP;

    // Create the socket
    SOCKET mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (!mSocket)  return -1;

    int enable = 1;
    if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int)) < 0) return -1;

    //Bind
    if (bind(mSocket, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR) return -1;

    double timeout = 0.5;

    // Set timeout
    DWORD lBuffer[2] = { 0, 0 };
    int lSize;
    lBuffer[0] = static_cast<DWORD>(1000.0 * timeout);
    lSize = sizeof(DWORD);
    if (setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)lBuffer, lSize) != 0) return -1;

    // Check that we get what we set.
    DWORD lBufferout[2] = { 0, 0 };
    if (getsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)lBufferout, &lSize) != 0) return -1;

    // Receive and time
    char buffer[64];
    sockaddr_in senderAddr;
    int senderAddrSize = sizeof(senderAddr);

    for (int k = 0; k < MAX_KOGNAS; k++)  // assume there may be multiple responses
    {
        int transferred = recvfrom(mSocket, buffer, sizeof(buffer), 0,
            (sockaddr*)&senderAddr, &senderAddrSize);

        if (transferred != SOCKET_ERROR)
        {
            if (transferred == 64)
            {
                int r = sscanf(buffer + 13, "%d", &p->KognaSerialNumber[k]);
                buffer[13] = 0;  // nunn terminate
                if (strcmp("I am Kogna SN", buffer) == 0)
                {
                    p->KognaIP[k] = senderAddr.sin_addr.S_un.S_addr;
                    p->n++;
                }
            }
        }
        else
        {
            break;  // timeout
        }
    }

    int iResult = shutdown(mSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) return -1;

    iResult = closesocket(mSocket);
    if (iResult == SOCKET_ERROR) return -1;

    p->result = 0;  // flag Thread finished.

    return 0;
}


// Function: JoinMulticastGroup
// Description:
//    This function joins the multicast socket on the specified multicast
//    group. The structures for IPv4 and IPv6 multicast joins are slightly
//    different which requires different handlers. For IPv6 the scope-ID
//    (interface index) is specified for the local interface whereas for IPv4
//    the actual IPv4 address of the interface is given.
int JoinMulticastGroup(SOCKET s, struct addrinfo* group, struct addrinfo* iface)
{
    struct ip_mreq   mreqv4;
    char* optval = NULL;
    int    optlevel, option, optlen, rc;

    rc = NO_ERROR;
    // Setup the v4 option values and ip_mreq structure
    optlevel = IPPROTO_IP;
    option = IP_ADD_MEMBERSHIP;
    optval = (char*)&mreqv4;
    optlen = sizeof(mreqv4);

    mreqv4.imr_multiaddr.s_addr = ((SOCKADDR_IN*)group->ai_addr)->sin_addr.s_addr;
    mreqv4.imr_interface.s_addr = ((SOCKADDR_IN*)iface->ai_addr)->sin_addr.s_addr;

    if (rc != SOCKET_ERROR)
    {
        // Join the group
        rc = setsockopt(s, optlevel, option, optval, optlen);
    }
    return rc;
}

// Function: SetSendInterface
// Description:
//    This routine sets the send (outgoing) interface of the socket.
//    Again, for v4 the IP address is used to specify the interface while
//    for v6 its the scope-ID.
int SetSendInterface(SOCKET s, struct addrinfo* iface)
{
    char* optval = NULL;
    int   optlevel, option, optlen, rc;

    rc = NO_ERROR;

    // Setup the v4 option values
    optlevel = IPPROTO_IP;
    option = IP_MULTICAST_IF;
    optval = (char*)&((SOCKADDR_IN*)iface->ai_addr)->sin_addr.s_addr;
    optlen = sizeof(((SOCKADDR_IN*)iface->ai_addr)->sin_addr.s_addr);

    // Set send IF
    if (rc != SOCKET_ERROR)
    {
        // Set the send interface
        rc = setsockopt(s, optlevel, option, optval, optlen);
    }
    return rc;
}

// Function: SetMulticastTtl
// Description: This routine sets the multicast TTL value for the socket.
int SetMulticastTtl(SOCKET s, int af, int ttl)
{
    char* optval = NULL;
    int   optlevel, option, optlen, rc;

    // Set the options for V4
    optlevel = IPPROTO_IP;
    option = IP_MULTICAST_TTL;
    optval = (char*)&ttl;
    optlen = sizeof(ttl);
    // Set the TTL value
    rc = setsockopt(s, optlevel, option, optval, optlen);
    return rc;
}

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))


int FindIPAddr(unsigned long* IP, unsigned long* Subnet, int max_addr, int* nfound)
{

    int i;

    /* Variables used by GetIpAddrTable */
    PMIB_IPADDRTABLE pIPAddrTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;


    // Before calling AddIPAddress we use GetIpAddrTable to get
    // an adapter to which we can add the IP.
    pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(sizeof(MIB_IPADDRTABLE));
    if (pIPAddrTable == NULL) return 1;

    // Make an initial call to GetIpAddrTable to get the
    // necessary size into the dwSize variable
    if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
    {
        FREE(pIPAddrTable);
        pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(dwSize);
    }
    if (pIPAddrTable == NULL) return 1;

    // Make a second call to GetIpAddrTable to get the
    // actual data we want
    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR) return 1;

    *nfound = (int)(pIPAddrTable->dwNumEntries);
    for (i = 0; i < *nfound && i < max_addr; i++)
    {
        IP[i] = (u_long)pIPAddrTable->table[i].dwAddr;
        Subnet[i] = (u_long)pIPAddrTable->table[i].dwMask;
    }

    if (pIPAddrTable) {
        FREE(pIPAddrTable);
        pIPAddrTable = NULL;
    }

    return 0;
}


// Function: ResolveAddress
// Description:
//    This routine resolves the specified address and returns a list of addrinfo
//    structure containing SOCKADDR structures representing the resolved addresses.
//    Note that if 'addr' is non-NULL, then getaddrinfo will resolve it whether
//    it is a string literal address or a hostname.
struct addrinfo* ResolveAddress(const char* addr, const char* port, int af, int type, int proto)
{
    struct addrinfo hints, * res = NULL;
    int             rc;

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = ((addr) ? 0 : AI_PASSIVE);
    hints.ai_family = af;
    hints.ai_socktype = type;
    hints.ai_protocol = proto;

    rc = getaddrinfo(addr, port, &hints, &res);
    if (rc != 0) return NULL;
    return res;
}
#endif