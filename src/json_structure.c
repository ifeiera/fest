#include "json_structure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSON_BUFFER_SIZE 32768 // 32KB initial buffer

static void appendString(char **buffer, size_t *bufferSize, size_t *position, const char *str)
{
    size_t len = strlen(str);
    while (*position + len + 1 >= *bufferSize)
    {
        *bufferSize *= 2;
        *buffer = (char *)realloc(*buffer, *bufferSize);
    }
    strcpy_s(*buffer + *position, *bufferSize - *position, str);
    *position += len;
}

// Function for appending GPU information
static void appendGPUInfo(char **buffer, size_t *bufferSize, size_t *position, GPUList *gpuList)
{
    appendString(buffer, bufferSize, position, "  \"gpu\": [\n");
    for (UINT i = 0; i < gpuList->count; i++)
    {
        char temp[1024];
        GPUInfo *gpu = &gpuList->gpus[i];
        _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                    "    {\n"
                    "      \"name\": \"%s\",\n"
                    "      \"vram\": %llu,\n"
                    "      \"shared_memory\": %llu,\n"
                    "      \"type\": \"%s\"\n"
                    "    }%s",
                    getGPUName(gpu),
                    getGPUDedicatedMemory(gpu),
                    getGPUSharedMemory(gpu),
                    isIntegratedGPU(gpu) ? "iGPU" : "dGPU",
                    i < gpuList->count - 1 ? ",\n" : "\n");
        appendString(buffer, bufferSize, position, temp);
    }
    appendString(buffer, bufferSize, position, "  ],\n");
}

// Function for appending Motherboard information
static void appendMotherboardInfo(char **buffer, size_t *bufferSize, size_t *position, MotherboardInfo *mbInfo)
{
    char temp[2048];
    _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                "  \"motherboard\": {\n"
                "    \"manufacturer\": \"%s\",\n"
                "    \"product\": \"%s\",\n"
                "    \"serial_number\": \"%s\",\n"
                "    \"bios_version\": \"%s\",\n"
                "    \"bios_serial\": \"%s\",\n"
                "    \"system_sku\": \"%s\"\n"
                "  },\n",
                getMotherboardManufacturer(mbInfo),
                getMotherboardProduct(mbInfo),
                getMotherboardSerial(mbInfo),
                getMotherboardBiosVersion(mbInfo),
                getMotherboardBiosSerial(mbInfo),
                getMotherboardSystemSKU(mbInfo));
    appendString(buffer, bufferSize, position, temp);
}

// Helper function for cleaning string from unnecessary whitespace
static void trimString(char *str)
{
    if (!str)
        return;

    // Trim trailing spaces
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1]))
    {
        str[len - 1] = '\0';
        len--;
    }

    // Trim leading spaces
    char *start = str;
    while (isspace((unsigned char)*start))
        start++;

    if (start != str)
    {
        size_t newLen = len - (start - str);
        memmove(str, start, newLen + 1);
    }
}

// Function for appending CPU information
static void appendCPUInfo(char **buffer, size_t *bufferSize, size_t *position, CPUList *cpuList)
{
    appendString(buffer, bufferSize, position, "  \"cpu\": [\n");
    for (UINT i = 0; i < cpuList->count; i++)
    {
        char temp[1024];
        CPUInfo *cpu = &cpuList->cpus[i];

        // Get CPU name and trim it
        char cpuName[256];
        strncpy_s(cpuName, sizeof(cpuName), getCPUName(cpu), _TRUNCATE);
        trimString(cpuName);

        _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                    "    {\n"
                    "      \"name\": \"%s\",\n"
                    "      \"cores\": %u,\n"
                    "      \"threads\": %u,\n"
                    "      \"clock_speed\": %u\n"
                    "    }%s",
                    cpuName,
                    getCPUCores(cpu),
                    getCPUThreads(cpu),
                    getCPUClockSpeed(cpu),
                    i < cpuList->count - 1 ? ",\n" : "\n");
        appendString(buffer, bufferSize, position, temp);
    }
    appendString(buffer, bufferSize, position, "  ],\n");
}

