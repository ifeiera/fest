#ifndef JSON_STRUCTURE_H
#define JSON_STRUCTURE_H

#include "gpu_info.h"
#include "motherboard_info.h"
#include "cpu_info.h"
#include "memory_info.h"
#include "storage_info.h"
#include "network_info.h"
#include "audio_info.h"
#include "battery_info.h"
#include "monitor_info.h"

// Function for generating JSON string
char *generateSystemInfoJSON(
    GPUList *gpuList,
    MotherboardInfo *mbInfo,
    CPUList *cpuList,
    MemoryInfo *memInfo,
    StorageList *storageList,
    NetworkList *networkList,
    AudioList *audioList,
    BatteryInfo *batteryInfo,
    MonitorList *monitorList);

// Function for freeing JSON string
void freeJSONString(char *jsonStr);

#endif // JSON_STRUCTURE_H
