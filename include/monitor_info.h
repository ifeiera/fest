#ifndef MONITOR_INFO_H
#define MONITOR_INFO_H

#include <windows.h>
#include <setupapi.h>

/**
 * @brief Comprehensive monitor/display information
 *
 * Contains detailed display specifications:
 * - Resolution capabilities
 * - Physical characteristics
 * - Identification details
 * - Display settings
 *
 * All physical measurements are in millimeters
 * Resolution values are in pixels
 */
typedef struct
{
    int width;                  // Current horizontal resolution
    int height;                 // Current vertical resolution
    BOOL isPrimary;             // Primary display indicator
    char deviceId[256];         // Unique device identifier
    char manufacturer[64];      // Display manufacturer name
    char aspectRatio[16];       // Display aspect ratio (e.g., "16:9")
    char nativeResolution[32];  // Maximum supported resolution
    int refreshRate;            // Current refresh rate in Hz
    char currentResolution[64]; // Active resolution with refresh rate
    int physicalWidthMm;        // Display width in millimeters
    int physicalHeightMm;       // Display height in millimeters
    char screenSize[32];        // Diagonal size in inches
} MonitorInfo;

/**
 * @brief Container for multiple monitor information
 *
 * Holds information about all connected displays:
 * - Array of MonitorInfo structures
 * - Number of active displays
 *
 * @note Caller must free using freeMonitorList()
 */
typedef struct
{
    MonitorInfo *monitors; // Array of monitor information
    UINT count;            // Number of connected displays
} MonitorList;

/**
 * @brief Retrieves information about connected displays
 *
 * This function:
 * 1. Enumerates active displays
 * 2. Collects display specifications
 * 3. Reads EDID information
 * 4. Determines physical characteristics
 *
 * @return MonitorList* Pointer to allocated monitor list, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeMonitorList()
 */
MonitorList *getMonitorList(void);

/**
 * @brief Frees memory allocated for monitor information list
 *
 * @param list Pointer to MonitorList structure to be freed
 */
void freeMonitorList(MonitorList *list);

/**
 * @brief Gets the unique device identifier of a display
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Device ID or empty string if monitor is NULL
 */
const char *getMonitorDeviceId(const MonitorInfo *monitor);

/**
 * @brief Gets the manufacturer of a display
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Manufacturer name or empty string if monitor is NULL
 */
const char *getMonitorManufacturer(const MonitorInfo *monitor);

/**
 * @brief Gets the aspect ratio of a display
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Aspect ratio string or empty string if monitor is NULL
 */
const char *getMonitorAspectRatio(const MonitorInfo *monitor);

/**
 * @brief Gets the native (maximum) resolution of a display
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Native resolution string or empty string if monitor is NULL
 */
const char *getMonitorNativeResolution(const MonitorInfo *monitor);

/**
 * @brief Gets the current resolution and refresh rate
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Current display mode or empty string if monitor is NULL
 */
const char *getMonitorCurrentResolution(const MonitorInfo *monitor);

/**
 * @brief Gets the physical size of the display
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return const char* Screen size in inches or empty string if monitor is NULL
 */
const char *getMonitorScreenSize(const MonitorInfo *monitor);

/**
 * @brief Gets the current horizontal resolution
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return int Width in pixels or 0 if monitor is NULL
 */
int getMonitorWidth(const MonitorInfo *monitor);

/**
 * @brief Gets the current vertical resolution
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return int Height in pixels or 0 if monitor is NULL
 */
int getMonitorHeight(const MonitorInfo *monitor);

/**
 * @brief Gets the current refresh rate
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return int Refresh rate in Hz or 0 if monitor is NULL
 */
int getMonitorRefreshRate(const MonitorInfo *monitor);

/**
 * @brief Checks if the display is the primary monitor
 *
 * @param monitor Pointer to MonitorInfo structure
 * @return BOOL TRUE if primary display, FALSE if not or monitor is NULL
 */
BOOL isMonitorPrimary(const MonitorInfo *monitor);

#endif // MONITOR_INFO_H