// Function for appending Memory information
static void appendMemoryInfo(char **buffer, size_t *bufferSize, size_t *position, MemoryInfo *memInfo)
{
    appendString(buffer, bufferSize, position, "  \"memory\": {\n");

    char temp[2048];
    _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                "    \"total\": %.2f,\n"
                "    \"available\": %.2f,\n"
                "    \"used\": %.2f,\n"
                "    \"usage_percent\": %lu,\n",
                bytesToGB(memInfo->totalPhys),
                bytesToGB(memInfo->availPhys),
                bytesToGB(memInfo->usedPhys),
                memInfo->memoryLoad);
    appendString(buffer, bufferSize, position, temp);

    // RAM slots
    appendString(buffer, bufferSize, position, "    \"ram_slots\": [\n");
    for (UINT i = 0; i < memInfo->slotList.count; i++)
    {
        RAMSlotInfo *slot = &memInfo->slotList.slots[i];
        _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                    "      {\n"
                    "        \"location\": \"%s\",\n"
                    "        \"capacity\": %.2f,\n"
                    "        \"speed\": %u,\n"
                    "        \"configured_speed\": %u,\n"
                    "        \"manufacturer\": \"%s\"\n"
                    "      }%s",
                    getRAMSlotLocation(slot),
                    bytesToGB(getRAMCapacity(slot)),
                    getRAMSpeed(slot),
                    getRAMConfiguredSpeed(slot),
                    getRAMManufacturer(slot),
                    i < memInfo->slotList.count - 1 ? ",\n" : "\n");
        appendString(buffer, bufferSize, position, temp);
    }
    appendString(buffer, bufferSize, position, "    ]\n");
    appendString(buffer, bufferSize, position, "  },\n");
}

// Function for appending Storage information
static void appendStorageInfo(char **buffer, size_t *bufferSize, size_t *position, StorageList *storageList)
{
    appendString(buffer, bufferSize, position, "  \"storage\": [\n");
    for (UINT i = 0; i < storageList->count; i++)
    {
        char temp[1024];
        LogicalDiskInfo *disk = &storageList->disks[i];
        _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                    "    {\n"
                    "      \"drive\": \"%s\",\n"
                    "      \"type\": \"%s\",\n"
                    "      \"model\": \"%s\",\n"
                    "      \"interface\": \"%s\",\n"
                    "      \"total_size\": %.2f,\n"
                    "      \"free_space\": %.2f,\n"
                    "      \"used_space\": %.2f\n"
                    "    }%s",
                    getDiskDrive(disk),
                    getDiskType(disk),
                    getDiskModel(disk),
                    getDiskInterface(disk),
                    getDiskTotalSize(disk),
                    getDiskFreeSpace(disk),
                    getDiskTotalSize(disk) - getDiskFreeSpace(disk),
                    i < storageList->count - 1 ? ",\n" : "\n");
        appendString(buffer, bufferSize, position, temp);
    }
    appendString(buffer, bufferSize, position, "  ],\n");
}

// Function for appending Network information
static void appendNetworkInfo(char **buffer, size_t *bufferSize, size_t *position, NetworkList *networkList)
{
    appendString(buffer, bufferSize, position, "  \"network\": {\n");

    // Ethernet adapters
    appendString(buffer, bufferSize, position, "    \"ethernet\": [\n");
    BOOL firstEthernet = TRUE;
    for (UINT i = 0; i < networkList->count; i++)
    {
        NetworkAdapterInfo *adapter = &networkList->adapters[i];
        if (isEthernetAdapter(adapter))
        {
            if (!firstEthernet)
                appendString(buffer, bufferSize, position, ",\n");
            char temp[1024];
            _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                        "      {\n"
                        "        \"name\": \"%s\",\n"
                        "        \"mac_address\": \"%s\",\n"
                        "        \"ip_address\": \"%s\",\n"
                        "        \"status\": \"%s\"\n"
                        "      }",
                        getAdapterName(adapter),
                        getAdapterMacAddress(adapter),
                        getAdapterIPAddress(adapter),
                        getAdapterStatus(adapter));
            appendString(buffer, bufferSize, position, temp);
            firstEthernet = FALSE;
        }
    }
    appendString(buffer, bufferSize, position, "\n    ],\n");

    // WiFi adapters
    appendString(buffer, bufferSize, position, "    \"wifi\": [\n");
    BOOL firstWiFi = TRUE;
    for (UINT i = 0; i < networkList->count; i++)
    {
        NetworkAdapterInfo *adapter = &networkList->adapters[i];
        if (isWiFiAdapter(adapter))
        {
            if (!firstWiFi)
                appendString(buffer, bufferSize, position, ",\n");
            char temp[1024];
            _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                        "      {\n"
                        "        \"name\": \"%s\",\n"
                        "        \"mac_address\": \"%s\",\n"
                        "        \"ip_address\": \"%s\",\n"
                        "        \"status\": \"%s\"\n"
                        "      }",
                        getAdapterName(adapter),
                        getAdapterMacAddress(adapter),
                        getAdapterIPAddress(adapter),
                        getAdapterStatus(adapter));
            appendString(buffer, bufferSize, position, temp);
            firstWiFi = FALSE;
        }
    }
    appendString(buffer, bufferSize, position, "\n    ]\n");
    appendString(buffer, bufferSize, position, "  },\n");
}

