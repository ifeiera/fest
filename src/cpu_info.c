#include "cpu_info.h"
#include "wmi_helper.h"
#include <stdio.h>

#pragma comment(lib, "pdh.lib")

CPUList *getCPUList(void)
{
    CPUList *list = (CPUList *)malloc(sizeof(CPUList));
    if (!list)
        return NULL;

    list->cpus = NULL;
    list->count = 0;

    // Initialize WMI
    WMISession *session = initializeWMI();
    if (!session)
    {
        free(list);
        return NULL;
    }

    // Query CPU information
    IEnumWbemClassObject *pEnumerator = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_Processor", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        // Count CPUs first
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            list->count++;
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);

        if (list->count > 0)
        {
            // Allocate memory for CPUs
            list->cpus = (CPUInfo *)malloc(sizeof(CPUInfo) * list->count);
            if (!list->cpus)
            {
                cleanupWMI(session);
                free(list);
                return NULL;
            }

            // Re-query untuk mendapatkan data
            if (executeWQLQuery(session, L"SELECT * FROM Win32_Processor", &pEnumerator))
            {
                UINT i = 0;
                while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
                {
                    CPUInfo *cpu = &list->cpus[i];

                    // Get CPU name
                    getWMIPropertyString(pclsObj, L"Name", cpu->name, sizeof(cpu->name));

                    // Get number of cores
                    VARIANT vtProp;
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"NumberOfCores", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        cpu->cores = vtProp.uintVal;
                    }
                    VariantClear(&vtProp);

                    // Get number of threads
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"NumberOfLogicalProcessors", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        cpu->threads = vtProp.uintVal;
                    }
                    VariantClear(&vtProp);

                    // Get clock speed
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"MaxClockSpeed", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        cpu->clockSpeed = vtProp.uintVal;
                    }
                    VariantClear(&vtProp);

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

void freeCPUList(CPUList *list)
{
    if (list)
    {
        if (list->cpus)
            free(list->cpus);
        free(list);
    }
}

const char *getCPUName(const CPUInfo *cpu)
{
    return cpu ? cpu->name : "";
}

UINT getCPUCores(const CPUInfo *cpu)
{
    return cpu ? cpu->cores : 0;
}

UINT getCPUThreads(const CPUInfo *cpu)
{
    return cpu ? cpu->threads : 0;
}

UINT getCPUClockSpeed(const CPUInfo *cpu)
{
    return cpu ? cpu->clockSpeed : 0;
}
