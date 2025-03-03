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

/**
 * @brief Generates a comprehensive JSON string of system information
 *
 * This function combines information from all hardware components into
 * a single JSON object with the following structure:
 *
 * {
 *   "gpu": [ ... ],           // Graphics adapters
 *   "motherboard": { ... },   // Motherboard details
 *   "cpu": [ ... ],           // Processors
 *   "memory": { ... },        // RAM configuration
 *   "storage": [ ... ],       // Storage devices
 *   "network": {              // Network adapters
 *     "ethernet": [ ... ],
 *     "wifi": [ ... ]
 *   },
 *   "audio": [ ... ],         // Audio devices
 *   "battery": { ... },       // Power/battery status
 *   "monitors": [ ... ]       // Display devices
 * }
 *
 * @param gpuList GPU information list
 * @param mbInfo Motherboard information
 * @param cpuList CPU information list
 * @param memInfo Memory information
 * @param storageList Storage device list
 * @param networkList Network adapter list
 * @param audioList Audio device list
 * @param batteryInfo Battery/power information
 * @param monitorList Monitor information list
 * @return char* Allocated JSON string, NULL if failed
 * @note Caller must free the returned string using freeJSONString()
 */
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

/**
 * @brief Frees memory allocated for JSON string
 *
 * @param jsonStr Pointer to JSON string to be freed
 */
void freeJSONString(char *jsonStr);

#endif // JSON_STRUCTURE_H
