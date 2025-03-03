#ifndef NETWORK_INFO_H
#define NETWORK_INFO_H

#include <windows.h>
#include <iphlpapi.h>

/**
 * @brief Information about a single network adapter
 *
 * Contains detailed network interface specifications:
 * - Adapter identification
 * - Network configuration
 * - Connection status
 * - Interface type
 *
 * Supports both Ethernet and WiFi adapters
 * IP addresses are in IPv4 format
 *
 * @note Maximum adapter name length is 255 characters
 */
typedef struct
{
    char name[256];      // Adapter friendly name
    char macAddress[64]; // Physical (MAC) address
    char ipAddress[16];  // IPv4 address
    char status[32];     // Connection status
    UINT type;           // Interface type (Ethernet/WiFi)
} NetworkAdapterInfo;

/**
 * @brief Container for multiple network adapter information
 *
 * Holds information about all network interfaces:
 * - Array of NetworkAdapterInfo structures
 * - Number of adapters
 *
 * Includes both active and inactive adapters
 *
 * @note Caller must free using freeNetworkList()
 */
typedef struct
{
    NetworkAdapterInfo *adapters; // Array of adapter information
    UINT count;                   // Number of adapters
} NetworkList;

/**
 * @brief Retrieves information about network adapters
 *
 * This function:
 * 1. Enumerates network interfaces
 * 2. Collects adapter details
 * 3. Determines connection status
 * 4. Identifies adapter types
 *
 * @return NetworkList* Pointer to allocated adapter list, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeNetworkList()
 */
NetworkList *getNetworkList(void);

/**
 * @brief Frees memory allocated for network adapter list
 *
 * @param list Pointer to NetworkList structure to be freed
 */
void freeNetworkList(NetworkList *list);

/**
 * @brief Gets the friendly name of a network adapter
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return const char* Adapter name or empty string if adapter is NULL
 */
const char *getAdapterName(const NetworkAdapterInfo *adapter);

/**
 * @brief Gets the MAC address of a network adapter
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return const char* MAC address or empty string if adapter is NULL
 */
const char *getAdapterMacAddress(const NetworkAdapterInfo *adapter);

/**
 * @brief Gets the IPv4 address of a network adapter
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return const char* IP address or empty string if adapter is NULL
 */
const char *getAdapterIPAddress(const NetworkAdapterInfo *adapter);

/**
 * @brief Gets the connection status of a network adapter
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return const char* Status string or empty string if adapter is NULL
 */
const char *getAdapterStatus(const NetworkAdapterInfo *adapter);

/**
 * @brief Checks if the adapter is an Ethernet interface
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return BOOL TRUE if Ethernet adapter, FALSE if not or adapter is NULL
 */
BOOL isEthernetAdapter(const NetworkAdapterInfo *adapter);

/**
 * @brief Checks if the adapter is a WiFi interface
 *
 * @param adapter Pointer to NetworkAdapterInfo structure
 * @return BOOL TRUE if WiFi adapter, FALSE if not or adapter is NULL
 */
BOOL isWiFiAdapter(const NetworkAdapterInfo *adapter);

#endif // NETWORK_INFO_H
