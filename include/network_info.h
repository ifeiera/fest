#ifndef NETWORK_INFO_H
#define NETWORK_INFO_H

#include <windows.h>
#include <iphlpapi.h>

// Structure for storing network adapter information
typedef struct
{
    char name[256];
    char macAddress[64];
    char ipAddress[16];
    char status[32];
    UINT type; // MIB_IF_TYPE_ETHERNET or IF_TYPE_IEEE80211
} NetworkAdapterInfo;

// Structure for storing array of adapters
typedef struct
{
    NetworkAdapterInfo *adapters;
    UINT count;
} NetworkList;

// Functions for getting network information
NetworkList *getNetworkList(void);
void freeNetworkList(NetworkList *list);

// Getter functions
const char *getAdapterName(const NetworkAdapterInfo *adapter);
const char *getAdapterMacAddress(const NetworkAdapterInfo *adapter);
const char *getAdapterIPAddress(const NetworkAdapterInfo *adapter);
const char *getAdapterStatus(const NetworkAdapterInfo *adapter);
BOOL isEthernetAdapter(const NetworkAdapterInfo *adapter);
BOOL isWiFiAdapter(const NetworkAdapterInfo *adapter);

#endif // NETWORK_INFO_H
