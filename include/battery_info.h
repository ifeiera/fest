#ifndef BATTERY_INFO_H
#define BATTERY_INFO_H

#include <windows.h>

/**
 * @brief System power and battery status information
 *
 * Contains information about:
 * - Battery charge level
 * - Power source status
 * - System type detection
 *
 * For desktop systems:
 * - percent will be 100
 * - powerPlugged will be TRUE
 * - isDesktop will be TRUE
 */
typedef struct
{
    int percent;       // Battery charge percentage (0-100)
    BOOL powerPlugged; // Power connection status
    BOOL isDesktop;    // System type indicator
} BatteryInfo;

/**
 * @brief Retrieves current battery and power information
 *
 * This function:
 * 1. Detects if system is desktop or laptop
 * 2. Gets battery charge level if applicable
 * 3. Checks power source connection
 *
 * @return BatteryInfo* Pointer to allocated battery information, NULL if failed
 * @note Caller is responsible for freeing the returned structure using freeBatteryInfo()
 */
BatteryInfo *getBatteryInfo(void);

/**
 * @brief Frees memory allocated for battery information
 *
 * @param info Pointer to BatteryInfo structure to be freed
 */
void freeBatteryInfo(BatteryInfo *info);

/**
 * @brief Gets the current battery charge percentage
 *
 * @param info Pointer to BatteryInfo structure
 * @return int Battery level (0-100) or 0 if info is NULL or system is desktop
 */
int getBatteryPercent(const BatteryInfo *info);

/**
 * @brief Checks if system is running on external power
 *
 * @param info Pointer to BatteryInfo structure
 * @return BOOL TRUE if plugged in or desktop, FALSE if on battery or info is NULL
 */
BOOL isPowerPlugged(const BatteryInfo *info);

/**
 * @brief Determines if the system is a desktop computer
 *
 * @param info Pointer to BatteryInfo structure
 * @return BOOL TRUE if desktop system, FALSE if laptop or info is NULL
 */
BOOL isDesktopSystem(const BatteryInfo *info);

#endif // BATTERY_INFO_H
