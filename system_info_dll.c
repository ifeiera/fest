#include "system_info_dll.h"
#include "system_info_internal.h"
#include "json_structure.h"
#include <process.h>

// Variabel global untuk kontrol monitoring
static struct
{
    BOOL isRunning;
    int updateInterval;
    HANDLE monitorThread;
    HANDLE stopEvent;
    SystemInfoCallback callback;

    // Data structures
    StaticInfo staticInfo;
    DynamicInfo dynamicInfo;

    // Thread synchronization
    HANDLE mutex;
    BOOL isFirstRun;
} g_MonitorContext = {0};

// Thread function untuk monitoring
static unsigned __stdcall monitoringThread(void *arg)
{
    while (g_MonitorContext.isRunning)
    {
        // Check if stop is requested
        if (WaitForSingleObject(g_MonitorContext.stopEvent, 0) == WAIT_OBJECT_0)
            break;

        // Get static info on first run
        if (g_MonitorContext.isFirstRun)
        {
            WaitForSingleObject(g_MonitorContext.mutex, INFINITE);
            g_MonitorContext.staticInfo.cpuList = getCPUList();
            g_MonitorContext.staticInfo.gpuList = getGPUList();
            g_MonitorContext.staticInfo.mbInfo = getMotherboardInfo();
            g_MonitorContext.staticInfo.audioList = getAudioList();
            g_MonitorContext.staticInfo.monitorList = getMonitorList();
            g_MonitorContext.isFirstRun = FALSE;
            ReleaseMutex(g_MonitorContext.mutex);
        }

        // Get dynamic info
        WaitForSingleObject(g_MonitorContext.mutex, INFINITE);
        g_MonitorContext.dynamicInfo.memInfo = getMemoryInfo();
        g_MonitorContext.dynamicInfo.storageList = getStorageList();
        g_MonitorContext.dynamicInfo.batteryInfo = getBatteryInfo();
        g_MonitorContext.dynamicInfo.networkList = getNetworkList();
        ReleaseMutex(g_MonitorContext.mutex);

        // Generate JSON
        char *jsonOutput = generateSystemInfoJSON(
            g_MonitorContext.staticInfo.gpuList,
            g_MonitorContext.staticInfo.mbInfo,
            g_MonitorContext.staticInfo.cpuList,
            g_MonitorContext.dynamicInfo.memInfo,
            g_MonitorContext.dynamicInfo.storageList,
            g_MonitorContext.dynamicInfo.networkList,
            g_MonitorContext.staticInfo.audioList,
            g_MonitorContext.dynamicInfo.batteryInfo,
            g_MonitorContext.staticInfo.monitorList);

        // Send data through callback
        if (jsonOutput && g_MonitorContext.callback)
        {
            g_MonitorContext.callback(jsonOutput);
            freeJSONString(jsonOutput);
        }

        // Cleanup dynamic info
        if (g_MonitorContext.dynamicInfo.memInfo)
            freeMemoryInfo(g_MonitorContext.dynamicInfo.memInfo);
        if (g_MonitorContext.dynamicInfo.storageList)
            freeStorageList(g_MonitorContext.dynamicInfo.storageList);
        if (g_MonitorContext.dynamicInfo.batteryInfo)
            freeBatteryInfo(g_MonitorContext.dynamicInfo.batteryInfo);
        if (g_MonitorContext.dynamicInfo.networkList)
            freeNetworkList(g_MonitorContext.dynamicInfo.networkList);

        // Wait for interval
        Sleep(g_MonitorContext.updateInterval);
    }

    return 0;
}

SYSTEM_INFO_API BOOL startSystemMonitoring(int updateIntervalMs)
{
    if (g_MonitorContext.isRunning)
        return FALSE;

    // Initialize context
    g_MonitorContext.isRunning = TRUE;
    g_MonitorContext.updateInterval = updateIntervalMs;
    g_MonitorContext.isFirstRun = TRUE;
    g_MonitorContext.mutex = CreateMutex(NULL, FALSE, NULL);
    g_MonitorContext.stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Start monitoring thread
    g_MonitorContext.monitorThread = (HANDLE)_beginthreadex(NULL, 0, monitoringThread, NULL, 0, NULL);

    return g_MonitorContext.monitorThread != NULL;
}

SYSTEM_INFO_API void stopSystemMonitoring(void)
{
    if (!g_MonitorContext.isRunning)
        return;

    // Signal thread to stop
    SetEvent(g_MonitorContext.stopEvent);
    g_MonitorContext.isRunning = FALSE;

    // Wait for thread to finish
    if (g_MonitorContext.monitorThread)
    {
        WaitForSingleObject(g_MonitorContext.monitorThread, INFINITE);
        CloseHandle(g_MonitorContext.monitorThread);
    }

    // Cleanup static info
    if (g_MonitorContext.staticInfo.gpuList)
        freeGPUList(g_MonitorContext.staticInfo.gpuList);
    if (g_MonitorContext.staticInfo.mbInfo)
        freeMotherboardInfo(g_MonitorContext.staticInfo.mbInfo);
    if (g_MonitorContext.staticInfo.cpuList)
        freeCPUList(g_MonitorContext.staticInfo.cpuList);
    if (g_MonitorContext.staticInfo.audioList)
        freeAudioList(g_MonitorContext.staticInfo.audioList);
    if (g_MonitorContext.staticInfo.monitorList)
        freeMonitorList(g_MonitorContext.staticInfo.monitorList);

    // Cleanup handles
    CloseHandle(g_MonitorContext.mutex);
    CloseHandle(g_MonitorContext.stopEvent);

    // Reset context
    memset(&g_MonitorContext, 0, sizeof(g_MonitorContext));
}

SYSTEM_INFO_API void setUpdateInterval(int updateIntervalMs)
{
    g_MonitorContext.updateInterval = updateIntervalMs;
}

SYSTEM_INFO_API void setSystemInfoCallback(SystemInfoCallback callback)
{
    g_MonitorContext.callback = callback;
}
