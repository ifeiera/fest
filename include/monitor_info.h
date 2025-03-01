#ifndef MONITOR_INFO_H
#define MONITOR_INFO_H

#include <windows.h>
#include <setupapi.h>

// Structure for storing EDID information
typedef struct
{
    BYTE bitDepth;
    char colorSpace[64];
    char manufactureDate[32];
} EDIDInfo;

// Structure for storing monitor information
typedef struct
{
    int width;                  // Resolusi width
    int height;                 // Resolusi height
    BOOL isPrimary;             // TRUE if primary monitor
    char deviceId[256];         // Device ID
    char manufacturer[64];      // Manufacturer
    char aspectRatio[16];       // Aspect ratio (e.g., "16:9")
    char nativeResolution[32];  // Native resolution
    int refreshRate;            // Refresh rate in Hz
    char currentResolution[64]; // Current resolution with refresh rate
    int physicalWidthMm;        // Physical width in mm
    int physicalHeightMm;       // Physical height in mm
    char screenSize[32];        // Screen size in inches
    EDIDInfo edidInfo;          // EDID information
} MonitorInfo;

// Structure for storing array of monitors
typedef struct
{
    MonitorInfo *monitors;
    UINT count;
} MonitorList;

// Functions for getting monitor information
MonitorList *getMonitorList(void);
void freeMonitorList(MonitorList *list);

// Getter functions
const char *getMonitorDeviceId(const MonitorInfo *monitor);
const char *getMonitorManufacturer(const MonitorInfo *monitor);
const char *getMonitorAspectRatio(const MonitorInfo *monitor);
const char *getMonitorNativeResolution(const MonitorInfo *monitor);
const char *getMonitorCurrentResolution(const MonitorInfo *monitor);
const char *getMonitorScreenSize(const MonitorInfo *monitor);
int getMonitorWidth(const MonitorInfo *monitor);
int getMonitorHeight(const MonitorInfo *monitor);
int getMonitorRefreshRate(const MonitorInfo *monitor);
BOOL isMonitorPrimary(const MonitorInfo *monitor);

#endif // MONITOR_INFO_H
