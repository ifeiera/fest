#ifndef NETWORK_INFO_H
#define NETWORK_INFO_H

#include <windows.h>
#include <iphlpapi.h>

// Struktur untuk menyimpan informasi adapter jaringan
typedef struct
{
    char name[256];
    char macAddress[64];
    char ipAddress[16];
    char status[32];
    UINT type; // MIB_IF_TYPE_ETHERNET atau IF_TYPE_IEEE80211
} NetworkAdapterInfo;

// Struktur untuk menyimpan array dari adapter
typedef struct
{
    NetworkAdapterInfo *adapters;
    UINT count;
} NetworkList;

// Fungsi-fungsi untuk mendapatkan informasi network
NetworkList *getNetworkList(void);
void freeNetworkList(NetworkList *list);

// Fungsi getter
const char *getAdapterName(const NetworkAdapterInfo *adapter);
const char *getAdapterMacAddress(const NetworkAdapterInfo *adapter);
const char *getAdapterIPAddress(const NetworkAdapterInfo *adapter);
const char *getAdapterStatus(const NetworkAdapterInfo *adapter);
BOOL isEthernetAdapter(const NetworkAdapterInfo *adapter);
BOOL isWiFiAdapter(const NetworkAdapterInfo *adapter);

#endif // NETWORK_INFO_H
