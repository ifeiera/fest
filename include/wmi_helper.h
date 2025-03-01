#ifndef WMI_HELPER_H
#define WMI_HELPER_H

#include <windows.h>
#include <wbemidl.h>
#include <oleauto.h>

// Structure for storing WMI session
typedef struct
{
    IWbemServices *pSvc;
    IWbemLocator *pLoc;
    BOOL initialized;
} WMISession;

// WMI helper functions
WMISession *initializeWMI(void);
void cleanupWMI(WMISession *session);
BOOL executeWQLQuery(WMISession *session, const wchar_t *query, IEnumWbemClassObject **ppEnumerator);
BOOL getWMIPropertyString(IWbemClassObject *pclsObj, const wchar_t *property, char *buffer, size_t bufferSize);

#endif // WMI_HELPER_H