// Function for appending Audio information
static void appendAudioInfo(char **buffer, size_t *bufferSize, size_t *position, AudioList *audioList)
{
    appendString(buffer, bufferSize, position, "  \"audio\": [\n");
    for (UINT i = 0; i < audioList->count; i++)
    {
        char temp[1024];
        AudioDeviceInfo *device = &audioList->devices[i];
        _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                    "    {\n"
                    "      \"name\": \"%s\",\n"
                    "      \"manufacturer\": \"%s\"\n"
                    "    }%s",
                    getAudioDeviceName(device),
                    getAudioDeviceManufacturer(device),
                    i < audioList->count - 1 ? ",\n" : "\n");
        appendString(buffer, bufferSize, position, temp);
    }
    appendString(buffer, bufferSize, position, "  ],\n");
}

// Function for appending Battery information
static void appendBatteryInfo(char **buffer, size_t *bufferSize, size_t *position, BatteryInfo *batteryInfo)
{
    char temp[1024];
    _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                "  \"battery\": {\n"
                "    \"is_desktop\": %s,\n"
                "    \"percent\": %d,\n"
                "    \"power_plugged\": %s\n"
                "  },\n",
                isDesktopSystem(batteryInfo) ? "true" : "false",
                getBatteryPercent(batteryInfo),
                isPowerPlugged(batteryInfo) ? "true" : "false");
    appendString(buffer, bufferSize, position, temp);
}

// Function for appending Monitor information
static void appendMonitorInfo(char **buffer, size_t *bufferSize, size_t *position, MonitorList *monitorList)
{
    appendString(buffer, bufferSize, position, "  \"monitors\": [\n");
    for (UINT i = 0; i < monitorList->count; i++)
    {
        char temp[1024];
        MonitorInfo *monitor = &monitorList->monitors[i];
        _snprintf_s(temp, sizeof(temp), _TRUNCATE,
                    "    {\n"
                    "      \"is_primary\": %s,\n"
                    "      \"current_resolution\": \"%s\",\n"
                    "      \"aspect_ratio\": \"%s\",\n"
                    "      \"refresh_rate\": %d,\n"
                    "      \"screen_size\": \"%s\",\n"
                    "      \"manufacturer\": \"%s\",\n"
                    "      \"device_id\": \"%s\"\n"
                    "    }%s",
                    isMonitorPrimary(monitor) ? "true" : "false",
                    getMonitorCurrentResolution(monitor),
                    getMonitorAspectRatio(monitor),
                    getMonitorRefreshRate(monitor),
                    getMonitorScreenSize(monitor),
                    getMonitorManufacturer(monitor),
                    getMonitorDeviceId(monitor),
                    i < monitorList->count - 1 ? ",\n" : "\n");
        appendString(buffer, bufferSize, position, temp);
    }
    appendString(buffer, bufferSize, position, "  ]\n");
}

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
    MonitorList *monitorList)
{
    size_t bufferSize = JSON_BUFFER_SIZE;
    size_t position = 0;
    char *jsonBuffer = (char *)malloc(bufferSize);
    if (!jsonBuffer)
        return NULL;

    // Start JSON object
    appendString(&jsonBuffer, &bufferSize, &position, "{\n");

    // Add information for each component
    if (gpuList)
        appendGPUInfo(&jsonBuffer, &bufferSize, &position, gpuList);
    if (mbInfo)
        appendMotherboardInfo(&jsonBuffer, &bufferSize, &position, mbInfo);
    if (cpuList)
        appendCPUInfo(&jsonBuffer, &bufferSize, &position, cpuList);
    if (memInfo)
        appendMemoryInfo(&jsonBuffer, &bufferSize, &position, memInfo);
    if (storageList)
        appendStorageInfo(&jsonBuffer, &bufferSize, &position, storageList);
    if (networkList)
        appendNetworkInfo(&jsonBuffer, &bufferSize, &position, networkList);
    if (audioList)
        appendAudioInfo(&jsonBuffer, &bufferSize, &position, audioList);
    if (batteryInfo)
        appendBatteryInfo(&jsonBuffer, &bufferSize, &position, batteryInfo);
    if (monitorList)
        appendMonitorInfo(&jsonBuffer, &bufferSize, &position, monitorList);

    // Remove trailing comma if exists
    if (position > 2 && jsonBuffer[position - 2] == ',')
    {
        position -= 2;
        jsonBuffer[position] = '\n';
        jsonBuffer[position + 1] = '\0';
    }

    // Close JSON object
    appendString(&jsonBuffer, &bufferSize, &position, "}\n");

    return jsonBuffer;
}

// Function for freeing JSON string
void freeJSONString(char *jsonStr)
{
    if (jsonStr)
        free(jsonStr);
}
