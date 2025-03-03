#include "audio_info.h"
#include "wmi_helper.h"
#include <stdio.h>

/**
 * @brief Retrieves information about all audio devices in the system
 *
 * This function queries the Windows Management Instrumentation (WMI) to get
 * information about installed sound devices. It collects:
 * - Device names
 * - Manufacturer information
 *
 * @return AudioList* Pointer to allocated list of audio devices, NULL if failed
 * @note Caller is responsible for freeing the returned list using freeAudioList()
 */
AudioList *getAudioList(void)
{
    AudioList *list = (AudioList *)malloc(sizeof(AudioList));
    if (!list)
        return NULL;

    list->devices = NULL;
    list->count = 0;

    // Initialize WMI connection
    WMISession *session = initializeWMI();
    if (!session)
    {
        free(list);
        return NULL;
    }

    // Query Win32_SoundDevice class for audio device information
    IEnumWbemClassObject *pEnumerator = NULL;
    if (executeWQLQuery(session, L"SELECT * FROM Win32_SoundDevice", &pEnumerator))
    {
        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        // First pass: Count total number of audio devices
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
        {
            list->count++;
            pclsObj->lpVtbl->Release(pclsObj);
        }
        pEnumerator->lpVtbl->Release(pEnumerator);

        if (list->count > 0)
        {
            // Allocate memory for the device array
            list->devices = (AudioDeviceInfo *)malloc(sizeof(AudioDeviceInfo) * list->count);
            if (!list->devices)
            {
                cleanupWMI(session);
                free(list);
                return NULL;
            }

            // Second pass: Collect detailed information for each device
            if (executeWQLQuery(session, L"SELECT * FROM Win32_SoundDevice", &pEnumerator))
            {
                UINT i = 0;
                while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn != 0)
                {
                    AudioDeviceInfo *device = &list->devices[i];

                    // Get device name with fallback for unknown devices
                    if (!getWMIPropertyString(pclsObj, L"Name", device->name, sizeof(device->name)))
                    {
                        strcpy_s(device->name, sizeof(device->name), "Unknown Audio Device");
                    }

                    // Get manufacturer with N/A fallback
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

/**
 * @brief Frees memory allocated for AudioList structure
 *
 * @param list Pointer to AudioList structure to be freed
 */
void freeAudioList(AudioList *list)
{
    if (list)
    {
        if (list->devices)
            free(list->devices);
        free(list);
    }
}

/**
 * @brief Gets the name of an audio device
 *
 * @param device Pointer to AudioDeviceInfo structure
 * @return const char* Device name or empty string if device is NULL
 */
const char *getAudioDeviceName(const AudioDeviceInfo *device)
{
    return device ? device->name : "";
}

/**
 * @brief Gets the manufacturer of an audio device
 *
 * @param device Pointer to AudioDeviceInfo structure
 * @return const char* Manufacturer name or empty string if device is NULL
 */
const char *getAudioDeviceManufacturer(const AudioDeviceInfo *device)
{
    return device ? device->manufacturer : "";
}
