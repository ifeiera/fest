#include "network_info.h"
#include <stdio.h>
#include <ctype.h>

#pragma comment(lib, "iphlpapi.lib")

/**
 * @brief Determines if a network adapter is virtual or system-created
 *
 * This function checks the adapter description against common keywords
 * that indicate virtual or system adapters such as:
 * - Virtual adapters
 * - Pseudo interfaces
 * - Loopback adapters
 * - Microsoft system adapters
 *
 * @param description Adapter description string to check
 * @return BOOL TRUE if system adapter, FALSE if physical adapter
 */
static BOOL isSystemAdapter(const char *description)
{
    char lowerDesc[256];
    strncpy_s(lowerDesc, sizeof(lowerDesc), description, _TRUNCATE);

    // Convert to lowercase for case-insensitive comparison
    for (int i = 0; lowerDesc[i]; i++)
    {
        lowerDesc[i] = tolower(lowerDesc[i]);
    }

    return (strstr(lowerDesc, "virtual") != NULL ||
            strstr(lowerDesc, "pseudo") != NULL ||
            strstr(lowerDesc, "loopback") != NULL ||
            strstr(lowerDesc, "microsoft") != NULL);
}

/**
 * @brief Retrieves information about physical network adapters
 *
 * This function uses the Windows IP Helper API to enumerate and collect
 * information about network adapters, including:
 * - Adapter name and description
 * - MAC address
 * - IP address
 * - Connection status
 * - Adapter type (Ethernet/WiFi)
 *
 * Virtual and system adapters are filtered out.
 *
 * @return NetworkList* Pointer to allocated list of network adapters, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeNetworkList()
 */
NetworkList *getNetworkList(void)
{
    NetworkList *list = (NetworkList *)malloc(sizeof(NetworkList));
    if (!list)
        return NULL;

    list->adapters = NULL;
    list->count = 0;

    // Initialize IP adapter info structure
    PIP_ADAPTER_INFO pAdapterInfo = NULL;
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    if (!pAdapterInfo)
    {
        free(list);
        return NULL;
    }

    // Get required buffer size and reallocate if needed
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
        if (!pAdapterInfo)
        {
            free(list);
            return NULL;
        }
    }

    // Enumerate network adapters
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
    {
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;

        while (pAdapter)
        {
            // Filter out virtual and system adapters
            if (!isSystemAdapter(pAdapter->Description))
            {
                list->count++;
                list->adapters = (NetworkAdapterInfo *)realloc(list->adapters,
                                                               sizeof(NetworkAdapterInfo) * list->count);

                NetworkAdapterInfo *adapter = &list->adapters[list->count - 1];

                // Store adapter identification
                strncpy_s(adapter->name, sizeof(adapter->name),
                          pAdapter->Description, _TRUNCATE);
                strncpy_s(adapter->macAddress, sizeof(adapter->macAddress),
                          pAdapter->AdapterName, _TRUNCATE);

                // Determine connection status from IP address
                if (strcmp(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0") != 0)
                {
                    strncpy_s(adapter->ipAddress, sizeof(adapter->ipAddress),
                              pAdapter->IpAddressList.IpAddress.String, _TRUNCATE);
                    strcpy_s(adapter->status, sizeof(adapter->status), "Connected");
                }
                else
                {
                    strcpy_s(adapter->ipAddress, sizeof(adapter->ipAddress), "N/A");
                    strcpy_s(adapter->status, sizeof(adapter->status), "Not Connected");
                }

                // Store adapter type for interface identification
                adapter->type = pAdapter->Type;
            }

            pAdapter = pAdapter->Next;
        }
    }

    // Cleanup allocated resources
    if (pAdapterInfo)
    {
        free(pAdapterInfo);
    }

    return list;
}

/**
 * @brief Frees memory allocated for NetworkList structure
 *
 * @param list Pointer to NetworkList structure to be freed
 */
void freeNetworkList(NetworkList *list)
{
    if (list)
    {
        if (list->adapters)
            free(list->adapters);
        free(list);
    }
}

/**
 * @brief Gets the adapter name/description
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return const char* Adapter name or empty string if adapter is NULL
 */
const char *getAdapterName(const NetworkAdapterInfo *adapter)
{
    return adapter ? adapter->name : "";
}

/**
 * @brief Gets the adapter's MAC address
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return const char* MAC address or empty string if adapter is NULL
 */
const char *getAdapterMacAddress(const NetworkAdapterInfo *adapter)
{
    return adapter ? adapter->macAddress : "";
}

/**
 * @brief Gets the adapter's IP address
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return const char* IP address or empty string if adapter is NULL
 */
const char *getAdapterIPAddress(const NetworkAdapterInfo *adapter)
{
    return adapter ? adapter->ipAddress : "";
}

/**
 * @brief Gets the adapter's connection status
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return const char* Status string ("Connected"/"Not Connected") or empty string if adapter is NULL
 */
const char *getAdapterStatus(const NetworkAdapterInfo *adapter)
{
    return adapter ? adapter->status : "";
}

/**
 * @brief Checks if the adapter is an Ethernet interface
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return BOOL TRUE if Ethernet adapter, FALSE if not or adapter is NULL
 */
BOOL isEthernetAdapter(const NetworkAdapterInfo *adapter)
{
    return adapter ? (adapter->type == MIB_IF_TYPE_ETHERNET) : FALSE;
}

/**
 * @brief Checks if the adapter is a WiFi interface
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return BOOL TRUE if WiFi adapter, FALSE if not or adapter is NULL
 */
BOOL isWiFiAdapter(const NetworkAdapterInfo *adapter)
{
    return adapter ? (adapter->type == IF_TYPE_IEEE80211) : FALSE;
}
