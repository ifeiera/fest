#ifndef STORAGE_INFO_H
#define STORAGE_INFO_H

#include <windows.h>

/**
 * @brief Information about a physical storage device
 *
 * Contains hardware-level disk information:
 * - Device model/manufacturer
 * - Connection type
 * - System identifiers
 *
 * Used for mapping physical disks to logical volumes
 *
 * @note Maximum model name length is 255 characters
 */
typedef struct
{
    char model[256];        // Device model/manufacturer
    char interfaceType[64]; // Connection type (SATA, NVMe, etc.)
    char deviceID[256];     // Unique device identifier
} PhysicalDiskInfo;

/**
 * @brief Information about a logical storage volume
 *
 * Contains volume-level disk information:
 * - Drive letter and type
 * - Volume identification
 * - Storage capacity
 * - Connection details
 *
 * All sizes are reported in gigabytes (GB)
 */
typedef struct
{
    char drive[4];          // Volume letter (e.g., "C:")
    char type[32];          // Volume type (Fixed/Removable)
    char model[256];        // Volume label or disk model
    char interfaceType[64]; // Connection interface type
    double totalSize;       // Total capacity in GB
    double freeSpace;       // Available space in GB
} LogicalDiskInfo;

/**
 * @brief Container for storage volume information
 *
 * Holds information about all storage volumes:
 * - Array of LogicalDiskInfo structures
 * - Number of volumes
 *
 * Includes both fixed and removable drives
 *
 * @note Caller must free using freeStorageList()
 */
typedef struct
{
    LogicalDiskInfo *disks; // Array of volume information
    UINT count;             // Number of volumes
} StorageList;

/**
 * @brief Retrieves information about storage volumes
 *
 * This function:
 * 1. Enumerates storage volumes
 * 2. Collects volume details
 * 3. Calculates space metrics
 * 4. Identifies volume types
 *
 * @return StorageList* Pointer to allocated volume list, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeStorageList()
 */
StorageList *getStorageList(void);

/**
 * @brief Frees memory allocated for storage information list
 *
 * @param list Pointer to StorageList structure to be freed
 */
void freeStorageList(StorageList *list);

/**
 * @brief Gets the drive letter of a volume
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return const char* Drive letter or empty string if disk is NULL
 */
const char *getDiskDrive(const LogicalDiskInfo *disk);

/**
 * @brief Gets the type of a storage volume
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return const char* Volume type or empty string if disk is NULL
 */
const char *getDiskType(const LogicalDiskInfo *disk);

/**
 * @brief Gets the model/label of a storage volume
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return const char* Volume label or empty string if disk is NULL
 */
const char *getDiskModel(const LogicalDiskInfo *disk);

/**
 * @brief Gets the interface type of a storage volume
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return const char* Interface type or empty string if disk is NULL
 */
const char *getDiskInterface(const LogicalDiskInfo *disk);

/**
 * @brief Gets the total capacity of a storage volume
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return double Total size in GB or 0.0 if disk is NULL
 */
double getDiskTotalSize(const LogicalDiskInfo *disk);

/**
 * @brief Gets the available free space on a volume
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return double Free space in GB or 0.0 if disk is NULL
 */
double getDiskFreeSpace(const LogicalDiskInfo *disk);

#endif // STORAGE_INFO_H
