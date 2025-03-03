#ifndef SYSTEM_INFO_DLL_H
#define SYSTEM_INFO_DLL_H

#include <windows.h>

#ifdef SYSTEM_INFO_EXPORTS
#define SYSTEM_INFO_API __declspec(dllexport)
#else
#define SYSTEM_INFO_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Starts the system monitoring service
     *
     * This function:
     * 1. Initializes monitoring thread
     * 2. Sets up data collection
     * 3. Begins periodic updates
     *
     * The monitoring service will collect system information
     * at the specified interval and deliver it via callback
     *
     * @param updateIntervalMs Time between updates in milliseconds
     * @return BOOL TRUE if started successfully, FALSE if already running or failed
     */
    SYSTEM_INFO_API BOOL startSystemMonitoring(int updateIntervalMs);

    /**
     * @brief Stops the system monitoring service
     *
     * This function:
     * 1. Signals monitoring thread to stop
     * 2. Waits for thread completion
     * 3. Cleans up allocated resources
     *
     * Safe to call even if monitoring is not active
     */
    SYSTEM_INFO_API void stopSystemMonitoring(void);

    /**
     * @brief Updates the monitoring interval
     *
     * Changes how frequently the system information
     * is collected and delivered via callback
     *
     * @param updateIntervalMs New interval between updates in milliseconds
     */
    SYSTEM_INFO_API void setUpdateInterval(int updateIntervalMs);

    /**
     * @brief Callback function type for receiving system information
     *
     * This function will be called periodically with updated
     * system information in JSON format
     *
     * @param jsonData JSON-formatted system information string
     */
    typedef void (*SystemInfoCallback)(const char *jsonData);

    /**
     * @brief Sets the callback for receiving system information
     *
     * The callback will be invoked at the specified interval
     * with current system information in JSON format
     *
     * @param callback Function pointer to receive updates
     */
    SYSTEM_INFO_API void setSystemInfoCallback(SystemInfoCallback callback);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_INFO_DLL_H
