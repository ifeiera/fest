#include "storage_info.h"
#include "wmi_helper.h"
#include <stdio.h>

/**
 * @brief Structure to map physical disks to logical drive letters
 */
typedef struct
{
    char logicalDrive[4];
    PhysicalDiskInfo info;
} DiskMapping;

/**
 * @brief Container for disk mapping information
 */
typedef struct
{
    DiskMapping *mappings;
    UINT count;
} DiskMappingList;

/**
 * @brief Retrieves physical disk information and maps to logical drives
 *
 * This function queries WMI to get physical disk information and establishes
 * the relationship between physical disks and their logical drive letters.
 *
 * @param session Active WMI session
 * @return DiskMappingList* List of disk mappings, NULL if failed
 */
static DiskMappingList *getPhysicalDiskMappings(WMISession *session)
{
    DiskMappingList *mappings = (DiskMappingList *)malloc(sizeof(DiskMappingList));
    if (!mappings)
        return NULL;

    mappings->mappings = NULL;
    mappings->count = 0;

    IEnumWbemClassObject *pDiskEnum = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_DiskDrive", &pDiskEnum))
    {
        IWbemClassObject *pDiskObj = NULL;
        ULONG uReturn = 0;

        while (SUCCEEDED(pDiskEnum->lpVtbl->Next(pDiskEnum, WBEM_INFINITE, 1, &pDiskObj, &uReturn)) && uReturn != 0)
        {
            VARIANT vtProp;
            PhysicalDiskInfo diskInfo;
            wchar_t deviceID[256];

            // Get disk device ID
            if (SUCCEEDED(pDiskObj->lpVtbl->Get(pDiskObj, L"DeviceID", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
            {
                wcscpy_s(deviceID, 256, vtProp.bstrVal);
            }
            VariantClear(&vtProp);

            // Get disk model
            getWMIPropertyString(pDiskObj, L"Model", diskInfo.model, sizeof(diskInfo.model));

            // Get interface type
            getWMIPropertyString(pDiskObj, L"InterfaceType", diskInfo.interfaceType, sizeof(diskInfo.interfaceType));

            // Get associated partitions
            IEnumWbemClassObject *pPartEnum = NULL;
            wchar_t query[512];
            swprintf_s(query, 512, L"ASSOCIATORS OF {Win32_DiskDrive.DeviceID='%s'} WHERE AssocClass = Win32_DiskDriveToDiskPartition", deviceID);

            if (executeWQLQuery(session, query, &pPartEnum))
            {
                IWbemClassObject *pPartObj = NULL;
                ULONG uPartReturn = 0;

                while (SUCCEEDED(pPartEnum->lpVtbl->Next(pPartEnum, WBEM_INFINITE, 1, &pPartObj, &uPartReturn)) && uPartReturn != 0)
                {
                    VARIANT vtPartProp;
                    if (SUCCEEDED(pPartObj->lpVtbl->Get(pPartObj, L"DeviceID", 0, &vtPartProp, 0, 0)))
                    {
                        // Get logical disks for each partition
                        IEnumWbemClassObject *pLogicalEnum = NULL;
                        wchar_t partQuery[512];
                        swprintf_s(partQuery, 512, L"ASSOCIATORS OF {Win32_DiskPartition.DeviceID='%s'} WHERE AssocClass = Win32_LogicalDiskToPartition", vtPartProp.bstrVal);

                        if (executeWQLQuery(session, partQuery, &pLogicalEnum))
                        {
                            IWbemClassObject *pLogicalObj = NULL;
                            ULONG uLogicalReturn = 0;

                            while (SUCCEEDED(pLogicalEnum->lpVtbl->Next(pLogicalEnum, WBEM_INFINITE, 1, &pLogicalObj, &uLogicalReturn)) && uLogicalReturn != 0)
                            {
                                VARIANT vtLogicalProp;
                                if (SUCCEEDED(pLogicalObj->lpVtbl->Get(pLogicalObj, L"DeviceID", 0, &vtLogicalProp, 0, 0)))
                                {
                                    // Add new mapping
                                    mappings->count++;
                                    mappings->mappings = (DiskMapping *)realloc(mappings->mappings, sizeof(DiskMapping) * mappings->count);

                                    DiskMapping *mapping = &mappings->mappings[mappings->count - 1];
                                    wcstombs_s(NULL, mapping->logicalDrive, 4, vtLogicalProp.bstrVal, _TRUNCATE);
                                    memcpy(&mapping->info, &diskInfo, sizeof(PhysicalDiskInfo));
                                }
                                VariantClear(&vtLogicalProp);
                                pLogicalObj->lpVtbl->Release(pLogicalObj);
                            }
                            pLogicalEnum->lpVtbl->Release(pLogicalEnum);
                        }
                    }
                    VariantClear(&vtPartProp);
                    pPartObj->lpVtbl->Release(pPartObj);
                }
                pPartEnum->lpVtbl->Release(pPartEnum);
            }
            pDiskObj->lpVtbl->Release(pDiskObj);
        }
        pDiskEnum->lpVtbl->Release(pDiskEnum);
    }

    return mappings;
}

/**
 * @brief Gets logical disk information for a physical disk
 *
 * This function follows the association chain:
 * Physical Disk -> Partition -> Logical Disk
 * to get information about logical drives associated with a physical disk.
 *
 * @param session Active WMI session
 * @param deviceID Physical disk device ID
 * @param disk Pointer to LogicalDiskInfo structure to fill
 */
static void getLogicalDisksForPhysicalDisk(WMISession *session, const wchar_t *deviceID, LogicalDiskInfo *disk)
{
    wchar_t query[512];
    swprintf_s(query, 512,
               L"ASSOCIATORS OF {Win32_DiskDrive.DeviceID='%s'} "
               L"WHERE AssocClass = Win32_DiskDriveToDiskPartition",
               deviceID);

    IEnumWbemClassObject *pPartEnum = NULL;
    if (executeWQLQuery(session, query, &pPartEnum))
    {
        IWbemClassObject *pPartObj = NULL;
        ULONG uPartReturn = 0;

        while (SUCCEEDED(pPartEnum->lpVtbl->Next(pPartEnum, WBEM_INFINITE, 1, &pPartObj, &uPartReturn)) && uPartReturn != 0)
        {
            VARIANT vtPartID;
            if (SUCCEEDED(pPartObj->lpVtbl->Get(pPartObj, L"DeviceID", 0, &vtPartID, 0, 0)))
            {
                wchar_t partQuery[512];
                swprintf_s(partQuery, 512,
                           L"ASSOCIATORS OF {Win32_DiskPartition.DeviceID='%s'} "
                           L"WHERE AssocClass = Win32_LogicalDiskToPartition",
                           vtPartID.bstrVal);

                IEnumWbemClassObject *pLogicalEnum = NULL;
                if (executeWQLQuery(session, partQuery, &pLogicalEnum))
                {
                    IWbemClassObject *pLogicalObj = NULL;
                    ULONG uLogicalReturn = 0;

                    while (SUCCEEDED(pLogicalEnum->lpVtbl->Next(pLogicalEnum, WBEM_INFINITE, 1, &pLogicalObj, &uLogicalReturn)) && uLogicalReturn != 0)
                    {
                        // Get drive letter
                        char driveLetter[4] = {0};
                        if (getWMIPropertyString(pLogicalObj, L"DeviceID", driveLetter, sizeof(driveLetter)))
                        {
                            strcpy_s(disk->drive, sizeof(disk->drive), driveLetter);
                        }

                        // Get drive type
                        VARIANT vtDriveType;
                        if (SUCCEEDED(pLogicalObj->lpVtbl->Get(pLogicalObj, L"DriveType", 0, &vtDriveType, 0, 0)) && vtDriveType.vt != VT_NULL)
                        {
                            switch (vtDriveType.uintVal)
                            {
                            case 2:
                                strcpy_s(disk->type, sizeof(disk->type), "Removable Disk");
                                break;
                            case 3:
                                strcpy_s(disk->type, sizeof(disk->type), "Local Disk");
                                break;
                            case 4:
                                strcpy_s(disk->type, sizeof(disk->type), "Network Drive");
                                break;
                            case 5:
                                strcpy_s(disk->type, sizeof(disk->type), "CD/DVD Drive");
                                break;
                            case 6:
                                strcpy_s(disk->type, sizeof(disk->type), "RAM Disk");
                                break;
                            default:
                                strcpy_s(disk->type, sizeof(disk->type), "Unknown");
                                break;
                            }
                        }
                        VariantClear(&vtDriveType);

                        // Get free space
                        VARIANT vtFreeSpace;
                        if (SUCCEEDED(pLogicalObj->lpVtbl->Get(pLogicalObj, L"FreeSpace", 0, &vtFreeSpace, 0, 0)) && vtFreeSpace.vt != VT_NULL)
                        {
                            disk->freeSpace = _wtof(vtFreeSpace.bstrVal) / (1024.0 * 1024.0 * 1024.0);
                        }
                        VariantClear(&vtFreeSpace);

                        // Get volume name for model if empty
                        if (strlen(disk->model) == 0)
                        {
                            getWMIPropertyString(pLogicalObj, L"VolumeName", disk->model, sizeof(disk->model));
                        }

                        pLogicalObj->lpVtbl->Release(pLogicalObj);
                    }
                    pLogicalEnum->lpVtbl->Release(pLogicalEnum);
                }
            }
            VariantClear(&vtPartID);
            pPartObj->lpVtbl->Release(pPartObj);
        }
        pPartEnum->lpVtbl->Release(pPartEnum);
    }
}

/**
 * @brief Retrieves comprehensive storage device information
 *
 * This function collects detailed information about storage devices including:
 * 1. Physical disk properties:
 *    - Model and manufacturer
 *    - Interface type (SATA, NVMe, etc.)
 *    - Total capacity
 *
 * 2. Logical drive information:
 *    - Drive letters
 *    - Drive types (Local, Removable, etc.)
 *    - Free space
 *    - Volume names
 *
 * @return StorageList* Pointer to allocated storage information list, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeStorageList()
 */
StorageList *getStorageList(void)
{
    StorageList *list = (StorageList *)malloc(sizeof(StorageList));
    if (!list)
        return NULL;

    list->disks = NULL;
    list->count = 0;

    // Initialize WMI connection
    WMISession *session = initializeWMI();
    if (!session)
    {
        free(list);
        return NULL;
    }

    // Query physical disk information
    IEnumWbemClassObject *pEnumerator = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_DiskDrive", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        // Count disks first
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            list->count++;
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);

        if (list->count > 0)
        {
            // Allocate memory for disks
            list->disks = (LogicalDiskInfo *)malloc(sizeof(LogicalDiskInfo) * list->count);
            if (!list->disks)
            {
                cleanupWMI(session);
                free(list);
                return NULL;
            }

            // Re-query to get data
            if (executeWQLQuery(session, L"SELECT * FROM Win32_DiskDrive", &pEnumerator))
            {
                UINT i = 0;
                while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
                {
                    LogicalDiskInfo *disk = &list->disks[i];

                    // Initialize with default values
                    memset(disk, 0, sizeof(LogicalDiskInfo));
                    strcpy_s(disk->drive, sizeof(disk->drive), "N/A");
                    strcpy_s(disk->type, sizeof(disk->type), "Local Disk");

                    // Get disk model
                    getWMIPropertyString(pclsObj, L"Model", disk->model, sizeof(disk->model));

                    // Get interface type
                    getWMIPropertyString(pclsObj, L"InterfaceType", disk->interfaceType, sizeof(disk->interfaceType));

                    // Get size information
                    VARIANT vtProp;
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"Size", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        disk->totalSize = _wtof(vtProp.bstrVal) / (1024.0 * 1024.0 * 1024.0);
                    }
                    VariantClear(&vtProp);

                    // Get device ID and logical disk information
                    VARIANT vtDeviceID;
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"DeviceID", 0, &vtDeviceID, 0, 0)) && vtDeviceID.vt != VT_NULL)
                    {
                        getLogicalDisksForPhysicalDisk(session, vtDeviceID.bstrVal, disk);
                        VariantClear(&vtDeviceID);
                    }

                    pclsObj->lpVtbl->Release(pclsObj);
                    i++;
                }
                pEnumerator->lpVtbl->Release(pEnumerator);
            }
        }
    }

    cleanupWMI(session);
    return list;
}

