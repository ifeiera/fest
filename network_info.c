#include "network_info.h"
#include <stdio.h>
#include <ctype.h>

#pragma comment(lib, "iphlpapi.lib")

// Helper function untuk mengecek apakah adapter adalah virtual/system
static BOOL isSystemAdapter(const char *description)
{
    char lowerDesc[256];
    strncpy_s(lowerDesc, sizeof(lowerDesc), description, _TRUNCATE);

    // Convert to lowercase
    for (int i = 0; lowerDesc[i]; i++)
    {
        lowerDesc[i] = tolower(lowerDesc[i]);
    }

    return (strstr(lowerDesc, "virtual") != NULL ||
            strstr(lowerDesc, "pseudo") != NULL ||
            strstr(lowerDesc, "loopback") != NULL ||
            strstr(lowerDesc, "microsoft") != NULL);
}

NetworkList *getNetworkList(void)
{
    NetworkList *list = (NetworkList *)malloc(sizeof(NetworkList));
    if (!list)
        return NULL;

    list->adapters = NULL;
    list->count = 0;

    // Alokasi memori untuk adapter info
    PIP_ADAPTER_INFO pAdapterInfo = NULL;
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    if (!pAdapterInfo)
    {
        free(list);
        return NULL;
    }

    // Realokasi memori jika buffer terlalu kecil
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

    // Dapatkan informasi adapter jaringan
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
    {
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;

        while (pAdapter)
        {
            // Skip virtual dan system adapter
            if (!isSystemAdapter(pAdapter->Description))
            {
                list->count++;
                list->adapters = (NetworkAdapterInfo *)realloc(list->adapters,
                                                               sizeof(NetworkAdapterInfo) * list->count);

                NetworkAdapterInfo *adapter = &list->adapters[list->count - 1];

                // Copy adapter details
                strncpy_s(adapter->name, sizeof(adapter->name),
                          pAdapter->Description, _TRUNCATE);
                strncpy_s(adapter->macAddress, sizeof(adapter->macAddress),
                          pAdapter->AdapterName, _TRUNCATE);

                // Get IP address dan status
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

                // Set adapter type
                adapter->type = pAdapter->Type;
            }

            pAdapter = pAdapter->Next;
        }
    }

    // Cleanup
    if (pAdapterInfo)
    {
        free(pAdapterInfo);
    }

    return list;
}

void freeNetworkList(NetworkList *list)
{
    if (list)
    {
        if (list->adapters)
            free(list->adapters);
        free(list);
    }
}

const char *getAdapterName(const NetworkAdapterInfo *adapter)
{
    return adapter ? adapter->name : "";
}

const char *getAdapterMacAddress(const NetworkAdapterInfo *adapter)
{
    return adapter ? adapter->macAddress : "";
}

const char *getAdapterIPAddress(const NetworkAdapterInfo *adapter)
{
    return adapter ? adapter->ipAddress : "";
}

const char *getAdapterStatus(const NetworkAdapterInfo *adapter)
{
    return adapter ? adapter->status : "";
}

BOOL isEthernetAdapter(const NetworkAdapterInfo *adapter)
{
    return adapter ? (adapter->type == MIB_IF_TYPE_ETHERNET) : FALSE;
}

BOOL isWiFiAdapter(const NetworkAdapterInfo *adapter)
{
    return adapter ? (adapter->type == IF_TYPE_IEEE80211) : FALSE;
}
