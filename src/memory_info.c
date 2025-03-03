#include "memory_info.h"
#include "wmi_helper.h"
#include <stdio.h>

/**
 * @brief Converts bytes to gigabytes
 *
 * @param bytes Value in bytes to convert
 * @return double Value in gigabytes
 */
double bytesToGB(UINT64 bytes)
{
    return (double)bytes / (1024.0 * 1024.0 * 1024.0);
}

/**
 * @brief Retrieves comprehensive memory information from the system
 *
 * This function collects two types of memory information:
 * 1. System-wide memory statistics:
 *    - Total physical memory
 *    - Available memory
 *    - Memory usage percentage
 *
 * 2. Physical RAM slot information via WMI:
 *    - Capacity per slot
 *    - Speed ratings
 *    - Slot locations
 *    - Manufacturer details
 *
 * @return MemoryInfo* Pointer to allocated memory information structure, NULL if failed
 * @note Caller is responsible for freeing the returned structure using freeMemoryInfo()
 */
MemoryInfo *getMemoryInfo(void)
{
    MemoryInfo *info = (MemoryInfo *)malloc(sizeof(MemoryInfo));
    if (!info)
        return NULL;

    // Initialize RAM slot list
    info->slotList.slots = NULL;
    info->slotList.count = 0;

    // Query system-wide memory statistics
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&memStatus))
    {
        free(info);
        return NULL;
    }

    // Store system memory metrics
    info->totalPhys = memStatus.ullTotalPhys;
    info->availPhys = memStatus.ullAvailPhys;
    info->usedPhys = memStatus.ullTotalPhys - memStatus.ullAvailPhys;
    info->memoryLoad = memStatus.dwMemoryLoad;

    // Initialize WMI for detailed RAM information
    WMISession *session = initializeWMI();
    if (!session)
    {
        free(info);
        return NULL;
    }

    // Query physical memory modules
    IEnumWbemClassObject *pEnumerator = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_PhysicalMemory", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        // First pass: Count total number of RAM slots
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            info->slotList.count++;
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);

        if (info->slotList.count > 0)
        {
            // Allocate memory for RAM slot array
            info->slotList.slots = (RAMSlotInfo *)malloc(sizeof(RAMSlotInfo) * info->slotList.count);
            if (!info->slotList.slots)
            {
                cleanupWMI(session);
                free(info);
                return NULL;
            }

            // Second pass: Collect detailed RAM module information
            if (executeWQLQuery(session, L"SELECT * FROM Win32_PhysicalMemory", &pEnumerator))
            {
                UINT i = 0;
                while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
                {
                    RAMSlotInfo *slot = &info->slotList.slots[i];
                    VARIANT vtProp;

                    // Get module capacity in bytes
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"Capacity", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        slot->capacity = _wtoi64(vtProp.bstrVal);
                    }
                    else
                    {
                        slot->capacity = 0;
                    }
                    VariantClear(&vtProp);

                    // Get rated memory speed in MHz
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"Speed", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        slot->speed = vtProp.uintVal;
                    }
                    else
                    {
                        slot->speed = 0;
                    }
                    VariantClear(&vtProp);

                    // Get actual configured speed in MHz
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"ConfiguredClockSpeed", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        slot->configuredSpeed = vtProp.uintVal;
                    }
                    else
                    {
                        slot->configuredSpeed = slot->speed; // Use rated speed as fallback
                    }
                    VariantClear(&vtProp);

                    // Get physical slot location identifier
                    getWMIPropertyString(pclsObj, L"DeviceLocator", slot->slot, sizeof(slot->slot));

                    // Get module manufacturer information
                    getWMIPropertyString(pclsObj, L"Manufacturer", slot->manufacturer, sizeof(slot->manufacturer));

                    pclsObj->lpVtbl->Release(pclsObj);
                    i++;
                }
                pEnumerator->lpVtbl->Release(pEnumerator);
            }
        }
    }

    cleanupWMI(session);
    return info;
}

/**
 * @brief Frees memory allocated for MemoryInfo structure
 *
 * @param info Pointer to MemoryInfo structure to be freed
 */
void freeMemoryInfo(MemoryInfo *info)
{
    if (info)
    {
        if (info->slotList.slots)
        {
            free(info->slotList.slots);
        }
        free(info);
    }
}

/**
 * @brief Gets the physical location identifier of a RAM slot
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return const char* Slot location string or "Unknown" if slot is NULL
 */
const char *getRAMSlotLocation(const RAMSlotInfo *slot)
{
    return slot ? slot->slot : "Unknown";
}

/**
 * @brief Gets the manufacturer of the RAM module
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return const char* Manufacturer name or "Unknown" if slot is NULL
 */
const char *getRAMManufacturer(const RAMSlotInfo *slot)
{
    return slot ? slot->manufacturer : "Unknown";
}

/**
 * @brief Gets the capacity of the RAM module
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return UINT64 Capacity in bytes or 0 if slot is NULL
 */
UINT64 getRAMCapacity(const RAMSlotInfo *slot)
{
    return slot ? slot->capacity : 0;
}

/**
 * @brief Gets the rated speed of the RAM module
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return UINT Speed in MHz or 0 if slot is NULL
 */
UINT getRAMSpeed(const RAMSlotInfo *slot)
{
    return slot ? slot->speed : 0;
}

/**
 * @brief Gets the actual configured speed of the RAM module
 *
 * @param slot Pointer to RAMSlotInfo structure
 * @return UINT Configured speed in MHz or 0 if slot is NULL
 */
UINT getRAMConfiguredSpeed(const RAMSlotInfo *slot)
{
    return slot ? slot->configuredSpeed : 0;
}
