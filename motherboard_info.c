#include "motherboard_info.h"
#include "wmi_helper.h"
#include <stdio.h>

MotherboardInfo *getMotherboardInfo(void)
{
    MotherboardInfo *info = (MotherboardInfo *)malloc(sizeof(MotherboardInfo));
    if (!info)
        return NULL;

    // Initialize WMI
    WMISession *session = initializeWMI();
    if (!session)
    {
        free(info);
        return NULL;
    }

    // Query baseboard information
    IEnumWbemClassObject *pEnumerator = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_BaseBoard", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            getWMIPropertyString(pclsObj, L"Product", info->productName, sizeof(info->productName));
            getWMIPropertyString(pclsObj, L"Manufacturer", info->manufacturer, sizeof(info->manufacturer));
            getWMIPropertyString(pclsObj, L"SerialNumber", info->serialNumber, sizeof(info->serialNumber));

            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);
    }

    // Query BIOS information
    if (executeWQLQuery(session, L"SELECT * FROM Win32_BIOS", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            getWMIPropertyString(pclsObj, L"SMBIOSBIOSVersion", info->biosVersion, sizeof(info->biosVersion));
            getWMIPropertyString(pclsObj, L"SerialNumber", info->biosSerial, sizeof(info->biosSerial));

            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);
    }

    // Query system information untuk SKU
    if (executeWQLQuery(session, L"SELECT * FROM Win32_ComputerSystem", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            getWMIPropertyString(pclsObj, L"SystemSKUNumber", info->systemSKU, sizeof(info->systemSKU));
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);
    }

    cleanupWMI(session);
    return info;
}

void freeMotherboardInfo(MotherboardInfo *info)
{
    if (info)
    {
        free(info);
    }
}

const char *getMotherboardProduct(const MotherboardInfo *info)
{
    return info ? info->productName : "";
}

const char *getMotherboardManufacturer(const MotherboardInfo *info)
{
    return info ? info->manufacturer : "";
}

const char *getMotherboardSerial(const MotherboardInfo *info)
{
    return info ? info->serialNumber : "";
}

const char *getMotherboardBiosVersion(const MotherboardInfo *info)
{
    return info ? info->biosVersion : "";
}

const char *getMotherboardSystemSKU(const MotherboardInfo *info)
{
    return info ? info->systemSKU : "";
}

const char *getMotherboardBiosSerial(const MotherboardInfo *info)
{
    return info ? info->biosSerial : "";
}
