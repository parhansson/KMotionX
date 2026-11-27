#include "stdafx.h"
#include "Ping.h"
#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>
#include <ftdi.h>
#include <pthread.h>
#include <string.h>

// #define VID 0x0403 // Replace with your Vendor ID (example: FTDI default)
// #define PID 0xF231 // Replace with your Product ID (example: FTDI default)

void *ScanKFLOPs(void *lpdwParam);

int hotplug_callback(struct libusb_context *ctx, struct libusb_device *device,
                     libusb_hotplug_event event, void *user_data);
libusb_context *ctx = NULL;
libusb_hotplug_callback_handle hp_handle;
pthread_t event_thread;
volatile bool event_thread_running = false;  // set while event_thread is started and not yet joined
volatile bool event_thread_stop = false;

pthread_mutex_t *KognaListMutex = NULL;
pthread_mutex_t *KFLOPListMutex = NULL;

int nKognas = 0;
KOGNA_INFO Kognas[MAX_KOGNAS]; // Adapter List
bool volatile FirstKognasScanComplete = false;

uint8_t nKFLOPs;
KFLOP_INFO KFLOPs[MAX_KFLOPS]; // KFLOP Online list
void CreateKognaListMutex()
{
    // Create a mutex with no initial owner
    KognaListMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    memset(KognaListMutex, 0, sizeof(pthread_mutex_t));

    // Initialize the mutex
    pthread_mutex_init(KognaListMutex, NULL);
}
int FindKognas()
{
    CreateKognaListMutex();
    return 0;
}
void CleanupFindKFLOPs()
{
    // Clean up
    // The event thread must be stopped before libusb_exit, libusb asserts if
    // libusb_exit is called while another thread is inside libusb_handle_events.
    if (event_thread_running)
    {
        event_thread_stop = true;
        pthread_join(event_thread, NULL);
        event_thread_running = false;
    }
    if (ctx != NULL)
    {
        libusb_hotplug_deregister_callback(ctx, hp_handle);
        libusb_exit(ctx);
        ctx = NULL;
    }
    if (KFLOPListMutex != NULL)
    {
        pthread_mutex_destroy(KFLOPListMutex);
    }

    free(KFLOPListMutex);

    KFLOPListMutex = NULL;
    
    if (KognaListMutex != NULL)
    {
        pthread_mutex_destroy(KognaListMutex);
    }
    
    free(KognaListMutex);
    KognaListMutex = NULL;
}
int FindKFLOPs()
{
    log_info("FindKFLOPs");
    KFLOPListMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (KFLOPListMutex == NULL)
    {
        log_err("Failed to allocate memory for mutex");
        exit(EXIT_FAILURE);
    }
    // Initialize the mutex
    pthread_mutex_init(KFLOPListMutex, NULL);

    int rc;
    // Initialize libusb
    if (libusb_init(&ctx) != 0)
    {
        log_err("libusb initialization failed");
        return 1;
    }

    // Check for hotplug support
    if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG))
    {
        log_err("Hotplug capability is not supported on this system");
        libusb_exit(ctx);
        return 1;
    }



    // Register the hotplug callback
    rc = libusb_hotplug_register_callback(ctx,
                                          LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                          LIBUSB_HOTPLUG_ENUMERATE,
                                          VENDOR,
                                          LIBUSB_HOTPLUG_MATCH_ANY, // PRODUCT or PRODUCT_FTDI_DEFAULT, filtered in add_device
                                          LIBUSB_HOTPLUG_MATCH_ANY,
                                          hotplug_callback, NULL, &hp_handle);
    if (rc != LIBUSB_SUCCESS)
    {
        log_err("Error registering hotplug callback");
        libusb_exit(ctx);
        return 1;
    }

    // Start event handling in a separate thread, joined in CleanupFindKFLOPs
    event_thread_stop = false;
    if (pthread_create(&event_thread, NULL, &::ScanKFLOPs, (void *)ctx) != 0)
    {
        log_err("Failed to create event thread");
        return 1;
    }
    event_thread_running = true;

    // Main event handling loop
    // while (1) {
    // Handle events
    // libusb_handle_events(ctx);

    // Print connected devices
    int dwWaitResult = pthread_mutex_lock(KFLOPListMutex); // no time-out interval
    if (dwWaitResult == 0)                                 // Equivalent to WAIT_OBJECT_0
    {

        log_info("Connected devices:\n");
        for (int i = 0; i < nKFLOPs; i++)
        {
            log_info("Device %d: VID=0x%04x PID=0x%04x Serial=%s Description=%s\n",
                   i + 1, KFLOPs[i].vid, KFLOPs[i].pid, KFLOPs[i].SerialNumber, KFLOPs[i].Description);
        }
        pthread_mutex_unlock(KFLOPListMutex);
    }

    // Sleep for a while to avoid constant polling
    // usleep(1000000); // 1 second
    //}
    log_info("FindKFLOPs DONE");
    return 0;
}

// KFLOPs may use the Dynomotion PID or the FTDI default PID (built-in Windows drivers)
static bool IsKFLOPId(uint16_t vid, uint16_t pid)
{
    return vid == VENDOR && (pid == PRODUCT || pid == PRODUCT_FTDI_DEFAULT);
}

