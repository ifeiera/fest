#ifndef WMI_HELPER_H
#define WMI_HELPER_H

#include <windows.h>
#include <wbemidl.h>
#include <oleauto.h>

/**
 * @brief Container for WMI connection resources
 *
 * Maintains Windows Management Instrumentation session:
 * - Service connection
 * - COM locator
 * - Connection state
 *
 * Used for querying system hardware information
 * through the Windows WMI interface
 *
 * @note Must be initialized before use and cleaned up after
 */
typedef struct
{
    IWbemServices *pSvc; // WMI service connection
    IWbemLocator *pLoc;  // COM service locator
    BOOL initialized;    // Connection state flag
} WMISession;

/**
 * @brief Initializes WMI connection
 *
 * This function:
 * 1. Initializes COM library
 * 2. Creates WMI locator
 * 3. Connects to WMI service
 * 4. Sets up security
 *
 * @return WMISession* Pointer to initialized session, NULL if failed
 * @note Caller must clean up using cleanupWMI()
 */
WMISession *initializeWMI(void);

/**
 * @brief Cleans up WMI session resources
 *
 * This function:
 * 1. Releases WMI interfaces
 * 2. Uninitializes COM
 * 3. Frees session structure
 *
 * @param session Pointer to WMISession to clean up
 */
void cleanupWMI(WMISession *session);

/**
 * @brief Executes a WQL query against WMI
 *
 * Performs a synchronous query using Windows Query Language:
 * - Validates session and query
 * - Executes query
 * - Returns result enumerator
 *
 * @param session Active WMI session
 * @param query WQL query string (wide character)
 * @param ppEnumerator Pointer to receive result enumerator
 * @return BOOL TRUE if query succeeded, FALSE if failed
 */
BOOL executeWQLQuery(WMISession *session, const wchar_t *query, IEnumWbemClassObject **ppEnumerator);

/**
 * @brief Retrieves string property from WMI object
 *
 * Extracts string value from WMI class object:
 * - Validates parameters
 * - Retrieves property variant
 * - Converts to string
 *
 * @param pclsObj WMI class object
 * @param property Property name (wide character)
 * @param buffer Buffer to receive string value
 * @param bufferSize Size of buffer in bytes
 * @return BOOL TRUE if property retrieved, FALSE if failed
 */
BOOL getWMIPropertyString(IWbemClassObject *pclsObj, const wchar_t *property, char *buffer, size_t bufferSize);

#endif // WMI_HELPER_H
