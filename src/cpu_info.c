#include "cpu_info.h"
#include "wmi_helper.h"
#include <stdio.h>

#pragma comment(lib, "pdh.lib")

/**
 * @brief Retrieves detailed information about CPU(s) installed in the system
 *
 * This function queries the Windows Management Instrumentation (WMI) to get
 * comprehensive CPU information including:
 * - Processor name and model
 * - Number of physical cores
 * - Number of logical processors (threads)
 * - Maximum clock speed
 *
 * Uses Win32_Processor WMI class to gather detailed CPU specifications.
 *
 * @return CPUList* Pointer to allocated list of CPU information, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeCPUList()
 */
CPUList *getCPUList(void)
{
    CPUList *list = (CPUList *)malloc(sizeof(CPUList));
    if (!list)
        return NULL;

    list->cpus = NULL;
    list->count = 0;

    // Initialize WMI connection
    WMISession *session = initializeWMI();
    if (!session)
    {
        free(list);
        return NULL;
    }

    // Query Win32_Processor class for CPU information
    IEnumWbemClassObject *pEnumerator = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_Processor", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        // First pass: Count total number of processors
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            list->count++;
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);

        if (list->count > 0)
        {
            // Allocate memory for CPU array
            list->cpus = (CPUInfo *)malloc(sizeof(CPUInfo) * list->count);
            if (!list->cpus)
            {
                cleanupWMI(session);
                free(list);
                return NULL;
            }

            // Second pass: Collect detailed CPU information
            if (executeWQLQuery(session, L"SELECT * FROM Win32_Processor", &pEnumerator))
            {
                UINT i = 0;
                while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
                {
                    CPUInfo *cpu = &list->cpus[i];

                    // Get processor name and model
                    getWMIPropertyString(pclsObj, L"Name", cpu->name, sizeof(cpu->name));

                    // Get number of physical cores
                    VARIANT vtProp;
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"NumberOfCores", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        cpu->cores = vtProp.uintVal;
                    }
                    VariantClear(&vtProp);

                    // Get number of logical processors (threads)
                    if (SUCCEEDED(pclsObj->lpVtbl->Get(pclsObj, L"NumberOfLogicalProcessors", 0, &vtProp, 0, 0)) && vtProp.vt != VT_NULL)
                    {
                        cpu->threads = vtProp.uintVal;
                    }
                    VariantClear(&vtProp);

                    // Get maximum clock speed in MHz
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

/**
 * @brief Frees memory allocated for CPUList structure
 *
 * @param list Pointer to CPUList structure to be freed
 */
void freeCPUList(CPUList *list)
{
    if (list)
    {
        if (list->cpus)
            free(list->cpus);
        free(list);
    }
}

/**
 * @brief Gets the name and model of the CPU
 *
 * @param cpu Pointer to CPUInfo structure
 * @return const char* CPU name or empty string if cpu is NULL
 */
const char *getCPUName(const CPUInfo *cpu)
{
    return cpu ? cpu->name : "";
}

/**
 * @brief Gets the number of physical cores
 *
 * @param cpu Pointer to CPUInfo structure
 * @return UINT Number of cores or 0 if cpu is NULL
 */
UINT getCPUCores(const CPUInfo *cpu)
{
    return cpu ? cpu->cores : 0;
}

/**
 * @brief Gets the number of logical processors (threads)
 *
 * @param cpu Pointer to CPUInfo structure
 * @return UINT Number of threads or 0 if cpu is NULL
 */
UINT getCPUThreads(const CPUInfo *cpu)
{
    return cpu ? cpu->threads : 0;
}

/**
 * @brief Gets the maximum clock speed in MHz
 *
 * @param cpu Pointer to CPUInfo structure
 * @return UINT Clock speed in MHz or 0 if cpu is NULL
 */
UINT getCPUClockSpeed(const CPUInfo *cpu)
{
    return cpu ? cpu->clockSpeed : 0;
}
