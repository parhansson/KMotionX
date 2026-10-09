// LogitechGArxControlLib.h : Defines the exported functions for the API
//
// Logitech Gaming Arx Control SDK
//
// Copyright (C) 2011-2014 Logitech. All rights reserved.
// Author: Tiziano Pigliucci
// Email: devtechsupport@logitech.com

#pragma once

#pragma pack(push, 1)

#define LOGI_CUSTOMICON_BYTEARRAY_SIZE 144 * 144 * 4

#define LOGI_ARX_ORIENTATION_PORTRAIT 0x01
#define LOGI_ARX_ORIENTATION_LANDSCAPE 0x10

#define LOGI_ARX_EVENT_FOCUS_ACTIVE 0x01
#define LOGI_ARX_EVENT_FOCUS_INACTIVE 0x02
#define LOGI_ARX_EVENT_TAP_ON_TAG 0x04
#define LOGI_ARX_EVENT_MOBILEDEVICE_ARRIVAL 0x08
#define LOGI_ARX_EVENT_MOBILEDEVICE_REMOVAL 0x10

#define LOGI_ARX_DEVICETYPE_IPHONE 0x01
#define LOGI_ARX_DEVICETYPE_IPAD 0x02
#define LOGI_ARX_DEVICETYPE_ANDROID_SMALL 0x03
#define LOGI_ARX_DEVICETYPE_ANDROID_NORMAL 0x04
#define LOGI_ARX_DEVICETYPE_ANDROID_LARGE 0x05
#define LOGI_ARX_DEVICETYPE_ANDROID_XLARGE 0x06
#define LOGI_ARX_DEVICETYPE_ANDROID_OTHER 0x07



typedef void (__cdecl *logiArxCb)(unsigned __int32 eventType, unsigned __int32 eventValue, wchar_t *eventArg, void * context);

typedef struct
{
    logiArxCb arxCallBack;
    void* arxContext;
}logiArxCbContext;


//Config
bool LogiArxGetConfigOptionNumber(wchar_t *configPath, double *defaultValue);
bool LogiArxGetConfigOptionBool(wchar_t *configPath, bool *defaultValue);
bool LogiArxGetConfigOptionColor(wchar_t *configPath, int *defaultRed, int *defaultGreen, int *defaultBlue);
bool LogiArxGetConfigOptionKeyInput(wchar_t *configPath, wchar_t *defaultValue, int bufferSize);
bool LogiArxSetConfigOptionLabel(wchar_t *configPath, wchar_t *label);

//Initializes the applet on the app with the given friendlyName
bool LogiArxInit(wchar_t * identifier, wchar_t * friendlyName, logiArxCbContext *callbackContext);

//Initialize the applet with a custom icon
bool LogiArxInitWithIcon(wchar_t * identifier, wchar_t * friendlyName, logiArxCbContext *callbackContext, unsigned char *iconByteArray);

//CONTENT TRANSFER

//Sends a file to the device/s from local filePath and assigns fileName to it. mimeType if assigned, specifies the MIME type of the file
bool LogiArxAddFileAs(wchar_t * filePath, wchar_t * fileName, wchar_t *mimeType = L"");

//Sends content to the device/s saving it to a virtual file called fileName. mimeType if assigned, specifies the MIME type of the file
bool LogiArxAddContentAs(const void* content, int size, wchar_t * fileName, wchar_t *mimeType = L"");

//Sends UTF8 string to the device/s saving it to a virtual file called fileName. mimeType if assigned, specifies the MIME type of the file
bool LogiArxAddUTF8StringAs(wchar_t* stringContent, wchar_t * fileName, wchar_t *mimeType = L"");

//Compresses the image specified by the RGBA byte array bitmap (interpreting the array using width and height) into a png file with name specified by fileName,  then sends it over to the device
bool LogiArxAddImageFromBitmap(unsigned char *bitmap, int width, int height, wchar_t * fileName);

//Sets which of the sent file is the index (first one to be displayed in the applet)
bool LogiArxSetIndex(wchar_t * fileName);

//DATA UPDATE

//Change at runtime a property on the tag with id tagId from the old value to the newValue
bool LogiArxSetTagPropertyById(wchar_t * tagId, wchar_t * prop, wchar_t *newValue);

//Change at runtime a property on the tags with class tagClass from the old value to the newValue
bool LogiArxSetTagsPropertyByClass(wchar_t * tagsClass, wchar_t * prop, wchar_t *newValue);

//Change at runtime the content of a tag with id tagId from the old to the newContent
bool LogiArxSetTagContentById(wchar_t * tagId, const wchar_t *newContent);

//Change at runtime the content on the tags with class tagClass from the old to the newContent
bool LogiArxSetTagsContentByClass(wchar_t * tagsClass, const wchar_t *newContent);

//Each function returns a bool, to get detailed info on the last error code call this function
int LogiArxGetLastError();

//Shuts down the applet on the app
void LogiArxShutdown();


#pragma pack(pop)