/**
 * @brief Frees memory allocated for StorageList structure
 *
 * @param list Pointer to StorageList structure to be freed
 */
void freeStorageList(StorageList *list)
{
    if (list)
    {
        if (list->disks)
            free(list->disks);
        free(list);
    }
}

/**
 * @brief Gets the drive letter of the logical disk
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return const char* Drive letter (e.g. "C:") or empty string if disk is NULL
 */
const char *getDiskDrive(const LogicalDiskInfo *disk)
{
    return disk ? disk->drive : "";
}

/**
 * @brief Gets the type of the logical disk
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return const char* Drive type (Local Disk, Removable Disk, etc.) or empty string if disk is NULL
 */
const char *getDiskType(const LogicalDiskInfo *disk)
{
    return disk ? disk->type : "";
}

/**
 * @brief Gets the model/name of the disk
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return const char* Disk model or volume name, empty string if disk is NULL
 */
const char *getDiskModel(const LogicalDiskInfo *disk)
{
    return disk ? disk->model : "";
}

/**
 * @brief Gets the interface type of the disk
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return const char* Interface type (SATA, NVMe, etc.) or empty string if disk is NULL
 */
const char *getDiskInterface(const LogicalDiskInfo *disk)
{
    return disk ? disk->interfaceType : "";
}

/**
 * @brief Gets the total size of the disk
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return double Total size in gigabytes or 0.0 if disk is NULL
 */
double getDiskTotalSize(const LogicalDiskInfo *disk)
{
    return disk ? disk->totalSize : 0.0;
}

/**
 * @brief Gets the available free space on the disk
 *
 * @param disk Pointer to LogicalDiskInfo structure
 * @return double Free space in gigabytes or 0.0 if disk is NULL
 */
double getDiskFreeSpace(const LogicalDiskInfo *disk)
{
    return disk ? disk->freeSpace : 0.0;
}
