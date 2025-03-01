#ifndef BATTERY_INFO_H
#define BATTERY_INFO_H

#include <windows.h>

// Structure for storing battery information
typedef struct
{
    int percent;       // Battery percentage (0-100)
    BOOL powerPlugged; // TRUE if power is plugged in
    BOOL isDesktop;    // TRUE if system is a desktop
} BatteryInfo;

// Function for getting battery information
BatteryInfo *getBatteryInfo(void);
void freeBatteryInfo(BatteryInfo *info);

// Getter functions
int getBatteryPercent(const BatteryInfo *info);
BOOL isPowerPlugged(const BatteryInfo *info);
BOOL isDesktopSystem(const BatteryInfo *info);

#endif // BATTERY_INFO_H