// Function to update the device array on device arrival
void add_device(libusb_device *device, libusb_device_descriptor *desc)
{
    uint16_t vid = desc->idVendor;
    uint16_t pid = desc->idProduct;
    if (!IsKFLOPId(vid, pid))
        return;

    // Lock the mutex before updating the device list
    if (pthread_mutex_lock(KFLOPListMutex) != 0)
        return;

    if (nKFLOPs < MAX_KFLOPS)
    {
        struct ftdi_context *ftdi = ftdi_new();
        if (!ftdi)
        {
            log_err("Failed to create ftdi context");
            pthread_mutex_unlock(KFLOPListMutex);
            return;
        }

        KFLOP_INFO *info = &KFLOPs[nKFLOPs];
        memset(info, 0, sizeof(*info));

        // Get the serial number and description (returns < 0 on error)
        int rc = ftdi_usb_get_strings(ftdi, device, info->Manufacturer, sizeof(info->Manufacturer),
                                      info->Description, sizeof(info->Description),
                                      info->SerialNumber, sizeof(info->SerialNumber));
        if (rc < 0)
        {
            log_err("ftdi_usb_get_strings failed: %d (%s)", rc, ftdi_get_error_string(ftdi));
        }
        else if (strstr(info->Description, "KFLOP") != NULL ||
                 strstr(info->Description, "KMotion") != NULL ||
                 strstr(info->Description, "Dynomotion") != NULL)
        {
            info->vid = vid;
            info->pid = pid;

            // The bus number is 0 on some systems (e.g. macOS), the device address never is.
            // Pack both into LocId, CKMotionIO::Connect opens the device by bus and address.
            uint8_t bus_number = libusb_get_bus_number(device);
            uint8_t device_address = libusb_get_device_address(device);
            info->LocId = (bus_number << 8) | device_address;
            info->bus_number = bus_number;
            info->device_address = device_address;

            nKFLOPs++;
            log_info("KFLOP list updated: %d devices found", nKFLOPs);
            log_info("Device %d: Bus %d, Address %d, LocId=%d, SerialNumber=%s, Description=%s",
                     nKFLOPs - 1, info->bus_number, info->device_address, info->LocId, info->SerialNumber, info->Description);
        }

        ftdi_free(ftdi);
    }

    // Unlock the mutex after updating
    pthread_mutex_unlock(KFLOPListMutex);
}

// Function to remove a device from the list
void remove_device(libusb_device *device, libusb_device_descriptor *desc)
{
    // Lock the mutex before updating the device list
    int dwWaitResult = pthread_mutex_lock(KFLOPListMutex); // no time-out interval
    if (dwWaitResult == 0)
    {
        uint16_t vid = desc->idVendor;
        uint16_t pid = desc->idProduct;
        for (int i = 0; i < nKFLOPs; i++)
        {
            uint8_t bus_number = libusb_get_bus_number(device);
            uint8_t device_address = libusb_get_device_address(device);
            int locationId = (bus_number << 8) | device_address;
            // if (KFLOPs[i].vid == vid && KFLOPs[i].pid == pid && strcmp(KFLOPs[i].SerialNumber, serial_number) == 0)
            if (KFLOPs[i].vid == vid && KFLOPs[i].pid == pid && KFLOPs[i].LocId == locationId)
            {
                log_info("KFLOP on bus %03u address %03u removed", bus_number, device_address);
                // Shift the rest of the array to fill the gap
                for (int j = i; j < nKFLOPs - 1; j++)
                {
                    KFLOPs[j] = KFLOPs[j + 1];
                }
                nKFLOPs--;
                break;
            }
        }

        // Unlock the mutex after updating
        pthread_mutex_unlock(KFLOPListMutex);
    }
}

// Hotplug callback for libusb
int hotplug_callback(struct libusb_context *ctx, struct libusb_device *device,
                     libusb_hotplug_event event, void *user_data)
{
    struct libusb_device_descriptor desc;
    libusb_get_device_descriptor(device, &desc);

    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
    {
        log_info("Device connected: VID=0x%04x PID=0x%04x", desc.idVendor, desc.idProduct);
        add_device(device, &desc);
    }
    else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)
    {
        log_info("Device disconnected: VID=0x%04x PID=0x%04x", desc.idVendor, desc.idProduct);
        // Assuming the serial number was already fetched and known
        remove_device(device, &desc); // Here you would pass the correct serial number if known
    }

    return 0;
}

void *ScanKFLOPs(void *lpdwParam)
{
    libusb_context *ctx = (libusb_context *)lpdwParam;
    while (!event_thread_stop)
    {
        // Bounded wait so the thread notices event_thread_stop
        struct timeval tv = {0, 100000};
        int rc = libusb_handle_events_timeout(ctx, &tv);
        if (rc != LIBUSB_SUCCESS)
        {
            log_err("libusb_handle_events() failed: %s", libusb_error_name(rc));
            break;
        }
    }
    return NULL;
}
