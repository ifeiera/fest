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

/**
 * @brief Container for relatively static system information
 *
 * Holds hardware information that rarely changes during runtime:
 * - Graphics adapters configuration
 * - Motherboard details
 * - CPU specifications
 * - Audio devices
 * - Display setup
 *
 * This information is typically collected once at startup
 * or when hardware configuration changes are detected
 *
 * @note All pointers must be freed individually
 */
typedef struct
{
    GPUList *gpuList;         // Graphics adapters
    MotherboardInfo *mbInfo;  // System board
    CPUList *cpuList;         // Processors
    AudioList *audioList;     // Audio devices
    MonitorList *monitorList; // Display devices
} StaticInfo;

/**
 * @brief Container for frequently updated system information
 *
 * Holds system metrics that change regularly:
 * - Memory usage
 * - Storage space
 * - Battery status
 * - Network status
 *
 * This information is collected at each monitoring interval
 * to provide current system status
 *
 * @note All pointers must be freed individually
 */
typedef struct
{
    MemoryInfo *memInfo;      // Memory metrics
    StorageList *storageList; // Storage volumes
    BatteryInfo *batteryInfo; // Power status
    NetworkList *networkList; // Network adapters
} DynamicInfo;

#endif // SYSTEM_INFO_INTERNAL_H
