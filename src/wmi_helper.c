#include "wmi_helper.h"
#include <stdio.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "ole32.lib")

WMISession *initializeWMI(void)
{
    HRESULT hr;
    WMISession *session = (WMISession *)malloc(sizeof(WMISession));
    if (!session)
        return NULL;

    session->pSvc = NULL;
    session->pLoc = NULL;
    session->initialized = FALSE;

    // Initialize COM
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        free(session);
        return NULL;
    }

    // Set general COM security levels
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

    // Obtain the initial locator to WMI
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

    // Connect to WMI through the IWbemLocator::ConnectServer method
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

    // Set security levels on the proxy
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
