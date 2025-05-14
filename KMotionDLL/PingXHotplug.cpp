#include "StdAfx.h"
#include "Ping.h"
#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <ftdi.h>
#include <pthread.h>
#include <string.h>

#define VID 0x0403 // Replace with your Vendor ID (example: FTDI default)
#define PID 0xF231 // Replace with your Product ID (example: FTDI default)

void *ScanKFLOPs(void *lpdwParam);

int hotplug_callback(struct libusb_context *ctx, struct libusb_device *device,
                     libusb_hotplug_event event, void *user_data);
libusb_context *ctx = NULL;
libusb_hotplug_callback_handle hp_handle;

pthread_mutex_t *KognaListMutex = NULL;
pthread_mutex_t *KFLOPListMutex = NULL;

int nKognas = 0;
KOGNA_INFO Kognas[MAX_KOGNAS]; // Adapter List
bool volatile FirstKognasScanComplete = false;

uint8_t nKFLOPs;
KFLOP_INFO KFLOPs[MAX_KFLOPS]; // KFLOP Online list
int FindKognas()
{
    // Create a mutex with no initial owner
    KognaListMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));

    // Initialize the mutex
    pthread_mutex_init(KFLOPListMutex, NULL);

    return 0;
}
void CleanupFindKFLOPs()
{
    // Clean up
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

    // Initialize the mutex
    pthread_mutex_init(KFLOPListMutex, NULL);

    // Register the hotplug callback
    rc = libusb_hotplug_register_callback(ctx,
                                          LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                          LIBUSB_HOTPLUG_ENUMERATE,
                                          // LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                          VID, PID,
                                          LIBUSB_HOTPLUG_MATCH_ANY,
                                          hotplug_callback, NULL, &hp_handle);
    if (rc != LIBUSB_SUCCESS)
    {
        log_err("Error registering hotplug callback");
        libusb_exit(ctx);
        return 1;
    }

    pthread_t event_thread;

    // Start event handling in a separate thread
    if (pthread_create(&event_thread, NULL, &::ScanKFLOPs, (void *)ctx) != 0)
    {
        log_err("Failed to create event thread");
        return 1;
    }
    // Wait for the event thread to finish (if needed)
    // pthread_join(event_thread, NULL);

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

// Function to update the device array on device arrival
void add_device(libusb_device *device, libusb_device_descriptor *desc)
{
    uint16_t vid = desc->idVendor;
    uint16_t pid = desc->idProduct;
    if (vid == VID && pid == PID)
    {

        // Lock the mutex before updating the device list
        int dwWaitResult = pthread_mutex_lock(KFLOPListMutex); // no time-out interval
        if (dwWaitResult == 0)
        {

            if (nKFLOPs < MAX_KFLOPS)
            {
                struct ftdi_context *ftdi = ftdi_new();
                if (!ftdi)
                {
                    log_err("Failed to create ftdi context");
                    pthread_mutex_unlock(KFLOPListMutex);
                    return;
                }

                // Open FTDI device
                if (ftdi_usb_open_dev(ftdi, device) == 0)
                {
                    KFLOP_INFO *info = &KFLOPs[nKFLOPs];
                    info->vid = desc->idVendor;
                    info->pid = desc->idProduct;

                    // Get bus number and device address
                    // TODO maybe we should use both bus and device adress for better accuracy
                    // OR serialnumber
                    uint8_t bus_number = libusb_get_bus_number(device);
                    uint8_t device_address = libusb_get_device_address(device);
                    // hack to pack bus and address into LocId
                    info->LocId = (bus_number << 8) | device_address;
                    // Print bus number and device address
                    log_info("Device on bus %03u address %03u connected", bus_number, device_address);

                    // Get the serial number and description
                    if (ftdi_usb_get_strings(ftdi, device, info->Manufacturer, sizeof(info->Manufacturer), info->Description, sizeof(info->Description), info->SerialNumber, sizeof(info->SerialNumber)) == 0)
                    {
                        nKFLOPs++;
                    }

                    ftdi_usb_close(ftdi);
                }
                else
                {
                    log_err("Unable to open FTDI device");
                }

                ftdi_free(ftdi);
            }

            // Unlock the mutex after updating
            pthread_mutex_unlock(KFLOPListMutex);
        }
    }
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
                log_info("Device on bus %03u address %03u removed", bus_number, device_address);
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
    while (1)
    {
        int rc = libusb_handle_events(ctx);
        if (rc != LIBUSB_SUCCESS)
        {
            log_err("libusb_handle_events() failed: %s", libusb_error_name(rc));
            break;
        }
        usleep(10000); // Optional sleep
    }
    return NULL;
}
