#include "battery_info.h"
#include <stdio.h>

/**
 * @brief Retrieves battery information and power status from the system
 *
 * This function queries the Windows power management system to get:
 * - Battery percentage
 * - Power connection status
 * - System type (desktop/laptop) detection
 *
 * The function uses Windows GetSystemPowerStatus API and includes logic to
 * detect desktop systems based on battery flags and status.
 *
 * @return BatteryInfo* Pointer to allocated battery information structure, NULL if failed
 * @note Caller is responsible for freeing the returned structure using freeBatteryInfo()
 */
BatteryInfo *getBatteryInfo(void)
{
    BatteryInfo *info = (BatteryInfo *)malloc(sizeof(BatteryInfo));
    if (!info)
        return NULL;

    // Set default values assuming desktop system
    info->percent = 100;
    info->powerPlugged = TRUE;
    info->isDesktop = TRUE;

    // Query system power status
    SYSTEM_POWER_STATUS powerStatus;
    if (GetSystemPowerStatus(&powerStatus))
    {
        // Check for valid battery percentage (255 indicates unknown/no battery)
        if (powerStatus.BatteryLifePercent != 255)
        {
            info->percent = (int)powerStatus.BatteryLifePercent;
            info->isDesktop = FALSE; // Valid battery percentage indicates laptop
        }

        // Check AC power status (1 = connected, 0 = disconnected)
        info->powerPlugged = (powerStatus.ACLineStatus == 1);

        // Desktop system detection based on battery flags:
        // 128 = No system battery
        // 255 = Unknown status
        // Special case: 255% life with flag 1 also indicates desktop
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

/**
 * @brief Frees memory allocated for BatteryInfo structure
 *
 * @param info Pointer to BatteryInfo structure to be freed
 */
void freeBatteryInfo(BatteryInfo *info)
{
    if (info)
    {
        free(info);
    }
}

/**
 * @brief Gets the current battery percentage
 *
 * @param info Pointer to BatteryInfo structure
 * @return int Battery percentage (0-100) or 100 if info is NULL (assuming desktop)
 */
int getBatteryPercent(const BatteryInfo *info)
{
    return info ? info->percent : 100;
}

/**
 * @brief Checks if the system is connected to AC power
 *
 * @param info Pointer to BatteryInfo structure
 * @return BOOL TRUE if plugged in or info is NULL (assuming desktop), FALSE if on battery
 */
BOOL isPowerPlugged(const BatteryInfo *info)
{
    return info ? info->powerPlugged : TRUE;
}

/**
 * @brief Determines if the system is a desktop computer
 *
 * @param info Pointer to BatteryInfo structure
 * @return BOOL TRUE if desktop or info is NULL, FALSE if laptop
 */
BOOL isDesktopSystem(const BatteryInfo *info)
{
    return info ? info->isDesktop : TRUE;
}
