#ifndef BATTERY_INFO_H
#define BATTERY_INFO_H

#include <windows.h>

// Struktur untuk menyimpan informasi baterai
typedef struct
{
    int percent;       // Persentase baterai (0-100)
    BOOL powerPlugged; // TRUE jika tersambung listrik
    BOOL isDesktop;    // TRUE jika sistem adalah desktop
} BatteryInfo;

// Fungsi untuk mendapatkan informasi baterai
BatteryInfo *getBatteryInfo(void);
void freeBatteryInfo(BatteryInfo *info);

// Fungsi getter
int getBatteryPercent(const BatteryInfo *info);
BOOL isPowerPlugged(const BatteryInfo *info);
BOOL isDesktopSystem(const BatteryInfo *info);

#endif // BATTERY_INFO_H
