#ifndef MONITOR_INFO_H
#define MONITOR_INFO_H

#include <windows.h>
#include <setupapi.h>

// Struktur untuk menyimpan informasi EDID
typedef struct
{
    BYTE bitDepth;
    char colorSpace[64];
    char manufactureDate[32];
} EDIDInfo;

// Struktur untuk menyimpan informasi monitor
typedef struct
{
    int width;                  // Lebar resolusi
    int height;                 // Tinggi resolusi
    BOOL isPrimary;             // TRUE jika monitor utama
    char deviceId[256];         // ID perangkat
    char manufacturer[64];      // Manufacturer
    char aspectRatio[16];       // Rasio aspek (mis: "16:9")
    char nativeResolution[32];  // Resolusi native
    int refreshRate;            // Refresh rate dalam Hz
    char currentResolution[64]; // Resolusi saat ini dengan refresh rate
    int physicalWidthMm;        // Lebar fisik dalam mm
    int physicalHeightMm;       // Tinggi fisik dalam mm
    char screenSize[32];        // Ukuran diagonal dalam inci
    EDIDInfo edidInfo;          // Informasi dari EDID
} MonitorInfo;

// Struktur untuk menyimpan array dari monitor
typedef struct
{
    MonitorInfo *monitors;
    UINT count;
} MonitorList;

// Fungsi-fungsi untuk mendapatkan informasi monitor
MonitorList *getMonitorList(void);
void freeMonitorList(MonitorList *list);

// Fungsi getter
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
