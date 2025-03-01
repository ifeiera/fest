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

    // Function for starting system monitoring
    SYSTEM_INFO_API BOOL startSystemMonitoring(int updateIntervalMs);

    // Function for stopping monitoring
    SYSTEM_INFO_API void stopSystemMonitoring(void);

    // Function for changing update interval
    SYSTEM_INFO_API void setUpdateInterval(int updateIntervalMs);

    // Function callback for receiving JSON data
    typedef void (*SystemInfoCallback)(const char *jsonData);

    // Function for setting callback
    SYSTEM_INFO_API void setSystemInfoCallback(SystemInfoCallback callback);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_INFO_DLL_H
