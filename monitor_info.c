#include "monitor_info.h"
#include <math.h>
#include <stdio.h>

// Helper function untuk mendapatkan bit depth dari EDID
static BYTE getEDIDBitDepth(const BYTE *edid)
{
    BYTE features = edid[24];
    if (features & 0x80)
        return 10; // Jika bit 7 diset
    if (features & 0x40)
        return 8; // Jika bit 6 diset
    return 6;     // Default
}

// Helper function untuk mendapatkan color space dari EDID
static void getEDIDColorSpace(const BYTE *edid, char *colorSpace, size_t size)
{
    BYTE features = edid[24];
    strcpy_s(colorSpace, size, "Standard Dynamic Range (SDR)");
}

// Helper function untuk mendapatkan tanggal manufaktur dari EDID
static void getEDIDManufactureDate(const BYTE *edid, char *date, size_t size)
{
    int week = edid[16];
    int year = edid[17] + 1990;
    _snprintf_s(date, size, _TRUNCATE, "Week %d, %d", week, year);
}

// Helper function untuk menghitung pixel density
static double calculatePPI(int width, int height, double diagonal_inch)
{
    if (diagonal_inch <= 0)
        return 0;
    return sqrt((double)(width * width + height * height)) / diagonal_inch;
}

// Helper function untuk mendapatkan ukuran monitor dari EDID
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

// Callback function untuk EnumDisplayMonitors
static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MonitorList *list = (MonitorList *)dwData;

    // Realokasi array untuk monitor baru
    list->count++;
    list->monitors = (MonitorInfo *)realloc(list->monitors, sizeof(MonitorInfo) * list->count);
    if (!list->monitors)
        return FALSE;

    MonitorInfo *monitor = &list->monitors[list->count - 1];
    memset(monitor, 0, sizeof(MonitorInfo));

    // Dapatkan informasi monitor dasar
    MONITORINFOEXA monitorInfo = {0};
    monitorInfo.cbSize = sizeof(MONITORINFOEXA);
    if (GetMonitorInfoA(hMonitor, (LPMONITORINFO)&monitorInfo))
    {
        // Set resolusi dan status primary
        monitor->width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        monitor->height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
        monitor->isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;

        // Dapatkan informasi device
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

        // Dapatkan mode display saat ini
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

        // Get physical dimensions
        int widthMm = 0, heightMm = 0;
        if (getMonitorSizeFromEDID(monitorInfo.szDevice, &widthMm, &heightMm))
        {
            monitor->physicalWidthMm = widthMm;
            monitor->physicalHeightMm = heightMm;

            // Calculate diagonal size in inches
            double diagonal_mm = sqrt((double)widthMm * widthMm + (double)heightMm * heightMm);
            double diagonal_inch = diagonal_mm / 25.4;
            _snprintf_s(monitor->screenSize, sizeof(monitor->screenSize), _TRUNCATE,
                        "%.1f inch", diagonal_inch);
        }
    }

    return TRUE;
}

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

void freeMonitorList(MonitorList *list)
{
    if (list)
    {
        if (list->monitors)
            free(list->monitors);
        free(list);
    }
}

// Implementasi fungsi getter
const char *getMonitorDeviceId(const MonitorInfo *monitor)
{
    static char escapedId[256];
    const char *originalId = monitor ? monitor->deviceId : "";
    size_t i = 0, j = 0;

    // Escape backslashes
    while (originalId[i] != '\0' && j < sizeof(escapedId) - 2) // -2 untuk null terminator dan extra backslash
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

const char *getMonitorManufacturer(const MonitorInfo *monitor)
{
    return monitor ? monitor->manufacturer : "";
}

const char *getMonitorAspectRatio(const MonitorInfo *monitor)
{
    return monitor ? monitor->aspectRatio : "";
}

const char *getMonitorNativeResolution(const MonitorInfo *monitor)
{
    return monitor ? monitor->nativeResolution : "";
}

const char *getMonitorCurrentResolution(const MonitorInfo *monitor)
{
    return monitor ? monitor->currentResolution : "";
}

const char *getMonitorScreenSize(const MonitorInfo *monitor)
{
    return monitor ? monitor->screenSize : "";
}

int getMonitorWidth(const MonitorInfo *monitor)
{
    return monitor ? monitor->width : 0;
}

int getMonitorHeight(const MonitorInfo *monitor)
{
    return monitor ? monitor->height : 0;
}

int getMonitorRefreshRate(const MonitorInfo *monitor)
{
    return monitor ? monitor->refreshRate : 0;
}

BOOL isMonitorPrimary(const MonitorInfo *monitor)
{
    return monitor ? monitor->isPrimary : FALSE;
}
