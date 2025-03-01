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

    // Fungsi untuk memulai monitoring sistem
    SYSTEM_INFO_API BOOL startSystemMonitoring(int updateIntervalMs);

    // Fungsi untuk menghentikan monitoring
    SYSTEM_INFO_API void stopSystemMonitoring(void);

    // Fungsi untuk mengubah interval update
    SYSTEM_INFO_API void setUpdateInterval(int updateIntervalMs);

    // Fungsi callback untuk menerima data JSON
    typedef void (*SystemInfoCallback)(const char *jsonData);

    // Fungsi untuk set callback
    SYSTEM_INFO_API void setSystemInfoCallback(SystemInfoCallback callback);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_INFO_DLL_H
