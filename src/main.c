/**
 * NEED TO UPDATE!!!
 * NOT FOR DLL BUILD
 * NEW VERSION IN SYSTEM_INFO_DLL.H AND SYSTEM_INFO_DLL.C
 */
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

/**
 * @brief Container for static hardware information
 *
 * Holds information that rarely changes during system operation:
 * - GPU configuration
 * - Motherboard details
 * - CPU information
 * - Network adapters
 * - Audio devices
 * - Monitor setup
 */
typedef struct
{
    GPUList *gpuList;
    MotherboardInfo *mbInfo;
    CPUList *cpuList;
    NetworkList *networkList;
    AudioList *audioList;
    MonitorList *monitorList;
} StaticInfo;

/**
 * @brief Container for dynamic system information
 *
 * Holds information that frequently changes:
 * - Memory usage
 * - Storage space
 * - Battery status
 */
typedef struct
{
    MemoryInfo *memInfo;
    StorageList *storageList;
    BatteryInfo *batteryInfo;
} DynamicInfo;

/**
 * @brief Thread synchronization and data sharing context
 *
 * Provides thread-safe access to shared resources:
 * - Mutex for synchronization
 * - Shared data structures
 * - Thread completion tracking
 * - First run flag
 */
typedef struct
{
    HANDLE mutex;             // Synchronization mutex
    StaticInfo *staticInfo;   // Static hardware information
    DynamicInfo *dynamicInfo; // Dynamic system metrics
    HANDLE threadsComplete;   // Thread completion event
    int threadsRunning;       // Active thread counter
    BOOL isFirstRun;          // First run indicator
} ThreadContext;

/**
 * @brief Thread function for collecting static hardware information
 *
 * This thread runs once on first execution to collect:
 * - CPU configuration
 * - GPU details
 * - Motherboard information
 * - Network adapters
 * - Audio devices
 * - Monitor configuration
 *
 * @param arg Pointer to ThreadContext structure
 * @return unsigned Thread exit code
 */
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

/**
 * @brief Thread function for collecting dynamic system information
 *
 * This thread runs periodically to collect:
 * - Memory usage statistics
 * - Storage space information
 * - Battery status updates
 *
 * @param arg Pointer to ThreadContext structure
 * @return unsigned Thread exit code
 */
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

/**
 * @brief Frees resources allocated for static information
 *
 * Cleans up:
 * - GPU list
 * - Motherboard info
 * - CPU list
 * - Network adapters
 * - Audio devices
 * - Monitor list
 *
 * @param info Pointer to StaticInfo structure
 */
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

/**
 * @brief Frees resources allocated for dynamic information
 *
 * Cleans up:
 * - Memory info
 * - Storage list
 * - Battery info
 *
 * @param info Pointer to DynamicInfo structure
 */
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

/**
 * @brief Main application entry point
 *
 * This function:
 * 1. Initializes data structures and synchronization objects
 * 2. Creates threads for collecting system information
 * 3. Periodically generates and outputs JSON data
 * 4. Handles cleanup of resources
 *
 * @return int Program exit code
 */
int main()
{
    // Initialize data structures
    StaticInfo staticInfo = {0};
    DynamicInfo dynamicInfo = {0};

    // Setup thread synchronization
    ThreadContext ctx = {0};
    ctx.mutex = CreateMutex(NULL, FALSE, NULL);
    ctx.threadsComplete = CreateEvent(NULL, TRUE, FALSE, NULL);
    ctx.staticInfo = &staticInfo;
    ctx.dynamicInfo = &dynamicInfo;
    ctx.isFirstRun = TRUE;

    HANDLE threads[2];

    while (1) // Main monitoring loop
    {
        // Reset synchronization objects
        ResetEvent(ctx.threadsComplete);
        ctx.threadsRunning = 2;

        // Start collection threads
        threads[0] = (HANDLE)_beginthreadex(NULL, 0, getStaticHardwareInfoThread, &ctx, 0, NULL);
        threads[1] = (HANDLE)_beginthreadex(NULL, 0, getDynamicInfoThread, &ctx, 0, NULL);

        // Wait for data collection
        WaitForSingleObject(ctx.threadsComplete, INFINITE);

        // Generate and output JSON
        char *jsonOutput = generateSystemInfoJSON(
            staticInfo.gpuList, staticInfo.mbInfo, staticInfo.cpuList,
            dynamicInfo.memInfo, dynamicInfo.storageList, staticInfo.networkList,
            staticInfo.audioList, dynamicInfo.batteryInfo, staticInfo.monitorList);

        if (jsonOutput)
        {
            printf("%s", jsonOutput);
            freeJSONString(jsonOutput);
        }

        // Cleanup iteration resources
        cleanupDynamicInfo(&dynamicInfo);
        CloseHandle(threads[0]);
        CloseHandle(threads[1]);

        ctx.isFirstRun = FALSE;
        Sleep(1000); // 1 second update interval
    }

    // Final cleanup
    cleanupStaticInfo(&staticInfo);
    CloseHandle(ctx.mutex);
    CloseHandle(ctx.threadsComplete);

    return 0;
}
