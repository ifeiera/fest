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

// Struktur untuk menyimpan informasi statis
typedef struct
{
    GPUList *gpuList;
    MotherboardInfo *mbInfo;
    CPUList *cpuList;
    NetworkList *networkList;
    AudioList *audioList;
    MonitorList *monitorList;
} StaticInfo;

// Struktur untuk menyimpan informasi dinamis
typedef struct
{
    MemoryInfo *memInfo;
    StorageList *storageList;
    BatteryInfo *batteryInfo;
} DynamicInfo;

// Struktur untuk thread synchronization
typedef struct
{
    HANDLE mutex;
    StaticInfo *staticInfo;
    DynamicInfo *dynamicInfo;
    HANDLE threadsComplete;
    int threadsRunning;
    BOOL isFirstRun;
} ThreadContext;

// Thread function untuk hardware info statis
static unsigned __stdcall getStaticHardwareInfoThread(void *arg)
{
    ThreadContext *ctx = (ThreadContext *)arg;

    // Hanya jalankan pada first run
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

// Thread function untuk info yang sering berubah
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

// Fungsi untuk membersihkan static info
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

// Fungsi untuk membersihkan dynamic info
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

        // Cleanup dynamic info setiap iterasi
        cleanupDynamicInfo(&dynamicInfo);

        // Cleanup thread handles
        CloseHandle(threads[0]);
        CloseHandle(threads[1]);

        ctx.isFirstRun = FALSE; // Set false setelah first run

        Sleep(1000); // Delay 1 detik antara updates

        // Bisa tambahkan kondisi untuk keluar dari loop
        // misalnya dengan check keyboard input atau signal
    }

    // Final cleanup
    cleanupStaticInfo(&staticInfo);
    CloseHandle(ctx.mutex);
    CloseHandle(ctx.threadsComplete);

    return 0;
}
