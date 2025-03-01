// Not for dll build
// NEED TO UPDATE!!!
// Newer version in system_info_dll.h and system_info_dll.c

#include "gpu_info.h"
#include "motherboard_info.h"
#include "cpu_info.h"
#include "memory_info.h"
#include "storage_info.h"
#include "network_info.h"
#include "audio_info.h"
#include "battery_info.h"
#include "monitor_info.h"
#include "json_structure.h"
#include <windows.h>
#include <process.h>

// Structure for storing static information
typedef struct
{
    GPUList *gpuList;
    MotherboardInfo *mbInfo;
    CPUList *cpuList;
    NetworkList *networkList;
    AudioList *audioList;
    MonitorList *monitorList;
} StaticInfo;

// Structure for storing dynamic information
typedef struct
{
    MemoryInfo *memInfo;
    StorageList *storageList;
    BatteryInfo *batteryInfo;
} DynamicInfo;

// Structure for thread synchronization
typedef struct
{
    HANDLE mutex;
    StaticInfo *staticInfo;
    DynamicInfo *dynamicInfo;
    HANDLE threadsComplete;
    int threadsRunning;
    BOOL isFirstRun;
} ThreadContext;

// Thread function for static hardware info
static unsigned __stdcall getStaticHardwareInfoThread(void *arg)
{
    ThreadContext *ctx = (ThreadContext *)arg;

    // Only run on first run
    if (ctx->isFirstRun)
    {
        WaitForSingleObject(ctx->mutex, INFINITE);
        ctx->staticInfo->cpuList = getCPUList();
        ctx->staticInfo->gpuList = getGPUList();
        ctx->staticInfo->mbInfo = getMotherboardInfo();
        ctx->staticInfo->networkList = getNetworkList();
        ctx->staticInfo->audioList = getAudioList();
        ctx->staticInfo->monitorList = getMonitorList();
        ReleaseMutex(ctx->mutex);
    }

    if (InterlockedDecrement(&ctx->threadsRunning) == 0)
        SetEvent(ctx->threadsComplete);
    return 0;
}

// Thread function for dynamic info that often changes
static unsigned __stdcall getDynamicInfoThread(void *arg)
{
    ThreadContext *ctx = (ThreadContext *)arg;
    WaitForSingleObject(ctx->mutex, INFINITE);

    ctx->dynamicInfo->memInfo = getMemoryInfo();
    ctx->dynamicInfo->storageList = getStorageList();
    ctx->dynamicInfo->batteryInfo = getBatteryInfo();

    ReleaseMutex(ctx->mutex);

    if (InterlockedDecrement(&ctx->threadsRunning) == 0)
        SetEvent(ctx->threadsComplete);
    return 0;
}

// Function for cleaning static info
static void cleanupStaticInfo(StaticInfo *info)
{
    if (!info)
        return;
    if (info->gpuList)
        freeGPUList(info->gpuList);
    if (info->mbInfo)
        freeMotherboardInfo(info->mbInfo);
    if (info->cpuList)
        freeCPUList(info->cpuList);
    if (info->networkList)
        freeNetworkList(info->networkList);
    if (info->audioList)
        freeAudioList(info->audioList);
    if (info->monitorList)
        freeMonitorList(info->monitorList);
}

// Function for cleaning dynamic info
static void cleanupDynamicInfo(DynamicInfo *info)
{
    if (!info)
        return;
    if (info->memInfo)
        freeMemoryInfo(info->memInfo);
    if (info->storageList)
        freeStorageList(info->storageList);
    if (info->batteryInfo)
        freeBatteryInfo(info->batteryInfo);
}

int main()
{
    // Initialize info structures
    StaticInfo staticInfo = {0};
    DynamicInfo dynamicInfo = {0};

    // Initialize thread context
    ThreadContext ctx = {0};
    ctx.mutex = CreateMutex(NULL, FALSE, NULL);
    ctx.threadsComplete = CreateEvent(NULL, TRUE, FALSE, NULL);
    ctx.staticInfo = &staticInfo;
    ctx.dynamicInfo = &dynamicInfo;
    ctx.isFirstRun = TRUE;

    HANDLE threads[2];

    while (1) // Loop utama
    {
        // Reset event dan thread counter
        ResetEvent(ctx.threadsComplete);
        ctx.threadsRunning = 2;

        // Create threads
        threads[0] = (HANDLE)_beginthreadex(NULL, 0, getStaticHardwareInfoThread, &ctx, 0, NULL);
        threads[1] = (HANDLE)_beginthreadex(NULL, 0, getDynamicInfoThread, &ctx, 0, NULL);

        // Wait for all threads to complete
        WaitForSingleObject(ctx.threadsComplete, INFINITE);

        // Generate and print JSON
        char *jsonOutput = generateSystemInfoJSON(
            staticInfo.gpuList, staticInfo.mbInfo, staticInfo.cpuList,
            dynamicInfo.memInfo, dynamicInfo.storageList, staticInfo.networkList,
            staticInfo.audioList, dynamicInfo.batteryInfo, staticInfo.monitorList);

        if (jsonOutput)
        {
            printf("%s", jsonOutput);
            freeJSONString(jsonOutput);
        }

        // Cleanup dynamic info every iteration
        cleanupDynamicInfo(&dynamicInfo);

        // Cleanup thread handles
        CloseHandle(threads[0]);
        CloseHandle(threads[1]);

        ctx.isFirstRun = FALSE; // Set false after first run

        Sleep(1000); // Delay 1 second between updates

        // Can add condition to exit loop
        // for example with keyboard input or signal
    }

    // Final cleanup
    cleanupStaticInfo(&staticInfo);
    CloseHandle(ctx.mutex);
    CloseHandle(ctx.threadsComplete);

    return 0;
}
