#include "monitor_info.h"
#include <math.h>
#include <stdio.h>

/**
 * @brief Calculates pixels per inch (PPI) for display
 *
 * @param width Screen width in pixels
 * @param height Screen height in pixels
 * @param diagonal_inch Screen diagonal size in inches
 * @return double Calculated PPI or 0 if invalid input
 */
static double calculatePPI(int width, int height, double diagonal_inch)
{
    if (diagonal_inch <= 0)
        return 0;
    return sqrt((double)(width * width + height * height)) / diagonal_inch;
}

/**
 * @brief Retrieves physical monitor dimensions from EDID data
 *
 * This function accesses the monitor's EDID data through the Windows registry
 * to get the physical dimensions of the display panel.
 *
 * @param deviceName Monitor device name
 * @param widthMm Pointer to store width in millimeters
 * @param heightMm Pointer to store height in millimeters
 * @return BOOL TRUE if successful, FALSE if failed
 */
static BOOL getMonitorSizeFromEDID(const char *deviceName, int *widthMm, int *heightMm)
{
    *widthMm = 0;
    *heightMm = 0;

    // Setup device info
    const GUID GUID_DEVINTERFACE_MONITOR = {0xe6f07b5f, 0xee97, 0x4a90, {0xb0, 0x76, 0x33, 0xf5, 0x7b, 0xf4, 0xea, 0xa7}};
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_MONITOR, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
        return FALSE;

    SP_DEVICE_INTERFACE_DATA devInfo = {0};
    devInfo.cbSize = sizeof(devInfo);

    // Enumerate devices
    DWORD monitorIndex = 0;
    while (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_MONITOR, monitorIndex, &devInfo))
    {
        monitorIndex++;

        // Get device path
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfo, NULL, 0, &requiredSize, NULL);

        PSP_DEVICE_INTERFACE_DETAIL_DATA pDevDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        if (!pDevDetail)
            continue;

        pDevDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        SP_DEVINFO_DATA devInfoData = {0};
        devInfoData.cbSize = sizeof(devInfoData);

        if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfo, pDevDetail, requiredSize, NULL, &devInfoData))
        {
            free(pDevDetail);
            continue;
        }

        // Get registry key
        HKEY hEDIDRegKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (hEDIDRegKey && hEDIDRegKey != INVALID_HANDLE_VALUE)
        {
            BYTE edid[1024];
            DWORD edidSize = sizeof(edid);
            if (RegQueryValueEx(hEDIDRegKey, "EDID", NULL, NULL, edid, &edidSize) == ERROR_SUCCESS)
            {
                // Get physical dimensions from EDID bytes 21 and 22
                *widthMm = ((edid[68] & 0xF0) << 4) + edid[66];
                *heightMm = ((edid[68] & 0x0F) << 8) + edid[67];

                RegCloseKey(hEDIDRegKey);
                free(pDevDetail);
                SetupDiDestroyDeviceInfoList(hDevInfo);
                return TRUE;
            }
            RegCloseKey(hEDIDRegKey);
        }
        free(pDevDetail);
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return FALSE;
}

/**
 * @brief Callback function for EnumDisplayMonitors
 *
 * This function is called for each monitor in the system. It collects:
 * - Basic monitor information (resolution, refresh rate)
 * - Physical characteristics (size, dimensions)
 * - Display settings (aspect ratio, color depth)
 * - Device identification
 *
 * @param hMonitor Handle to the monitor
 * @param hdcMonitor Handle to device context
 * @param lprcMonitor Monitor rectangle
 * @param dwData Pointer to MonitorList structure
 * @return BOOL TRUE to continue enumeration, FALSE to stop
 */
