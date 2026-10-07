#ifdef _KMOTIONX
#ifndef PING_H_
#define PING_H_
#include "dbg.h"
int FindKognas();
int FindKFLOPs();
void CleanupFindKFLOPs();
#define MAX_KOGNAS 16
#define MAX_KFLOPS 16

typedef struct
{
    unsigned long KognaIP;
    unsigned long AdapterIP;
    int KognaSerialNumber;
} KOGNA_INFO;


extern int nKognas;
extern KOGNA_INFO Kognas[MAX_KOGNAS];  // Kogna Online list
extern volatile bool FirstKognasScanComplete;

extern uint8_t nKFLOPs;
//struct mimicing FT_DEVICE_LIST_INFO_NODE for convenience
// typedef struct _ft_device_list_info_node {
// 	ULONG Flags;
//  ULONG Type;
// 	ULONG ID;
// 	DWORD LocId;
// 	char SerialNumber[16];
// 	char Description[64];
// 	FT_HANDLE ftHandle;
// } FT_DEVICE_LIST_INFO_NODE;

typedef struct
{
    int LocId;
    char SerialNumber[16]; 
    char Description[64];
    char Manufacturer[64];
    uint16_t vid;
    uint16_t pid;
    uint8_t bus_number;
    uint8_t device_address;     
} KFLOP_INFO;

extern KFLOP_INFO KFLOPs[MAX_KFLOPS];  // KFLOP Online list

extern pthread_mutex_t *KognaListMutex;
extern pthread_mutex_t *KFLOPListMutex;
#endif //PING_H_
#else
#pragma once

int FindKognas();
int FindKFLOPs();

#define MAX_KOGNAS 16
#define MAX_KFLOPS 16

typedef struct
{
    unsigned long KognaIP;
    unsigned long AdapterIP;
    int KognaSerialNumber;
} KOGNA_INFO;


extern int nKognas;
extern KOGNA_INFO Kognas[MAX_KOGNAS];  // Kogna Online list
extern volatile bool FirstKognasScanComplete;

extern DWORD nKFLOPs;
extern FT_DEVICE_LIST_INFO_NODE KFLOPs[MAX_KFLOPS];  // KFLOP Online list

extern HANDLE KognaListMutex;
extern HANDLE KFLOPListMutex;
#endif