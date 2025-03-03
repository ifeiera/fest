#include "system_info_dll.h"
#include "system_info_internal.h"
#include "json_structure.h"
#include <process.h>

/**
 * @brief Global context for system monitoring
 *
 * This structure maintains the state and resources for the monitoring system:
 * - Thread control and synchronization
 * - Update interval and callback
 * - Static and dynamic system information
 */
static struct
{
    BOOL isRunning;              // Monitoring state
    int updateInterval;          // Update interval in milliseconds
    HANDLE monitorThread;        // Monitoring thread handle
    HANDLE stopEvent;            // Event for stopping the thread
    SystemInfoCallback callback; // Callback for sending updates

    // System information containers
    StaticInfo staticInfo;   // Static hardware information
    DynamicInfo dynamicInfo; // Dynamic system metrics

    // Synchronization
    HANDLE mutex;    // Mutex for thread safety
    BOOL isFirstRun; // First run flag for static info
} g_MonitorContext = {0};

/**
 * @brief Thread function for system monitoring
 *
 * This function runs in a separate thread and:
 * 1. Collects static system information on first run
 * 2. Periodically collects dynamic system information
 * 3. Generates JSON output and sends through callback
 * 4. Handles cleanup of collected information
 *
 * @param arg Thread argument (unused)
 * @return unsigned Thread exit code
 */
static unsigned __stdcall monitoringThread(void *arg)
{
    while (g_MonitorContext.isRunning)
    {
        // Check for stop request
        if (WaitForSingleObject(g_MonitorContext.stopEvent, 0) == WAIT_OBJECT_0)
            break;

        // Collect static information once
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

        // Collect dynamic information
        WaitForSingleObject(g_MonitorContext.mutex, INFINITE);
        g_MonitorContext.dynamicInfo.memInfo = getMemoryInfo();
        g_MonitorContext.dynamicInfo.storageList = getStorageList();
        g_MonitorContext.dynamicInfo.batteryInfo = getBatteryInfo();
        g_MonitorContext.dynamicInfo.networkList = getNetworkList();
        ReleaseMutex(g_MonitorContext.mutex);

        // Generate and send JSON data
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

        if (jsonOutput && g_MonitorContext.callback)
        {
            g_MonitorContext.callback(jsonOutput);
            freeJSONString(jsonOutput);
        }

        // Cleanup dynamic information
        if (g_MonitorContext.dynamicInfo.memInfo)
            freeMemoryInfo(g_MonitorContext.dynamicInfo.memInfo);
        if (g_MonitorContext.dynamicInfo.storageList)
            freeStorageList(g_MonitorContext.dynamicInfo.storageList);
        if (g_MonitorContext.dynamicInfo.batteryInfo)
            freeBatteryInfo(g_MonitorContext.dynamicInfo.batteryInfo);
        if (g_MonitorContext.dynamicInfo.networkList)
            freeNetworkList(g_MonitorContext.dynamicInfo.networkList);

        Sleep(g_MonitorContext.updateInterval);
    }

    return 0;
}

/**
 * @brief Starts the system monitoring process
 *
 * Initializes monitoring context and starts a background thread
 * that periodically collects system information.
 *
 * @param updateIntervalMs Interval between updates in milliseconds
 * @return BOOL TRUE if monitoring started successfully, FALSE if already running or failed
 */
SYSTEM_INFO_API BOOL startSystemMonitoring(int updateIntervalMs)
{
    if (g_MonitorContext.isRunning)
        return FALSE;

    // Initialize monitoring context
    g_MonitorContext.isRunning = TRUE;
    g_MonitorContext.updateInterval = updateIntervalMs;
    g_MonitorContext.isFirstRun = TRUE;
    g_MonitorContext.mutex = CreateMutex(NULL, FALSE, NULL);
    g_MonitorContext.stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Start monitoring thread
    g_MonitorContext.monitorThread = (HANDLE)_beginthreadex(NULL, 0, monitoringThread, NULL, 0, NULL);

    return g_MonitorContext.monitorThread != NULL;
}

/**
 * @brief Stops the system monitoring process
 *
 * Signals the monitoring thread to stop, waits for completion,
 * and cleans up all allocated resources.
 */
SYSTEM_INFO_API void stopSystemMonitoring(void)
{
    if (!g_MonitorContext.isRunning)
        return;

    // Signal and wait for thread completion
    SetEvent(g_MonitorContext.stopEvent);
    g_MonitorContext.isRunning = FALSE;

    if (g_MonitorContext.monitorThread)
    {
        WaitForSingleObject(g_MonitorContext.monitorThread, INFINITE);
        CloseHandle(g_MonitorContext.monitorThread);
    }

    // Cleanup static information
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

    // Cleanup synchronization objects
    CloseHandle(g_MonitorContext.mutex);
    CloseHandle(g_MonitorContext.stopEvent);

    memset(&g_MonitorContext, 0, sizeof(g_MonitorContext));
}

/**
 * @brief Sets the update interval for system monitoring
 *
 * @param updateIntervalMs New interval between updates in milliseconds
 */
SYSTEM_INFO_API void setUpdateInterval(int updateIntervalMs)
{
    g_MonitorContext.updateInterval = updateIntervalMs;
}

/**
 * @brief Sets the callback function for receiving system information updates
 *
 * @param callback Function pointer to receive JSON-formatted system information
 */
SYSTEM_INFO_API void setSystemInfoCallback(SystemInfoCallback callback)
{
    g_MonitorContext.callback = callback;
}
