#include "motherboard_info.h"
#include "wmi_helper.h"
#include <stdio.h>

/**
 * @brief Retrieves comprehensive motherboard and system information
 *
 * This function queries multiple WMI classes to collect:
 * 1. Baseboard (Motherboard) information:
 *    - Product name and model
 *    - Manufacturer
 *    - Serial number
 *
 * 2. BIOS information:
 *    - BIOS version
 *    - BIOS serial number
 *
 * 3. System information:
 *    - System SKU number
 *
 * @return MotherboardInfo* Pointer to allocated motherboard information structure, NULL if failed
 * @note Caller is responsible for freeing the returned structure using freeMotherboardInfo()
 */
MotherboardInfo *getMotherboardInfo(void)
{
    MotherboardInfo *info = (MotherboardInfo *)malloc(sizeof(MotherboardInfo));
    if (!info)
        return NULL;

    // Initialize WMI connection
    WMISession *session = initializeWMI();
    if (!session)
    {
        free(info);
        return NULL;
    }

    // Query Win32_BaseBoard for motherboard information
    IEnumWbemClassObject *pEnumerator = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_BaseBoard", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            // Get motherboard identification details
            getWMIPropertyString(pclsObj, L"Product", info->productName, sizeof(info->productName));
            getWMIPropertyString(pclsObj, L"Manufacturer", info->manufacturer, sizeof(info->manufacturer));
            getWMIPropertyString(pclsObj, L"SerialNumber", info->serialNumber, sizeof(info->serialNumber));

            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);
    }

    // Query Win32_BIOS for BIOS information
    if (executeWQLQuery(session, L"SELECT * FROM Win32_BIOS", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            // Get BIOS version and serial information
            getWMIPropertyString(pclsObj, L"SMBIOSBIOSVersion", info->biosVersion, sizeof(info->biosVersion));
            getWMIPropertyString(pclsObj, L"SerialNumber", info->biosSerial, sizeof(info->biosSerial));

            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);
    }

    // Query Win32_ComputerSystem for system SKU
    if (executeWQLQuery(session, L"SELECT * FROM Win32_ComputerSystem", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            // Get system SKU number
            getWMIPropertyString(pclsObj, L"SystemSKUNumber", info->systemSKU, sizeof(info->systemSKU));
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);
    }

    cleanupWMI(session);
    return info;
}

/**
 * @brief Frees memory allocated for MotherboardInfo structure
 *
 * @param info Pointer to MotherboardInfo structure to be freed
 */
void freeMotherboardInfo(MotherboardInfo *info)
{
    if (info)
    {
        free(info);
    }
}

/**
 * @brief Gets the motherboard product name/model
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* Product name or empty string if info is NULL
 */
const char *getMotherboardProduct(const MotherboardInfo *info)
{
    return info ? info->productName : "";
}

/**
 * @brief Gets the motherboard manufacturer
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* Manufacturer name or empty string if info is NULL
 */
const char *getMotherboardManufacturer(const MotherboardInfo *info)
{
    return info ? info->manufacturer : "";
}

/**
 * @brief Gets the motherboard serial number
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* Serial number or empty string if info is NULL
 */
const char *getMotherboardSerial(const MotherboardInfo *info)
{
    return info ? info->serialNumber : "";
}

/**
 * @brief Gets the BIOS version
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* BIOS version string or empty string if info is NULL
 */
const char *getMotherboardBiosVersion(const MotherboardInfo *info)
{
    return info ? info->biosVersion : "";
}

/**
 * @brief Gets the system SKU number
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* System SKU or empty string if info is NULL
 */
const char *getMotherboardSystemSKU(const MotherboardInfo *info)
{
    return info ? info->systemSKU : "";
}

/**
 * @brief Gets the BIOS serial number
 *
 * @param info Pointer to MotherboardInfo structure
 * @return const char* BIOS serial number or empty string if info is NULL
 */
const char *getMotherboardBiosSerial(const MotherboardInfo *info)
{
    return info ? info->biosSerial : "";
}
