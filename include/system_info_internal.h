#ifndef SYSTEM_INFO_INTERNAL_H
#define SYSTEM_INFO_INTERNAL_H

#include "gpu_info.h"
#include "motherboard_info.h"
#include "cpu_info.h"
#include "memory_info.h"
#include "storage_info.h"
#include "network_info.h"
#include "audio_info.h"
#include "battery_info.h"
#include "monitor_info.h"

// Structure for storing static information
typedef struct
{
    GPUList *gpuList;
    MotherboardInfo *mbInfo;
    CPUList *cpuList;
    AudioList *audioList;
    MonitorList *monitorList;
} StaticInfo;

// Structure for storing dynamic information
typedef struct
{
    MemoryInfo *memInfo;
    StorageList *storageList;
    BatteryInfo *batteryInfo;
    NetworkList *networkList;
} DynamicInfo;

#endif // SYSTEM_INFO_INTERNAL_H
