#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <windows.h>

/**
 * @brief Information about a single RAM module/slot
 *
 * Contains detailed memory module specifications:
 * - Module capacity
 * - Operating speeds
 * - Physical location
 * - Manufacturer details
 *
 * Speeds are reported in MHz, capacity in bytes
 *
 * @note Maximum manufacturer name length is 255 characters
 */
typedef struct
{
    UINT64 capacity;        // Module size in bytes
    UINT speed;             // Maximum supported speed in MHz
    UINT configuredSpeed;   // Current operating speed in MHz
    char slot[64];          // Physical slot identifier
    char manufacturer[256]; // Module manufacturer name
} RAMSlotInfo;

/**
 * @brief Container for RAM slot information
 *
 * Holds information about all memory modules:
 * - Array of RAMSlotInfo structures
 * - Number of populated slots
 *
 * @note Part of MemoryInfo structure
 */
typedef struct
{
    RAMSlotInfo *slots; // Array of RAM slot information
    UINT count;         // Number of populated slots
} RAMSlotList;

/**
 * @brief Comprehensive system memory information
 *
 * Contains both system-wide memory metrics and
 * detailed information about individual RAM modules:
 * - Total/available/used physical memory
 * - Memory usage percentage
 * - RAM slot configuration
 *
 * All sizes are reported in bytes for maximum precision
 */
typedef struct
{
    UINT64 totalPhys;     // Total physical memory
    UINT64 availPhys;     // Available physical memory
    UINT64 usedPhys;      // Used physical memory
    DWORD memoryLoad;     // Memory usage (0-100%)
    RAMSlotList slotList; // Detailed RAM configuration
} MemoryInfo;

/**
 * @brief Retrieves comprehensive memory information
 *
 * This function:
 * 1. Collects system memory metrics
 * 2. Enumerates RAM modules
 * 3. Gathers detailed module specifications
 *
 * @return MemoryInfo* Pointer to allocated memory information, NULL if failed
 * @note Caller is responsible for freeing the returned structure using freeMemoryInfo()
 */
MemoryInfo *getMemoryInfo(void);

/**
 * @brief Frees memory allocated for memory information
 *
 * @param info Pointer to MemoryInfo structure to be freed
 */
void freeMemoryInfo(MemoryInfo *info);

/**
 * @brief Converts bytes to gigabytes
 *
 * @param bytes Value in bytes
 * @return double Value in gigabytes with decimal precision
 */
double bytesToGB(UINT64 bytes);

/**
 * @brief Gets the physical location identifier of a RAM slot
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return const char* Slot location string or empty string if slot is NULL
 */
const char *getRAMSlotLocation(const RAMSlotInfo *slot);

/**
 * @brief Gets the manufacturer of a RAM module
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return const char* Manufacturer name or empty string if slot is NULL
 */
const char *getRAMManufacturer(const RAMSlotInfo *slot);

/**
 * @brief Gets the capacity of a RAM module
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return UINT64 Module capacity in bytes or 0 if slot is NULL
 */
UINT64 getRAMCapacity(const RAMSlotInfo *slot);

/**
 * @brief Gets the maximum supported speed of a RAM module
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return UINT Maximum speed in MHz or 0 if slot is NULL
 */
UINT getRAMSpeed(const RAMSlotInfo *slot);

/**
 * @brief Gets the current operating speed of a RAM module
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return UINT Current speed in MHz or 0 if slot is NULL
 */
UINT getRAMConfiguredSpeed(const RAMSlotInfo *slot);

#endif // MEMORY_INFO_H
