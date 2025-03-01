#include "battery_info.h"
#include <stdio.h>

BatteryInfo *getBatteryInfo(void)
{
    BatteryInfo *info = (BatteryInfo *)malloc(sizeof(BatteryInfo));
    if (!info)
        return NULL;

    // Initialize default values for desktop system
    info->percent = 100;
    info->powerPlugged = TRUE;
    info->isDesktop = TRUE;

    // Get system power status
    SYSTEM_POWER_STATUS powerStatus;
    if (GetSystemPowerStatus(&powerStatus))
    {
        // Get battery percentage if available
        if (powerStatus.BatteryLifePercent != 255)
        {
            info->percent = (int)powerStatus.BatteryLifePercent;
            info->isDesktop = FALSE; // Has valid battery, not desktop
        }

        // Check if power is plugged in
        info->powerPlugged = (powerStatus.ACLineStatus == 1);

        // Detect if system is desktop based on battery flag
        if (powerStatus.BatteryFlag == 128 ||
            powerStatus.BatteryFlag == 255 ||
            (powerStatus.BatteryLifePercent == 255 && powerStatus.BatteryFlag == 1))
        {
            info->isDesktop = TRUE;
            info->percent = 100;
            info->powerPlugged = TRUE;
        }
    }

    return info;
}

void freeBatteryInfo(BatteryInfo *info)
{
    if (info)
    {
        free(info);
    }
}

int getBatteryPercent(const BatteryInfo *info)
{
    return info ? info->percent : 100;
}

BOOL isPowerPlugged(const BatteryInfo *info)
{
    return info ? info->powerPlugged : TRUE;
}

BOOL isDesktopSystem(const BatteryInfo *info)
{
    return info ? info->isDesktop : TRUE;
}
