#include "memory_info.h"
#include "wmi_helper.h"
#include <stdio.h>

double bytesToGB(UINT64 bytes)
{
    return (double)bytes / (1024.0 * 1024.0 * 1024.0);
}

MemoryInfo *getMemoryInfo(void)
{
    MemoryInfo *info = (MemoryInfo *)malloc(sizeof(MemoryInfo));
    if (!info)
        return NULL;

    // Initialize with default values
    info->slotList.slots = NULL;
    info->slotList.count = 0;

    // Get system memory status
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&memStatus))
    {
        free(info);
        return NULL;
    }

    info->totalPhys = memStatus.ullTotalPhys;
    info->availPhys = memStatus.ullAvailPhys;
    info->usedPhys = memStatus.ullTotalPhys - memStatus.ullAvailPhys;
    info->memoryLoad = memStatus.dwMemoryLoad;

    // Initialize WMI for RAM slot information
    WMISession *session = initializeWMI();
    if (!session)
    {
        free(info);
        return NULL;
    }

    // Query physical RAM information
    IEnumWbemClassObject *pEnumerator = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_PhysicalMemory", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        // Count number of slots first
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            info->slotList.count++;
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);

        if (info->slotList.count > 0)
        {
            info->slotList.slots = (RAMSlotInfo *)malloc(sizeof(RAMSlotInfo) * info->slotList.count);
            if (!info->slotList.slots)
            {
                cleanupWMI(session);
                free(info);
                return NULL;
            }

            // Re-query to get data
            if (executeWQLQuery(session, L"SELECT * FROM Win32_PhysicalMemory", &pEnumerator))
            {
                UINT i = 0;
                while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
                {
                    RAMSlotInfo *slot = &info->slotList.slots[i];
                    VARIANT vtProp;

                    // Get RAM capacity
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"Capacity", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        slot->capacity = _wtoi64(vtProp.bstrVal);
                    }
                    else
                    {
                        slot->capacity = 0;
                    }
                    VariantClear(&vtProp);

                    // Get RAM speed
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"Speed", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        slot->speed = vtProp.uintVal;
                    }
                    else
                    {
                        slot->speed = 0;
                    }
                    VariantClear(&vtProp);

                    // Get configured RAM speed
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"ConfiguredClockSpeed", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        slot->configuredSpeed = vtProp.uintVal;
                    }
                    else
                    {
                        slot->configuredSpeed = slot->speed; // Fallback to speed if configuredspeed is not available
                    }
                    VariantClear(&vtProp);

                    // Get RAM slot location
                    getWMIPropertyString(pclsObj, L"DeviceLocator", slot->slot, sizeof(slot->slot));

                    // Get RAM manufacturer
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

const char *getRAMSlotLocation(const RAMSlotInfo *slot)
{
    return slot ? slot->slot : "Unknown";
}

const char *getRAMManufacturer(const RAMSlotInfo *slot)
{
    return slot ? slot->manufacturer : "Unknown";
}

UINT64 getRAMCapacity(const RAMSlotInfo *slot)
{
    return slot ? slot->capacity : 0;
}

UINT getRAMSpeed(const RAMSlotInfo *slot)
{
    return slot ? slot->speed : 0;
}

UINT getRAMConfiguredSpeed(const RAMSlotInfo *slot)
{
    return slot ? slot->configuredSpeed : 0;
}