static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MonitorList *list = (MonitorList *)dwData;

    // Reallocate array for new monitor
    list->count++;
    list->monitors = (MonitorInfo *)realloc(list->monitors, sizeof(MonitorInfo) * list->count);
    if (!list->monitors)
        return FALSE;

    MonitorInfo *monitor = &list->monitors[list->count - 1];
    memset(monitor, 0, sizeof(MonitorInfo));

    // Get basic monitor information
    MONITORINFOEXA monitorInfo = {0};
    monitorInfo.cbSize = sizeof(MONITORINFOEXA);
    if (GetMonitorInfoA(hMonitor, (LPMONITORINFO)&monitorInfo))
    {
        // Set resolution and primary status
        monitor->width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        monitor->height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
        monitor->isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;

        // Get device information
        DISPLAY_DEVICEA displayDevice = {0};
        displayDevice.cb = sizeof(displayDevice);
        if (EnumDisplayDevicesA(monitorInfo.szDevice, 0, &displayDevice, 0))
        {
            strcpy_s(monitor->deviceId, sizeof(monitor->deviceId), displayDevice.DeviceID);

            // Extract manufacturer
            char *start = strstr(displayDevice.DeviceID, "\\") + 1;
            char *end = strstr(start, "\\");
            if (start && end && (end - start) < sizeof(monitor->manufacturer))
            {
                strncpy_s(monitor->manufacturer, sizeof(monitor->manufacturer), start, end - start);
            }
        }

        // Get current display mode
        DEVMODEA dm = {0};
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsA(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &dm))
        {
            // Set refresh rate
            monitor->refreshRate = dm.dmDisplayFrequency;

            // Calculate aspect ratio
            int gcd = 1;
            DWORD width = dm.dmPelsWidth;
            DWORD height = dm.dmPelsHeight;
            for (DWORD i = 1; i <= width && i <= height; i++)
            {
                if (width % i == 0 && height % i == 0)
                    gcd = i;
            }
            _snprintf_s(monitor->aspectRatio, sizeof(monitor->aspectRatio), _TRUNCATE, "%lu:%lu",
                        width / gcd, height / gcd);

            // Set resolutions
            _snprintf_s(monitor->nativeResolution, sizeof(monitor->nativeResolution), _TRUNCATE,
                        "%lu x %lu", width, height);

            _snprintf_s(monitor->currentResolution, sizeof(monitor->currentResolution), _TRUNCATE,
                        "%s @ %d Hz", monitor->nativeResolution, dm.dmDisplayFrequency);
        }

        // Get physical dimensions and EDID data
        int widthMm = 0, heightMm = 0;
        BYTE edid[1024] = {0};
        DWORD edidSize = sizeof(edid);
        BOOL gotEDID = FALSE;

        // Setup device info for EDID access
        const GUID GUID_DEVINTERFACE_MONITOR = {0xe6f07b5f, 0xee97, 0x4a90, {0xb0, 0x76, 0x33, 0xf5, 0x7b, 0xf4, 0xea, 0xa7}};
        HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_MONITOR, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

        if (hDevInfo != INVALID_HANDLE_VALUE)
        {
            SP_DEVICE_INTERFACE_DATA devInfo = {0};
            devInfo.cbSize = sizeof(devInfo);

            // Enumerate devices
            DWORD monitorIndex = 0;
            while (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_MONITOR, monitorIndex, &devInfo))
            {
                monitorIndex++;

                // Get device path
                DWORD requiredSize = 0;
                SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfo, NULL, 0, &requiredSize, NULL);

                PSP_DEVICE_INTERFACE_DETAIL_DATA pDevDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
                if (!pDevDetail)
                    continue;

                pDevDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                SP_DEVINFO_DATA devInfoData = {0};
                devInfoData.cbSize = sizeof(devInfoData);

                if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfo, pDevDetail, requiredSize, NULL, &devInfoData))
                {
                    free(pDevDetail);
                    continue;
                }

                // Get registry key
                HKEY hEDIDRegKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
                if (hEDIDRegKey && hEDIDRegKey != INVALID_HANDLE_VALUE)
                {
                    if (RegQueryValueEx(hEDIDRegKey, "EDID", NULL, NULL, edid, &edidSize) == ERROR_SUCCESS)
                    {
                        gotEDID = TRUE;

                        // Get physical dimensions
                        monitor->physicalWidthMm = ((edid[68] & 0xF0) << 4) + edid[66];
                        monitor->physicalHeightMm = ((edid[68] & 0x0F) << 8) + edid[67];

                        // Calculate screen size
                        double diagonal_mm = sqrt((double)monitor->physicalWidthMm * monitor->physicalWidthMm +
                                                  (double)monitor->physicalHeightMm * monitor->physicalHeightMm);
                        double diagonal_inch = diagonal_mm / 25.4;
                        _snprintf_s(monitor->screenSize, sizeof(monitor->screenSize), _TRUNCATE,
                                    "%.1f inch", diagonal_inch);
                    }
                    RegCloseKey(hEDIDRegKey);
                }
                free(pDevDetail);
            }
            SetupDiDestroyDeviceInfoList(hDevInfo);
        }
    }

    return TRUE;
}

