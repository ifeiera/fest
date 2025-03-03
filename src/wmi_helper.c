#include "wmi_helper.h"
#include <stdio.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "ole32.lib")

/**
 * @brief Initializes Windows Management Instrumentation (WMI) connection
 *
 * This function performs the following initialization steps:
 * 1. Initializes COM library
 * 2. Sets COM security levels
 * 3. Creates WMI locator instance
 * 4. Establishes connection to ROOT\CIMV2 namespace
 * 5. Sets security levels on the proxy
 *
 * @return WMISession* Pointer to initialized WMI session, NULL if failed
 * @note Caller must call cleanupWMI() to free resources
 */
WMISession *initializeWMI(void)
{
    HRESULT hr;
    WMISession *session = (WMISession *)malloc(sizeof(WMISession));
    if (!session)
        return NULL;

    session->pSvc = NULL;
    session->pLoc = NULL;
    session->initialized = FALSE;

    // Initialize COM in multi-threaded mode
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        free(session);
        return NULL;
    }

    // Set COM security levels for WMI access
    hr = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);

    if (FAILED(hr))
    {
        CoUninitialize();
        free(session);
        return NULL;
    }

    // Create WMI locator instance
    hr = CoCreateInstance(
        &CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        &IID_IWbemLocator,
        (LPVOID *)&session->pLoc);

    if (FAILED(hr))
    {
        CoUninitialize();
        free(session);
        return NULL;
    }

    // Connect to ROOT\CIMV2 namespace
    hr = session->pLoc->lpVtbl->ConnectServer(
        session->pLoc,
        L"ROOT\\CIMV2",
        NULL,
        NULL,
        0,
        0,
        0,
        0,
        &session->pSvc);

    if (FAILED(hr))
    {
        session->pLoc->lpVtbl->Release(session->pLoc);
        CoUninitialize();
        free(session);
        return NULL;
    }

    // Set security levels on the WMI connection
    hr = CoSetProxyBlanket(
        (IUnknown *)session->pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE);

    if (FAILED(hr))
    {
        session->pSvc->lpVtbl->Release(session->pSvc);
        session->pLoc->lpVtbl->Release(session->pLoc);
        CoUninitialize();
        free(session);
        return NULL;
    }

    session->initialized = TRUE;
    return session;
}

/**
 * @brief Cleans up WMI session and releases resources
 *
 * This function:
 * - Releases WMI service interface
 * - Releases WMI locator
 * - Uninitializes COM library
 * - Frees session memory
 *
 * @param session Pointer to WMI session to be cleaned up
 */
void cleanupWMI(WMISession *session)
{
    if (session)
    {
        if (session->pSvc)
            session->pSvc->lpVtbl->Release(session->pSvc);
        if (session->pLoc)
            session->pLoc->lpVtbl->Release(session->pLoc);

        CoUninitialize();
        free(session);
    }
}

/**
 * @brief Executes a WQL query on the WMI connection
 *
 * This function executes a Windows Query Language (WQL) query
 * with forward-only and immediate return flags.
 *
 * @param session Active WMI session
 * @param query WQL query string (wide character)
 * @param ppEnumerator Pointer to store query result enumerator
 * @return BOOL TRUE if query succeeded, FALSE if failed
 */
BOOL executeWQLQuery(WMISession *session, const wchar_t *query, IEnumWbemClassObject **ppEnumerator)
{
    if (!session || !session->initialized || !session->pSvc)
        return FALSE;

    HRESULT hr = session->pSvc->lpVtbl->ExecQuery(
        session->pSvc,
        L"WQL",
        (BSTR)query,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        ppEnumerator);

    return SUCCEEDED(hr);
}

/**
 * @brief Retrieves a string property from a WMI object
 *
 * This function gets a wide string property from a WMI object
 * and converts it to a narrow string.
 *
 * @param pclsObj WMI class object
 * @param property Property name (wide character)
 * @param buffer Buffer to store the string value
 * @param bufferSize Size of the buffer
 * @return BOOL TRUE if property was retrieved, FALSE if failed or NULL
 */
BOOL getWMIPropertyString(IWbemClassObject *pclsObj, const wchar_t *property, char *buffer, size_t bufferSize)
{
    VARIANT vtProp;
    HRESULT hr;

    hr = pclsObj->lpVtbl->Get(pclsObj, property, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && vtProp.vt != VT_NULL)
    {
        wcstombs_s(NULL, buffer, bufferSize, vtProp.bstrVal, _TRUNCATE);
        VariantClear(&vtProp);
        return TRUE;
    }

    buffer[0] = '\0';
    return FALSE;
}
