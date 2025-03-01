#include "audio_info.h"
#include "wmi_helper.h"
#include <stdio.h>

AudioList *getAudioList(void)
{
    AudioList *list = (AudioList *)malloc(sizeof(AudioList));
    if (!list)
        return NULL;

    list->devices = NULL;
    list->count = 0;

    // Initialize WMI
    WMISession *session = initializeWMI();
    if (!session)
    {
        free(list);
        return NULL;
    }

    // Query sound device information
    IEnumWbemClassObject *pEnumerator = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_SoundDevice", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        // Count devices first
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            list->count++;
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);

        if (list->count > 0)
        {
            // Allocate memory for devices
            list->devices = (AudioDeviceInfo *)malloc(sizeof(AudioDeviceInfo) * list->count);
            if (!list->devices)
            {
                cleanupWMI(session);
                free(list);
                return NULL;
            }

            // Re-query to get data
            if (executeWQLQuery(session, L"SELECT * FROM Win32_SoundDevice", &pEnumerator))
            {
                UINT i = 0;
                while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
                {
                    AudioDeviceInfo *device = &list->devices[i];

                    // Get device name
                    if (!getWMIPropertyString(pclsObj, L"Name", device->name, sizeof(device->name)))
                    {
                        strcpy_s(device->name, sizeof(device->name), "Unknown Audio Device");
                    }

                    // Get manufacturer
                    if (!getWMIPropertyString(pclsObj, L"Manufacturer", device->manufacturer, sizeof(device->manufacturer)))
                    {
                        strcpy_s(device->manufacturer, sizeof(device->manufacturer), "N/A");
                    }

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

void freeAudioList(AudioList *list)
{
    if (list)
    {
        if (list->devices)
            free(list->devices);
        free(list);
    }
}

const char *getAudioDeviceName(const AudioDeviceInfo *device)
{
    return device ? device->name : "";
}

const char *getAudioDeviceManufacturer(const AudioDeviceInfo *device)
{
    return device ? device->manufacturer : "";
}