/**
 * @brief Retrieves information about all connected monitors
 *
 * This function enumerates all connected displays and collects comprehensive
 * information about each monitor including:
 * - Resolution and refresh rate
 * - Physical dimensions and DPI
 * - Display identification and manufacturer
 * - Connection status and capabilities
 *
 * @return MonitorList* Pointer to allocated list of monitor information, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeMonitorList()
 */
MonitorList *getMonitorList(void)
{
    MonitorList *list = (MonitorList *)malloc(sizeof(MonitorList));
    if (!list)
        return NULL;

    list->monitors = NULL;
    list->count = 0;

    // Enumerate all monitors
    if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)list))
    {
        free(list);
        return NULL;
    }

    return list;
}

/**
 * @brief Frees memory allocated for MonitorList structure
 *
 * @param list Pointer to MonitorList structure to be freed
 */
void freeMonitorList(MonitorList *list)
{
    if (list)
    {
        if (list->monitors)
            free(list->monitors);
        free(list);
    }
}

// Getter function implementations with documentation

/**
 * @brief Gets the device ID with escaped backslashes
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Escaped device ID string or empty string if monitor is NULL
 */
const char *getMonitorDeviceId(const MonitorInfo *monitor)
{
    static char escapedId[256];
    const char *originalId = monitor ? monitor->deviceId : "";
    size_t i = 0, j = 0;

    // Escape backslashes
    while (originalId[i] != '\0' && j < sizeof(escapedId) - 2) // -2 for null terminator and extra backslash
    {
        if (originalId[i] == '\\')
        {
            escapedId[j++] = '\\';
            escapedId[j++] = '\\';
        }
        else
        {
            escapedId[j++] = originalId[i];
        }
        i++;
    }
    escapedId[j] = '\0';

    return escapedId;
}

/**
 * @brief Gets the monitor manufacturer name
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Manufacturer name or empty string if monitor is NULL
 */
const char *getMonitorManufacturer(const MonitorInfo *monitor)
{
    return monitor ? monitor->manufacturer : "";
}

/**
 * @brief Gets the monitor's aspect ratio
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Aspect ratio string (e.g. "16:9") or empty string if monitor is NULL
 */
const char *getMonitorAspectRatio(const MonitorInfo *monitor)
{
    return monitor ? monitor->aspectRatio : "";
}

/**
 * @brief Gets the monitor's native resolution
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Native resolution string (e.g. "1920 x 1080") or empty string if monitor is NULL
 */
const char *getMonitorNativeResolution(const MonitorInfo *monitor)
{
    return monitor ? monitor->nativeResolution : "";
}

/**
 * @brief Gets the monitor's current resolution with refresh rate
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Current resolution string (e.g. "1920 x 1080 @ 60 Hz") or empty string if monitor is NULL
 */
const char *getMonitorCurrentResolution(const MonitorInfo *monitor)
{
    return monitor ? monitor->currentResolution : "";
}

/**
 * @brief Gets the monitor's physical screen size
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Screen size string (e.g. "27.0 inch") or empty string if monitor is NULL
 */
const char *getMonitorScreenSize(const MonitorInfo *monitor)
{
    return monitor ? monitor->screenSize : "";
}

/**
 * @brief Gets the monitor's width in pixels
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return int Width in pixels or 0 if monitor is NULL
 */
int getMonitorWidth(const MonitorInfo *monitor)
{
    return monitor ? monitor->width : 0;
}

/**
 * @brief Gets the monitor's height in pixels
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return int Height in pixels or 0 if monitor is NULL
 */
int getMonitorHeight(const MonitorInfo *monitor)
{
    return monitor ? monitor->height : 0;
}

/**
 * @brief Gets the monitor's current refresh rate
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return int Refresh rate in Hz or 0 if monitor is NULL
 */
int getMonitorRefreshRate(const MonitorInfo *monitor)
{
    return monitor ? monitor->refreshRate : 0;
}

/**
 * @brief Checks if this is the primary display
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return BOOL TRUE if primary display, FALSE if secondary or monitor is NULL
 */
BOOL isMonitorPrimary(const MonitorInfo *monitor)
{
    return monitor ? monitor->isPrimary : FALSE;
}
